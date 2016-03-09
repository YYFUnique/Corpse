//MachineCode.cpp: implementation of the CMachineCode class.

////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineCode.h"
#include <IPHlpApi.h>
#include <iptypes.h>
#include <tchar.h>
#include "..\ErrorInfo.h"
#include "..\DebugInfo.h"
#include "..\TextTools.h"
#include "..\Hash\Md5.h"
//#include "..\..\公用库\LsLanUser\LsLanUser.h"
#include "../OsInfo.h"
#include <Wincrypt.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  IDENTIFY_BUFFER_SIZE  512
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.

#pragma pack(push,PackFlag,1)

typedef struct _VERSIONOUTPARAMS
{
	   BYTE bVersion;      // Binary driver version.
	   BYTE bRevision;     // Binary driver revision.
	   BYTE bReserved;     // Not used.
	   BYTE bIDEDeviceMap; // Bit map of IDE devices.
	   DWORD fCapabilities; // Bit mask of driver capabilities.
	   DWORD dwReserved[4]; // For future use.
}VERSIONOUTPARAMS, *PVERSIONOUTPARAMS, *LPVERSIONOUTPARAMS;

typedef struct _CMDINPARAMS
{
	DWORD cBufferSize;
	IDEREGS irDriveRegs;
	BYTE bDriveNumber;
	BYTE bReserved[3]; // Reserved for future expansion.
	DWORD dwReserved[4]; // For future use.
	//BYTE bBuffer[1]; // Input buffer.
}CMDINPARAMS, *PCMDINPARAMS, *LPCMDINPARAMS;

//Structure returned by PhysicalDrive IOCTL for several commands
typedef struct _CMDOUTPARAMS
{
	   DWORD         cBufferSize;   //  Size of bBuffer in bytes
	   DRIVERSTATUS  DriverStatus;  //  Driver status structure.
	   BYTE          bBuffer[512];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
}CMDOUTPARAMS, *PCMDOUTPARAMS, *LPCMDOUTPARAMS;

//CMDOUTPARAMS 的bBuffer结构
typedef struct _IDSECTOR {
	USHORT wGenConfig;
	USHORT wNumCyls;
	USHORT wReserved;
	USHORT wNumHeads;
	USHORT wBytesPerTrack;
	USHORT wBytesPerSector;
	USHORT wSectorsPerTrack;
	USHORT wVendorUnique[3];
	CHAR sSerialNumber[20];
	USHORT wBufferType;
	USHORT wBufferSize;
	USHORT wECCSize;
	CHAR sFirmwareRev[8];
	CHAR sModelNumber[40];
	USHORT wMoreVendorUnique;
	USHORT wDoubleWordIO;
	USHORT wCapabilities;
	USHORT wReserved1;
	USHORT wPIOTiming;
	USHORT wDMATiming;
	USHORT wBS;
	USHORT wNumCurrentCyls;
	USHORT wNumCurrentHeads;
	USHORT wNumCurrentSectorsPerTrack;
	ULONG ulCurrentSectorCapacity;
	USHORT wMultSectorStuff;
	ULONG ulTotalAddressableSectors;
	USHORT wSingleWordDMA;
	USHORT wMultiWordDMA;
	BYTE bReserved[128];
} IDSECTOR, *PIDSECTOR;

#pragma pack(pop,PackFlag)

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition

