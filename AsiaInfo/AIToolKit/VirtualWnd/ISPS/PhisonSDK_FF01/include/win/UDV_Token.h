#ifndef __UDV_TOKEN_H__
#define __UDV_TOKEN_H__

//#include "stdafx.h"
#include "UDV_DataType.h"
#ifdef _WINDOWS
typedef char TokenDriveIdentifier;
#endif



#pragma pack(1)
typedef struct UDVTokenListInfo
{
    unsigned int dwTokenListInfoStructSize;		// the Size of this struct
    TokenDriveIdentifier a_cDriveIdentifier[8];	// buffer to save null terminated drive identifier list
    unsigned int dwCountDriveIdentifier;	// buffer to save null terminated drive identifier list
    TokenDriveIdentifier cPublicAreaDriveIdentifier;	// drive identifier of the public area
    TokenDriveIdentifier cCDROMDriveIdentifier;	// drive identifier of the CD-ROM partition
    TokenDriveIdentifier cSecureAreaDriveIdentifier;	// drive identifier of the secure area
} UDVTokenListInfo;
#pragma pack()

#pragma pack(1)
typedef struct UDVDeviceInfo
{
    unsigned int dwDeviceInfoStructSize;				// the Size of this struct

    unsigned char a_byVID[4+1];									// vendor ID (2 characters in hexadecimal notation)
    unsigned char a_byPID[4+1];									// product ID (2 characters in hexadecimal notation)
    //2010.05.04 start for SN 50 bytes
    //	unsigned char a_bySerialNumber[24+1];				// serial number of 20 bytes in hexadecimal coding
    unsigned char a_bySerialNumber[128+1];				// serial number of 50 bytes in hexadecimal coding
    //2010.05.04 end
    unsigned char a_byVendorName[24+1];					// USB vendor name string (24 characters)
    unsigned char a_byProductName[24+1];				// USB product name string (24 characters)
    //2011.03.02 start
    unsigned char a_byVendorInfo[8+1];					// Inquiry vendor information string (8 characters)
    unsigned char a_byProductInfo[16+1];				// Inquiry product information string (16 characters)
    unsigned char a_byRevisionInfo[4+1];				// Inquiry revision information string (4 characters)
    //2011.03.02 end
    unsigned char a_byFirmwareVersion[10+1];			// firmware version string (10 characters)

    unsigned int dwPublicSize;									// the size of the public area in blocks of 512 Bytes
    unsigned int dwCDROMSize;									// the size of the CD-ROM in sectors of 512 Bytes
    unsigned int dwSecureSize;									// the size of the secure partition in blocks of 512 Bytes
    unsigned int dwNormalHiddenSize;						// the size of the normal hidden area in blocks of 512 bytes
    unsigned int dwProtectedHiddenSize;				// the size of the protected hidden area in blocks of 512 bytes

    int iMode;																	// Mode of device configuration
    int iController_ID;													// Current ID of Controller

    int iIsExistedPasswordPrivateArea;					// Protected status of private area (1 : Password existed, 0: No password )
    int iIsLoginPrivateArea;										// Log-in status for private area (1 : login, 0: logout )
    unsigned int dwMaxNOA_PrivateArea;					// Maximal Number Of unsuccessful Attempts to log in into the private area
    unsigned int dwNOA_PrivateArea;						// Current Number Of unsuccessful Attempts to log in into the private area

    int iIsCDLocked;														// CD locked status

    int iIsExistedPasswordNormalHidden;				// Protected status of normal hidden area (1 : Password existed, 0: No password )
    int iIsLoginNormalHiddenArea;							// Log-in status for normal hidden area (1 : login, 0: logout )
    unsigned int dwMaxNOA_NormalHiddenArea;		// Maximal Number Of unsuccessful Attempts to log in into the normal hidden area
    unsigned int dwNOA_NormalHiddenArea;				// Current Number Of unsuccessful Attempts to log in into the normal hidden area

    //2011.03.02 start
    unsigned short wFirmwareType;					// Firmware Type(FF01, FF03...)
    //2011.03.02 end

#ifdef _2PASSWORD_
    int iMemberCount_PrivateArea;								//Current Number of member user for private area
    int iMemberCount_HiddenArea;								//Current Number of member user for hidden area
#endif

} UDVDeviceInfo;

#pragma pack()

#endif

