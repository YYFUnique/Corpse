
// AccountEncryptToolDoc.h : CAccountEncryptToolDoc ��Ľӿ�
//


#pragma once


typedef struct _ENCRYPT_FILE_TYPE
{
	LPCTSTR	lpszFileType;
	DWORD	dwFileVersion;
	DWORD	dwBodySize;
	DWORD	dwFileCRC;
	LPCTSTR	lpszEncryptPwd;
	LPCTSTR	lpszFileContent;
}ENCRYPT_FILE_TYPE,*PENCRYPT_FILE_TYPE;

typedef struct _GROUP_TYPE
{
	DWORD dwUpdate;			//�������ʱ��
	DWORD dwGroupCount;	//������
}GROUP_TYPE,*PGROUP_TYPE;

typedef struct _GROUP_TYPE_INFO
{
	LPCTSTR lpszGroupName;
}GROUP_TYPE_INFO,*PGROUP_TYPE_INFO;

typedef struct _ACCOUNT_TYPE
{
	DWORD dwUpdateTime;
	DWORD dwItem;
}ACCOUNT_TYPE,*PACCOUNT_TYPE;

typedef struct _ACCOUNT_INFO
{
	DWORD	dwGroupId;				//�����ĸ�����
	LPCTSTR	lpszItmeName;		//��ʾ��
	LPCTSTR	lpszItemIconPath;	//ͼ��·��
	LPCTSTR  lpszMgrPath;			//�����ַ
	LPCTSTR  lpszExtendInfo;		//��չ��Ϣ
}ACCOUNT_INFO,*PACCOUNT_INFO;

typedef CList<GROUP_TYPE_INFO,GROUP_TYPE_INFO&> CGroupTypeInfo;
typedef CList<ACCOUNT_INFO,ACCOUNT_INFO&> CAccountInfo;

class CAccountEncryptToolDoc : public CDocument
{
protected: // �������л�����
	CAccountEncryptToolDoc();
	DECLARE_DYNCREATE(CAccountEncryptToolDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
	BOOL WriteEncryptFile(LPCTSTR lpszPathName);
	BOOL GetEncrytedFileContent(LPCTSTR lpszFileName,ENCRYPT_FILE_TYPE & EncryptFileType);
	CView* GetView(CRuntimeClass* pClass);
// ʵ��
public:
	virtual ~CAccountEncryptToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	ENCRYPT_FILE_TYPE m_EncrytpFileType;
	BOOL GetGroupTypeInfo(GROUP_TYPE& GroupType,CGroupTypeInfo& GroupTypeInfo);
	BOOL GetGroupInfo(CAccountInfo& AccountTypeInfo);
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


