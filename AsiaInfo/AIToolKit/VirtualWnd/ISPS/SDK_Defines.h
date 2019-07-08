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

#include <vector>
#include <string>


namespace TMPS { namespace Firmware {


struct DriveLetters_t
{
	char sysLetter;
	char dataLetter;

	DriveLetters_t() : sysLetter(0), dataLetter(0) {}
};
typedef std::vector<DriveLetters_t> DriveLettersList_t;

enum DeviceVersion_t
{
	DeviceUnknown = 0,
	DeviceVer1x = 1,
	DeviceVer2x = 2
};

struct DeviceInfo_t
{
	std::string serial;
	std::string VID;
	std::string PID;

	std::string firmwareVer;
	DeviceVersion_t deviceVersion;

	size_t hiddenSectors; // PartitionID_HA
	size_t dataSectors;   // PartitionID_DATA ("TMPS2 DAT")
	size_t sysSectors;    // PartitionID_SYS  ("TMPS2 SYS")
};

enum PartitionID_t
{
	// IMPORTANT: keep the sequence, and don't manually set value
	PartitionID_HA,
	PartitionID_DATA,
	PartitionID_SYS,
	PartiiionID_INVALID
};

namespace LED_Defines
{
	enum LED_Index_t
	{
		LED_UnknownIndex = -1,
		LED_All = 0,
		LED_Yellow = 1,
		LED_Red = 2,
		LED_Blue = 3
	};

	enum LED_Status_t
	{
		LED_UnknownStatus = -1,
		LED_Off = 0,
		LED_On = 1,
		LED_Blink = 2,
		LED_MarqueeStatus = 99
	};
}
using namespace LED_Defines;

}} // namespace TMPS::Firmware