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
#pragma once

#include "FileIO_Defines.h"
#include "FirmwareResult.h"

struct tm; // the one in <ctime>


namespace TMPS { namespace Firmware { namespace IO {

Result MountFileSys(FSYS* _fs, const byte_t _partitionID, const char _driveLetter);
Result UnmountFileSys(const byte_t _partitionID);

Result FormatFileSys(FSYS* _fs, const FormatRule _rule, const uint32_t _allocUnit);
Result GetFileSysSectors(FSYS* _fs, const byte_t _partitionID, uint64_t *_totalNum, uint64_t *_freeNum);

Result FileOpen(FSYS* _fs, FD *_pFD, const CharT *_path, const OpenMode _mode);
Result FileClose(FD *_pFD);

Result FileSize(FD _fd, uint32_t *_size);

Result FileRead(FD _fd, void *_buf, const uint32_t _bufLen, uint32_t *_readCount);
Result FileWrite(FD _fd, const void *_buf, const uint32_t _bufLen, uint32_t *_writeCount);
Result FileFlush(FD _fd);

Result FileRename(FSYS* _fs, const CharT *_pathOld, const CharT *_pathNew);
Result FileDelete(FSYS* _fs, const CharT *_path);

Result FileStatus(FSYS* _fs, const CharT *_path, FileInfo *_info);
Result FileConfigReadOnly(FSYS* _fs, const CharT *_path, const bool _set);
Result FileSetModTime(FSYS* _fs, const CharT *_path, const uint16_t date, const uint16_t time);

Result MakeDir(FSYS* _fs, const CharT *_path);
Result OpenDir(FSYS* _fs, DIR *_dir, const CharT *_path);
Result ReadDir(DIR &_dir, FileNode *_node);

void FatTimeToCTime(const uint16_t _fatDate, const uint16_t _fatTime, tm *_tm);
void CTimeToFatTime(const tm &_tm, uint16_t *_fatDate, uint16_t *_fatTime);

}}} // namespace TMPS::Firmware::IO
