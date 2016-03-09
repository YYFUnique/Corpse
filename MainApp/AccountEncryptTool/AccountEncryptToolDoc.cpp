
// AccountEncryptToolDoc.cpp : CAccountEncryptToolDoc ���ʵ��
//

#include "stdafx.h"
#include "AccountEncryptTool.h"
#include "AccountEncryptToolDoc.h"
#include "AccountEncryptToolView.h"

#include "LsCommon/ErrorInfo.h"
#include "LsCommon/Hash/Hash.h"

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAccountEncryptToolDoc

IMPLEMENT_DYNCREATE(CAccountEncryptToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CAccountEncryptToolDoc, CDocument)
END_MESSAGE_MAP()


// CAccountEncryptToolDoc ����/����

CAccountEncryptToolDoc::CAccountEncryptToolDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CAccountEncryptToolDoc::~CAccountEncryptToolDoc()
{
}

BOOL CAccountEncryptToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}

BOOL CAccountEncryptToolDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (CDocument::OnOpenDocument(lpszPathName) == FALSE)
		return FALSE;


	if (GetEncrytedFileContent(lpszPathName,m_EncrytpFileType) == FALSE)
		return FALSE;

	return TRUE;
}

BOOL CAccountEncryptToolDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (CDocument::OnSaveDocument(lpszPathName) == FALSE)
		return FALSE;

	return WriteEncryptFile(lpszPathName);
}


// CAccountEncryptToolDoc ���л�

void CAccountEncryptToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CAccountEncryptToolDoc ���

#ifdef _DEBUG
void CAccountEncryptToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAccountEncryptToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAccountEncryptToolDoc ����
CView* CAccountEncryptToolDoc::GetView(CRuntimeClass* pClass)
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		pView = GetNextView(pos);
		if (pView -> IsKindOf(pClass))
			break;
		else
			pView = NULL;
	}
	return pView;
}

BOOL CAccountEncryptToolDoc::WriteEncryptFile(LPCTSTR lpszPathName)
{
	CString strTestContent = _T("testtest");
	
	m_EncrytpFileType.lpszFileType = _T("Account");
	m_EncrytpFileType.dwFileVersion = htonl(1);
	m_EncrytpFileType.dwBodySize = strTestContent.GetLength()*sizeof(TCHAR);
	m_EncrytpFileType.dwFileCRC = htonl(0);
	m_EncrytpFileType.lpszEncryptPwd = _T("abc");
	m_EncrytpFileType.lpszFileContent = strTestContent;

	HANDLE hFile = CreateFile(lpszPathName,GENERIC_WRITE,FILE_SHARE_DELETE|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("���ļ�%sʧ��"),lpszPathName);
		return FALSE;
	}

	DWORD dwFileHeadLen = (_tcslen(m_EncrytpFileType.lpszFileType)+1) * sizeof(TCHAR) +
		3*sizeof(DWORD) + (_tcslen(m_EncrytpFileType.lpszEncryptPwd)+1) * sizeof(TCHAR);
	DWORD dwDataLen = (_tcslen(m_EncrytpFileType.lpszFileContent)+1) * sizeof(TCHAR);

	BYTE* pFileBuffer = new BYTE[dwDataLen + dwFileHeadLen];

	DWORD dwOffset = 0;
	do 
	{
		dwOffset = (DWORD)(_tcslen(m_EncrytpFileType.lpszFileType)+1)*sizeof(TCHAR);
		_tcscpy_s((TCHAR*)pFileBuffer,dwOffset,m_EncrytpFileType.lpszFileType);
	
		*(DWORD*)(pFileBuffer+dwOffset) = m_EncrytpFileType.dwFileVersion;
		dwOffset += sizeof(DWORD);

		*(DWORD*)(pFileBuffer+dwOffset) = m_EncrytpFileType.dwBodySize;
		dwOffset += sizeof(DWORD);

		CHash::GetHash(HASH_TYPE_CRC32,pFileBuffer+dwFileHeadLen,dwDataLen,pFileBuffer+dwOffset);
		dwOffset+=sizeof(DWORD);

		DWORD dwPwdLen = (DWORD)(_tcslen(m_EncrytpFileType.lpszEncryptPwd)+1)*sizeof(TCHAR);
		_tcscpy_s((TCHAR*)((LPBYTE)pFileBuffer+dwOffset),dwPwdLen,m_EncrytpFileType.lpszEncryptPwd);
		dwOffset += dwPwdLen;

		_tcscpy_s((TCHAR*)((LPBYTE)pFileBuffer+dwFileHeadLen),dwDataLen,m_EncrytpFileType.lpszFileContent);
	} while (FALSE);

	DWORD dwWriteByte = 0;
	WriteFile(hFile,pFileBuffer,dwFileHeadLen+dwDataLen,&dwWriteByte,NULL);
	if (pFileBuffer != NULL)
	{
		delete[] pFileBuffer;
		pFileBuffer = NULL;
	}
	
	return CloseHandle(hFile);
}