#if (_MSC_VER < 1500)
typedef enum _STORAGE_PROPERTY_ID
{
	StorageDeviceProperty = 0,
	StorageAdapterProperty
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

typedef enum _STORAGE_QUERY_TYPE {
	PropertyStandardQuery = 0,          // Retrieves the descriptor
	PropertyExistsQuery,                // Used to test whether the descriptor is supported
	PropertyMaskQuery,                  // Used to retrieve a mask of writeable fields in the descriptor
	PropertyQueryMaxDefined			   // use to validate the value
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

typedef struct _STORAGE_PROPERTY_QUERY
{
	STORAGE_PROPERTY_ID PropertyId;// ID of the property being retrieved
	STORAGE_QUERY_TYPE QueryType;// Flags indicating the type of query being performed
	UCHAR AdditionalParameters[1];// Space for additional parameters if necessary
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _STORAGE_DEVICE_DESCRIPTOR {
	// Sizeof(STORAGE_DEVICE_DESCRIPTOR)
	ULONG Version;
	// Total size of the descriptor, including the space for additional
	// data and id strings
	ULONG Size;
	// The SCSI-2 device type
	UCHAR DeviceType;
	// The SCSI-2 device type modifier (if any) - this may be zero
	UCHAR DeviceTypeModifier;
	// Flag indicating whether the device's media (if any) is removable.  This
	// field should be ignored for media-less devices
	BOOLEAN RemovableMedia;
	// Flag indicating whether the device can support mulitple outstanding
	// commands.  The actual synchronization in this case is the responsibility
	// of the port driver.
	BOOLEAN CommandQueueing;
	// Byte offset to the zero-terminated ascii string containing the device's
	// vendor id string.  For devices with no such ID this will be zero
	ULONG VendorIdOffset;
	// Byte offset to the zero-terminated ascii string containing the device's
	// product id string.  For devices with no such ID this will be zero
	ULONG ProductIdOffset;
	// Byte offset to the zero-terminated ascii string containing the device's
	// product revision string.  For devices with no such string this will be
	// zero
	ULONG ProductRevisionOffset;
	// Byte offset to the zero-terminated ascii string containing the device's
	// serial number.  For devices with no serial number this will be zero
	ULONG SerialNumberOffset;
	// Contains the bus type (as defined above) of the device.  It should be
	// used to interpret the raw device properties at the end of this structure
	// (if any)
	STORAGE_BUS_TYPE BusType;
	// The number of bytes of bus-specific data which have been appended to
	// this descriptor
	ULONG RawPropertiesLength;
	// Place holder for the first byte of the bus specific property data
	UCHAR RawDeviceProperties[1];
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;
#endif

#define IO_MANAGER_TYPE		1
#define SCSI_TYPE			2
#define ZERO_IO_TYPE		3
#define NET_ID_TYPE			4
#define GUID_TYPE			5


DWORD CMachineCode::m_dwOldMachineCode=0;
BYTE CMachineCode::m_byNewMachineCode[MACHINECODE_SIZE]={0};
////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////

CMachineCode::CMachineCode()
{
	
}

CMachineCode::~CMachineCode()
{
	
}

BOOL CMachineCode::GetMachineCode(DWORD& dwOldMachineCode)
{
	// 首先从注册表中获取保存的老机器码
	if (GetOldHwCodeFromReg(dwOldMachineCode))
	{
		if (dwOldMachineCode)
			return TRUE;
	}

	if (m_dwOldMachineCode)
	{
		dwOldMachineCode = m_dwOldMachineCode;
		return TRUE;
	}

	// 判断程序是否在虚拟机中运行，如果在虚拟机中运行则跳过所有和磁盘相关的获取机器码方式
	if (!IsRunInVM())
	{
		GetMachineCodeByIOControl(m_dwOldMachineCode,m_byNewMachineCode);
		if(m_dwOldMachineCode == 0)
			GetMachineCodeByAsScsiDrive(m_dwOldMachineCode,m_byNewMachineCode);
		if(m_dwOldMachineCode == 0)
			GetMachineCodeByZeroRights(m_dwOldMachineCode,m_byNewMachineCode);
	}
	
	if(m_dwOldMachineCode == 0)
		GetMachineCodeByNetAdaptID(m_dwOldMachineCode,m_byNewMachineCode);
	if(m_dwOldMachineCode == 0)
		GetLastSaveHwId(m_dwOldMachineCode);
	if(m_dwOldMachineCode == 0)
		GenerateHwId(m_dwOldMachineCode);
	if(m_dwOldMachineCode == 0 )
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("获取主机旧机器码失败!"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}
	dwOldMachineCode = m_dwOldMachineCode;
	return TRUE;
}

BOOL CMachineCode::GetMachineCodeEx(BYTE* pbyMachineCode)
{
	BOOL bIsRunInVM=IsRunInVM();//是否在虚似机中运行

	//在虚似机中，每次重新获取机器码，而不是从原有m_byNewMachineCode中读取
	//解决在虚似机中运行，频繁卸载、安装网卡时，m_byNewMachineCode全局静态变量值不正确问题。
	//原因：心跳包线程会频繁调用此函数，在多线程中写入m_byNewMachineCode时未保护。
	if(bIsRunInVM)
	{
		ZeroMemory(m_byNewMachineCode,MACHINECODE_SIZE);
	}

	// 首先从注册表中获取新机器码
	if (GetNewHwCodeFromReg(pbyMachineCode))
	{
		OutputDebugStringWhenDebugMode(_T("GetNewHwCodeFromReg注册表中获取新机器码"));
		if (!ArrayIsEmpty(pbyMachineCode, MACHINECODE_SIZE))
		{
			OutputDebugStringWhenDebugMode(_T("GetNewHwCodeFromReg注册表中获取新机器码 且不为0"));
			return TRUE;
		}
	}

	if (!ArrayIsEmpty(m_byNewMachineCode,MACHINECODE_SIZE))
	{
		memcpy(pbyMachineCode,m_byNewMachineCode,MACHINECODE_SIZE);
		OutputDebugStringWhenDebugMode(_T("原有m_byNewMachineCode全局变量不为0机器存在，直接返回不再调用函数读取"));
		return TRUE;
	}
	
	OutputDebugStringWhenDebugMode(_T("GetNewHwCodeFromReg(pbyMachineCode),从HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NewHwCode,CustomCode 失败"));
	// 判断程序是否在虚拟机中运行，如果在虚拟机中运行则跳过所有和磁盘相关的获取机器码方式
	if (!bIsRunInVM)
	{

		OutputDebugStringWhenDebugMode(_T("bIsRunInVM==False 程序在真机中运行"));

		GetMachineCodeByIOControl(m_dwOldMachineCode,m_byNewMachineCode);
		if(ArrayIsEmpty(m_byNewMachineCode,MACHINECODE_SIZE))
			GetMachineCodeByAsScsiDrive(m_dwOldMachineCode,m_byNewMachineCode);
		if(ArrayIsEmpty(m_byNewMachineCode,MACHINECODE_SIZE))
			GetMachineCodeByZeroRights(m_dwOldMachineCode,m_byNewMachineCode);
	}

	if(ArrayIsEmpty(m_byNewMachineCode,MACHINECODE_SIZE))
	{
		GetMachineCodeByNetAdaptID(m_dwOldMachineCode,m_byNewMachineCode);
		OutputDebugStringWhenDebugMode(_T("GetMachineCodeByNetAdaptID,通过适配器计算机器码"));
	}

	if(ArrayIsEmpty(m_byNewMachineCode,MACHINECODE_SIZE))
	{
		GetLastSaveHwExId(m_byNewMachineCode);
		OutputDebugStringWhenDebugMode(_T("GetLastSaveHwExId,获取随机计算机器码"));
	}

	if(ArrayIsEmpty(m_byNewMachineCode,MACHINECODE_SIZE))
	{
		OutputDebugStringWhenDebugMode(_T("GenerateHwExId,生成随机机器码"));
		GenerateHwExId();
		GetLastSaveHwExId(m_byNewMachineCode);
	}

	if (ArrayIsEmpty(m_byNewMachineCode,MACHINECODE_SIZE))
	{
		OutputDebugStringWhenDebugMode(_T("通过所有方式获取机器码失败,未知错误."));
		SetErrorInfo(CUSTOM_ERROR,0,_T("获取主机新机器码失败!"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}

	memcpy(pbyMachineCode,m_byNewMachineCode,MACHINECODE_SIZE);
	return TRUE;
}

BOOL CMachineCode::GetMachineCodeEx(CString& strMachineCodeEx)
{
	BYTE MachineCodeBuffer[16];
	if (GetMachineCodeEx(MachineCodeBuffer) == FALSE)
		return FALSE;

	strMachineCodeEx = ConvertBinToString(MachineCodeBuffer,sizeof(MachineCodeBuffer));

	return TRUE;
}

BOOL CMachineCode::ArrayIsEmpty(PBYTE byArray,int iArrayLen)
{
	for (int i=0;i<iArrayLen;i++)
	{
		if(byArray[i] != 0)
			return FALSE;
	}
	return TRUE;
}

BOOL CMachineCode::GetMachineCodeByIOControl(DWORD& dwMachineCode,BYTE *pbyMachineCode)
{
	CHAR chSerialNumber[MAX_PATH] = {0};
	CHAR chModelNumber[MAX_PATH] = {0};
	DWORD dwSerialLen = 0;
	DWORD dwModelLen = 0;
	if(!GetDiskSerialNumAndModelNumByIoControl(chSerialNumber,chModelNumber,dwSerialLen,dwModelLen))
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("采用管理权限的IO方式获取主机硬盘序列号和硬盘型号失败!"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}
	
	//计算新机器码
	CalculateMachineCodeEx(IO_MANAGER_TYPE,chSerialNumber,dwSerialLen,(BYTE*)chModelNumber,dwModelLen,pbyMachineCode);
	
	//计算旧机器码
	dwMachineCode = 0;
	CalculateMachineCodeWithIO(dwMachineCode,chSerialNumber,(int)strlen(chSerialNumber));
	CalculateMachineCodeWithIO(dwMachineCode,chModelNumber,(int)strlen(chModelNumber));
	if (dwMachineCode)
	{
		DWORD dwTemp=RndAuthAlg(dwMachineCode^0xffffffff);
		dwTemp=dwTemp*89127+0xdf3971;
		if (dwTemp)
			dwMachineCode=dwTemp;
	}
	if (dwMachineCode)//为序列号标记为硬盘
	{
		dwMachineCode&=0x3fffffff;
		dwMachineCode|=0x40000000;
	}
	return TRUE;
}

BOOL CMachineCode::GetDiskSerialNumAndModelNumByIoControl(CHAR chSerialNumber[],char chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen)
{
	OSVERSIONINFO VersionInformation;
	VersionInformation.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInformation);
	DWORD dwMachineCode = 0;
	
	if (VersionInformation.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		DWORD dwOsDiskIndex=0;
		if (GetPhysicalHardIndex(GetOsInLogicalDrive(),dwOsDiskIndex) == TRUE)
			if (GetHardDiskSerialNumByIoControl(dwOsDiskIndex,chSerialNumber,chModelNumber,dwSerialLen,dwModelLen) == TRUE)
				return TRUE;

		for(DWORD dwHardIndex=0;dwHardIndex<16;dwHardIndex++)
		{
			if (GetHardDiskSerialNumByIoControl(dwHardIndex,chSerialNumber,chModelNumber,dwSerialLen,dwModelLen) == FALSE)
				continue;

			return TRUE;
		}
	}
	return FALSE;
}


BOOL CMachineCode::GetHardDiskSerialNumByIoControl(DWORD dwHardIndex,CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen)
{
	TCHAR szDriveText[MAX_PATH];
	_stprintf(szDriveText,_T("\\\\.\\PHYSICALDRIVE%d"),dwHardIndex);
	HANDLE hDevice=CreateFile(szDriveText,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, 0, NULL);

	BOOL bSuccess = FALSE;
	do 
	{
		if (hDevice==INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("打开物理磁盘%s失败"),szDriveText);
			break;
		}

		VERSIONOUTPARAMS VersionParams;
		ZeroMemory(&VersionParams,sizeof(VERSIONOUTPARAMS));
		DWORD cbBytesReturned;
		if (!DeviceIoControl (hDevice, DFP_GET_VERSION,0,0,&VersionParams,sizeof(VERSIONOUTPARAMS),
			&cbBytesReturned, NULL))
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取磁盘信息失败"));
			break;
		}

		if (!VersionParams.bIDEDeviceMap)
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("获取到的磁盘信息不是IDE设备信息"));
			break;
		}

		CMDINPARAMS  InParams;
		CMDOUTPARAMS OutParam;

		// Now, get the ID sector for all IDE devices in the system.
		// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
		// otherwise use the IDE_ATA_IDENTIFY command
		ZeroMemory(&InParams,sizeof(CMDINPARAMS));
		ZeroMemory(&OutParam,sizeof(CMDOUTPARAMS));

		InParams.cBufferSize=512;
		InParams.irDriveRegs.bSectorCountReg=1;
		InParams.irDriveRegs.bSectorNumberReg=1;
		BYTE DriveNo = (BYTE)dwHardIndex;
		// Compute the drive number.
		InParams.irDriveRegs.bDriveHeadReg=0xA0|((DriveNo&1)<<4);//0xa0 or 0xb0

		// The command can either be IDE identify or ATAPI identify.
		InParams.irDriveRegs.bCommandReg =(VersionParams.bIDEDeviceMap >> DriveNo & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
		InParams.bDriveNumber = DriveNo;

		if (!DeviceIoControl(hDevice,DFP_RECEIVE_DRIVE_DATA,&InParams,sizeof(CMDINPARAMS),&OutParam,
			sizeof(CMDOUTPARAMS),&cbBytesReturned, NULL))
		{
			SetErrorInfo(SYSTEM_ERROR,0,_T("获取磁盘数据失败"));
			break;
		}

		if (!(OutParam.bBuffer[0] & 0x0040))
		{
			SetErrorInfo(CUSTOM_ERROR,0,_T("当前磁盘不是Fixed磁盘"));
			break;
		}

		IDSECTOR* phdinfo=(IDSECTOR*)OutParam.bBuffer;
		dwSerialLen = sizeof(phdinfo->sSerialNumber);
		dwModelLen = sizeof(phdinfo->sModelNumber);
		phdinfo->sSerialNumber[sizeof(phdinfo->sSerialNumber)]=0;//此处访问对于sSerialNumber数组已越界，但因为后面一项的wBufferType未使用，因此保留此做法
		//去末尾的空格
		//for(int i=0;i<sizeof(phdinfo->sSerialNumber) && phdinfo->sSerialNumber[i];i++)
		int i = 0;
		for(i=(int)strlen(phdinfo->sSerialNumber)-1;i>=0;i--)
			if (phdinfo->sSerialNumber[i]==' ')
				phdinfo->sSerialNumber[i]=0;
			else
				break;
		memcpy(chSerialNumber,phdinfo->sSerialNumber,strlen(phdinfo->sSerialNumber));
		phdinfo->sModelNumber[sizeof(phdinfo->sModelNumber)]=0;//访问越界处理方式同上
		//去末尾的空格
		//for(i=0;i<sizeof(phdinfo->sModelNumber) && phdinfo->sModelNumber[i];i++)
		for(i=(int)strlen(phdinfo->sModelNumber)-1;i>=0;i--)
			if (phdinfo->sModelNumber[i]==' ')
				phdinfo->sModelNumber[i]=0;
			else
				break;
		memcpy(chModelNumber,phdinfo->sModelNumber,strlen(phdinfo->sModelNumber));

		bSuccess = TRUE;
	} while (FALSE);

	if (hDevice != INVALID_HANDLE_VALUE)
		CloseHandle(hDevice);

	return bSuccess;
}

void CMachineCode::CalculateMachineCodeWithIO(DWORD& dwMachineCode,CHAR chNumber[],int iLen)
{
	while(iLen>=4)
	{
		dwMachineCode+=*(DWORD*)(chNumber+iLen-4);
		dwMachineCode=dwMachineCode*0x123+11;
		iLen-=4;
	}
	while(iLen>=2)
	{
		dwMachineCode+=*(WORD*)(chNumber+iLen-2);
		dwMachineCode=dwMachineCode*0x13+11;
		iLen-=2;
	}	
	while(iLen>=1)
	{
		dwMachineCode+=*(BYTE*)(chNumber+iLen-1);
		dwMachineCode=dwMachineCode*0x13+11;
		iLen-=1;
	}
}

DWORD CMachineCode::RndAuthAlg(DWORD dwNum)
{
	const static DWORD dwDataTable[] =
	{
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
			0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A8,
			0x0EDB8823, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
			0x09B64C2B, 0x7EB47CBD, 0xE7882D09, 0x90BF1D91,
			0x1DB17064, 0x6AB020F2, 0xF3B87148, 0xB4BE41DE
	};
	
	dwNum=(dwNum^0x1010101)*79127+498431;
	
	for(DWORD i=0;i<4;i++)
		dwNum = ((dwNum) >> 8) ^ dwDataTable[ (((BYTE*)&dwNum)[i] ^ ((dwNum) & 0x000000FF))%20 ]+0x389cb;
	
	dwNum=(dwNum^0x5050505)*879124+0xf390b;
	
	return ~dwNum;
}

BOOL CMachineCode::GetMachineCodeByAsScsiDrive(DWORD& dwMachineCode,BYTE *pbyMachineCode)
{
	CHAR chSerialNumber[MAX_PATH] = {0};
	CHAR chModelNumber[MAX_PATH] = {0};
	DWORD dwSerialLen = 0;
	DWORD dwModelLen = 0;
	if (!GetDiskInfoAsScsiDrive(chSerialNumber,chModelNumber,dwSerialLen,dwModelLen))
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("采用管理权限的SCSI方式获取主机硬盘序列号和硬盘型号失败!"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}
	
	//计算新机器码
	CalculateMachineCodeEx(SCSI_TYPE,chSerialNumber,dwSerialLen,(BYTE*)chModelNumber,dwModelLen,pbyMachineCode);
	
	//计算旧机器码	
	dwMachineCode = 0;
	CalculateDiskId(dwMachineCode,chSerialNumber,(DWORD)strlen(chSerialNumber));
	CalculateDiskId(dwMachineCode,chModelNumber,(DWORD)strlen(chModelNumber));	
	if (dwMachineCode)
	{
		DWORD dwTemp=RndAuthAlg(dwMachineCode^0xffffffff);
		dwTemp=dwTemp*897+1234567;
		if (dwTemp)
			dwMachineCode=dwTemp;
	}	
	//为序列号标记为硬盘
	if (dwMachineCode)
	{
		dwMachineCode&=0x3fffffff;
		dwMachineCode|=0x40000000;
	}	
	
	return TRUE;
}

BOOL CMachineCode::GetDiskInfoAsScsiDrive(CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen)
{
	DWORD dwOsDiskIndex=0;
	if (GetPhysicalHardIndex(GetOsInLogicalDrive(),dwOsDiskIndex) == TRUE)
		if (GetHardDiskSerialNumByScsiDrive(dwOsDiskIndex,chSerialNumber,chModelNumber,dwSerialLen,dwModelLen) == TRUE)
			return TRUE;

	for (int nController = 0;nController < 16;nController++)
	{
		if (GetHardDiskSerialNumByScsiDrive(nController,chSerialNumber,chModelNumber,dwSerialLen,dwModelLen) == FALSE)
			continue;
		
		return TRUE;
	}

	return FALSE;
}

BOOL CMachineCode::GetHardDiskSerialNumByScsiDrive(DWORD dwHardIndex,CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen)
{
	TCHAR szDriveName[MAX_PATH];
	_stprintf(szDriveName,_T("\\\\.\\Scsi%d:"),dwHardIndex);
	HANDLE hScsiDriveIOCTL = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		hScsiDriveIOCTL = CreateFile (szDriveName,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
			break;

		for (int nDrive = 0;nDrive < 2;nDrive++)
		{
			char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
			SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
			SENDCMDINPARAMS *pin = (SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
			DWORD dummy;
			memset (buffer, 0, sizeof (buffer));
			p -> HeaderLength = sizeof (SRB_IO_CONTROL);
			p -> Timeout = 10000;
			p -> Length = SENDIDLENGTH;
			p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
			memcpy ((char *) p -> Signature,"SCSIDISK", 8);
			pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
			pin -> bDriveNumber = nDrive;
			if (DeviceIoControl (hScsiDriveIOCTL,IOCTL_SCSI_MINIPORT,buffer,
				sizeof (SRB_IO_CONTROL)+sizeof (SENDCMDINPARAMS) - 1,buffer,
				sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,&dummy, NULL))
			{
				SENDCMDOUTPARAMS *pOut = (SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
				IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
				if (pId->sModelNumber[0] && pId->sSerialNumber[0])
				{
					dwSerialLen = sizeof(pId->sSerialNumber);
					dwModelLen = sizeof(pId->sModelNumber);
					//去末尾的空格					
					for (int i= dwSerialLen-1;i>=0 && (pId->sSerialNumber[i]==' ' || pId->sSerialNumber[i]=='\0');i--)
						pId->sSerialNumber[i] = '\0';
					memcpy(chSerialNumber,pId->sSerialNumber,strlen(pId->sSerialNumber));

					for (int j= dwModelLen-1;j>=0 && (pId->sModelNumber[j]==' ' || pId->sModelNumber[j]=='\0');j--)
						pId->sModelNumber[j] = '\0';
					memcpy(chModelNumber,pId->sModelNumber,strlen(pId->sModelNumber));
					bSuccess = TRUE;
					break;
				}
			}
		}
	} while (FALSE);

	if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		CloseHandle (hScsiDriveIOCTL);

	return bSuccess;
}

void CMachineCode::CalculateDiskId(DWORD& dwHwId,CHAR pDiskInfo[],DWORD dwInfoLen)
{
	while(dwInfoLen>=4)
	{
		dwHwId+=*(DWORD*)(pDiskInfo+dwInfoLen-4);
		dwHwId=dwHwId*0x123+11;
		dwInfoLen-=4;
	}
	while(dwInfoLen>=2)
	{
		dwHwId+=*(WORD*)(pDiskInfo+dwInfoLen-2);
		dwHwId=dwHwId*0x13+11;
		dwInfoLen-=2;
	}	
	while(dwInfoLen>=1)
	{
		dwHwId+=*(BYTE*)(pDiskInfo+dwInfoLen-1);
		dwHwId=dwHwId*0x7+11;
		dwInfoLen-=1;
	}
}

BOOL CMachineCode::GetMachineCodeByZeroRights(DWORD& dwMachineCode,BYTE *pbyMachineCode)
{
	CHAR chSerialNumber[MAX_PATH] = {0};
	CHAR chModelNumber[MAX_PATH] = {0};
	DWORD dwSerialLen = 0;
	DWORD dwModelLen = 0;
	if (!GetDiskInfoWithZeroRights(chSerialNumber,chModelNumber,dwSerialLen,dwModelLen))
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("采用0权限的IO方式获取主机硬盘序列号和硬盘型号失败!"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}
	
	//计算新机器码
	CalculateMachineCodeEx(ZERO_IO_TYPE,chSerialNumber,dwSerialLen,(BYTE*)chModelNumber,dwModelLen,pbyMachineCode);
	
	//计算旧机器码
	dwMachineCode = 0;
	CalculateDiskId(dwMachineCode,chSerialNumber,(DWORD)strlen(chSerialNumber));
	CalculateDiskId(dwMachineCode,chModelNumber,(DWORD)strlen(chModelNumber));
	
	//为序列号标记为硬盘
	if (dwMachineCode)
	{
		dwMachineCode=dwMachineCode*4321+7777;
		dwMachineCode&=0x3fffffff;
		dwMachineCode|=0x40000000;
	}
	
	return TRUE;
}

BOOL CMachineCode::GetDiskInfoWithZeroRights(CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen)
{
	DWORD dwOsDiskIndex=0;
	if (GetPhysicalHardIndex(GetOsInLogicalDrive(),dwOsDiskIndex) == TRUE)
		if (GetHardDiskSerialNumByZeroRights(dwOsDiskIndex,chSerialNumber,chModelNumber,dwSerialLen,dwModelLen) == TRUE)
			return TRUE;

	for (int nDrive = 0; nDrive < 16; nDrive++)
	{
		if (GetHardDiskSerialNumByZeroRights(nDrive,chSerialNumber,chModelNumber,dwSerialLen,dwModelLen) == FALSE)
			continue;

		return TRUE;
	}
	return FALSE;
}

BOOL CMachineCode::GetHardDiskSerialNumByZeroRights(DWORD dwHardIndex,CHAR chSerialNumber[],CHAR chModelNumber[],DWORD& dwSerialLen,DWORD& dwModelLen)
{
	BOOL bSuccess = FALSE;
	HANDLE hPhysicalDriveIOCTL = NULL;
	do 
	{
		TCHAR szDriverName[MAX_PATH];
		_stprintf(szDriverName,_T("\\\\.\\PhysicalDrive%d"),dwHardIndex);

		hPhysicalDriveIOCTL = CreateFile(szDriverName,0,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING, 0, NULL);
		if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
			break;

		STORAGE_PROPERTY_QUERY StoragePropertyQuery;
		memset (&StoragePropertyQuery, 0, sizeof (STORAGE_PROPERTY_QUERY));
		StoragePropertyQuery.PropertyId = StorageDeviceProperty;
		StoragePropertyQuery.QueryType = PropertyStandardQuery;
		char buffer[10000];
		memset(buffer,0,sizeof(buffer));

		DWORD cbBytesReturned = 0;
		if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,&StoragePropertyQuery,
			sizeof (STORAGE_PROPERTY_QUERY),&buffer,sizeof (buffer),& cbBytesReturned,NULL))
		{
			STORAGE_DEVICE_DESCRIPTOR * pStorageDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)buffer;
			if (pStorageDeviceDescriptor->BusType == BusTypeUsb)
				break;

			char *pszSerialNumber=buffer+pStorageDeviceDescriptor->SerialNumberOffset;
			char *pszModelNumber=buffer+pStorageDeviceDescriptor->ProductIdOffset;
			if (strlen(pszModelNumber) && strlen(pszSerialNumber) && strncmp(pszSerialNumber,"20",2) && strncmp(pszSerialNumber,"00",2))
			{
				dwSerialLen = (DWORD)strlen(pszSerialNumber);
				dwModelLen = (DWORD)strlen(pszModelNumber);
				memcpy(chSerialNumber,pszSerialNumber,40);
				memcpy(chModelNumber,pszModelNumber,strlen(pszModelNumber));
				bSuccess = TRUE;
				break;
			}
		}
		
	} while (FALSE);

	if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		CloseHandle(hPhysicalDriveIOCTL);

	return bSuccess;
}

BOOL CMachineCode::GetMachineCodeByNetAdaptID(DWORD& dwMachineCode,BYTE *pbyMachineCode)
{
	BYTE byAddress[MAX_ADAPTER_ADDRESS_LENGTH+1] = {0};
	DWORD dwAddressLen = 0;
	if(!GetNetAdaptID(byAddress,dwAddressLen))
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("采用获取网卡ID作为主机硬盘序列号失败!"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}
	dwMachineCode = 0;
	CalculateNetAdaptId(dwMachineCode,byAddress,dwAddressLen);
	CalculateMachineCodeEx(NET_ID_TYPE,byAddress,dwAddressLen,0,0,pbyMachineCode);

	OutputDebugStringWhenDebugMode(_T("GetMachineCodeByNetAdaptID 获取成功"));

	return TRUE;
}

void CMachineCode::CalculateNetAdaptId(DWORD& dwMachineCode,BYTE *pAddress, DWORD dwAddressLen)
{
	for(DWORD i=0;i<dwAddressLen;i++)
	{
		dwMachineCode+=311*pAddress[i]+79;
		dwMachineCode<<=4;
	}
	
	dwMachineCode&=0x3fffffff;
	dwMachineCode|=0x80000000;//标记序列号为网卡ID
}


BOOL CMachineCode::GetNetAdaptID(BYTE byAddress[],DWORD& dwAddressLen)
{

	CLsIpAdapterInfoList LsIpAdapterInfoList;
	if (!LsGetAdaptersInfo(LsIpAdapterInfoList))
		return FALSE;

	POSITION pos=LsIpAdapterInfoList.GetHeadPosition();
	while(pos)
	{
		LS_IP_ADAPTER_INFO& LsIpAdapterInfo=LsIpAdapterInfoList.GetNext(pos);
		// 如果不在虚拟机中运行，且发现了虚拟网卡，则跳过该网卡
		// 否则不跳过获取虚拟网卡
		if (!IsRunInVM() && _tcsstr(LsIpAdapterInfo.strDescription,_T("VMware")) != NULL)
			continue;

		if (LsIpAdapterInfo.AddressLength>=4 && LsIpAdapterInfo.Type==MIB_IF_TYPE_ETHERNET)// 硬件地址最小4字节
		{
			memcpy(byAddress,LsIpAdapterInfo.Address,MAX_ADAPTER_ADDRESS_LENGTH);
			dwAddressLen = LsIpAdapterInfo.AddressLength;
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CMachineCode::GetLastSaveHwId(DWORD& dwMachineCode)
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),0,KEY_READ,&hKey)!=ERROR_SUCCESS)
		return FALSE;
	
	dwMachineCode = 0;DWORD dwDataLen=sizeof(DWORD);
	if (RegQueryValueEx(hKey,_T("HwId"),0,0,(BYTE*)&dwMachineCode,&dwDataLen)!=ERROR_SUCCESS)
		return FALSE;
	
	RegCloseKey(hKey);
	
	return TRUE;
}

