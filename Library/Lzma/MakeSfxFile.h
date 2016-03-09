#pragma once
#include <vector>

typedef std::vector<IMAGE_SECTION_HEADER*>   CImageSectionHeader;
typedef struct tagIMAGE_SECTION_INFO
{
	LPBYTE lpData;
	UINT     nLen;
}IMAGE_SECTION_INFO,*PIMAGE_SECTION_INFO;
typedef std::vector<IMAGE_SECTION_INFO> CImageData;

//��ѹ�ļ���ѡ��
typedef enum tagUNPACKAGE_OPTION
{
	UNPACKAGE_OPTION_TEMP = 0,							//��ѹ����ʱĿ¼
	UNPACKAGE_OPTION_DESKTOP = 1,					//��ѹ����ǰ����
	UNPACKAGE_OPTION_DOCUMENT = 2,				//��ѹ���ҵ��ĵ�
	UNPACKAGE_OPTION_LOCALTION = 3,				//��ѹ����ǰĿ¼
	UNPACKAGE_OPTION_SELF_SELECT = 4,				//�û��Լ�ѡ��
	UNPACKAGE_OPTION_ENVIRONMENT = 5,			//ʹ�û�������
}UNPACKAGE_OPTION;

//�߼�ѡ��
#define	ONE_INSTANCE_CHECK		0x1		//��ʵ��ģʽ
#define    SILENCE_MODE					0x2		//��Ĭģʽ
#define	 SELF_DELETE_MODE			0x4		//�Ի�ģʽ

//UAC���
typedef enum tagUAC_OPTION
{
	UAC_OPTION_NONE = 0,			//��
	UAC_OPTION_TOPMOST = 1,	//����߿���Ȩ������
	UAC_OPTION_ADMIN = 2,			//�Թ���ԱȨ������
}UAC_OPTION;

//����Ŀ¼
typedef enum tagWORK_DIRECTORY
{
	WORK_DIRECTORY_NONE = 0,										//���޸�
	WORK_DIRECTORY_UNPACKAGE_FOLDER = 1,				//�ͷ��ļ�Ŀ¼
	WORK_DIRECTORY_SFXMAKER_LOCALTION = 2,			//�Խ�ѹ�ļ�Ŀ¼
}WORK_DIRECTORY;

//����ɾ���ͷ��ļ�
typedef enum tagDELETE_UNPACKAGE
{
	DELETE_UNPACKAGE_NONE = 0,
	DELETE_UNPACKAGE_EXIST = 1,
	DELETE_UNPACKAGE_REBOOT = 2,
}DELETE_UNPACKAGE;

//��ѹ��Ĳ���
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
	UNPACKAGE_OPERATE UnpackageOperate;		//��ѹ����
	UNPACKAGE_OPTION   UnpackageOption;		//��ѹѡ����λ��
	DWORD   dwRunMode;										//����ʱ��鴦��
	UAC_OPTION  UacOption;									//Uac��Ȩ��Ŀǰ��δʹ��
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
	DWORD   m_dwMiniPointer;			//���ε�һ�����������ʼλ��
	DWORD   m_dwEmptyDataLen;		//�����������Ҫ��ӵĿ����ݵĳ���
	DWORD   m_dwSectionDataLen;	//ԭʼ�������ݳ���
	CImageData		m_SectionData;		//�����������������
	LPBYTE				m_pDosHeader;		//Դ�ļ�Dosͷ��Dos Stubͷ
	LPBYTE				m_pSectionData;		//Ԫ��������������
	IMAGE_NT_HEADERS    m_NtHeader;	//Դ�ļ�PEͷ�ṹ
	CImageSectionHeader	m_ImageSectionHeader;	//�ļ�������Ϣ�������������
};