#pragma once

#include <afxtempl.h>
/////////////////////////////////////////////////////////////////////////////
// CFileDataItem window

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class LS_COMMON_API CFileDataItem
{
// Construction
public:
	CFileDataItem();

// Attributes
public:

//==================================================================
//�� �� ��: GetFileItemCount
//��    ��: ��ȡ��������
//�������: ���������
//�� �� ֵ: ������������
//==================================================================
	DWORD GetFileItemCount();

//==================================================================
//�� �� ��: SetDataItem
//��    ��: ��������������,��С�������������������������������.
//			����Ѵ��ھ�����ͬ���������͵������������ø��������ֵ
//�������: [in] ����������
//			[in] �������
//			[in] �����������׵�ַ
//�� �� ֵ: ��ӻ���������������ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL SetDataItem(DWORD dwItemType,DWORD dwDataLen,const void* pDataBuffer);

//==================================================================
//�� �� ��: SetDataItem
//��    ��: ��������������,��С�������������������������������.
//			����Ѵ��ھ�����ͬ���������͵������������ø��������ֵ
//�������: [in] ����������
//			[in] ����������
//�� �� ֵ: ��ӻ���������������ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL SetDataItem(DWORD dwItemType,DWORD dwValue);

//==================================================================
//�� �� ��: GetDataItem
//��    ��: ��������������,��ȡFILE_DATA_ITEM����Ӧ�����ݳ��Ⱥ����ݻ����׵�ַ
//�������: [in]  dwItemType,����������
//			[out] dwDataLen, ���ݳ���
//			[out] pDataBuffer, ���ݻ����׵�ַ
//�� �� ֵ: ��ȡָ��������ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetDataItem(DWORD dwItemType,DWORD& dwDataLen,const void*& pDataBuffer);

//==================================================================
//�� �� ��: GetAllDataBuffer
//��    ��: ��ȡ������������������Ϣ,�����ο��뻺����.
//			�������ݻ���ΪNULLʱ,Ϊ��ȡ����������Ϣ���ڴ��С
//�������: [out] pDataBuffer, ���ݻ��汣����������������������Ϣ
//			[in,out]  dwDataLen,���ݻ����С
//�� �� ֵ: ��ȡ��������������������Ϣ�ɹ�����TRUE,���򷵻�FALSE
//==================================================================
	BOOL GetAllDataBuffer(void* pDataBuffer,DWORD& dwDataLen);

//==================================================================
//�� �� ��: EnumDataItem
//��    ��: ��ȡ����������ָ�����������͵���һ��������Ϣ.
//			��ָ������������Ϊ-1,��Ϊ��ȡ���������е�һ�����ݵ���Ϣ.
//�������: [in,out] dwItemType, ����ָ������������,������ȡ��������������
//			[out]    dwDataLen, ��ȡ�������ݳ���
//			[out]    pDataBuffer, ��ȡ�������ݻ����׵�ַ
//�� �� ֵ: ��ȡ�ɹ�����TURE,ʧ�ܷ���FALSE
//==================================================================
	BOOL EnumDataItem(DWORD& dwItemType,DWORD& dwDataLen,const void*& pDataBuffer);

//==================================================================
//�� �� ��: WriteDataToFile
//��    ��: ����������������д��ָ���ļ�
//�������: [in] lpcszFileName,д���ļ�ȫ·����
//�� �� ֵ: д��ɹ�����TURE,ʧ�ܷ���FALSE
//==================================================================
	BOOL WriteDataToFile(LPCTSTR lpcszFileName);

//==================================================================
//�� �� ��: ClearAllDataItem
//��    ��: ɾ����������������������
//�������: ���������
//�� �� ֵ: �޷���ֵ
//==================================================================
	void ClearAllDataItem();

//==================================================================
//�� �� ��: RemoveDataItem
//��    ��: ɾ������ָ�����������͵�������
//�������: [in] dwItemType,����������
//�� �� ֵ: �޷���ֵ
//==================================================================
	void RemoveDataItem(DWORD dwItemType);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileDataItem)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileDataItem();

	// Generated message map functions
protected:
	typedef struct _FILE_DATA_ITEM
	{
		DWORD dwItemType;
		DWORD dwDataLen;
		void* pDataBuffer;
	}FILE_DATA_ITEM,*PFILE_DATA_ITEM;
	typedef CList<FILE_DATA_ITEM,FILE_DATA_ITEM&> CFileDataItemList;
	CFileDataItemList m_FileDataItemList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
