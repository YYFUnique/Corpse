/*-----------------------------------------------------------------------------
 *
 *   Copyright (C) 2002-2015, Asiainfo Inc.  All Rights Reserved.
 *
 *   This program is an unpublished copyrighted work which is proprietary
 *   to Asiainfo, Inc. and contains confidential information that is not
 *   to be reproduced or disclosed to any other person or entity without
 *   prior written consent from Asiainfo, Inc. in each and every instance.
 *
 *   WARNING:  Unauthorized reproduction of this program as well as
 *   unauthorized preparation of derivative works based upon the
 *   program or distribution of copies by sale, rental, lease or
 *   lending are violations of federal copyright laws and state trade
 *   secret laws, punishable by civil and criminal penalties.
 *
 *-----------------------------------------------------------------------------*/

#include "FileIO.h"

#include "FatFS/ff.h"
#include "FatFS/fresult.h"

#include <cassert>
#include <cstring>
#include <ctime>


namespace TMPS { namespace Firmware { namespace IO {


// result explainer
//
struct FatFS_Explainer : public Result::Explainer
{
    virtual const char* GetDesc(const int _code) { return FFResult::ToString(_code); }
    virtual bool GetFailJudge(const int _code) { return (_code != FFResult::FR_OK); }
} s_explainer;

inline Result _MakeResult(int _code)
{
    return Result(_code, s_explainer);
}


// Internal utility class
class Internal
{
public:
	static ::FATFS* GetNativeFS(IO::FSYS* _fileSys) { 
		if (! _fileSys) return NULL;
		return static_cast<::FATFS*>(_fileSys->opaque_);
	}
	static ::DIR* GetNativeDIR(IO::DIR *_dir) {
		if (! _dir) return NULL;
		return static_cast<::DIR*>(_dir->opaque_);
	}
};


// FSYS
//
FSYS::FSYS()
    : opaque_(new ::FATFS())
{
}

FSYS::~FSYS()
{
    delete static_cast<::FATFS*>(opaque_);
    opaque_ = NULL;
}


// DIR
//
DIR::DIR()
    : opaque_(new ::DIR())
{
}

DIR::~DIR()
{
    delete static_cast<::DIR*>(opaque_);
    opaque_ = NULL;
}


// utility methods
//
inline byte_t FatFsOpenMode(OpenMode _mode)
{
    return (_mode == READ) ? (FA_READ | FA_OPEN_EXISTING)
                           : (FA_WRITE | FA_CREATE_ALWAYS);
}


// public functions implementation
//
Result MountFileSys(FSYS* _fs, const byte_t _partitionID, const char _driveLetter)
{
    assert(_fs != NULL);
	return _MakeResult(::f_mount(_partitionID, Internal::GetNativeFS(_fs), _driveLetter));
}

Result UnmountFileSys(const byte_t _partitionID)
{
    return _MakeResult(::f_mount(_partitionID, NULL, 0));
}

Result FormatFileSys(FSYS* _fs, const FormatRule _rule, const uint32_t _allocUnit)
{
    return _MakeResult(::f_mkfs(Internal::GetNativeFS(_fs), _rule, _allocUnit));
}

Result GetFileSysSectors(FSYS* _fs, const byte_t _partitionID, uint64_t *_totalNum, uint64_t *_freeNum)
{
    assert(_totalNum != NULL && _freeNum != NULL);

    CharT path[4];
    path[0] = CharT('0') + _partitionID;
    path[1] = CharT(':');
    path[2] = CharT('/');
	path[3] = 0;

    ::FATFS *pfs = Internal::GetNativeFS(_fs);
    DWORD fre_clust = 0;
	Result res = _MakeResult(::f_getfree(pfs, path, &fre_clust));
    if (! res.IsFailCode())
    {
        *_totalNum = static_cast<uint64_t>(pfs->n_fatent - 2) * pfs->csize;
        *_freeNum  = static_cast<uint64_t>(fre_clust) * pfs->csize;
    }

    return res;
}

Result FileOpen(FSYS* _fs, FD *_pFD, const CharT *_path, const OpenMode _mode)
{
    assert(_pFD != NULL && _path != NULL);

    ::FIL* fp = new ::FIL;
    memset(fp, 0, sizeof(::FIL));

    Result res = _MakeResult(::f_open(Internal::GetNativeFS(_fs), fp, _path, FatFsOpenMode(_mode)));
    if (res.IsFailCode())
    {
        delete fp;
        fp = NULL;
    }

    *_pFD = fp;

    return res;
}

Result FileClose(FD *_pFD)
{
    assert(_pFD != NULL);

    ::FIL* fp = (::FIL*) *_pFD;

    Result res = (fp == NULL || fp->fs == NULL)
        ? _MakeResult(FFResult::FR_OK)
        : _MakeResult(::f_close(fp));

    delete fp;
    *_pFD = fp = NULL;

    return res;
}

Result FileSize(FD _fd, uint32_t *_size)
{
    assert(_fd != NULL && _size != NULL);

    *_size = ((::FIL*)_fd)->fsize;

    return _MakeResult(FFResult::FR_OK);
}

Result FileRead(FD _fd, void *_buf, const uint32_t _bufLen, uint32_t *_readCount)
{
    assert(_fd != NULL && _buf != NULL);
    return _MakeResult(::f_read((::FIL*)_fd, _buf, _bufLen, _readCount));
}

Result FileWrite(IO::FD _fd, const void *_buf, const uint32_t _bufLen, uint32_t *_writeCount)
{
    assert(_fd != NULL && _buf != NULL);
    return _MakeResult(::f_write((::FIL*)_fd, _buf, _bufLen, _writeCount));
}

Result FileFlush(FD _fd)
{
    assert(_fd != NULL);
    return _MakeResult(::f_sync((::FIL*)_fd));
}

Result FileRename(FSYS* _fs, const CharT *_pathOld, const CharT *_pathNew)
{
    assert(_pathOld != NULL && _pathNew != NULL);

    // path_new cannot contains drive letter -_-
    if (_pathNew[0] && _pathNew[1] == L':' && _pathNew[2] == L'\\')
        _pathNew += 3;

    return _MakeResult(::f_rename(Internal::GetNativeFS(_fs), _pathOld, _pathNew));
}

Result FileDelete(FSYS* _fs, const CharT *_path)
{
    assert(_path != NULL);
	return _MakeResult(::f_unlink(Internal::GetNativeFS(_fs), _path));
}

Result FileStatus(FSYS* _fs, const CharT *_path, FileInfo *_info)
{
    assert(_path != NULL && _info != NULL);

    ::FILINFO info = {0};
    Result res = _MakeResult(::f_stat(Internal::GetNativeFS(_fs), _path, &info));
    if (! res.IsFailCode())
    {
        _info->size = info.fsize;
        _info->modDate = info.fdate;
        _info->modTime = info.ftime;
        _info->readOnly = ((info.fattrib & AM_RDO) != 0);
    }

    return res;
}

Result FileConfigReadOnly(FSYS* _fs, const CharT *_path, const bool _set)
{
    assert(_path != NULL);

    byte_t value = (_set) ? 1 : 0;
    return _MakeResult(::f_chmod(Internal::GetNativeFS(_fs), _path, value, AM_RDO));
}

Result FileSetModTime(FSYS* _fs, const CharT *_path, const uint16_t date, const uint16_t time)
{
    assert(_path != NULL);

    ::FILINFO info = {0};
    info.fdate = date;
    info.ftime = time;
    return _MakeResult(::f_utime(Internal::GetNativeFS(_fs), _path, &info));
}

Result MakeDir(FSYS* _fs, const CharT *_path)
{
    assert(_path != NULL);
    return _MakeResult(::f_mkdir(Internal::GetNativeFS(_fs), _path));
}

Result OpenDir(FSYS* _fs, DIR *_dir, const CharT *_path)
{
	::DIR* pDir = Internal::GetNativeDIR(_dir);
    assert(_dir != NULL && pDir != NULL);

    return _MakeResult(::f_opendir(Internal::GetNativeFS(_fs), pDir, _path));
}

Result ReadDir(DIR &_dir, FileNode *_node)
{
	::DIR* pDir = Internal::GetNativeDIR(&_dir);
    assert(pDir != NULL && _node != NULL);

    ::FILINFO info;
    CharT buff[PathBuffLen];
    info.lfname = buff;
    info.lfsize = PathBuffLen;

    Result res = _MakeResult(::f_readdir(pDir, &info));
    if (! res.IsFailCode())
    {
        _node->isDir = ((info.fattrib & AM_DIR) != 0);
        _node->info.size = info.fsize;
        _node->info.modDate = info.fdate;
        _node->info.modTime = info.ftime;

        if (info.lfname != NULL && info.lfname[0] != CharT('\0'))
            _node->name = info.lfname;
        else
            _node->name = info.fname;
    }

    return res;
}

void FatTimeToCTime(const uint16_t _fatDate, const uint16_t _fatTime, tm *_tm)
{
    assert(_tm != NULL);

    _tm->tm_year = (_fatDate >> 9) + 1980 - 1900;
    _tm->tm_mon  = ((_fatDate >> 5) & 0x0F) - 1;
    _tm->tm_mday = (_fatDate & 0x1F);

    _tm->tm_hour = (_fatTime >> 11);
    _tm->tm_min  = ((_fatTime >> 5) & 0x3F);
    _tm->tm_sec  = ((_fatTime & 0x1F) << 1);
}

void CTimeToFatTime(const tm &_tm, uint16_t *_fatDate, uint16_t *_fatTime)
{
    assert(_fatDate != NULL && _fatTime != NULL);

    *_fatDate = (static_cast<uint16_t>(_tm.tm_year + 1900 - 1980) << 9)
              | (static_cast<uint16_t>(_tm.tm_mon + 1) << 5)
              | (static_cast<uint16_t>(_tm.tm_mday));
    *_fatTime = (static_cast<uint16_t>(_tm.tm_hour) << 11)
              | (static_cast<uint16_t>(_tm.tm_min) << 5)
              | (static_cast<uint16_t>(_tm.tm_sec) >> 1);
}

}}} // namespace TMPS::Firmware::IO
