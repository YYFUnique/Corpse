#include "StdAfx.h"
#include "AppMonitor.h"
#include "Service.h"
#include "Deamon.h"
#include "Monitor.h"
#include "DeamonClassFactory.h"
#include "DllCore/Utils/Security.h"

extern HINSTANCE ghModule;

void WINAPI ServiceMain(int nArgc, LPWSTR lpszArgv[])
{
	EnablePrivilege(SE_TCB_NAME);

	CAppMonitor* pAppMonitor = CAppMonitor::GetInstance();

	LPCTSTR lpszFileMap = _T("Global\\ShareMemory");

	//创建共享内存
	HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,
																PAGE_EXECUTE_READWRITE, 0, sizeof(CAppMonitor),lpszFileMap);
	if (hMap != NULL)
	{
		LPVOID pBuffer = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		CopyMemory(pBuffer, pAppMonitor, sizeof(CAppMonitor));
	}


	//APPMONITOR_INFO AppMonitorInfo = {_T("C:\\Aquarius.exe"),_T("Aquarius.exe")};

	//pAppMonitor->AddMonitorAppInfo(AppMonitorInfo);

#ifdef _DEBUG
	//允许手动停止服务
	pAppMonitor->SetAcceptCodes(SERVICE_ACCEPT_STOP 
														| SERVICE_ACCEPT_PAUSE_CONTINUE
														| SERVICE_ACCEPT_SHUTDOWN 
														| SERVICE_ACCEPT_SESSIONCHANGE);
#else
	pAppMonitor->SetAcceptCodes(SERVICE_ACCEPT_SHUTDOWN
														| SERVICE_ACCEPT_SESSIONCHANGE);
#endif

	pAppMonitor->SetPollTime(1000);

	pAppMonitor->Run(lpszArgv[0]);
}

//需要注意的是Rundll32.exe区分32位应用和64位应用
//32位动态库只能使用32位Rundll32.exe调用
// rundll32 Deamon.dll, RundllInstallA LanGuard
void CALLBACK RundllInstallA(HWND hWnd, HINSTANCE hInstance, LPCSTR szCmdLine, INT nCmdShow)
{
	InstallService(szCmdLine);
}

void CALLBACK RundllUninstallA(HWND hWnd, HINSTANCE hInstance, LPCSTR szCmdLine, INT nCmdShow)
{
	UninstallService(szCmdLine);
}

// DllGetClassObject() is called when COM needs to get a class factory.
STDAPI DllGetClassObject ( REFCLSID rclsid, REFIID riid, void** ppv )
{
	// Check that the client is asking for the CSimpleMsgBoxImpl factory.
	if ( !InlineIsEqualGUID ( rclsid, CLSID_Monitor ))
		return CLASS_E_CLASSNOTAVAILABLE;

	// Check that ppv really points to a void*.
	if ( IsBadWritePtr ( ppv, sizeof(void*) ))
		return E_POINTER;

	*ppv = NULL;

	// Construct a new class factory object.
	CDeamonClassFactory* pFactory = new CDeamonClassFactory;

	if ( NULL == pFactory )
		return E_OUTOFMEMORY;

	// AddRef() the factory since we're using it.
	pFactory->AddRef();

	// QI() the factory for the interface the client wants.
	HRESULT hRet = pFactory->QueryInterface ( riid, ppv );

	// We're done with the factory, so Release() it.
	pFactory->Release();

	return hRet;
}

// DllCanUnloadNow() is called when COM wants to unload our DLL from memory.
// We check our lock count, which will be nonzero if there are any COM
// objects still in memory.
// Return S_FALSE to prevent the DLL from being unloaded, or S_OK to let it
// be unloaded.
STDAPI DllCanUnloadNow()
{
	return /*g_uDllLockCount > 0 ? S_FALSE :*/ S_OK;
}