BOOL CMachineCode::GenerateHwId(DWORD& dwMachineCode)
{
	dwMachineCode=(DWORD)time(0)*GetTickCount()*0x3241+459;
	dwMachineCode&=0x1fffffff;
	dwMachineCode|=0xc0000000; // 标记序列号为特殊ID
	
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),0,KEY_WRITE,&hKey)!=ERROR_SUCCESS)
		return FALSE;
	
	if (RegSetValueEx(hKey,_T("HwId"),0,REG_DWORD,(BYTE*)&dwMachineCode,sizeof(DWORD))!=ERROR_SUCCESS)
		return FALSE;
	
	RegCloseKey(hKey);
	
	return TRUE;
}

BOOL CMachineCode::GetLastSaveHwExId(BYTE* pbyMachineCodeCode)
{
	TCHAR szSubKey[MAX_PATH]={0};
	_stprintf(szSubKey,_T("SOFTWARE\\Lonsin\\%s"),_T("LanGuard"));
	
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szSubKey,0,KEY_READ,&hKey)!=ERROR_SUCCESS)
		return FALSE;
	
	DWORD dwDataLen=16;
	if (RegQueryValueEx(hKey,_T("HwIdEx"),0,0,pbyMachineCodeCode,&dwDataLen)!=ERROR_SUCCESS)
		memset(pbyMachineCodeCode,0,sizeof(pbyMachineCodeCode));
	
	RegCloseKey(hKey);
	
	return TRUE;
}

