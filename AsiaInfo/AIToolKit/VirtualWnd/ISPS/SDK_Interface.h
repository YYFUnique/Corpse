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

#include "libTM/common/base_types.h"
#include "SDK_Defines.h"
#include "FirmwareResult.h"

class SDK_Interface
{
protected:
	friend class SDK_Factory;
	virtual Result InitAPI() = 0;
	virtual Result CloseAPI() = 0;

public:
    virtual ~SDK_Interface() {}

	virtual Result GetSDKVersion(uint32_t *_version) = 0;

	virtual Result GetDriveLetterList(DriveLettersList_t *_list) = 0;
	virtual Result GetDriveLetters(const char _refLetter, DriveLetters_t *_letters) = 0;

	virtual Result GetDeviceInfo(const char _driveLetter, DeviceInfo_t *_info) = 0;
	virtual Result GetUnitSize(const char _driveLetter, uint32_t *_size) = 0;

	virtual Result GetWriteProtectStatus(const char _driveLetter, bool *_isEnable) = 0;
	virtual Result SetWriteProtect(const char _driveLetter, const bool _isEnable, const bool _isTemp) = 0;

	virtual Result ReadSector(
		const PartitionID_t _partID, const char _driveLetter,
		const uint32_t _pageIndex, const uint32_t _pageCount,
		const unsigned char* _buf
	) = 0;

	virtual Result WriteSector(
		const PartitionID_t _partID, const char _driveLetter,
		const uint32_t _pageIndex, const uint32_t _pageCount,
		const unsigned char* _buf
	) = 0;

	virtual Result FormatPartition(const char _driveLetter, const char* _label) = 0;

	virtual Result ResizePublicPartitions(
		const char _driveLetter, const uint32_t _size,
		const char* _labelSystem, const char* _labelData
	) = 0;

	virtual Result SetVID(const char _driveLetter, const char* _vid) = 0;
	virtual Result SetPID(const char _driveLetter, const char* _pid) = 0;
	virtual Result SetVendorName(const char _driveLetter, const std::string &_name) = 0;  // max length = 8
	virtual Result SetProductName(const char _driveLetter, const std::string &_name) = 0; // max length = 16

	virtual Result GetLEDStatus(const char _driveLetter, LED_Status_t _status[3]) = 0;
	virtual Result SetLEDStatus(const char _driveLetter, const LED_Index_t _index, const LED_Status_t _status) = 0;

	virtual Result MakeResult(int _code) = 0;
	virtual Result DumpDeviceInfo(const char _driveLetter, std::string *_msg) = 0;
};

SDK_Interface& SDK();


struct DeviceSettings
{
	static const char* SysLabel() { return "ISPS2 SYS"; }
	static const char* DataLabel() { return "ISPS2 DAT"; }
	static const char* VID() { return "2203"; }
	static const char* VendorName() { return "YX"; }
	static const char* V3_PID() { return "3837"; }
	static const char* V3_ProductName() { return "ISPS2 DISK"; }
	static const char* V2_PID() { return "3836"; }
	static const char* V2_ProductName() { return "ISPS2 DISK"; }
	static const char* V1_PID() { return "3835"; }
	static const char* V1_ProductName() { return "ISPS DISK"; }
};