// DllRegisterServer() creates the registy entries that tells COM where our 
// server is located and its threading model.
STDAPI DllRegisterServer()
{
	HRESULT hRet = S_FALSE;
	HKEY  hCLSIDKey = NULL, hInProcSvrKey = NULL;
	LONG  lRet;
	TCHAR szModulePath [MAX_PATH];
	TCHAR szClassDescription[] = _T("AppMonitorCtrl class");

	do 
	{
		// Create a key under CLSID for our COM server.
		TCHAR szRegPath[MAX_PATH];
		LPOLESTR lpszOleUuidstr = NULL;

		USES_CONVERSION;
		StringFromCLSID(CLSID_Monitor,&lpszOleUuidstr);
		LPTSTR lpszUuidstr = OLE2T(lpszOleUuidstr);
		PathCombine(szRegPath,_T("CLSID"),lpszUuidstr);
		CoTaskMemFree(lpszOleUuidstr);

		lRet = RegCreateKeyEx ( HKEY_CLASSES_ROOT, szRegPath,0, NULL, REG_OPTION_NON_VOLATILE, 
			KEY_SET_VALUE | KEY_CREATE_SUB_KEY,NULL, &hCLSIDKey, NULL);

		if ( ERROR_SUCCESS != lRet ) 
			break;

		// The default value of the key is a human-readable description of the coclass.

		lRet = RegSetValueEx ( hCLSIDKey, NULL, 0, REG_SZ, (const BYTE*) szClassDescription,
			sizeof(szClassDescription) );

		if ( ERROR_SUCCESS != lRet )
			break;

		// Create the InProcServer32 key, which holds info about our coclass.

		lRet = RegCreateKeyEx ( hCLSIDKey, _T("InProcServer32"), 0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_SET_VALUE, NULL, &hInProcSvrKey, NULL );

		if ( ERROR_SUCCESS != lRet )
			break;

		// The default value of the InProcServer32 key holds the full path to our DLL.

		GetModuleFileName ( ghModule, szModulePath, MAX_PATH );

		lRet = RegSetValueEx ( hInProcSvrKey, NULL, 0, REG_SZ, (const BYTE*) szModulePath, 
			sizeof(TCHAR) * (lstrlen(szModulePath)+1) );

		if ( ERROR_SUCCESS != lRet )
			break;

		// The ThreadingModel value tells COM how it should handle threads in our DLL.
		// The concept of apartments is beyond the scope of this article, but for
		// simple, single-threaded DLLs, use Apartment.

		LPCTSTR lpszThreadModel  = _T("Apartment");
		lRet = RegSetValueEx ( hInProcSvrKey, _T("ThreadingModel"), 0, REG_SZ,(const BYTE*) lpszThreadModel,sizeof(TCHAR) * (lstrlen(lpszThreadModel)+1));

		if ( ERROR_SUCCESS != lRet )
			break;

		hRet = S_OK;
	} while (FALSE);

	if ( NULL != hCLSIDKey )
		RegCloseKey ( hCLSIDKey );

	if ( NULL != hInProcSvrKey )
		RegCloseKey ( hInProcSvrKey );

	if (hRet != S_OK)
		hRet = HRESULT_FROM_WIN32(lRet);

	return hRet;
}

// DllUnregisterServer() deleted the registy entries that DllRegisterServer() created.
STDAPI DllUnregisterServer()
{
	// Delete our registry entries.  Note that you must delete from the deepest
	// key and work upwards, because on NT/2K, RegDeleteKey() doesn't delete 
	// keys that have subkeys on NT/2K.

	TCHAR szRegProcServerPath[MAX_PATH],szRegImageOlePath[MAX_PATH];
	LPOLESTR lpszOleUuidstr = NULL;
	StringFromCLSID(CLSID_Monitor,&lpszOleUuidstr);
	LPTSTR lpszUuidstr = OLE2T(lpszOleUuidstr);

	PathCombine(szRegImageOlePath,_T("CLSID"),lpszUuidstr);
	PathCombine(szRegProcServerPath,szRegImageOlePath,_T("InProcServer32"));

	RegDeleteKey ( HKEY_CLASSES_ROOT, szRegProcServerPath);
	RegDeleteKey ( HKEY_CLASSES_ROOT, szRegImageOlePath);

	//RegDeleteKey ( HKEY_CLASSES_ROOT, _T("CLSID\\{7D51904E-1645-4a8c-BDE0-0F4A44FC38C4}\\InProcServer32") );
	//RegDeleteKey ( HKEY_CLASSES_ROOT, _T("CLSID\\{7D51904E-1645-4a8c-BDE0-0F4A44FC38C4}") );

	CoTaskMemFree(lpszOleUuidstr);

	return S_OK;
}