BOOL CMachineCode::GenerateHwExId()
{
	GUID RandomGuid;
	HRESULT hr=CoCreateGuid(&RandomGuid);
	if (hr!=S_OK)
	{
		SetErrorInfo(SYSTEM_ERROR,hr,_T("产生随机扩展机器码时产生随机GUID失败"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}
	//计算新机器码
	BYTE byMachineCode[MACHINECODE_SIZE] = {0};
	CalculateMachineCodeEx(GUID_TYPE,&RandomGuid,MACHINECODE_SIZE,0,0,byMachineCode);

	TCHAR szSubKey[MAX_PATH]={0};
	_stprintf(szSubKey,_T("SOFTWARE\\Lonsin\\%s"),_T("LanGuard"));

	HKEY hKey;
	DWORD dwErrorCode=RegOpenKeyEx(HKEY_LOCAL_MACHINE,szSubKey,0,KEY_WRITE,&hKey);
	if (dwErrorCode!=ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR,dwErrorCode,_T("产生随机扩展机器码时打开注册表失败"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
		return FALSE;
	}
	
	BOOL bSuccess=FALSE;
	dwErrorCode=RegSetValueEx(hKey,_T("HwIdEx"),0,REG_BINARY,byMachineCode,MACHINECODE_SIZE);
	if (dwErrorCode==ERROR_SUCCESS)
		bSuccess=TRUE;
	else
	{
		SetErrorInfo(SYSTEM_ERROR,dwErrorCode,_T("产生随机扩展机器码时保存注册表失败"));
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
	}

	
	RegCloseKey(hKey);
	
	return bSuccess;
}

void CMachineCode::CalculateMachineCodeEx(DWORD dwType,const void* pbyHardwardId1,DWORD dwHardwardIdLen1,BYTE* pbyHardwareId2,DWORD dwHardwardIdLen2,BYTE byMachineCodeEx[MACHINECODE_SIZE])
{
	//HCRYPTPROV hCryptProv=0;HCRYPTHASH hHash=0;
	
/*	BOOL bSuccess=FALSE;

	do 
	{
		if (CryptAcquireContext(&hCryptProv,_T("LsLanguard"),MS_STRONG_PROV,PROV_RSA_FULL,0)==0)
			if(CryptAcquireContext(&hCryptProv,_T("LsLanguard"),MS_STRONG_PROV,PROV_RSA_FULL,CRYPT_NEWKEYSET)==0)
				return;
			
		if (CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)==FALSE)
			break;

		if (CryptHashData(hHash,(BYTE*)"lonsin",6,0)==FALSE)
			break;

		if (pbyHardwardId1)
			if (CryptHashData(hHash,(BYTE*)pbyHardwardId1,dwHardwardIdLen1,0)==FALSE)
				break;

		if (pbyHardwareId2)
			if (CryptHashData(hHash,pbyHardwareId2,dwHardwardIdLen1,0)==FALSE)
				break;

		DWORD dwHashDataLen=16;
		if (CryptGetHashParam(hHash,HP_HASHVAL,byMachineCodeEx,&dwHashDataLen,0)==FALSE)
			break;

		bSuccess=TRUE;

	} while (0);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hCryptProv)
		CryptReleaseContext(hCryptProv,0);*/

	CMD5 md5;
	md5.Init();
	md5.Update((unsigned char *)"lonsin",6);
	
	if (pbyHardwardId1)
		md5.Update((unsigned char*)pbyHardwardId1,dwHardwardIdLen1);
	
	if (pbyHardwareId2)
		md5.Update(pbyHardwareId2,dwHardwardIdLen1);
	
	md5.Final ((unsigned char*)byMachineCodeEx);

	byMachineCodeEx[0] &= 0x0f;

	//标记获取方式
	switch(dwType)
	{
	case IO_MANAGER_TYPE:
		byMachineCodeEx[0] |= 0x10;
		break;
	case SCSI_TYPE:
		byMachineCodeEx[0] |= 0x20;
		break;
	case ZERO_IO_TYPE:
		byMachineCodeEx[0] |= 0x30;
		break;
	case NET_ID_TYPE:
		byMachineCodeEx[0] |= 0x40;
		break;
	case GUID_TYPE:
		byMachineCodeEx[0] |= 0x50;
		break;
	}
}

// 从注册表获取老机器码
BOOL CMachineCode::GetOldHwCodeFromReg(DWORD &dwOldMachineCode)
{
	TCHAR szSubKey[MAX_PATH] = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	TCHAR szOldHwCodeValue[] = _T("OldHwCode");
	TCHAR szCustomCode[] = _T("CustomCode");
	
	BOOL bIsSuccessed = TRUE;
	HKEY hKey;
	do 
	{
		DWORD dwErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_READ, &hKey);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwErrorCode, _T("从注册表获取老机器码时，打开注册表失败！"));
			bIsSuccessed = FALSE;
			break;
		}

		DWORD	dwCustomCode = 0;
		DWORD	dwDataLen = sizeof(DWORD);
		dwErrorCode = RegQueryValueEx(hKey, szCustomCode, 0, 0, (PBYTE)&dwCustomCode, &dwDataLen);
		if (dwErrorCode != ERROR_SUCCESS || dwCustomCode == 0)
		{
			bIsSuccessed = FALSE;
			break;
		}

		dwDataLen = sizeof(DWORD);
		dwErrorCode = RegQueryValueEx(hKey, szOldHwCodeValue, 0, 0, (BYTE*)&dwOldMachineCode, &dwDataLen);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwErrorCode, _T("获取老机器码时，查询老机器码键值(OldHwCode)失败！"));
			bIsSuccessed = FALSE;
			break;
		}
	} while (0);
	
	RegCloseKey(hKey);
	
	return bIsSuccessed;
}

