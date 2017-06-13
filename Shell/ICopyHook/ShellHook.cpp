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
		wsprintf(szMsg, _T("是否允许将目录从 %s 复制文件到 %s ?"), pszSrcFile, pszDestFile);
		nID = MessageBox(NULL, szMsg, _T("提示"), MB_YESNO | MB_ICONQUESTION| MB_SETFOREGROUND);
	}

	return nID;
}

HRESULT STDMETHODCALLTYPE CShellHook::Execute (LPSHELLEXECUTEINFO pei)
{
	//下列代码在XP正常，但是在Windows7中表现不一样，可能是Win7启动进程方式以改变
	/*	TCHAR        szMsg[1024];

	wsprintf(szMsg, _T("是否允许执行 %s %s ?"), pei->lpFile, pei->lpParameters);
	return (MessageBox(NULL, szMsg, _T("提示"), MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND) == IDYES) ? S_FALSE : S_OK;
	*/
	return S_OK;
}
