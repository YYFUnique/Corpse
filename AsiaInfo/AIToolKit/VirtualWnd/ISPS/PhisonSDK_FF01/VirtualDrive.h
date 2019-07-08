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

#include "SDK_Defines.h"
#include "FileIO_Defines.h"
#include "FirmwareResult.h"

#include "mutex_fwd.h"

#include <string>
#include <vector>
#include <cassert>


namespace TMPS { namespace Firmware {


class VirtualDrive;


class VirtualDriveManager
{
public:
	VirtualDriveManager();
	~VirtualDriveManager();

	Result MountDrives(const char _refLetter);
	Result UnmountDrives();
	bool IsAlreadyMounted() const { return isMounted_; };

	VirtualDrive* GetDrive(const PartitionID_t _pid) const {
		assert(_pid >= PartitionID_HA && _pid < PartiiionID_INVALID);
		return drives_[_pid];
	}
	VirtualDrive* GetDrive(const IO::StringT &_path) const;

private:
	static const size_t DrivesCount = PartiiionID_INVALID;
	std::vector<VirtualDrive*> drives_;
	bool isMounted_;
};


class VirtualDrive
{
public:
	enum ReturnCode_t
	{
		Code_OK,
		Code_GeneralFail,
		Code_AlreadyMounted,
		Code_WrongDrive
	};

	struct FileObj_t
	{
		IO::StringT  name;
		IO::StringT  dir;
		uint32_t     size;
		uint32_t     utcDate;
		uint32_t     utcTime;
		bool         isDir;
		bool         isReadOnly;

		static bool SortByName(const FileObj_t &_lhs, const FileObj_t &_rhs) { return _lhs.name < _rhs.name; }
		static bool SortByNameDesc(const FileObj_t &_lhs, const FileObj_t &_rhs) { return ! SortByName(_lhs, _rhs); }
	};
	typedef std::vector<FileObj_t> FileObjList_t;

	static const char GetVirtualLetter(const PartitionID_t _pid);

	PartitionID_t GetPID() { return pid_; }

	const IO::StringT& RootPath() { return rootPath_; }
	bool CheckRoot(const IO::StringT &_path);

	const IO::StringT& WinRootPath() { return winRootPath_; }

	Result FileOpen(IO::FD *p_fd, const IO::StringT &_path, const bool _readOnly);
	Result FileClose(IO::FD *p_fd);

	Result FileRead(IO::FD _fd, void *_buf, const uint32_t _bufLen, uint32_t *_readCount);
	Result FileWrite(IO::FD _fd, const void *_buf, const uint32_t _bufLen, uint32_t *_writeCount);
	Result FileFlush(IO::FD _fd);

	bool FileExists(const IO::StringT &_path);
	bool FolderExists(const IO::StringT &_path);

	Result RenameFile(const IO::StringT &_path, const IO::StringT &_newPath);
	Result RemoveFile(const IO::StringT &_path);

	Result CopyFileFromDrive(const IO::StringT &_dstPath, const IO::StringT &_srcPath);
	Result CopyFileToDrive(const IO::StringT &_dstPath, const IO::StringT &_srcPath);
	Result CopyFileBetweenDrives(const IO::StringT &_dstPath, const IO::StringT &_srcPath);

	Result CopyFolderFromDrive(const IO::StringT &_dstPath, const IO::StringT &_srcPath);
	Result CopyFolderToDrive(const IO::StringT &_dstPath, const IO::StringT &_srcPath);

	uint32_t GetFileSize(IO::FD _fd);
	Result GetFileSize(const IO::StringT &_path, uint32_t *_size);
	Result GetFileTime(const IO::StringT &_path, uint16_t *_date, uint16_t *_time);
	Result ConfigFileReadOnly(const IO::StringT &_path, const bool _enable);
	Result GetFileHash(const IO::StringT &_path, char _hash[33]);

	Result ListFileObjects(const IO::StringT &_basePath, const int32_t _depth, FileObjList_t *_list);
	Result CreateFolderTree(const IO::StringT &_path);
	Result RemoveFolder(const IO::StringT &_path);

	Result GetWriteProtectStatus(bool *_enable);
	Result SetWriteProtection(const bool _enable, const bool _isTemp);

	Result FormatFileSys();
	Result GetDriveSizeInfo(uint64_t *_totalSize, uint64_t *_freeSize); // size in bytes

private:
	VirtualDrive(const PartitionID_t _pid, const char _refLetter, const VirtualDriveManager* _manager);
	VirtualDrive(const VirtualDrive &_rhs);
	VirtualDrive& operator= (const VirtualDrive &_rhs);
	~VirtualDrive();

	PartitionID_t pid_;
	char refLetter_;
	IO::FSYS *fs_;
	IO::StringT rootPath_;
	IO::StringT winRootPath_;
	uint32_t deviceUnitSize_;
	TM::Mutex *mutex_;

	friend class VirtualDriveManager;
	const VirtualDriveManager *manager_;
};

}} // namespace TMPS::Firmware
