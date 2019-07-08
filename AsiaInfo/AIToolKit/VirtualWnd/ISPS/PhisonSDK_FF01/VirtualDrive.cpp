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
#include "StdAfx.h"
#include "VirtualDrive.h"
#include "VirtualDrive_Explainer.hpp"

#include "SDK_Interface.h"
#include "FileIO.h"
#include "FatFS/fresult.h"

#include "libTM/concurrency/mutex.h"
#include "libTM/hash/md5.h"
#include "libLog/DebugLog.h"

#include <map>
#include <vector>

#if _OS_WIN_
#	pragma warning(disable: 4996)
#endif


namespace TMPS { namespace Firmware {


VirtualDriveExplainer s_VD_explainer;


VirtualDriveManager::VirtualDriveManager()
	: drives_(VirtualDriveManager::DrivesCount, NULL)
	, isMounted_(false)
{
}

VirtualDriveManager::~VirtualDriveManager()
{
	if (this->IsAlreadyMounted())
		this->UnmountDrives();
}

Result VirtualDriveManager::MountDrives(const char _refLetter)
{
	if (IsAlreadyMounted())
		return _MakeError(VirtualDrive::Code_AlreadyMounted);

	DriveLetters_t letters;
	Result res = SDK().GetDriveLetters(_refLetter, &letters);
	if (res.IsFailCode())
	{
		LOGERROR("failed to get drive letter (via %c): %s(%d)", _refLetter, res.Desc(), res.Code());
		return res;
	}

	drives_[PartitionID_HA] = new VirtualDrive(PartitionID_HA, _refLetter, this);
	drives_[PartitionID_DATA] = new VirtualDrive(PartitionID_DATA, letters.dataLetter, this);
	drives_[PartitionID_SYS] = new VirtualDrive(PartitionID_SYS, letters.sysLetter, this);

	isMounted_ = true;

	return _MakeSuccess();
}

Result VirtualDriveManager::UnmountDrives()
{
	for (size_t i = 0; i < DrivesCount; ++i) {
		delete drives_[i];
		drives_[i] = NULL;
	}

	isMounted_ = false;

	return _MakeSuccess();
}

VirtualDrive* VirtualDriveManager::GetDrive(const IO::StringT &_path) const
{
	if (_path.empty())
		return NULL;

	PartitionID_t pid = static_cast<PartitionID_t>(_path[0] - IO::CharT('0'));
	if (pid < PartitionID_HA || pid >= PartiiionID_INVALID)
		return NULL;

	return GetDrive(pid);
}


VirtualDrive::VirtualDrive(const PartitionID_t _pid, const char _refLetter, const VirtualDriveManager* _manager)
	: pid_(_pid), refLetter_(_refLetter), fs_(NULL), mutex_(NULL), manager_(_manager)
{
	fs_ = new IO::FSYS;
	IO::MountFileSys(fs_, pid_, refLetter_);

	IO::CharT virtualLetter = static_cast<IO::CharT>(this->GetVirtualLetter(pid_));
	IO::StringT appending = IO::StringT(1, IO::CharT(':')).append(1, IO::PathSep);
	rootPath_ = IO::StringT(1, virtualLetter) + appending;
	winRootPath_ = IO::StringT(1, static_cast<IO::CharT>(refLetter_)) + appending;

	if (SDK().GetUnitSize(refLetter_, &deviceUnitSize_).IsFailCode())
	{
		// Fail to UDV_GetUnitSize(), we take UnitSize = 64
		deviceUnitSize_ = 1024;
	}

	mutex_ = new TM::Mutex();
}

VirtualDrive::~VirtualDrive()
{
	IO::UnmountFileSys(pid_);
	delete fs_;
	fs_ = NULL;
	delete mutex_;
	mutex_ = NULL;
}

const char VirtualDrive::GetVirtualLetter(const PartitionID_t _pid)
{
	return '0' + _pid;
}

bool VirtualDrive::CheckRoot(const IO::StringT &_path)
{
	return (_path[0] == rootPath_[0]);
}

Result VirtualDrive::FileOpen(IO::FD *p_fd, const IO::StringT &_path, const bool _readOnly)
{
	if (p_fd == NULL)
		return _MakeError();
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	IO::OpenMode mode = (_readOnly) ? IO::READ : IO::OVERWRITE;
	Result res = IO::FileOpen(fs_, p_fd, _path.c_str(), mode);
	if (res.IsFailCode())
		IO::FileClose(p_fd);

	if (! _readOnly)
		IO::FileFlush(*p_fd);

	return res;
}

Result VirtualDrive::FileClose(IO::FD *p_fd)
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	return IO::FileClose(p_fd);
}

Result VirtualDrive::FileRead(IO::FD _fd, void *_buf, const uint32_t _bufLen, uint32_t *_readCount)
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	return IO::FileRead(_fd, _buf, _bufLen, _readCount);
}

