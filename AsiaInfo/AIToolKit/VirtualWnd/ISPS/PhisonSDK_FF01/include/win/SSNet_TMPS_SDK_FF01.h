/****************************************************************************
VERSION		"15.06.16.01"
2015/06/16 for SSNet_TMPS_SDK_FF01

Changed List:
2015/06/16 :
1. Modify "CreateThread" to "::CreateEvent" in the UDV_CTimer::UDV_CTimer().
2. Remove "WaitForSingleObject(m_hThread, 500);" in the UDV_CTimer::~UDV_CTimer().
*****************************************************************************/

#ifndef __SSNET_TMPS_SDK_FF01_H__
#define __SSNET_TMPS_SDK_FF01_H__


#include "UDV_DataType.h"
#include "UDV_Token.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
get information form the DLL
input/Output :
    	buff	 = get information form the DLL by buffer. ( ex: "0x20070101") ( 4Bytes )

return : 0, OK

*****************************************************************************/
int GetDllInfo(char *buff);


/****************************************************************************
Purpose		:
This function will initialize the API return SDK version in parameter and protect the SDK which maybe be misappropriated by someone.
This function must be implemented successfully before others are implemented.

Parameter	:

Name				Type					Direction		Purpose
-----------------------------------------------------------------------------
dwSDK_Version		unsigned int*			output			returned SDK version
p_bySDK_PWS			const unsigned char*	input			Buffer address of SDK password (64bytes), it can't be NULL
iLengthSDK_PWS		int						input			Length of SDK password, it should be 64.

Return		:
				0, OK
				Refer to return code table.

Note		:
				1.	The SDK password is an essential parameter.
				2.	After implementing UDV_InitAPI, UDV_ListTokens must to be implemented first.
****************************************************************************/
int UDV_InitAPI(unsigned int* dwSDK_Vversion, const unsigned char* p_bySDK_PWS, int iLengthSDK_PWS);


/****************************************************************************
Purpose		:
This function shuts the API down and frees memory.

Parameter	:

Return		:
				0, OK
				Refer to return code table.

Note		:
				1.	After implementing UDV_CloseAPI, other functions are not allowed to implement except UDV_InitAPI; If SDK want to be re-initialized, UDV_InitAPI and UDV_ListTokens must to be implemented according to priority.

*****************************************************************************/
int UDV_CloseAPI();


/****************************************************************************
Purpose		:
This function will check the device whether it need to switch to composite mode(HID mode) or not.

Parameter	:

Name					Type			Direction		Purpose
-----------------------------------------------------------------------------
p_iCheckResult			int*			output			The checked result of the function.
    													true	: switch to composite mode(HID mode) imperatively
    													false	: Don't need to switch to composite mode(HID mode)

Return		:
    			0, OK
    			Refer to return code table.

Note		:
				This function always returns false on Windows Vista and Windows 7 whatever the privilege is.
*****************************************************************************/
int UDV_isHID(int* p_iCheckedResult );


/****************************************************************************
Purpose		¡G
Because the compute must to change the device to composite mode(HID mode) in limited user privilege,
where the compute needs some time to finish in this step,
so this function will check the compute whether it is ready for HID channel or not.

Parameter	¡G

Name					Type			Direction		Purpose
-----------------------------------------------------------------------------
p_iCheckResult			int*			output			The checked result of the function.
    													true	: ready
    													false	: no ready


Return		¡G
    			0, OK
    			Refer to return code table.

Note :
				1.	Please implement UDV_ListTokens and check the compute whether it is ready for HID mode again when you unplug and plug the device in the limited user privilege.
				2.	This function is only enabled with that isHID function has been implemented. 
				3.	This function always return "false" in the administrator privilege.
				4.	SDK supports only one device for HID mode.
				5.	Please don't use sleep( ) function for delay. sleep() function can interfere that OS re-identifies the token device.

				[This function is a dummy in this specification.]


For example to use as below:

 char drives[UDVMaxTokenNumber][UDVMaxPartitionNumber];
 memset(drives,0,sizeof(drives));

 if((dwResult= UDV_ListTokens(drives))==0) //change to HID mode
 {
  printf("Find device ERROR!! \n");
  return dwResult;
 }

 if (isHID())
 {
  do
  {
   // wait some time
  } while(!isReady_HIDmode());
 }

//You can implement other functions.
 DriveInfo pDriveInfoList[8];
 unsigned int dwListSize = 8;
 memset(&pDriveInfoList, 0, sizeof(pDriveInfoList));
 if((dwResult= UDV_GetDeviceInfo(pDriveInfoList,&dwListSize))!=0)
 {
  printf("GDGetDeviceInfo ERROR!! \n");
  return dwResult;
 }

*****************************************************************************/
int UDV_isReady_HIDmode(int* p_iCheckedResult);


