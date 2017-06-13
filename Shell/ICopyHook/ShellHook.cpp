#include "StdAfx.h"
#include "ShellHook.h"

extern ULONG      g_lLockNumber;
extern ULONG      g_lRefThisDll;

CShellHook::CShellHook() : m_lRefCount(0)
{
	g_lRefThisDll++;
}

CShellHook::~CShellHook()
{
	g_lRefThisDll--;
}


HRESULT STDMETHODCALLTYPE CShellHook::QueryInterface (REFIID iid, void **ppvObject)
{
	if(iid == IID_IUnknown)
	{
		m_lRefCount++;
		*ppvObject = this;
	}
	else if(iid == IID_IShellCopyHook)
	{
		m_lRefCount++;
		*ppvObject = (ICopyHook *)this;
	}
	else if(iid == IID_IShellExecuteHook)
	{
		m_lRefCount++;
		*ppvObject = (IShellExecuteHook *)this;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	return S_OK;
}

ULONG STDMETHODCALLTYPE CShellHook::AddRef ()
{
	m_lRefCount++;
	return m_lRefCount;
}

ULONG STDMETHODCALLTYPE CShellHook::Release ()
{
	m_lRefCount--;

	if(m_lRefCount == 0)
		delete this;

	return m_lRefCount;
}

UINT STDMETHODCALLTYPE CShellHook::CopyCallback (HWND hwnd, UINT wFunc, UINT wFlags, LPCTSTR pszSrcFile, DWORD dwSrcAttribs,
												   LPCTSTR pszDestFile, DWORD dwDestAttribs)
{
	TCHAR  szMsg[1024];
	UINT nID = IDYES;

	if (wFunc == FO_COPY)
	{
		wsprintf(szMsg, _T("�Ƿ�����Ŀ¼�� %s �����ļ��� %s ?"), pszSrcFile, pszDestFile);
		nID = MessageBox(NULL, szMsg, _T("��ʾ"), MB_YESNO | MB_ICONQUESTION| MB_SETFOREGROUND);
	}

	return nID;
}

HRESULT STDMETHODCALLTYPE CShellHook::Execute (LPSHELLEXECUTEINFO pei)
{
	//���д�����XP������������Windows7�б��ֲ�һ����������Win7�������̷�ʽ�Ըı�
	/*	TCHAR        szMsg[1024];

	wsprintf(szMsg, _T("�Ƿ�����ִ�� %s %s ?"), pei->lpFile, pei->lpParameters);
	return (MessageBox(NULL, szMsg, _T("��ʾ"), MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND) == IDYES) ? S_FALSE : S_OK;
	*/
	return S_OK;
}