// 从注册表获取新机器码
BOOL CMachineCode::GetNewHwCodeFromReg(PBYTE pbyNewMachineCode)
{
	TCHAR szSubKey[MAX_PATH] = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	TCHAR szValueName[] = _T("NewHwCode");
	TCHAR szCustomCode[]= _T("CustomCode");
	
	BOOL bIsSuccessed = TRUE;
	HKEY hKey;
	do 
	{
		DWORD dwErrorCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_READ, &hKey);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwErrorCode, _T("获取新机器码时，打开注册表失败！"));
			bIsSuccessed = FALSE;
			break;
		}
		
		DWORD	dwCustomCode = 0;DWORD	dwDataLen = sizeof(DWORD);
		dwErrorCode = RegQueryValueEx(hKey, szCustomCode, 0, 0, (PBYTE)&dwCustomCode, &dwDataLen);
		if (dwErrorCode != ERROR_SUCCESS || dwCustomCode == 0)
		{
			bIsSuccessed = FALSE;
			break;
		}
		
		dwDataLen = MACHINECODE_SIZE;
		dwErrorCode = RegQueryValueEx(hKey, szValueName, 0, 0, pbyNewMachineCode, &dwDataLen);
		if (dwErrorCode != ERROR_SUCCESS)
		{
			SetErrorInfo(SYSTEM_ERROR, dwErrorCode, _T("获取新机器码时，查询新机器码键值(NewHwCode)失败！"));
			bIsSuccessed = FALSE;
			break;
		}
	} while (0);
	
	RegCloseKey(hKey);
	
	return bIsSuccessed;
}