Result VirtualDrive::FileWrite(IO::FD _fd, const void *_buf, const uint32_t _bufLen, uint32_t *_writeCount)
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	return IO::FileWrite(_fd, _buf, _bufLen, _writeCount);
}

Result VirtualDrive::FileFlush(IO::FD _fd)
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	return IO::FileFlush(_fd);
}

bool VirtualDrive::FileExists(const IO::StringT &_path)
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	IO::FD fd;
	Result res = this->FileOpen(&fd, _path, true);
	if (! res.IsFailCode())
	{
		this->FileClose(&fd);
		return true;
	}
	else
	{
		return false;
	}
}

bool VirtualDrive::FolderExists(const IO::StringT &_path)
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	IO::DIR dir;
	Result res = IO::OpenDir(fs_, &dir, _path.c_str());	

	return (res.IsFailCode() == false);
}

Result VirtualDrive::RenameFile(const IO::StringT &_path, const IO::StringT &_newPath)
{
	if (_newPath.empty())
		return _MakeError();
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	return IO::FileRename(fs_, _path.c_str(), _newPath.c_str());
}

Result VirtualDrive::RemoveFile(const IO::StringT &_path)
{
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	Result res = IO::FileDelete(fs_, _path.c_str());
	if (res.IsFailCode())
	{
		if (res == IO::FFResult::FR_NO_FILE || res == IO::FFResult::FR_NO_PATH) // file not exist, treat it as success
			return _MakeSuccess();

		// try to remove read only and delete again
		LOGDEBUG("failed to delete file %S, now try to remove read-only and delete again", _path.c_str());

		if (this->ConfigFileReadOnly(_path, false).IsFailCode())
		{
			LOGERROR("failed to delete %S", _path.c_str());
			return res;
		}

		res = IO::FileDelete(fs_, _path.c_str());
		if (res.IsFailCode())
		{
			LOGERROR("failed to delete file %S in 2nd try, err: %s(%d)", _path.c_str(), res.Desc(), res.Code());
			return res;
		}
	}

	return res;
}

