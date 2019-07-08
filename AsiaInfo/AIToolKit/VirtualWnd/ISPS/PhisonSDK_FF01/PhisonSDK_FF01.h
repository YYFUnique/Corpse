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

#include "SDK_Interface.h"


namespace TMPS { namespace Firmware {


class PhisonSDK_FF01 : public SDK_Interface
{
public:
	virtual Result InitAPI();
	virtual Result CloseAPI();

	virtual Result GetSDKVersion(uint32_t *_version);

	virtual Result GetDriveLetterList(DriveLettersList_t *_list);
	virtual Result GetDriveLetters(const char _refLetter, DriveLetters_t *_letters);

	virtual Result GetDeviceInfo(const char _driveLetter, DeviceInfo_t *_info);
	virtual Result GetUnitSize(const char _driveLetter, uint32_t *_size);

	virtual Result GetWriteProtectStatus(const char _driveLetter, bool *_isEnable);
	virtual Result SetWriteProtect(const char _driveLetter, const bool _isEnable, const bool _isTemp);

	virtual Result ReadSector(
		const PartitionID_t _partID, const char _driveLetter,
		const uint32_t _pageIndex, const uint32_t _pageCount,
		const unsigned char* _buf
	);

	virtual Result WriteSector(
		const PartitionID_t _partID, const char _driveLetter,
		const uint32_t _pageIndex, const uint32_t _pageCount,
		const unsigned char* _buf
	);

	virtual Result FormatPartition(const char _driveLetter, const char* _label);

	virtual Result ResizePublicPartitions(
		const char _driveLetter, const uint32_t _size,
		const char* _labelSystem, const char* _labelData
	);

	virtual Result SetVID(const char _driveLetter, const char* _vid);
	virtual Result SetPID(const char _driveLetter, const char* _pid);
	virtual Result SetVendorName(const char _driveLetter, const std::string &_name);  // max length = 8
	virtual Result SetProductName(const char _driveLetter, const std::string &_name); // max length = 16

	virtual Result GetLEDStatus(const char _driveLetter, LED_Status_t _status[3]);
	virtual Result SetLEDStatus(const char _driveLetter, const LED_Index_t _index, const LED_Status_t _status);

	virtual Result MakeResult(int _code);
	virtual Result DumpDeviceInfo(const char _driveLetter, std::string *_msg);

private:
	uint32_t version_;
};


}} // namespace TMPS::Firmware
