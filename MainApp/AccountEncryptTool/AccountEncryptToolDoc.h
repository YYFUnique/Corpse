
// AccountEncryptToolDoc.h : CAccountEncryptToolDoc 类的接口
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
	DWORD dwUpdate;			//分组更新时间
	DWORD dwGroupCount;	//分组数
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
	DWORD	dwGroupId;				//属于哪个分组
	LPCTSTR	lpszItmeName;		//显示名
	LPCTSTR	lpszItemIconPath;	//图标路径
	LPCTSTR  lpszMgrPath;			//管理地址
	LPCTSTR  lpszExtendInfo;		//扩展信息
}ACCOUNT_INFO,*PACCOUNT_INFO;

typedef CList<GROUP_TYPE_INFO,GROUP_TYPE_INFO&> CGroupTypeInfo;
typedef CList<ACCOUNT_INFO,ACCOUNT_INFO&> CAccountInfo;

class CAccountEncryptToolDoc : public CDocument
{
protected: // 仅从序列化创建
	CAccountEncryptToolDoc();
	DECLARE_DYNCREATE(CAccountEncryptToolDoc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
	BOOL WriteEncryptFile(LPCTSTR lpszPathName);
	BOOL GetEncrytedFileContent(LPCTSTR lpszFileName,ENCRYPT_FILE_TYPE & EncryptFileType);
	CView* GetView(CRuntimeClass* pClass);
// 实现
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
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};