Result VirtualDrive::CopyFileBetweenDrives(const IO::StringT &_dstPath, const IO::StringT &_srcPath)
{
	VirtualDrive *srcDrive = manager_->GetDrive(_srcPath);
	VirtualDrive *dstDrive = manager_->GetDrive(_dstPath);

	if (! srcDrive || ! dstDrive)
		return _MakeError(Code_WrongDrive);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	// Get source file time
	IO::FileInfo srcInfo;
	srcInfo.modTime = srcInfo.modDate = 0;
	Result res = IO::FileStatus(srcDrive->fs_, _srcPath.c_str(), &srcInfo);
	if (res.IsFailCode())
	{
		LOGERROR("failed to get file stat, path: %S, error: %s(%d)", _srcPath.c_str(), res.Desc(), res.Code());
	}

	// do copy
	IO::FD srcFD = NULL, dstFD = NULL;
	do
	{
		// open source file
		res = IO::FileOpen(srcDrive->fs_, &srcFD, _srcPath.c_str(), IO::READ);
		if (res.IsFailCode())
		{
			LOGERROR("fail to open file on drive, path: %S, error: %s(%d)", _srcPath.c_str(), res.Desc(), res.Code());
			break;
		}

		// open destination file
		res = IO::FileOpen(dstDrive->fs_, &dstFD, _dstPath.c_str(), IO::OVERWRITE);
		if (res.IsFailCode())
		{
			LOGERROR("fail to open file on drive, path: %S, err: %s(%d)", _dstPath.c_str(), res.Desc(), res.Code());
			break;
		}

		// Use the (unitSize * 512) bytes as the buffer length for better performance
		uint32_t buffLen = deviceUnitSize_ * 512;
		std::vector<byte_t> buff;
		try { buff.resize(buffLen); } catch (std::bad_alloc)
		{
			LOGERROR("fail to allocate buffer");
			res = _MakeError();
			break;
		}

		// Copy source to destination
		uint32_t sizeRead = 0, totalRead = 0;
		uint32_t sizeWrite = 0, totalWrite = 0;
		do
		{
			sizeRead = sizeWrite = 0;

			res = IO::FileRead(srcFD, &buff[0], buffLen, &sizeRead);
			totalRead += sizeRead;

			if (res.IsFailCode() || sizeRead == 0)
				break;

			res = IO::FileWrite(dstFD, &buff[0], sizeRead, &sizeWrite);
			totalWrite += sizeWrite;
			if (res.IsFailCode() || sizeWrite < sizeRead)
				break;
		}
		while (true);

		// Check if the file is fully copied
		uint32_t fileSize;
		IO::FileSize(srcFD, &fileSize);
		if (fileSize != totalWrite)
		{
			LOGDEBUG("copy not completed (%S -> %S), total: %d, read: %d, write: %d",
			         _srcPath.c_str(), _dstPath.c_str(),
			         fileSize, totalRead, totalWrite);
			res = _MakeError();
		}

	} while (false);

	// close files
	IO::FileClose(&srcFD);
	IO::FileClose(&dstFD);

	// Set destination file time
	if (srcInfo.modDate != 0)
	{
		Result res2 = IO::FileSetModTime(dstDrive->fs_, _dstPath.c_str(), srcInfo.modDate, srcInfo.modTime);
		if (res2.IsFailCode())
		{
			LOGDEBUG("fail to set file time of %S, err: %s(%d)", _dstPath.c_str(), res2.Desc(), res2.Code());
		}
	}

	return res;
}

uint32_t VirtualDrive::GetFileSize(IO::FD _fd)
{
	assert(_fd != NULL);

	uint32_t size;
	IO::FileSize(_fd, &size);

	return size;
}

Result VirtualDrive::GetFileSize(const IO::StringT& _path, uint32_t *_size)
{
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	IO::FileInfo info;
	Result res = IO::FileStatus(fs_, _path.c_str(), &info);
	if (res.IsFailCode())
	{
		LOGERROR("failed to get file status, path: %S, err: %s(%d)", _path.c_str(), res.Desc(), res.Code());
		return res;
	}

	if (_size != NULL)
		*_size = info.size;

	return res;
}

Result VirtualDrive::GetFileTime(const IO::StringT &_path, uint16_t *_date, uint16_t *_time)
{
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	// Get the UTC dos file time
	IO::FileInfo info;
	Result res = IO::FileStatus(fs_, _path.c_str(), &info);
	if (res.IsFailCode())
	{
		LOGERROR("failed to get file time, path: %S, err: %s(%d)", _path.c_str(), res.Desc(), res.Code());
		return res;
	}

	if (_date != NULL)
		*_date = info.modDate;
	if (_time != NULL)
		*_time = info.modTime;

	return res;
}

Result VirtualDrive::ConfigFileReadOnly(const IO::StringT &_path, const bool _enable)
{
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	return IO::FileConfigReadOnly(fs_, _path.c_str(), _enable);
}