/****************************************************************************
Purpose		:
This function gets the drive letters of recognized devices and switch device to HID mode in limited user privilege.

Parameter	:

Name					Type				Direction		Purpose
-----------------------------------------------------------------------------
pTokenListInfo			UDVTokenListInfo*	output			list of token information
pdwListSize				unsigned int*		input/output	The count of device information allocated or holds the actual number of list member.
p_dwVID_Filter			unsigned int*		input			Buffer address of VID filter. It is optional.
p_dwPID_Filter			unsigned int*		input			Buffer address of PID filter. It is optional.
dwLenVID_PID_Filter		unsigned int		input			Length of VID and PID filter (Max : 256)



Return		:
				0, OK
				Refer to return code table.

Note		:
				1. Please don't use sleep( ) function for delay. sleep() function can interfere that OS re-identifies the token device.
				2. The filter of PID and VID are optional. You can set the length of VID and PID filter to zero to ignore them.
				3. In order to avoid mistaking for that SDK finds device with HID mode, please set the VID and PID filter for your produce.
				4. dwTokenListInfoStructSize of parameter structure must be initialized first before UDV_ListTokens is implemented.


 UDVMaxTokenNumber  = 16;
 UDVMaxPartitionNumber = 8;

Exp			:

unsigned int adwVID_Filter[] = {0x13FE, 0x13FE};
unsigned int adwPID_Filter[] = {0x1E27, 0x3327};
unsigned int dwLenVID_PID_Filter = 2;

unsigned int pdwTokenListInfoSize = UDVMaxTokenNumber;
UDVTokenListInfo pTokenListInfo[UDVMaxTokenNumber];
memset(&pTokenListInfo, 0, sizeof(pTokenListInfo));

for (i =0; i< UDVMaxTokenNumber; i++)
 pTokenListInfo[i].dwTokenListInfoStructSize = sizeof(UDVTokenListInfo);

if((dwResult= UDV_ListTokens(pTokenListInfo, &pdwTokenListInfoSize, adwVID_Filter, adwPID_Filter, dwLenVID_PID_Filter))!=0)
{
 printf("Find device ERROR!! \n");
 return dwResult;
}

*****************************************************************************/
int
UDV_ListTokens (
    UDVTokenListInfo* pTokenListInfo, unsigned int* pdwListSize
    ,const unsigned int *p_dwVID_Filter,const unsigned int *p_dwPID_Filter, unsigned int dwLenVID_PID_Filter
);



/****************************************************************************
Purpose		:
The function gets the information of recognized devices.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
pDriveInfoList			UDVDeviceInfo*			output			list of device information


Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1.	dwDeviceInfoStructSize of parameter structure must be initialized first before UDV_GetDeviceInfo is implemented.

*****************************************************************************/
int
UDV_GetDeviceInfo (
    TokenDriveIdentifier	tokenDriveIdentifier
    ,UDVDeviceInfo		*pDeviceInfoList
);



/****************************************************************************
Purpose		:
The function reads data from the normal hidden area.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
dwPageIndex				unsigned int			input			Page index of hidden area will be read and it can't be zero.
dwPageCount				unsigned int			input			Page count of the normal hidden area will be read and it can't be zero.
p_byBuf					unsigned char*			output			Buffer address to receive data and it can't be NULL.

Return		:
    			0, OK
    			Refer to return code table.

Note		:	
				1.	Unit:
						1 Page	= 	1	sector		= 	512	bytes
				2.	Please check size of normal hidden first, and it can't be zero.
*****************************************************************************/
int
UDV_ReadHiddenArea(
    TokenDriveIdentifier	tokenDriveIdentifier
    , unsigned int			dwPageIndex
    , unsigned int			dwPageCount
    , unsigned char*		p_byBuf
);



