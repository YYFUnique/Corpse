#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#include <Stierr.h>

#ifdef _WIN32
#pragma comment(lib,"ntdll.lib")
#elif defined _WIN64
#pragma comment(lib,"ntdll(X64).lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NTDEF_
	typedef LONG NTSTATUS, *PNTSTATUS;
#endif

#define NT_SUCCESS(status)          ((NTSTATUS)(status)>=0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)

#define ProcessBasicInformation 0

#define InitializeObjectAttributes( p, n, a, r, s ) { \
	(p)->Length = sizeof( OBJECT_ATTRIBUTES ); \
	(p)->RootDirectory = r; \
	(p)->Attributes = a; \
	(p)->ObjectName = n; \
	(p)->SecurityDescriptor = s; \
	(p)->SecurityQualityOfService = NULL; }


typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

typedef struct
{
	ULONG          AllocationSize;
	ULONG          ActualSize;
	ULONG          Flags;
	ULONG          Unknown1;
	UNICODE_STRING Unknown2;
	HANDLE         InputHandle;
	HANDLE         OutputHandle;
	HANDLE         ErrorHandle;
	UNICODE_STRING CurrentDirectory;
	HANDLE         CurrentDirectoryHandle;
	UNICODE_STRING SearchPaths;
	UNICODE_STRING ApplicationName;
	UNICODE_STRING CommandLine;
	PVOID          EnvironmentBlock;
	ULONG          Unknown[9];
	UNICODE_STRING Unknown3;
	UNICODE_STRING Unknown4;
	UNICODE_STRING Unknown5;
	UNICODE_STRING Unknown6;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;


typedef struct
{
	ULONG               AllocationSize;
	ULONG               Unknown1;
	HINSTANCE           ProcessHinstance;
	PVOID               ListDlls;
	PPROCESS_PARAMETERS ProcessParameters;
	ULONG               Unknown2;
	HANDLE              Heap;
} PEB, *PPEB;

typedef struct
{
	DWORD ExitStatus;
	PPEB  PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}   PROCESS_BASIC_INFORMATION;

//查询进程所用函数的参数结构
typedef struct _LS_SYSTEM_PROCESSE_INFO
{
    ULONG			NextEntryDelta;          //构成结构序列的偏移量；
    ULONG			ThreadCount;             //线程数目；
	ULONG			MemoryCount;			  // 内存使用量
	ULONG			Reseved1[5];
    //ULONG			Reserved1[6];           
    ULARGE_INTEGER	qProcessCreateTime;		//进程创建时间;
    __int64			UserTime;                //用户模式(Ring 3)的CPU时间；
    __int64			KernelTime;              //内核模式(Ring 0)的CPU时间；
    UNICODE_STRING	ProcessName;             //进程名称；
    ULONG			dwBasePriority;            //进程优先权；
    ULONG			ProcessId;               //进程标识符；
    ULONG			InheritedFromProcessId; //父进程的标识符；
    ULONG			HandleCount;             //句柄数目；
    DWORD			dwSessionId;
	ULONG			dwSpareUl3;
    SIZE_T			tPeakVirtualSize;
	SIZE_T			tVirtualSize;
	DWORD			dwPageFaultCount;
	DWORD			dwPeakWorkingSetSize; 
	DWORD			dwWorkingSetSize; 
	SIZE_T			tQuotaPeakPagedPoolUsage;
	SIZE_T			tQuotaPagedPoolUsage;
	SIZE_T			tQuotaPeakNonPagedPoolUsage;
	SIZE_T			tQuotaNonPagedPoolUsage;
	SIZE_T			tPagefileUsage;
	SIZE_T			tPeakPagefileUsage;
	SIZE_T			tPrivatePageCount;
	LARGE_INTEGER	qReadOperationCount;
	LARGE_INTEGER	qWriteOperationCount;
	LARGE_INTEGER	qOtherOperationCount;
	LARGE_INTEGER	qReadTransferCount;
	LARGE_INTEGER	qWriteTransferCount;
	LARGE_INTEGER	qOtherTransferCount;
}LS_SYSTEM_PROCESSES_INFO,*PLS_SYSTEM_PROCESSES_INFO;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
	SystemProcessorInformation = 1,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemNotImplemented1 = 4,
	SystemProcessInformation = 5,
	SystemCallCounts = 6,                       // 6 Y N   
	SystemConfigurationInformation = 7,         // 7 Y N   
	SystemProcessorTimes = 8,                   // 8 Y N   
	SystemGlobalFlag = 9,                       // 9 Y Y   
	SystemNotImplemented2 = 10,                  // 10 Y N   
	SystemModuleInformation = 11,                // 11 Y N   
	SystemLockInformation = 12,                  // 12 Y N   
	SystemNotImplemented3 = 13,                  // 13 Y N   
	SystemNotImplemented4 = 14,                  // 14 Y N   
	SystemNotImplemented5 = 15,                  // 15 Y N   
    SystemHandleInformation = 16,                // 16 Y N   
	SystemObjectInformation = 17,                // 17 Y N   
	SystemPagefileInformation = 18,              // 18 Y N   
	SystemInstructionEmulationCounts = 19,       // 19 Y N   
	SystemInvalidInfoClass1 = 20,                // 20   
	SystemCacheInformation = 21,                 // 21 Y Y   
	SystemPoolTagInformation = 22,               // 22 Y N   
	SystemProcessorStatistics = 23,              // 23 Y N   
	SystemDpcInformation = 24,                   // 24 Y Y   
	SystemNotImplemented6 = 25,                  // 25 Y N   
	SystemLoadImage = 26,                        // 26 N Y   
	SystemUnloadImage = 27,                      // 27 N Y   
	SystemTimeAdjustment = 28,                   // 28 Y Y   
	SystemNotImplemented7 = 29,                  // 29 Y N   
	SystemNotImplemented8 = 30,                  // 30 Y N   
	SystemNotImplemented9 = 31,                  // 31 Y N   
	SystemCrashDumpInformation = 32,             // 32 Y N   
	SystemExceptionInformation = 33,             // 33 Y N   
	SystemCrashDumpStateInformation = 34,        // 34 Y Y/N   
	SystemKernelDebuggerInformation = 35,        // 35 Y N   
	SystemContextSwitchInformation = 36,         // 36 Y N   
	SystemRegistryQuotaInformation = 37,         // 37 Y Y   
	SystemLoadAndCallImage = 38,                 // 38 N Y   
	SystemPrioritySeparation = 39,               // 39 N Y   
	SystemNotImplemented10 = 40,                 // 40 Y N   
	SystemNotImplemented11 = 41,                 // 41 Y N   
	SystemInvalidInfoClass2 = 42,                // 42   
	SystemInvalidInfoClass3 = 43,                // 43   
	SystemTimeZoneInformation = 44,              // 44 Y N   
    SystemLookasideInformation = 45,             // 45 Y N   
	SystemSetTimeSlipEvent = 46,                 // 46 N Y   
	SystemCreateSession = 47,                    // 47 N Y   
	SystemDeleteSession = 48,                    // 48 N Y   
	SystemInvalidInfoClass4 = 49,                // 49   
	SystemRangeStartInformation = 50,            // 50 Y N   
	SystemVerifierInformation = 51,              // 51 Y Y   
	SystemAddVerifier = 52,                      // 52 N Y   
	SystemSessionProcessesInformation = 53       // 53 Y N   
} SYSTEM_INFORMATION_CLASS;