Result VirtualDrive::GetFileHash(const IO::StringT &_path,  char _hash[33])
{
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);
	if (_hash == NULL)
		return _MakeError();

	// Init MD5 state structure
	md5_state_t md5_state;
	md5_init(&md5_state);

	// read and calculate the file content
	Result res = _MakeError();
	{
		TM::LockGuard<TM::Mutex> lock(*mutex_);

		// Open the file
		IO::FD fd;
		res = IO::FileOpen(fs_, &fd, _path.c_str(), IO::READ);
		if (res.IsFailCode())
		{
			LOGERROR("failed to open file %S, err: %s(%d)", _path.c_str(), res.Desc(), res.Code());
			return res;
		}

		// alloc read buffer
		const uint32_t buffLen = deviceUnitSize_ * 1024;
		std::vector<byte_t> buff;
		try { buff.resize(buffLen); } catch (std::bad_alloc)
		{
			LOGERROR("failed to alloc buffer");
			IO::FileClose(&fd);
			return _MakeError();
		}

		// read file content and do md5_append
		uint32_t sizeRead = 0, totalRead = 0;
		do
		{
			res = IO::FileRead(fd, &buff[0], buffLen, &sizeRead);
			if (res.IsFailCode())
				break;

			totalRead += sizeRead;

			md5_append(&md5_state, &buff[0], sizeRead);
		} while (sizeRead == buffLen);

		IO::FileClose(&fd);
	}

	// Finish and copy the MD5 digest to the output buffer.
	if (! res.IsFailCode())
	{
		const size_t MD5_DIGEST_LENGTH = 16;
		md5_byte_t md5_digest[MD5_DIGEST_LENGTH];

		md5_finish(&md5_state, md5_digest);

		md5_byte_t *read = md5_digest;
		md5_byte_t *end = md5_digest + MD5_DIGEST_LENGTH;
		char *out = _hash;
		while (read != end)
		{
			sprintf(out, "%02x", *read);
			++read;
			out += 2;
		}
		*out = '\0';
	}

	return res;
}

Result _doListFileObjects(IO::FSYS *_fs, const IO::StringT &_basePath, const int32_t _depth, VirtualDrive::FileObjList_t *_list)
{
	// Check if the folder path exist or not
	IO::DIR dir;
	Result res = IO::OpenDir(_fs, &dir, _basePath.c_str());
	if (res.IsFailCode())
	{
		LOGERROR("folder path (%S) does not exist, err: %s(%d)", _basePath.c_str(), res.Desc(), res.Code());
		return res;
	}

	IO::FileNode node;
	while (! IO::ReadDir(dir, &node).IsFailCode() && ! node.name.empty())
	{
		if (node.name[0] == IO::CharT('.') &&
			(node.name[1] == IO::CharT('\0') || node.name[1] == IO::CharT('.')))
			continue;
		if (node.info.modDate == 0 && node.info.modTime == 0 && node.info.size == 0)
			continue;

		// add new node
		_list->push_back(VirtualDrive::FileObj_t());
		VirtualDrive::FileObj_t &fileObj = _list->back();
		fileObj.name = node.name;
		fileObj.dir = _basePath;
		fileObj.isDir = node.isDir;;
		fileObj.isReadOnly = node.info.readOnly;
		fileObj.size = node.info.size;
		fileObj.utcDate = node.info.modDate;
		fileObj.utcTime = node.info.modTime;

		// recursive
		if (node.isDir && _depth > 0)
		{
			IO::StringT subPath = _basePath + IO::PathSep + node.name;
			res = _doListFileObjects(_fs, subPath, _depth-1, _list);
			if (res.IsFailCode())
				return res;
		}
	}

	return res;
}

Result VirtualDrive::ListFileObjects(const IO::StringT &_basePath, const int32_t _depth, FileObjList_t *_list)
{
	if (_basePath.empty() || _list == NULL)
		return _MakeError();
	if (! CheckRoot(_basePath))
		return _MakeError(Code_WrongDrive);

	_list->clear();

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	// Because Fatfs library does not support '\' for trailing, we should remove it
	IO::StringT::size_type lastPos = _basePath.length() - 1;
	if (_basePath[lastPos] == IO::PathSep)
		return _doListFileObjects(fs_, _basePath.substr(0, lastPos), _depth, _list);
	else
		return _doListFileObjects(fs_, _basePath, _depth, _list);
}

