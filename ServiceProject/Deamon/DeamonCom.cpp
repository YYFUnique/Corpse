#include "StdAfx.h"
#include "DeamonCom.h"
#include "AppMonitor.h"

CDeamonCom::CDeamonCom()
	:m_ulRef(0)
{

}

CDeamonCom::~CDeamonCom()
{

}


HRESULT WINAPI CDeamonCom::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IMonitor)
	{
		*ppvObject = (IMonitor *)this;
		((IMonitor *)(*ppvObject))->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG WINAPI CDeamonCom::AddRef(void)
{
	m_ulRef++;
	return m_ulRef;
}

ULONG WINAPI CDeamonCom::Release(void)
{
	if (--m_ulRef == 0)
	{
		delete this;
		return 0;
	}

	return m_ulRef;
}

HRESULT WINAPI CDeamonCom::AddAppInfo(LPCTSTR lpszFilePath)
{
	LPCTSTR lpszFileMap = _T("Global\\ShareMemory");
	
	CAppMonitor* pAppMonitor = NULL;
	//TCHAR szTipInfo[MAX_PATH];
	HANDLE hMap = OpenFileMapping(FILE_MAP_EXECUTE, FALSE, lpszFileMap);
	LPVOID lpBuffer = NULL;
	if (hMap != NULL)
	{
		pAppMonitor = (CAppMonitor*)MapViewOfFile(hMap, FILE_MAP_EXECUTE, 0, 0, sizeof(CAppMonitor));	
	}

	if (pAppMonitor == NULL)
		return E_OUTOFMEMORY;

	APPMONITOR_INFO AppMonitorInfo;

	if (PathFileExists(lpszFilePath) == FALSE)
		return STG_E_FILENOTFOUND;

	AppMonitorInfo.strFilePath = lpszFilePath;
	AppMonitorInfo.strProcessName = PathFindFileName(AppMonitorInfo.strFilePath);
	pAppMonitor->AddMonitorAppInfo(AppMonitorInfo);

	return S_OK;
}