typedef enum _THREADINFOCLASS {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	MaxThreadInfoClass
} THREADINFOCLASS;

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef struct _THREAD_BASIC_INFORMATION { // Information Class 0
	LONG     ExitStatus;
	PVOID    TebBaseAddress;
	CLIENT_ID ClientId;
	LONG AffinityMask;
	LONG Priority;
	LONG BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef enum _THREAD_STATE   
{
	StateInitialized,   
	StateReady,   
	StateRunning,   
	StateStandby,   
	StateTerminated,   
	StateWait,   
	StateTransition,   
	StateUnknown
}THREAD_STATE;  

typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation=1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileCopyOnWriteInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileObjectIdInformation,
	FileTrackingInformation,
	FileOleDirectoryInformation,
	FileContentIndexInformation,
	FileInheritContentIndexInformation,
	FileOleInformation,
	FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _IO_STATUS_BLOCK {
	NTSTATUS Status;
	ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllInformation,
	ObjectDataInformation
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

//DLL_API ULONG NTAPI RtlNtStatusToDosError(NTSTATUS);

DLL_API NTSTATUS WINAPI NtTerminateProcess(
								   IN HANDLE ProcessHandle OPTIONAL,
								   IN NTSTATUS ExitStatus);

DLL_API NTSTATUS WINAPI NtQuerySystemInformation (
						  IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
						  OUT PVOID SystemInformation,
						  IN ULONG SystemInformationLength,
						  OUT PULONG ReturnLength OPTIONAL);

DLL_API NTSTATUS WINAPI NtQueryInformationThread(
							IN HANDLE ThreadHandle,
							IN THREADINFOCLASS ThreadInformationClass,
							OUT PVOID ThreadInformation,
							IN ULONG ThreadInformationLength,
							OUT PULONG ReturnLength OPTIONAL) ;

DLL_API NTSTATUS WINAPI NtQueryInformationFile(
						   IN HANDLE  FileHandle,
						   OUT PIO_STATUS_BLOCK  IoStatusBlock,
						   OUT PVOID  FileInformation,
						   IN ULONG  Length,
						   IN FILE_INFORMATION_CLASS  FileInformationClass);

DLL_API NTSTATUS WINAPI NtQueryObject(
							  IN HANDLE               ObjectHandle,
							  IN OBJECT_INFORMATION_CLASS ObjectInformationClass,
							  OUT PVOID               ObjectInformation,
							  IN ULONG                Length,
							  OUT PULONG              ResultLength );

#ifdef __cplusplus
}
#endif
