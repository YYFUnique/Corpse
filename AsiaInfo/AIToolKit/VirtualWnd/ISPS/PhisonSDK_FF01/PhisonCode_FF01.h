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

#include "smart_enum.h"

SMART_ENUM(PhisonCode_FF01,
	UDVReturnOK = 0,
	UDVError = 1,
	UDVErrorcDriveLetterNotFound = 19,
	UDVErrorInputParameter = 20,
	UDVErrorGetDeviceInfo = 21,
	UDVErrorGetDeviceString = 22,
	UDVErrorGetCapacity = 23,
	UDVErrorNoHidden = 24,
	UDVErrorIndexOutOfCompass = 25,
	UDVErrorCountOutOfCompass = 26,
	UDVErrorAccessingOutOfCompass = 27,
	UDVErrorReadHiddenArea = 28,
	UDVErrorWriteDataToBuff = 29,
	UDVErrorIncorrectHiddenAreaPassword = 30,
	UDVErrorHiddenNOA_OutOfMAX = 31,
	UDVErrorWriteHiddenArea = 32,
	UDVErrorReadDataFromBuff = 33,
	UDVErrorDevice = 34,
	UDVErrorCDAlreadyLocked = 35,
	UDVErrorCDLock = 36,
	UDVErrorVerifyCDKey = 37,
	UDVErrorSecuritySessionNoOpened = 38,
	UDVErrorCDUnlock = 39,
	UDVErrorOpenSecuritySession = 40,
	UDVErrorCDNoLocked = 41,
	UDVErrorTerminateSecureSession = 42,
	UDVErrorOutOfCompassAccessCD = 43,
	UDVErrorWriteCDBuffer = 44,
	UDVErrorReadCDBuffer = 45,
	UDVErrorReadBuffer = 46,
	UDVErrorWriteBuffer = 47,
	UDVErrorPasswordExisted = 49,
	UDVErrorDeviceBeSeized = 50,
	UDVErrorLogout = 51,
	UDVErrorSetPassword = 52,
	UDVErrorPasswordNoExisted = 53,
	UDVErrorAlreadyLogin = 54,
	UDVErrorAlreadyLogout = 55,
	UDVErrorWrongPassword = 56,
	UDVErrorExceededMaxNOA = 57,
	UDVErrorLogin = 58,
	UDVErrorChangePassword = 59,
	UDVErrorUnblockPassword = 60,
	UDVErrorEncryptBuffer = 61,
	UDVErrorDecryptBuffer = 62,
	UDVErrorLoginDeviceSecureMechanism = 70,
	UDVErrorLogoutDeviceSecureMechanism = 71,
	UDVErrorPasswordNoExisted_for_DeviceSecureMechanism = 72,
	UDVErrorWrongPassword_for_DeviceSecureMechanism = 73,
	UDVErrorVeerSecuritySessionType = 74,
	UDVErrorNoInitSDK = 75,
	UDVErrorAlreadyInitSDK = 76,
	UDVErrorAlreadyCloseSDK = 77,
	UDVErrorSetDevicePartition = 78,
	UDVErrorWrong_SDK_Password = 79,
	UDVErrorWrong_HostPublicKey = 80,
	UDVErrorErasePartitionData = 81,
	UDVErrorNoKeyFromSmartCard = 82,
	UDVErrorSetDeviceInfo = 83,
	UDVErrorPartitionIsWriteProtected = 84,
	UDVErrorHardwareProblem = 85,
	UDVImageExistsInDeviceCD = 86,
	UDVErrorOpenFile = 87,
	UDVInvalidFileSize = 88,
	UDVErrorReadFile = 89,
	UDVErrorWriteFile = 90,
	UDVErrorNoMBR = 91,
	UDVErrorNoFAT = 92,
	UDVError_DeviceUnready = 93,
	UDVError_FunctionNotSupported = 100,
	UDVError_VolumeUnready = 101,
	UDVErrorUser = 102
)
#undef SMART_ENUM