/****************************************************************************
Purpose		:
The function writes data into the normal hidden area.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
dwPageIndex				unsigned int			input			Page index of the normal hidden area will be written and it can't be zero.
dwPageCount				unsigned int			input			Page count of the normal hidden area will be written and it can't be zero.
p_byBuf					unsigned char*			input			Buffer address to data and it can't be NULL.

Return		:
				0, OK
				Refer to return code table.

Note		:
				1.	Unit:
						1 Page	= 	1	sector		= 	512	bytes
				2.	Please check size of normal hidden first, and it can't be zero.
*****************************************************************************/
int
UDV_WriteHiddenArea(
    TokenDriveIdentifier	tokenDriveIdentifier
    , unsigned int			dwPageIndex
    , unsigned int			dwPageCount
    , unsigned char*		p_byBuf
);


/****************************************************************************
Purpose		:
The function writes data into CDROM partition.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
dwBlockIndex			unsigned int			input			Block index of CD area will be written and it can't be zero.
dwBlockCount			unsigned int			input			Block count of CD area will be written and it can't be zero.
p_byBlockData			unsigned char*			input			Buffer address to data and it can't be NULL.


Return		:
				0, OK
				Refer to return code table.

Note		:	
				1.	Unit:
						1 Block	= 	4	sector	s	= 	4096	bytes
				2.	The maximal block count is 32 because of limited size of access (64K bytes).
				3.	Please terminate the running application program which is implemented from CD partition.

*****************************************************************************/
int
UDV_WriteBlockCDROM (
    TokenDriveIdentifier	tokenDriveIdentifier
    , unsigned int			dwBlockIndex
    , unsigned int			dwBlockCount
    , unsigned char*		p_byBlockData
);


/****************************************************************************
Purpose		:
The function reads data from CDROM partition.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
dwBlockIndex			unsigned int			input			Block index of CD area will be read and it can't be zero.
dwBlockCount			unsigned int			input			Block count of CD area will be read and it can't be zero.
p_byBlockData			unsigned char*			output			Buffer address to receive data and it can't be NULL.

Return		:
    			0, OK
    			Refer to return code table.

Note		:	
				1.	Unit:
						1 Block	= 	4	sector	s	= 	4096	bytes
				2.	The maximal block count is 32 because of limited size of access (64K bytes).
*****************************************************************************/
int UDV_ReadBlockCDROM(
    TokenDriveIdentifier	tokenDriveIdentifier
    , unsigned int			dwBlockIndex
    , unsigned int			dwBlockCount
    , unsigned char*		p_byBlockData
);


/****************************************************************************
Purpose		:
This function instructs the device to set a Ready-Not-Ready pulse.
When the device gets this command, it enters Not-Ready state.
This action causes the host to reread all disk structures, which is useful if media changes have occurred.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token

Return		:
				0, OK
				Refer to return code table.

Note		:
				Please implement this function after the CD partition had been update finally.
*****************************************************************************/
int UDV_SetDevice_MediaChanged(TokenDriveIdentifier	tokenDriveIdentifier);


/****************************************************************************
Purpose		:
This function sets the password for specified partition.

Parameter	:

Name								Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier				TokenDriveIdentifier	input			Specify the drive identifier of token
p_byNewPassword						const unsigned char*	input			Buffer address of new password (ASCII Code 0x21-0x7E, Max : 16bytes), it can't be NULL.
dwLengthNewPassword					unsigned int			input			Length of new password , 0 < dwLengthNewPassword <= 16
byFlag_FormatPartition				unsigned char			input			Flag of formating secure partition. In other words, this flag decides to erase partition or not.
p_bySecurityPartitionVolumeLabel	const unsigned char*	input			set VolumeLabel, null Terminated CString, ASCII or UTF8 encoding
fileSystemType						unsigned int			input			File system type(0:Don't build 1.FAT 2.ExFat)

Return		:
    			0, OK
    			Refer to return code table.

Note		:	
				Each byte of password must be ASCII Code(0x21-0x7E).
*****************************************************************************/
int
UDV_SetPassword (
    TokenDriveIdentifier	tokenDriveIdentifier
    , const unsigned char*	p_byNewPassword
    , unsigned int			dwLengthNewPassword
    , unsigned char			byFlag_FormatPartition
    , const unsigned char	*p_bySecurityPartitionVolumeLabel
    , UDVFileSystemType		fileSystemType
);

