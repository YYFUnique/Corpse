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
#include "stdafx.h"
#include "PhisonSDK_FF01.h"

#include "FirmwareResult.h"
#include "PhisonCode_FF01.h"

#include "PhisonCode_FF01.h"
#include "include/win/SSNet_TMPS_SDK_FF01.h"

#include <sstream>
#include <memory.h>

//static const unsigned char g_UDVPassword[64] =
//{
//	0x5E, 0x01, 0x14, 0x5E, 0xC7, 0x7B, 0x91, 0xDA, 0xE4, 0xCC, 0xE0, 0x31, 0xAE, 0x39, 0x17, 0xA9,
//	0x5D, 0x27, 0x36, 0xE8, 0xEE, 0x9C, 0x17, 0x48, 0xBF, 0x81, 0xB9, 0x60, 0x72, 0x53, 0xBD, 0x85,
//	0xFC, 0x96, 0x2A, 0xAC, 0x17, 0xAD, 0xAE, 0x79, 0xF1, 0x6D, 0x87, 0xEB, 0x64, 0x85, 0xBA, 0xF3,
//	0xF1, 0x28, 0xCC, 0x56, 0xE1, 0x7A, 0xA2, 0x36, 0xD3, 0xB0, 0x38, 0x26, 0x6E, 0xFE, 0x62, 0x8C
//};

