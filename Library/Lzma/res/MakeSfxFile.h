#pragma once
#include <vector>

typedef std::vector<IMAGE_SECTION_HEADER*>   CImageSectionHeader;
typedef struct tagIMAGE_SECTION_INFO
{
	LPBYTE lpData;
	UINT     nLen;
}IMAGE_SECTION_INFO,*PIMAGE_SECTION_INFO;
typedef std::vector<IMAGE_SECTION_INFO> CImageData;

//解压文件夹选项
typedef enum tagUNPACKAGE_OPTION
{
	UNPACKAGE_OPTION_TEMP = 0,							//解压到临时目录
	UNPACKAGE_OPTION_DESKTOP = 1,					//解压到当前桌面
	UNPACKAGE_OPTION_DOCUMENT = 2,				//解压到我的文档
	UNPACKAGE_OPTION_LOCALTION = 3,				//解压到当前目录
	UNPACKAGE_OPTION_SELF_SELECT = 4,				//用户自己选择
	UNPACKAGE_OPTION_ENVIRONMENT = 5,			//使用环境变量
}UNPACKAGE_OPTION;

//高级选项
#define	ONE_INSTANCE_CHECK		0x1		//单实例模式
#define    SILENCE_MODE					0x2		//静默模式
#define	 SELF_DELETE_MODE			0x4		//自毁模式

//UAC相关
typedef enum tagUAC_OPTION
{
	UAC_OPTION_NONE = 0,			//无
	UAC_OPTION_TOPMOST = 1,	//以最高可用权限运行
	UAC_OPTION_ADMIN = 2,			//以管理员权限运行
}UAC_OPTION;

//工作目录
typedef enum tagWORK_DIRECTORY
{
	WORK_DIRECTORY_NONE = 0,										//不修改
	WORK_DIRECTORY_UNPACKAGE_FOLDER = 1,				//释放文件目录
	WORK_DIRECTORY_SFXMAKER_LOCALTION = 2,			//自解压文件目录
}WORK_DIRECTORY;

//关于删除释放文件
typedef enum tagDELETE_UNPACKAGE
{
	DELETE_UNPACKAGE_NONE = 0,
	DELETE_UNPACKAGE_EXIST = 1,
	DELETE_UNPACKAGE_REBOOT = 2,
}DELETE_UNPACKAGE;

//解压后的操作
typedef enum tagUNPACKAGE_OPERATE
{
	UNPACKAGE_OPERATE_OPENDIRECTORY = 0,
	UNPACKAGE_OPERATE_RUN = 1,
}UNPACKAGE_OPERATE;

typedef struct tagOPERATE_RUN
{
	TCHAR szFileName[MAX_PATH];
	TCHAR szParam[MAX_PATH];
	WORK_DIRECTORY  WorkDirectory;
	DELETE_UNPACKAGE DeleteUnpackage;
}OPERATE_RUN;

typedef struct tagUNPACKAGE_OPERATE_INFO
{
	union{
		BOOL bOpenDirectory;
		OPERATE_RUN OperateRun;
	};
}UNPACKAGE_OPERATE_INFO,*PUNPACKAGE_OPERATE_INFO;

typedef struct tagSFX_PACKAGE_INFO 
{
	UNPACKAGE_OPERATE UnpackageOperate;		//解压操作
	UNPACKAGE_OPTION   UnpackageOption;		//解压选项，存放位置
	DWORD   dwRunMode;										//运行时检查处理
	UAC_OPTION  UacOption;									//Uac提权，目前暂未使用
}SFX_PACKAGE_INFO,*PSFX_PACKAGE_INFO;


#define		SECTION_BLOCK_NAME				".Code"
#define		SECTION_BLOCK_DATA				".Data"

typedef enum tagSECTION_TYPE
{
	SECTION_TYPE_CODE = 0,
	SECTION_TYPE_DATA = 1,
}SECTION_TYPE;

#ifdef LS_STATIC_LIB_CALL
#define LZMA_API
#elif defined LZMA_EXPORTS
#define LZMA_API __declspec(dllexport)
#else
#define LZMA_API __declspec(dllimport)
#endif

class LZMA_API CMakeSfxFile
{
public:
	CMakeSfxFile();
	~CMakeSfxFile();

public:
	BOOL OpenTargetFile(LPCTSTR lpszTargetFileName);
	BOOL AppendData(LPBYTE lpData,UINT nLen,SECTION_TYPE SectionType = SECTION_TYPE_CODE);
	BOOL ReleaseData();
	BOOL MakeBinaryFile(LPCTSTR lpszBinaryFile);
protected:
	void ChangeSectionsPointer(DWORD dwNumberOfSections);
	void ChangeSectionOffset(DWORD dwNumberOfSections);
	BOOL FillSections(PIMAGE_SECTION_HEADER pImageSectionHeader,UINT nLen,LPCSTR lpszSectionName);
	void ReleaseFiles(LPVOID lpImageBase,PIMAGE_SECTION_HEADER pSectionBlock,PIMAGE_SECTION_HEADER pImageSectionData);
	void PackageRunMode(PUNPACKAGE_OPERATE_INFO pOperateInfo,LPCTSTR lpszPackagePath);
protected:
	DWORD   m_dwMiniPointer;			//区段第一块数据项的起始位置
	DWORD   m_dwEmptyDataLen;		//在区表后面需要添加的空数据的长度
	DWORD   m_dwSectionDataLen;	//原始区段数据长度
	CImageData		m_SectionData;		//新添加区段数据内容
	LPBYTE				m_pDosHeader;		//源文件Dos头和Dos Stub头
	LPBYTE				m_pSectionData;		//元素区段数据内容
	IMAGE_NT_HEADERS    m_NtHeader;	//源文件PE头结构
	CImageSectionHeader	m_ImageSectionHeader;	//文件区段信息，包含添加区段
};