// 检测是否在VMware虚拟机中运行
BOOL CMachineCode::IsRunInVmWare()
{
	BOOL bIsFoundVMware = FALSE;
#ifndef _AMD64_
	__try
	{
		__asm
		{
			push     edx
			push     ecx
			push     ebx
			mov      eax, 0x564d5868
			mov      ecx, 0x0A
			mov      edx, 0x5658
			in       eax, dx
			cmp      ebx, 0x564d5868
			setz     byte ptr[bIsFoundVMware]
			pop      ebx
			pop      ecx
			pop      edx
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER )
	{
	}
#endif
	return bIsFoundVMware;
}

// 检测是否在VPC虚拟机中运行
BOOL CMachineCode::IsRunInVPC()
{
	BOOL bIsFoundVPC = FALSE;
#ifndef _AMD64_
	__try
	{
		__asm
		{
			push ebx
			mov  ebx, 0
			mov  eax, 1
			_emit 0x0f
			_emit 0x3f
			_emit 0x07
			_emit 0x0b
			test ebx, ebx
			setz byte ptr[bIsFoundVPC]
			pop ebx
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
#endif
	return bIsFoundVPC;
}

// 检测是否在虚拟机中运行
BOOL CMachineCode::IsRunInVM()
{
	if (IsRunInVmWare() /*|| IsRunInVPC()*/)
		return TRUE;
	else
		return FALSE;
}