/****************************************************************************
Purpose		:
This function is unlocked the security area in device with password for it. If the specified password is correct the security area will be displayed (Removable area can be used).

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byOldPassword			const unsigned char*	input			Buffer address of old password (ASCII Code 0x21-0x7E, Max : 16bytes), it can't be NULL.
dwLengthOldPassword		unsigned int			input			Length of old password, 0 < dwLengthOldPassword <= 16


Return		:
    			0, OK
    			Refer to return code table.

Note		:	
				Each byte of password must be ASCII Code(0x21-0x7E).
*****************************************************************************/
int
UDV_Login(
    TokenDriveIdentifier	tokenDriveIdentifier
    , const unsigned char*	p_byOldPassword
    , unsigned int			dwLengthOldPassword
);


/****************************************************************************
Purpose		¡G
This function gets the flag that to check Secure Disk whether support Logout without entering the password.

Parameter	¡G

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byFlag_NPW			unsigned char*			output			Support Logout without entering the password
																0: No support
																1: Support


Return		¡G
				0, OK
				Refer to return code table.
*****************************************************************************/
int UDV_Support_Logout_NonPW( TokenDriveIdentifier	tokenDriveIdentifier, unsigned char* p_byFlag_NPW);


/****************************************************************************
Purpose		¡G
This function is locked the security area in device (Removable area can not be used).

Parameter	¡G

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byOldPassword			const unsigned char*	input			Buffer address of old password (ASCII Code 0x21-0x7E, Max : 16bytes), it can't be NULL.
dwLengthOldPassword		unsigned int			input			Length of old password, 0 < dwLengthOldPassword <= 16
byForce					unsigned char			input			flag of setting for that logout device forcibly(
    															0: logout safely
    															1: logout forcibly
    															)

Return		¡G
    			0, OK
    			Refer to return code table.

Note		:
				1.	Logout safely: SDK locks volume of partition first to check the partition has been used by another program or not.
				2.	Logout forcibly: SDK ask device to logout forcibly whether the partition has been used by another program or not. The data may be lost at this moment in time.
				3.	Each byte of password must be ASCII Code(0x21-0x7E).
				4.	UDV_Support_Logout_NonPW could check the device whether support Logout without entering the password
*****************************************************************************/
int
UDV_Logout(
    TokenDriveIdentifier	tokenDriveIdentifier
    , const unsigned char*	p_byOldPassword
    , unsigned int			dwLengthOldPassword
    , unsigned char			byForce
);


/****************************************************************************
Purpose		¡G
This function changes the password for the access security area from p_byOldPassword to p_byNewPassword.

Parameter	¡G

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byOldPassword			const unsigned char*	input			Buffer address of old password (ASCII Code 0x21-0x7E, Max : 16bytes), it can't be NULL.
dwLengthOldPassword		unsigned int			input			Length of old password , 0 < dwLengthOldPassword <= 16
p_byNewPassword			const unsigned char*	input			Buffer address of new password (ASCII Code 0x21-0x7E, Max : 16bytes), it can't be NULL.
dwLengthNewPassword		unsigned int			input			Length of new password , 0 < dwLengthOldPassword <= 16

Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1.	Each byte of password must be ASCII Code(0x21-0x7E).
				2.	After this function is implemented successfully, the status of device is invariable.
*****************************************************************************/
int
UDV_ChangePassword(
    TokenDriveIdentifier	tokenDriveIdentifier
    , const unsigned char*	p_byOldPassword
    , unsigned int			dwLengthOldPassword
    , const unsigned char*	p_byNewPassword
    , unsigned int			dwLengthNewPassword
);


/****************************************************************************
Purpose		:
This function disables the password for access security area by old password and keeps the data of partition.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byOldPassword			const unsigned char*	input			Buffer address of old password (ASCII Code 0x21-0x7E, Max : 16bytes), it can't be NULL.
dwLengthOldPassword		unsigned int			input			Length of old password , 0 < dwLengthOldPassword <= 16

Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1.	Each byte of password must be ASCII Code(0x21-0x7E).
				2.	After this function is implemented successfully, the data of partition is invariable.

*****************************************************************************/
int
UDV_DisablePassword(
    TokenDriveIdentifier	tokenDriveIdentifier
    , const unsigned char*	p_byOldPassword
    , unsigned int			dwLengthOldPassword
);


