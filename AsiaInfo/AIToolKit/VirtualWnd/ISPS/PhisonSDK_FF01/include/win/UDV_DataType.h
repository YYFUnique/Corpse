#ifndef __UDV_DATATYPE_H__
#define __UDV_DATATYPE_H__


// #ifndef _LINUX
// #define _LINUX
// #endif

#define UDVPrivatePartition		0x01
#define UDVHiddenPartition		0x02
#define UDVCDROMPartition		0x03
#define UDVMaxTokenNumber		32
#define UDVMaxPartitionNumber	8

enum  {
	UDVFileSystemType_None = 0,	// Don¡¦t build File system
	UDVFileSystemType_FAT = 1,		// Select file system type by SDK with partition capacity
	UDVFileSystemType_ExFAT = 2	// ExFAT
};

typedef unsigned int	UDVFileSystemType;

//#define _LINUX

#ifdef _LINUX

#define SCSI_IOCTL_DATA_OUT				0
#define SCSI_IOCTL_DATA_IN				1
#define SCSI_IOCTL_DATA_UNSPECIFIED		2
#ifndef _MACX
#define	FALSE	(0)
#define	TRUE	(!FALSE)
#endif
#define	DWORD	unsigned int
#define	BYTE	unsigned char
#define	BOOL	int
#define	WORD	unsigned short
#define	USHORT	unsigned short
#define	UCHAR	unsigned char
#define	ULONG	unsigned int

#endif

#endif
