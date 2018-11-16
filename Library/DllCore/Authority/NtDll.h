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


typedef struct tagPEB
{
	ULONG               AllocationSize;
	ULONG               Unknown1;
	HINSTANCE        ProcessHinstance;
	PVOID					ListDlls;
	PPROCESS_PARAMETERS ProcessParameters;
	ULONG               Unknown2;
	HANDLE              Heap;
} PEB, *PPEB;

typedef struct tagPROCESS_BASIC_INFORMATION
{
	DWORD ExitStatus;
	PPEB  PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION;

typedef struct tagSYSTEM_PROCESSOR_TIMES
{
	LARGE_INTEGER IdleTime;               //空闲时间
	LARGE_INTEGER KernelTime;             //内核模式时间
	LARGE_INTEGER UserTime;               //用户模式时间
	LARGE_INTEGER DpcTime;                //延迟过程调用时间
	LARGE_INTEGER InterruptTime;          //中断时间
	ULONG         InterruptCount;         //中断次数
}SYSTEM_PROCESSOR_TIMES,*PSYSTEM_PROCESSOR_TIMES;

typedef struct tagSYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER  IdleTime;                    //CPU空闲时间
	LARGE_INTEGER  ReadTransferCount;           //I/O读操作数目
	LARGE_INTEGER  WriteTransferCount;          //I/O写操作数目
	LARGE_INTEGER  OtherTransferCount;          //I/O其他操作数目
	ULONG          ReadOperationCount;          //I/O读数据数目
	ULONG          WriteOperationCount;         //I/O写数据数目
	ULONG          OtherOperationCount;         //I/O其他操作数据数目
	ULONG          AvailablePages;              //可获得的页数目
	ULONG          TotalCommittedPages;         //总共提交页数目
	ULONG          TotalCommitLimit;            //已提交页数目
	ULONG          PeakCommitment;              //页提交峰值
	ULONG          PageFaults;                  //页故障数目
	ULONG          WriteCopyFaults;             //Copy-On-Write故障数目
	ULONG          TransitionFaults;            //软页故障数目
	ULONG          Reserved1;
	ULONG          DemandZeroFaults;            //需求0故障数
	ULONG          PagesRead;                   //读页数目
	ULONG          PageReadIos;                 //读页I/O操作数
	ULONG          Reserved2[2];
	ULONG          PagefilePagesWritten;        //已写页文件页数
	ULONG          PagefilePageWriteIos;        //已写页文件操作数
	ULONG          MappedFilePagesWritten;      //已写映射文件页数
	ULONG          MappedFileWriteIos;          //已写映射文件操作数
	ULONG          PagedPoolUsage;              //分页池使用
	ULONG          NonPagedPoolUsage;           //非分页池使用
	ULONG          PagedPoolAllocs;             //分页池分配情况
	ULONG          PagedPoolFrees;              //分页池释放情况
	ULONG          NonPagedPoolAllocs;          //非分页池分配情况
	ULONG          NonPagedPoolFress;           //非分页池释放情况
	ULONG          TotalFreeSystemPtes;         //系统页表项释放总数
	ULONG          SystemCodePage;              //操作系统代码页数
	ULONG          TotalSystemDriverPages;      //可分页驱动程序页数
	ULONG          TotalSystemCodePages;        //操作系统代码页总数
	ULONG          SmallNonPagedLookasideListAllocateHits; //
	ULONG          SmallPagedLookasideListAllocateHits;
	ULONG          Reserved3;                   
	ULONG          MmSystemCachePage;          //系统缓存页数
	ULONG          PagedPoolPage;              //分页池页数
	ULONG          SystemDriverPage;           //可分页驱动页数
	ULONG          FastReadNoWait;             //异步快速读数目
	ULONG          FastReadWait;               //同步快速读数目
	ULONG          FastReadResourceMiss;       //快速读资源冲突数
	ULONG          FastReadNotPossible;        //快速读失败数
	ULONG          FastMdlReadNoWait;          //异步MDL快速读数目
	ULONG          FastMdlReadWait;            //同步MDL快速读数目
	ULONG          FastMdlReadResourceMiss;    //MDL读资源冲突数
	ULONG          FastMdlReadNotPossible;     //MDL读失败数
	ULONG          MapDataNoWait;              //异步映射数据次数
	ULONG          MapDataWait;                //同步映射数据次数
	ULONG          MapDataNoWaitMiss;          //异步映射数据冲突次数
	ULONG          MapDataWaitMiss;            //同步映射数据冲突次数
	ULONG          PinMappedDataCount;         //牵制映射数据数目
	ULONG          PinReadNoWait;              //牵制异步读数目
	ULONG          PinReadWait;                //牵制同步读数目
	ULONG          PinReadNoWaitMiss;          //牵制异步读冲突数目
	ULONG          PinReadWaitMiss;            //牵制同步读冲突数目
	ULONG          CopyReadNoWait;             //异步拷贝读次数
	ULONG          CopyReadWait;               //同步拷贝读次数
	ULONG          CopyReadNoWaitMiss;         //异步拷贝读故障次数
	ULONG          CopyReadWaitMiss;           //同步拷贝读故障次数
	ULONG          MdlReadNoWait;              //异步MDL读次数
	ULONG          MdlReadWait;                //同步MDL读次数
	ULONG          MdlReadNoWaitMiss;          //异步MDL读故障次数
	ULONG          MdlReadWaitMiss;            //同步MDL读故障次数
	ULONG          ReadAheadIos;               //向前读操作数目
	ULONG          LazyWriteIos;               //LAZY写操作数目
	ULONG          LazyWritePages;             //LAZY写页文件数目
	ULONG          DataFlushes;                //缓存刷新次数
	ULONG          DataPages;                  //缓存刷新页数
	ULONG          ContextSwitches;            //环境切换数目
	ULONG          FirstLevelTbFills;          //第一层缓冲区填充次数
	ULONG          SecondLevelTbFills;         //第二层缓冲区填充次数
	ULONG          SystemCall;                 //系统调用次数
}SYSTEM_PERFORMANCE_INFORMATION,*PSYSTEM_PERFORMANCE_INFORMATION;

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
	DWORD			dwWorkingSetSize;						// 工作设置(内存)
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

typedef struct tagSYSTEM_PAGEFILE_INFORMATION
{
	ULONG NetxEntryOffset;                //下一个结构的偏移量
	ULONG CurrentSize;                    //当前页文件大小
	ULONG TotalUsed;                      //当前使用的页文件数
	ULONG PeakUsed;                       //当前使用的页文件峰值数
	UNICODE_STRING FileName;              //页文件的文件名称
}SYSTEM_PAGEFILE_INFORMATION,*PSYSTEM_PAGEFILE_INFORMATION;


typedef struct tagSYSTEM_CACHE_INFORMATION
{
	ULONG SystemCacheWsSize;              //高速缓存大小
	ULONG SystemCacheWsPeakSize;          //高速缓存峰值大小
	ULONG SystemCacheWsFaults;            //高速缓存页故障数目
	ULONG SystemCacheWsMinimum;           //高速缓存最小页大小
	ULONG SystemCacheWsMaximum;           //高速缓存最大页大小
	ULONG TransitionSharedPages;          //共享页数目
	ULONG TransitionSharedPagesPeak;      //共享页峰值数目
	ULONG Reserved[2];
}SYSTEM_CACHE_INFORMATION,*PSYSTEM_CACHE_INFORMATION;

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