/****************************************************************************
Purpose		:
This function clears or sets a new password forcibly for partition with clearing partition.

Parameter	:

Name								Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier				TokenDriveIdentifier	input			Specify the drive identifier of token
p_bySDK_PWS							const unsigned char*	input			Buffer address of SDK password (64bytes)
iLengthSDK_PWS						int						input			Length of SDK password, it should be 64.
p_bySecurityPartitionVolumeLabel	const unsigned char*	input			set VolumeLabel, null Terminated CString, ASCII or UTF8 encoding
fileSystemType						unsigned int			input			File system type(0:Don't build 1.FAT 2.ExFat)


Return		:
    			0, OK
    			Refer to return code table.

Note		:	
				1.	The SDK password is an essential parameter. 
				2.	After this function is implemented successfully with clearing password, the status of device is login.

*****************************************************************************/
int
UDV_ResetPassword(
    TokenDriveIdentifier		tokenDriveIdentifier
    , const unsigned char*		p_bySDK_PWS
    , int						iLengthSDK_PWS
    , const unsigned char*		p_bySecurityPartitionVolumeLabel	// null Terminated CString, ASCII or UTF8 encoding
    , UDVFileSystemType			fileSystemType
);


/****************************************************************************
Purpose		:
This function sets CD-ROM size in 512-byte sector count.

Parameter	:

Name								Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier				TokenDriveIdentifier	input			Specify the drive identifier of token
dwCDpartitionSize					unsigned int			input			CD-ROM size in 512-byte sector count.
p_bySecurityPartitionVolumeLabel	const unsigned char*	input			set VolumeLabel, null Terminated CString, ASCII or UTF8 encoding
fileSystemType						unsigned int			input			File system type(0:Don't build 1.FAT 2.ExFat)

Return		:
    			0, OK
    			Refer to return code table.

Note		:	
				1.	Unit:	
						1 sector	= 	512	bytes
				2.	We recommend that input CD size is adjusted with the flash unit size for the data completeness and performance, where you can get the flash unit size by UDV_GetUnitSize function.
				3.	The device must to be unlock before this function is implemented:
					3.1.	The removable partition must be unlocked.
					3.2.	The removable partition must be writable.
				4.	Please terminate the running application program which is implemented from CD partition.
				5.	Please unplug and replug the device to enable the setting value.

Exp			:	
set minimum CD partition size for the device:
get UnitSize by UDV_GetUnitSize function.
UDV_ResizeCDPartition( tokenDriveIdentifier, UnitSize, p_bySecurityPartitionVolumeLabel, fileSystemType );

set minimum secure partition size for the device:
get UnitSize by UDV_GetUnitSize function.
get original_public_size and original_secure_size by UDV_GetDeviceInfo function.
UDV_ResizeCDPartition( tokenDriveIdentifier, (original_public_size + original_secure_size - UnitSize), p_bySecurityPartitionVolumeLabel, fileSystemType );
*****************************************************************************/
int
UDV_ResizeCDPartition (
    TokenDriveIdentifier		tokenDriveIdentifier
    , unsigned int				dwCDpartitionSize
    , const unsigned char*		p_bySecurityPartitionVolumeLabel	// null Terminated CString, ASCII or UTF8 encoding
    , UDVFileSystemType			fileSystemType
);


/****************************************************************************
Purpose		:
This function sets public partition size in 512-byte sector count.

Parameter	:

Name								Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier				TokenDriveIdentifier	input			Specify the drive identifier of token
dwpartitionSize						unsigned int			input			public partition size in 512-byte sector count.
p_byPublicPartitionVolumeLabel		const unsigned char*	input			Set PublicPartition VolumeLabel, null Terminated CString, ASCII or UTF8 encoding
p_bySecurityPartitionVolumeLabel	const unsigned char*	input			Set SecurityPartition VolumeLabel, null Terminated CString, ASCII or UTF8 encoding
fileSystemType						unsigned int			input			File system type(0:Don't build 1.FAT 2.ExFat)

Return		:
    			0, OK
    			Refer to return code table.


Note		:
			1.	Unit:		1 sector	= 	512	bytes
			2.	We recommend that input public size is adjusted with the flash unit size for the data completeness and performance, where you can get the flash unit size by UDV_GetUnitSize function.
			3.	The device must to be unlock before this function is implemented:
				3.1	The removable partition must be unlocked.
				3.2	The removable partition must be writable.
			4.	This function only supports the device with mode 7 and mode 8.
			5.	The partition size must be more than one unit size for secure or public.
			6.	Please unplug and replug the device to enable the setting value.
			7.	Please terminate the running application program which is implemented from the device.
*****************************************************************************/
int
UDV_ResizePublicPartition (
    TokenDriveIdentifier	tokenDriveIdentifier
    , unsigned int			dwPartitionSize
    , const unsigned char*	p_byPublicPartitionVolumeLabel
    , const unsigned char*	p_bySecurityPartitionVolumeLabel
    , UDVFileSystemType		fileSystemType

);


