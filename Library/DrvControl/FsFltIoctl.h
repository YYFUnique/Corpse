#pragma once

#pragma pack(push)
#pragma pack(8)

//设备类型
typedef enum tagDEVICE_TYPE_INFO
{
	DEVICE_TYPE_USB_STORAGE		=	0,
	DEVICE_TYPE_CD_ROM				=	1,
	DEVICE_TYPE_FLOPPY					=	2,
	DEVICE_TYPE_NETWORK			=	3,
	DEVICE_TYPE_DISK						=	4,
	DEVICE_TYPE_MAX						=	5,//最大值
	DEVICE_TYPE_OTHER					= 0x80,
}DEVICE_TYPE_INFO, *PDEVICE_TYPE_INFO;

// 过滤驱动隐藏文件的匹配方式
typedef enum tagMATCH_MODE
{
	MATCH_MODE_PRE			=	0,				// 仅匹配文件夹/文件名前缀
	MATCH_MODE_SUB		=	1,				// 仅匹配文件夹/文件名包含子串
	MATCH_MODE_NAME	=	2,				// 仅匹配文件夹/文件名
	MATCH_MODE_ALL			=	3,				// 匹配完整路径
}MATCH_MODE;

//驱动返回给应用层版本号信息
#define FS_FLT_DRV_VERSION				1

//访问命令字
typedef enum tagFS_FLT_COMMAND
{
	FS_FLT_GET_FLT_ENABLE = 1,									// 获取驱动过滤功能是否开启，输入为FS_FLT_COMMAND，输出为BOOL
	FS_FLT_SET_FLT_ENABLE,											// 设置驱动过滤功能是否开启，输入为SET_FLT_ENABLE_INPUT，输出为空
	FS_FLT_GET_DRV_VERSION = 3,									// 获取过滤驱动版本号，输入为FS_FLT_COMMAND, 输出为DWORD
	FS_FLT_GET_CAN_STOP_DRV,									// 获取是否能够停止驱动
	FS_FLT_SET_CAN_STOP_DRV,
	//FS_FLT_GET_DEVICE_ACCESS_RIGHTS,						// 获取(USB、光盘、软盘)存储设备访问权限，输入为LS_FS_FLT_COMMAND，输出为LS_FLT_DEVICE_ACCESS_RIGHTS
	//FS_FLT_SET_DEVICE_ACCESS_RIGHTS,						// 设置(USB、光盘、软盘)存储设备访问权限，输入为LS_FLT_DEVICE_ACCESS_RIGHTS，输出为空
	//FS_FLT_GET_FILE_OPERATION_LOG_COUNT,			//获取文件操作日志条数，输入为LS_FS_FLT_COMMAND，输出为ULONG
	//FS_FLT_GET_FILE_OPERATION_LOG,							//获取文件操作日志，输入为LS_FS_FLT_COMMAND，输出为LS_FILE_OPERATION_LOG_APP
	//FS_FLT_CLEAR_FILE_OPERATION_LOG,						//清除文件操作日志，输入为LS_FS_FLT_COMMAND，输出为空
	//FLT_SET_FILE_PATH_RULE,											//设置文件指定路径访问规则,最小输入为DWORD，输出为空
	//FS_FLT_SET_LANGUARD_PROCESS_ID,						//设置终端前台进程ID，输入为FSFLT_LANGUARD_PROCESS_ID , 输出为空
	//FS_FLT_SET_LS_CD_BURN_PROCESS_ID,					//设置龙信光盘刻录进程ID，输入为FSFLT_LANGUARD_PROCESS_ID，输出为空
	//FS_FLT_SET_ENABLE_CD_BURN,									//设置是否允许光盘刻录，输入为SET_FUNCTION_ENABLE，输出为空
	FS_FLT_GET_ENABLE_USB_DISK_EXECUTE,				//获取是否允许光盘刻录，输入为LS_FS_FLT_COMMAND，输出为BOOL
	FS_FLT_SET_ENABLE_USB_DISK_EXECUTE,					//设置是否允许光盘刻录，输入为SET_FUNCTION_ENABLE，输出为空
	FS_FLT_GET_PATH_FILE_RULE_COUNT,						// 获取过滤驱动隐藏文件信息规则条数
	FS_FLT_GET_PATH_FILE_INFO,										// 获取过滤驱动隐藏文件信息
	FS_FLT_SET_PATH_FILE_INFO,										// 设置过滤驱动隐藏文件信息
}FS_FLT_COMMAND;