Result VirtualDrive::CreateFolderTree(const IO::StringT &_path)
{
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	size_t pos = rootPath_.length();
	if (_path.length() <= pos)
		return _MakeSuccess();

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	while (true)
	{
		pos = _path.find_first_of(IO::PathSep, pos+1);

		Result res = IO::MakeDir(fs_, _path.substr(0, pos).c_str());
		if (res.IsFailCode() && res.Code() != IO::FFResult::FR_EXIST)
		{
			LOGERROR("failed to mkdir (%S), err: %s(%d)", _path.substr(0, pos).c_str(), res.Desc(), res.Code());
			return res;
		}

		if (pos == _path.npos ||
			pos == _path.length()-1) // skip trailing '\'
		{
			break;
		}
	}

	return _MakeSuccess();
}

Result VirtualDrive::RemoveFolder(const IO::StringT &_path)
{
	if (! CheckRoot(_path))
		return _MakeError(Code_WrongDrive);

	// Because Fatfs library does not support '\' for trailing, we should remove it
	IO::StringT path = _path;
	if (path[path.size() - 1] == IO::PathSep)
		path.erase(path.size() - 1);

	TM::LockGuard<TM::Mutex> lock(*mutex_);

	// open handle of the folder
	IO::DIR dir;
	Result res = IO::OpenDir(fs_, &dir, path.c_str());
	if (res.IsFailCode())
	{
		// folder not exist, treat it as success
		return _MakeSuccess();
	}

	// Delete all the files/dirs in the directory
	IO::FileNode node;
	while (! IO::ReadDir(dir, &node).IsFailCode() && ! node.name.empty())
	{
		if (node.name[0] == IO::CharT('.'))
			continue;

		// delete the sub item
		IO::StringT fileObjPath = path + IO::PathSep + node.name;
		res = (node.isDir) ? RemoveFolder(fileObjPath) : RemoveFile(fileObjPath);
		if (res.IsFailCode())
		{
			LOGERROR("Fail to recursive delete sub-item: %S", fileObjPath.c_str());
			return res;
		}
	}

	// delete the folder itself
	res = IO::FileDelete(fs_, path.c_str());
	if (res.IsFailCode())
	{
		LOGERROR("fail to delete folder: %S, err: %s(%d)", path.c_str(), res.Desc(), res.Code());
	}

	return res;
}

Result VirtualDrive::GetWriteProtectStatus(bool *_enable)
{
	return SDK().GetWriteProtectStatus(refLetter_, _enable);
}

Result VirtualDrive::SetWriteProtection(const bool _enable, const bool _isTemp)
{
	return SDK().SetWriteProtect(refLetter_, _enable, _isTemp);
}

Result VirtualDrive::FormatFileSys()
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	return IO::FormatFileSys(fs_, IO::FDISK, 64 * 1024); // block size = 64K
}

Result VirtualDrive::GetDriveSizeInfo(uint64_t *_totalSize, uint64_t *_freeSize)
{
	TM::LockGuard<TM::Mutex> lock(*mutex_);

	// Get volume information and free clusters
	uint64_t tot_sect, fre_sect;
	Result res = IO::GetFileSysSectors(fs_, pid_, &tot_sect, &fre_sect);
	if (res.IsFailCode())
	{
		LOGERROR("fail to GetFileSysSectors(): %s", res.Desc());
		return res;
	}

	// output in bytes
	if (_totalSize != NULL)
		*_totalSize = tot_sect * 512; // FatFS sector is fixed to 512 bytes
	if (_freeSize != NULL)
		*_freeSize  = fre_sect * 512; // FatFS sector is fixed to 512 bytes

	return _MakeSuccess();
}


}} // namespace TMPS::Firmware