/****************************************************************************
Purpose		:
This function gets the status from the specified partition.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byStatus				unsigned char*			output			Returned the status of partition. Don't input a constant.
p_dwNOA					unsigned int*			output			Returned the current number of retry password. Don't input a constant.
p_dwMAXNOA				unsigned int*			output			Returned the maximum number of retry password. Don't input a constant.


Return		:
					0, OK
					Refer to return code table.


Note		:
					The status is shown as below:

					byStatus	Signify
					------------------------------------------------
					0			status of partition is logout
					1			status of partition is login
					2			status of "No Password" but login

*****************************************************************************/
int
UDV_GetStatus(
    TokenDriveIdentifier	tokenDriveIdentifier
    , unsigned char*		p_byStatus
    , unsigned int*			p_dwNOA
    , unsigned int*			p_dwMAXNOA
);



/****************************************************************************
Purpose		:
The function sets device to write protect for the specified driver letter.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
byStatus_WriteProtect	unsigned char			input			Specify Protected type (0: writable , 1 : write protected)
byForce					unsigned char			input			flag of setting for that setting write-protected forcibly(
    															0: safely(lock volume first)
    															1: forcibly(non-locking volume)
    															)
byForce_MediaChanged	unsigned char			input			flag of making the firmware set media changed by himself(
																0: don't do this. (make the firmware set media changed by himself.)
																1: forcibly.	
    															)

Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1. Please set byForce_MediaChanged to 1 with [Win7] and [Win VISTA] at our suggestion.
				2. The specified partition will be remounted when the byForce_MediaChanged is 1.
				3. The all partitions of device will be remounted with Mode 7 when the byForce_MediaChanged is 1.
*****************************************************************************/
#ifdef __cplusplus
int UDV_SetWriteProtect(TokenDriveIdentifier	tokenDriveIdentifier,unsigned char byStatus_WriteProtect, unsigned char byForce, unsigned char byForce_MediaChanged = 1);
#else
int UDV_SetWriteProtect(TokenDriveIdentifier	tokenDriveIdentifier,unsigned char byStatus_WriteProtect, unsigned char byForce, unsigned char byForce_MediaChanged);
#endif


/****************************************************************************
Purpose		:
The function get protected status of device for the specified driver letter.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byStatus_WriteProtect	*unsigned char			output			Specify Protected type (0: writable , 1 : write protected)

Return		:
    			0, OK
    			Refer to return code table.
*****************************************************************************/
int UDV_GetWriteProtect(TokenDriveIdentifier tokenDriveIdentifier, unsigned char* p_byStatus_WriteProtect);


/****************************************************************************
Purpose		:
The function sets device to be write-protected temporarily for the specified driver letter.

Parameter	:

Name					Type			Direction		Purpose
-----------------------------------------------------------------------------
cDriveLetter			char			input			Specify the driver letter of device
byStatus_WriteProtect	unsigned char	input			Specify Protected type (0: writable , 1 : write protected)
byForce					unsigned char	input			flag of setting for that setting write-protected forcibly
														(
    													0: safely(lock volume first)
    													1: forcibly(non-locking volume)
    													)
byForce_MediaChanged	unsigned char	input			flag of making the firmware set media changed by himself(
														0: don't do this. (make the firmware set media changed by himself.)
														1: forcibly.	
														)
Return		:
				0, OK
				Refer to return code table.

Note		:
				1. When the device that has been set by UDV_SetWriteProtect_Temporarily is unplugged and plugged, the status of write-protected will be recovered.
				2. Please set byForce_MediaChanged to 1 with [Win7] and [Win VISTA] at our suggestion.
				3. The specified partition will be remounted when the byForce_MediaChanged is 1.
				4. The all partitions of device will be remounted with Mode 7 when the byForce_MediaChanged is 1.
*****************************************************************************/
#ifdef __cplusplus
int UDV_SetWriteProtect_Temporarily(TokenDriveIdentifier tokenDriveIdentifier, unsigned char byStatus_WriteProtect, unsigned char byForce, unsigned char byForce_MediaChanged = 1);
#else
int UDV_SetWriteProtect_Temporarily(TokenDriveIdentifier tokenDriveIdentifier, unsigned char byStatus_WriteProtect, unsigned char byForce, unsigned char byForce_MediaChanged);
#endif