BOOL CAccountEncryptToolDoc::GetEncrytedFileContent(LPCTSTR lpszFileName,ENCRYPT_FILE_TYPE & EncryptFileType)
{
	if (PathFileExists(lpszFileName) == FALSE)
	{
		SetErrorInfo(SYSTEM_ERROR,0,_T("��ȡ�ļ�����ʧ��"));
		return FALSE;
	}

	//EncryptFileType.lpszFileContent
	GROUP_TYPE GroupType;
	CGroupTypeInfo GroupTypeArray;
	GetGroupTypeInfo(GroupType,GroupTypeArray);

	CAccountEncryptToolView* pView = (CAccountEncryptToolView*)GetView(RUNTIME_CLASS(CAccountEncryptToolView));
	POSITION pos = GroupTypeArray.GetHeadPosition();
	int n=0;
	while(pos)
	{
		GROUP_TYPE_INFO& GroupTypeInfo = GroupTypeArray.GetNext(pos);
		pView->AddGroup(GroupTypeInfo.lpszGroupName,n++);
	}
	
	CAccountInfo AccountType;
	GetGroupInfo(AccountType);
	pos = AccountType.GetHeadPosition();
	while(pos)
	{
		ACCOUNT_INFO& AccountInfo = AccountType.GetNext(pos);
		pView->AddItemToGroup(AccountInfo);
	}

	pView->AdjustListWidth();
	return TRUE;
}

BOOL CAccountEncryptToolDoc::GetGroupTypeInfo(GROUP_TYPE& GroupType,CGroupTypeInfo& GroupTypeInfoArray)
{
	GroupType.dwGroupCount = 2;
	GroupType.dwUpdate = 1391236202;

	GROUP_TYPE_INFO GroupTypeInfo;
	GroupTypeInfo.lpszGroupName = _T("��վ");
	GroupTypeInfoArray.AddTail(GroupTypeInfo);
	GroupTypeInfo.lpszGroupName = _T("��Ϸ");
	GroupTypeInfoArray.AddTail(GroupTypeInfo);
	GroupTypeInfo.lpszGroupName = _T("Ӧ��");
	GroupTypeInfoArray.AddTail(GroupTypeInfo);

	return TRUE;
}

BOOL CAccountEncryptToolDoc::GetGroupInfo(CAccountInfo& AccountTypeInfo)
{
	ACCOUNT_INFO AccountInfo;
	AccountInfo.dwGroupId = 0;
	AccountInfo.lpszItmeName = _T("�Ա���");
	AccountInfo.lpszMgrPath = _T("www.taobao.com");
	AccountInfo.lpszItemIconPath = NULL;
	AccountInfo.lpszExtendInfo = NULL;
	AccountTypeInfo.AddTail(AccountInfo);

	AccountInfo.dwGroupId = 0;
	AccountInfo.lpszItmeName = _T("�ٶ���");
	AccountInfo.lpszMgrPath = _T("www.baidu.com");
	AccountInfo.lpszItemIconPath = NULL;
	AccountInfo.lpszExtendInfo = NULL;
	AccountTypeInfo.AddTail(AccountInfo);

	AccountInfo.dwGroupId = 1;
	AccountInfo.lpszItmeName = _T("CrossFire(��Խ����)");
	AccountInfo.lpszMgrPath = _T("C:\\");
	AccountInfo.lpszItemIconPath = NULL;
	AccountInfo.lpszExtendInfo = NULL;
	AccountTypeInfo.AddTail(AccountInfo);

	return TRUE;
}