//设置驱动过滤功能是否开启(驱动总开关)
typedef struct tagSET_FLT_ENABLE_INPUT
{
	FS_FLT_COMMAND FltCommand;
	BOOL bFltEnable;
}SET_FLT_ENABLE_INPUT,*PSET_FLT_ENABLE_INPUT;

typedef struct tagFS_FLT_COMMAND_SIZE
{
	FS_FLT_COMMAND FltCommand;
	ULONG dwMinInputBufferSize;
	ULONG dwMinOutputBufferSize;
}FS_FLT_COMMAND_SIZE, *PFS_FLT_COMMAND_SIZE;

typedef struct tagFS_FLT_LANGUARD_PROCESS_ID
{
	FS_FLT_COMMAND FltCommand;
	ULONG dwLanguardProcessId;//规则条数
}FS_FLT_LANGUARD_PROCESS_ID, *PFS_FLT_LANGUARD_PROCESS_ID;

//设置驱动过滤功能是否可以在U盘运行可执行程序
typedef struct tagSET_FUNCTION_ENABLE
{
	FS_FLT_COMMAND FltCommand;
	BOOL bEnable;
}SET_FUNCTION_ENABLE, *PSET_FUNCTION_ENABLE;

//文件指定路径
typedef struct tagUSER_FILE_PATH_RULE_ITEM
{
	//文件路径,如:C:\Docume~1\MyUser\My Documents\Test.txt (程序自动转换为设备名称如,C:\-->\Device\HarddiskVolume1\)
	WCHAR				wszFilePath[MAX_PATH]; 
	BOOL					bEnable;
	MATCH_MODE	MatchMode;
}USER_FILE_PATH_RULE_ITEM,*PUSER_FILE_PATH_RULE_ITEM;

// 过滤驱动隐藏文件信息
typedef struct tagUSER_FILE_PATH_RULE
{
	ULONG dwRuleItemCount;						//规则条数
	USER_FILE_PATH_RULE_ITEM FilePathRuleItem[1];
}USER_FILE_PATH_RULE, *PUSER_FILE_PATH_RULE;

// 设置过滤驱动隐藏文件信息
typedef struct tagSET_USER_FILE_PATH_RULE
{
	FS_FLT_COMMAND FltCommand;
	ULONG dwRuleItemCount;						//规则条数
	USER_FILE_PATH_RULE_ITEM FilePathRuleItem[1];
}SET_USER_FILE_PATH_RULE, *PSET_USER_FILE_PATH_RULE;

//每种命令字对应的缓冲区大小
const static FS_FLT_COMMAND_SIZE sFsFltCommandSize[]=
{
	{FS_FLT_GET_FLT_ENABLE , 0 , sizeof(BOOL)} ,																			// 获取过滤驱动是否开启
	{FS_FLT_SET_FLT_ENABLE , sizeof(SET_FLT_ENABLE_INPUT) , 0},												// 设置过滤驱动开启过滤功能
	{FS_FLT_GET_DRV_VERSION, 0, sizeof(DWORD)},																		// 获取过滤驱动版本
	{FS_FLT_GET_CAN_STOP_DRV , 0 , sizeof(BOOL)} ,																	// 获取是否可以停止过滤驱动
	{FS_FLT_SET_CAN_STOP_DRV, sizeof(SET_FUNCTION_ENABLE), 0},											// 设置过滤驱动是否可以停止
	/*{FS_FLT_SET_LANGUARD_PROCESS_ID , sizeof(FS_FLT_LANGUARD_PROCESS_ID)},
	{FS_FLT_SET_LS_CD_BURN_PROCESS_ID , sizeof(FS_FLT_LANGUARD_PROCESS_ID)},
	{FS_FLT_SET_ENABLE_CD_BURN , sizeof(SET_FUNCTION_ENABLE)},*/
	{FS_FLT_GET_ENABLE_USB_DISK_EXECUTE , 0 , sizeof(BOOL)},													// 获取是否可以在U盘执行应用程序
	{FS_FLT_SET_ENABLE_USB_DISK_EXECUTE , sizeof(SET_FUNCTION_ENABLE) , 0},					// 设置是否可以在U盘执行应用程序
	{FS_FLT_GET_PATH_FILE_RULE_COUNT, 0, sizeof(ULONG)},														// 获取过滤驱动隐藏文件规则条数
	{FS_FLT_GET_PATH_FILE_INFO, 0, sizeof(USER_FILE_PATH_RULE)},											// 获取过滤驱动隐藏文件信息
	{FS_FLT_SET_PATH_FILE_INFO, offsetof(SET_USER_FILE_PATH_RULE, FilePathRuleItem), 0},	// 设置过滤驱动隐藏文件信息
};
#pragma pack(pop)