/****************************************************************************
Purpose		:
This function sets the PID into device.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byPID					const unsigned char*	input			Buffer address of PID (Max : 4bytes)


Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1. Please unplug and replug the device to enable setting value.
				2. SDK can't set the specified PID for the device when that VID is 0x13FE.
*****************************************************************************/
int UDV_SetPID(TokenDriveIdentifier	tokenDriveIdentifier, const unsigned char* p_byPID);


/****************************************************************************
Purpose		:
This function sets the VID into device.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byVID					const unsigned char*	input			Buffer address of VID (Max : 4bytes)


Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1. Please unplug and replug the device to enable setting value.
				2.SDK sets default PID automatically for that VID is 0x13FE.
*****************************************************************************/
int UDV_SetVID(TokenDriveIdentifier	tokenDriveIdentifier, const unsigned char* p_byVID);


/****************************************************************************
Purpose		:
This function sets the vendor name into device.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byVendorName			const unsigned char*	input			Buffer address of vendor name (Max : 8bytes)
dwLength_VendorName		unsigned int			input			Length of vendor name


Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1. Please unplug and replug the device to enable setting value.
*****************************************************************************/
int UDV_SetVendorName(TokenDriveIdentifier	tokenDriveIdentifier, const unsigned char* p_byVendorName, unsigned int dwLength_VendorName);


/****************************************************************************
Purpose		:
This function sets the production name into device.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byProductionName		const unsigned char*	input			Buffer address of production name (Max : 16bytes)
dwLength_ProductionName	unsigned int			input			Length of production name


Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1. Please unplug and replug the device to enable setting value.
*****************************************************************************/
int UDV_SetProductionName(TokenDriveIdentifier	tokenDriveIdentifier, const unsigned char* p_byProductionName, unsigned int dwLength_ProductionName);


/****************************************************************************
Purpose		:
This function sets the inquiry reversion into device.

Parameter	:

Name						Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier		TokenDriveIdentifier	input			Specify the drive identifier of token
p_byInquiryReversion		const unsigned char*	input			Buffer address of inquiry reversion (Max : 4bytes)
iLength_InquiryReversion	int						input			Length of inquiry reversion


Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1. Please unplug and replug the device to enable setting value.
*****************************************************************************/
int UDV_SetInquiryReversion(TokenDriveIdentifier	tokenDriveIdentifier, const unsigned char* p_byInquiryReversion, int iLength_InquiryReversion);


/****************************************************************************
Purpose		:
This function gets the inquiry reversion from device.

Parameter	:

Name						Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier		TokenDriveIdentifier	input			Specify the drive identifier of token
p_byInquiryReversion		unsigned char*			output			Buffer address of inquiry reversion (4bytes)
iLength_InquiryReversion	int						input			Length of inquiry reversion


Return		:
    			0, OK
    			Refer to return code table.

*****************************************************************************/
int UDV_GetInquiryReversion(TokenDriveIdentifier	tokenDriveIdentifier, unsigned char* p_byInquiryReversion, int iLength_InquiryReversion);


/****************************************************************************
Purpose		:
This function formats partition for specific tokenDriveIdentifier.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_byVolumeLabel			const unsigned char*	input			Buffer address of volume label, null Terminated CString, ASCII or UTF8 encoding
fileSystemType			unsigned int			input			File system type(1.FAT 2.ExFat)

Return		:
    			0, OK
    			Refer to return code table.

*****************************************************************************/
int
UDV_FormatPartition (
    TokenDriveIdentifier	tokenDriveIdentifier
    ,const unsigned char* p_byVolumeLabel,UDVFileSystemType	fileSystemType

);