namespace TMPS { namespace Firmware {


static const unsigned char g_UDVPassword[64] =
{
    0xBF,0xD6,0xC7,0x69,0x95,0xBF,0x0C,0x0D,0x15,0x16,0x73,0x76,0x77,0x6B,0x54,0xD4,
	0x7F,0xF8,0xA3,0x93,0x18,0xBA,0xD1,0x36,0x71,0x08,0xD2,0x7B,0x07,0x8E,0xD4,0xC7,
	0xB6,0x47,0x06,0x81,0x56,0xB7,0xDD,0xFD,0x7F,0xB1,0x47,0x10,0xE6,0x58,0xAD,0x57,
	0x80,0x8B,0x3B,0x7A,0x53,0x9E,0xDA,0xCB,0xA0,0x72,0xBF,0x98,0x63,0x42,0x82,0x18
};


struct PhisonFF01_Explainer : public Result::Explainer
{
	virtual const char* GetDesc(const int _code) { return PhisonCode_FF01::ToString(_code); }
	virtual bool GetFailJudge(const int _code) { return (_code != PhisonCode_FF01::UDVReturnOK); }
};
static PhisonFF01_Explainer s_explainer;


Result PhisonSDK_FF01::MakeResult(const int _code)
{
	return Result(_code, s_explainer);
}

Result PhisonSDK_FF01::InitAPI()
{
	Result res = MakeResult(UDV_InitAPI(&version_, g_UDVPassword, sizeof(g_UDVPassword)));
	if (res.IsFailCode())
		return res;

	res = GetDriveLetterList(NULL);
	if (res.IsFailCode())
		UDV_CloseAPI();

	return res;
}

Result PhisonSDK_FF01::CloseAPI()
{
	return MakeResult(UDV_CloseAPI());
}

Result PhisonSDK_FF01::GetSDKVersion(uint32_t *_version)
{
	if (_version != NULL)
		*_version = version_;

	return MakeResult(PhisonCode_FF01::UDVReturnOK);
}

Result PhisonSDK_FF01::GetDriveLetterList(DriveLettersList_t *_list)
{
	// Query the device identifier list on the host
	unsigned int listSize = UDVMaxTokenNumber;
	UDVTokenListInfo tokenList[UDVMaxTokenNumber];

	memset(&tokenList, 0, sizeof(tokenList));
	for (unsigned int i = 0; i < UDVMaxTokenNumber; ++i)
		tokenList[i].dwTokenListInfoStructSize = sizeof(UDVTokenListInfo);

	Result res = MakeResult(UDV_ListTokens(tokenList, &listSize, NULL, NULL, 0));
	if (res.IsFailCode() || _list == NULL)
		return res;

	_list->clear();

	// Enum all the Phison device
	// Secure -> TMPS_DATA, Public -> TMPS_SYSTEM
	DriveLetters_t letter;
	for (unsigned int i = 0; i < listSize; ++i)
	{
		letter.sysLetter = toupper(tokenList[i].cPublicAreaDriveIdentifier);
		letter.dataLetter = toupper(tokenList[i].cSecureAreaDriveIdentifier);

		_list->push_back(letter);
	}

	return res;
}

Result PhisonSDK_FF01::GetDriveLetters(const char _refLetter, DriveLetters_t *_letters)
{
	DriveLettersList_t list;
	Result res = this->GetDriveLetterList(&list);
	if (res.IsFailCode())
		return res;

	char ref = toupper(_refLetter);
	for (DriveLettersList_t::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		char data = toupper(it->dataLetter);
		char sys = toupper(it->sysLetter);
		if (ref == data || ref == sys)
		{
			if (_letters != NULL)
			{
				_letters->dataLetter = data;
				_letters->sysLetter = sys;
			}
			return MakeResult(PhisonCode_FF01::UDVReturnOK);
		}
	}

	return MakeResult(PhisonCode_FF01::UDVErrorcDriveLetterNotFound);
}

Result PhisonSDK_FF01::GetDeviceInfo(const char _driveLetter, DeviceInfo_t *_info)
{
	UDVDeviceInfo info;
	memset(&info, 0, sizeof(UDVDeviceInfo));
	info.dwDeviceInfoStructSize = sizeof(UDVDeviceInfo);

	Result res = MakeResult(UDV_GetDeviceInfo(_driveLetter, &info));
	if (res.IsFailCode() || _info == NULL)
		return res;

	_info->serial = reinterpret_cast<char *>(info.a_bySerialNumber);
	_info->PID = reinterpret_cast<char *>(info.a_byPID);
	_info->VID = reinterpret_cast<char *>(info.a_byVID);

	_info->firmwareVer = reinterpret_cast<char *>(info.a_byFirmwareVersion);
	_info->dataSectors = info.dwSecureSize;
	_info->sysSectors = info.dwPublicSize;
	_info->hiddenSectors = info.dwNormalHiddenSize;

	_info->deviceVersion = Firmware::DeviceUnknown;
	switch (info.iController_ID)
	{
	case 0x2312: // TMPS2_B
	case 0x2267: // TMPS2_A
		if (_info->hiddenSectors > 0 && info.iMode == 7 && _info->VID == Firmware::DeviceSettings::VID())
			_info->deviceVersion = Firmware::DeviceVer2x;
		break;
	case 0x2233: // TMPS1
		if (info.iMode == 7 && _info->VID == Firmware::DeviceSettings::VID())
			_info->deviceVersion = Firmware::DeviceVer1x;
		break;
	default:
		break;
	}

	return res;
}

Result PhisonSDK_FF01::GetUnitSize(const char _driveLetter, uint32_t *_size)
{
	uint32_t size;
	Result res = MakeResult(UDV_GetUnitSize(_driveLetter, &size));
	if (_size != NULL)
		*_size = size;
	return res;
}

Result PhisonSDK_FF01::DumpDeviceInfo(const char _driveLetter, std::string *_msg)
{
	if (_msg == NULL)
		return MakeResult(PhisonCode_FF01::UDVErrorInputParameter);

	UDVDeviceInfo info;
	memset(&info, 0, sizeof(UDVDeviceInfo));
	info.dwDeviceInfoStructSize = sizeof(UDVDeviceInfo);

	Result res = MakeResult(UDV_GetDeviceInfo(_driveLetter, &info));
	if (res.IsFailCode())
		return res;

	uint32_t sectorSize = 512;

	std::stringstream ss;
	ss << "SerialNumber: " << info.a_bySerialNumber << ", RevisionInfo: " << info.a_byRevisionInfo << std::endl
	   << "Firmware version: " << info.a_byFirmwareVersion
	   << ", Firmware type: 0x" << std::uppercase << std::hex << info.wFirmwareType << std::endl
	   << "iControll_ID: " << std::hex << info.iController_ID << ", iMode: " << std::dec << info.iMode << std::endl
	   << std::endl
	   << "IsExistedPasswordPrivateArea: " << info.iIsExistedPasswordPrivateArea << std::endl
	   << "IsLoginPrivateArea: " << info.iIsLoginPrivateArea << std::endl
	   << "IsCDLocked: " << info.iIsCDLocked << std::endl
	   << "IsExistedPasswordNormalHidden: " << info.iIsExistedPasswordNormalHidden << std::endl
	   << "IsLoginNormalHiddenArea: " << info.iIsLoginNormalHiddenArea << std::endl
	   << "MaxNOA_PrivateArea: " << info.dwMaxNOA_PrivateArea << std::endl
	   << "NOA_PrivateArea: " << info.dwNOA_PrivateArea << std::endl
	   << "MaxNOA_NormalHiddenArea: " << info.dwMaxNOA_NormalHiddenArea << std::endl
	   << "NOA_NormalHiddenArea: " << info.dwNOA_NormalHiddenArea << std::endl
	   << std::endl
	   << "VID: " << info.a_byVID << ", VendorName: " << info.a_byVendorName << ", VendorInfo: " << info.a_byVendorInfo << std::endl
	   << "PID: " << info.a_byPID << ", ProductName: " << info.a_byProductName << ", ProductInfo: " << info.a_byProductInfo << std::endl
	   << "TMPS2_SYS size: " << static_cast<uint64_t>(info.dwPublicSize) * sectorSize << " bytes" << std::endl
	   << "TMPS2_DAT size: " << static_cast<uint64_t>(info.dwSecureSize) * sectorSize << " bytes" << std::endl
	   << "Hidden area size: " << static_cast<uint64_t>(info.dwNormalHiddenSize) * sectorSize << " bytes" << std::endl;

	*_msg = ss.str();

	return res;
}

Result PhisonSDK_FF01::GetWriteProtectStatus(const char _driveLetter, bool *_isEnable)
{
	unsigned char status; // (0: writable , 1 : write protected)
	Result res = MakeResult(UDV_GetWriteProtect(_driveLetter, &status));
	if (_isEnable != NULL)
		*_isEnable = (status == 1);
	return res;
}

Result PhisonSDK_FF01::SetWriteProtect(const char _driveLetter, const bool _isEnable, const bool _isTemp)
{
	unsigned char status = (_isEnable) ? 1 : 0; // (0: writable , 1 : write protected)

	Result res = MakeResult(0);
	if (_isTemp)
		res = MakeResult(UDV_SetWriteProtect_Temporarily(_driveLetter, status, 0));
	else
		res = MakeResult(UDV_SetWriteProtect(_driveLetter, status, 0));

	if (res.IsFailCode())
	{
		// try again with no lock volume
		if (_isTemp)
			res = MakeResult(UDV_SetWriteProtect_Temporarily(_driveLetter, status, 1));
		else
			res = MakeResult(UDV_SetWriteProtect(_driveLetter, status, 1));
	}

	return res;
}

Result PhisonSDK_FF01::ReadSector(const PartitionID_t _partID, const char _driveLetter,
                              const uint32_t _pageIndex, const uint32_t _pageCount,
                              const unsigned char* _buf)
{
	if (_partID == Firmware::PartitionID_HA)
		return MakeResult(UDV_ReadHiddenArea(_driveLetter, _pageIndex, _pageCount, const_cast<unsigned char*>(_buf)));
	else //if (_partID == Firmware::PartitionID_DATA || _partID == Firmware::PartitionID_SYS)
		return MakeResult(UDV_ReadSector_TargetPartition(_driveLetter, _pageIndex, _pageCount, const_cast<unsigned char*>(_buf)));
}

Result PhisonSDK_FF01::WriteSector(const PartitionID_t _partID, const char _driveLetter,
                               const uint32_t _pageIndex, const uint32_t _pageCount,
                               const unsigned char* _buf)
{
	if (_partID == Firmware::PartitionID_HA)
		return MakeResult(UDV_WriteHiddenArea(_driveLetter, _pageIndex, _pageCount, const_cast<unsigned char*>(_buf)));
	else //if (_partID == Firmware::PartitionID_DATA || _partID == Firmware::PartitionID_SYS)
		return MakeResult(UDV_WriteSector_into_WPPartition(_driveLetter, _pageIndex, _pageCount, const_cast<unsigned char*>(_buf)));
}

Result PhisonSDK_FF01::FormatPartition(const char _driveLetter, const char* _label)
{
	int fs_type = 1; // 1.FAT 2.ExFat
	return MakeResult(
		UDV_FormatPartition(_driveLetter, reinterpret_cast<const unsigned char*>(_label), fs_type)
	);
}

Result PhisonSDK_FF01::ResizePublicPartitions(const char _driveLetter, const uint32_t _size,
                                          const char* _labelSystem, const char* _labelData)
{
	int fs_type = 1; // 1.FAT 2.ExFat
	return MakeResult(
		UDV_ResizePublicPartition(_driveLetter, _size,
		                          reinterpret_cast<const unsigned char*>(_labelSystem),
		                          reinterpret_cast<const unsigned char*>(_labelData),
		                          fs_type)
	);
}

Result PhisonSDK_FF01::SetVID(const char _driveLetter, const char* _vid)
{
	return MakeResult(UDV_SetVID(_driveLetter, reinterpret_cast<const unsigned char*>(_vid)));
}

Result PhisonSDK_FF01::SetPID(const char _driveLetter, const char* _pid)
{
	return MakeResult(UDV_SetPID(_driveLetter, reinterpret_cast<const unsigned char*>(_pid)));
}

Result PhisonSDK_FF01::SetVendorName(const char _driveLetter, const std::string &_name)
{
	return MakeResult(UDV_SetVendorName(_driveLetter, reinterpret_cast<const unsigned char*>(_name.c_str()), static_cast<unsigned int>(_name.length())));
}

Result PhisonSDK_FF01::SetProductName(const char _driveLetter, const std::string &_name)
{
	return MakeResult(UDV_SetProductionName(_driveLetter, reinterpret_cast<const unsigned char*>(_name.c_str()), static_cast<unsigned int>(_name.length())));
}

Result PhisonSDK_FF01::GetLEDStatus(const char _driveLetter, LED_Status_t _status[3])
{
	if (_status == NULL)
		return MakeResult(PhisonCode_FF01::UDVErrorInputParameter);

	unsigned char green = 0, red = 0, blue = 0;
	Result res = MakeResult(UDV_Get_ExtraLED_Status(_driveLetter, &green , &red, &blue));
	if (! res.IsFailCode())
	{
		_status[0] = static_cast<LED_Status_t>(green);
		_status[1] = static_cast<LED_Status_t>(red);
		_status[2] = static_cast<LED_Status_t>(blue);
	}
	return res;
}

Result PhisonSDK_FF01::SetLEDStatus(const char _driveLetter, const LED_Index_t _index, const LED_Status_t _status)
{
	return MakeResult(UDV_Set_ExtraLED_Status(_driveLetter, _index, _status));
}


}} // namespace TMPS::Firmware