/****************************************************************************
Purpose		:
This function gets flash unit size of the specified partition.

Parameter	:

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_dwUnitSize			unsigned int*			output			Partition unit size in 512-byte sector count.

Return		:
    			0, OK
    			Refer to return code table.

Note		:
				1.	Please refer the unit size to adjust the removable and CD partition.
*****************************************************************************/
int UDV_GetUnitSize (TokenDriveIdentifier	tokenDriveIdentifier, unsigned int*  p_dwUnitSize);


/****************************************************************************
Purpose		¡G
This function sets the serial number into device.

Parameter	¡G

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
p_bySerialNumber		unsigned char*			input			Buffer address of serial number (Max : 34bytes)
dwLength_SerialNumber	unsigned int			input			Length of production name


Return		¡G
    			0, OK
    			Refer to return code table.

Note:
The tail 16bytes of serial number are reserved.
*****************************************************************************/
int UDV_SetSerialNumber(TokenDriveIdentifier	tokenDriveIdentifier,const unsigned char* p_bySerialNumber, unsigned int dwLength_SerialNumber);


/****************************************************************************
Purpose     ¡G
The function sets extra LED status.
 

Name					Type			Direction	Purpose
-----------------------------------------------------------------------------
cDriveLetter			char			input		Specify the driver letter of device
byIndex_LED				unsigned char	input		0(LED1 + LED2 + LED3), 1(LED1), 2(LED2), 3(LED3)
byStatus_LED			unsigned char	input		0(OFF), 1(ON), 2(Blink)

Return		¡G 
0, OK
Refer to return code table. 

*****************************************************************************/
int UDV_Set_ExtraLED_Status(TokenDriveIdentifier	tokenDriveIdentifier, unsigned char byIndex_LED ,unsigned char byStatus_LED);

/****************************************************************************
Purpose     ¡G
The function sets extra LED status.
 

Name					Type			Direction	Purpose
-----------------------------------------------------------------------------
cDriveLetter			char			input		Specify the driver letter of device
p_byStatus_LED1			unsigned char	output		LED1 status(0:OFF, 1:ON, 2:Blink)
p_byStatus_LED2			unsigned char	output		LED2 status(0:OFF, 1:ON, 2:Blink)
p_byStatus_LED3			unsigned char	output		LED3 status(0:OFF, 1:ON, 2:Blink)

Return		¡G 
0, OK
Refer to return code table. 

*****************************************************************************/
int UDV_Get_ExtraLED_Status(TokenDriveIdentifier	tokenDriveIdentifier, unsigned char* p_byStatus_LED1 ,unsigned char* p_byStatus_LED2, unsigned char* p_byStatus_LED3);

/****************************************************************************
Purpose		¡G
The function writes data into write-protected partition area.

Parameter	¡G

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
dwPageIndex				unsigned int			input			Page index of partition area will be written.
dwPageCount				unsigned int			input			Page count of partition area will be written.
p_byBuf					unsigned char*			input			Buffer address to data.

Return		¡G
0, OK
Refer to return code table.


Note			:
    			1 Page	= 	1	sector
    			1 sector	= 	512	bytes
For performance, SDK doesn't check input parameter with the valid access.
*****************************************************************************/
int UDV_WriteSector_into_WPPartition (
						 TokenDriveIdentifier	tokenDriveIdentifier
						 , unsigned int	dwPageIndex
						 , unsigned int	dwPageCount
						 , unsigned char*	p_byBuf
						 );

/****************************************************************************
Purpose		¡G
The function reads data from target partition area.

Parameter	¡G

Name					Type					Direction		Purpose
-----------------------------------------------------------------------------
tokenDriveIdentifier	TokenDriveIdentifier	input			Specify the drive identifier of token
dwPageIndex				unsigned int			input			Page index of partition area will be read.
dwPageCount				unsigned int			input			Page count of partition area will be read.
p_byBuf					unsigned char*			input			Buffer address to data.

Return		¡G
0, OK
Refer to return code table.


Note			:
    			1 Page	= 	1	sector
    			1 sector	= 	512	bytes
For performance, SDK doesn't check input parameter with the valid access.
*****************************************************************************/
int UDV_ReadSector_TargetPartition (
						 TokenDriveIdentifier	tokenDriveIdentifier
						 , unsigned int	dwPageIndex
						 , unsigned int	dwPageCount
						 , unsigned char*	p_byBuf
						 );

#ifdef __cplusplus
}
#endif

#endif



