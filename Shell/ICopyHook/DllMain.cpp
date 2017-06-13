#include "stdafx.h"
#include "DllMain.h"
#include "ShellHook.h"
#include "ShellCopyClassFactory.h"
#include <atlbase.h>

HINSTANCE  g_hinstThisDll = NULL;        // Our DLL's module handle
ULONG        g_lLockNumber = 0;
ULONG        g_lRefThisDll = 0;

BOOL APIENTRY DllMain ( HINSTANCE hModule, DWORD  dwCallReason, LPVOID    lpReserved )
{
	g_hinstThisDll = hModule;
	switch ( dwCallReason )
	{
	case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hModule);
		break;
	}

	return TRUE;
}

HRESULT RegisterClsid(LPTSTR lpszClsid, LPTSTR lpszModule)
{
	HKEY        hKey;
	HKEY        hSubKey;
	TCHAR        szSubKey[1024];
	TCHAR        szModel[] = _T("Apartment");
	LSTATUS        status;

	lstrcpy(szSubKey, _T("CLSID\\"));
	lstrcat(szSubKey, lpszClsid);
	status = RegCreateKey(HKEY_CLASSES_ROOT, szSubKey, &hKey);
	if(status == ERROR_SUCCESS)
	{
		status = RegCreateKey(hKey, _T("InProcServer32"), &hSubKey);
		if(status == ERROR_SUCCESS)
		{
			RegSetValue(hSubKey, NULL, REG_SZ, lpszModule, 0);
			RegSetValueEx(hSubKey, _T("ThreadingModel"), 0, REG_SZ, (BYTE*)szModel, sizeof(szModel));
			RegCloseKey(hSubKey);
		}
		RegCloseKey(hKey);
	}

	return HRESULT_FROM_WIN32(status);
}

BOOL OsIsWow64Process()
{
	static BOOL bWow64Process=-1;

	if (bWow64Process==TRUE || bWow64Process==FALSE)
		return bWow64Process;

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);

	HMODULE hModule=GetModuleHandle(_T("Kernel32.dll"));

	if (hModule==NULL)
		return FALSE;

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hModule,"IsWow64Process");
	if (fnIsWow64Process==NULL)
		return FALSE;//win2000

	bWow64Process=FALSE;
	if (fnIsWow64Process(GetCurrentProcess(),&bWow64Process)==FALSE)
		return FALSE;

	return bWow64Process;
}

HRESULT UnRegisterClsid(LPTSTR lpszClsid)
{
	TCHAR        szSubKey[1024];
	LSTATUS        status;

	lstrcpy(szSubKey, _T("CLSID\\"));
	lstrcat(szSubKey, lpszClsid);
	lstrcat(szSubKey, _T("\\InProcServer32"));
	status = RegDeleteKey(HKEY_CLASSES_ROOT, szSubKey);

	lstrcpy(szSubKey, _T("CLSID\\"));
	lstrcat(szSubKey, lpszClsid);
	status = RegDeleteKey(HKEY_CLASSES_ROOT, szSubKey);

	return HRESULT_FROM_WIN32(status);
}

// DllGetClassObject() is called when COM needs to get a class factory.
STDAPI DllGetClassObject (REFCLSID rclsid, REFIID riid, void** ppv)
{
	if (!InlineIsEqualGUID(rclsid, CLSID_ShellHook))
		return CLASS_E_CLASSNOTAVAILABLE;

	if (IsBadWritePtr(ppv, sizeof(void*)))
		return E_POINTER;

	*ppv = NULL;

	CShellCopyClassFactory* pFactory = new CShellCopyClassFactory;

	if (pFactory == NULL)
		return E_OUTOFMEMORY;

	pFactory->AddRef();
	HRESULT hRet = pFactory->QueryInterface(riid, ppv);

	pFactory->Release();

	return hRet;
}

STDAPI DllCanUnloadNow (void)
{
	if (g_lRefThisDll == 0 && g_lLockNumber == 0)
		return S_OK;
	else
		return S_FALSE;
}

STDAPI DllRegisterServer()
{
	//32位系统需要32位插件，64位系统需要64位插件
	if (OsIsWow64Process() != FALSE)
	{
		MessageBox(NULL, _T("该插件不适用于当前系统版本，请与软件供应商联系！"),_T("提示"), MB_OK|MB_ICONSTOP);
		return E_UNEXPECTED ;
	}

	TCHAR        szModule[MAX_PATH];
	HKEY			hKey;
	TCHAR        szClsid[64];
	LPTSTR		lpszStr;
	StringFromCLSID (CLSID_ShellHook, &lpszStr);

	_stprintf_s(szClsid, _countof(szClsid), _T("%s"), lpszStr);
	CoTaskMemFree(lpszStr); 

	if(RegCreateKey(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\CopyHookHandlers\\zCopyHook"), &hKey) == ERROR_SUCCESS)
	{
		RegSetValue(hKey, NULL, REG_SZ, szClsid, 0);
		RegCloseKey(hKey);    
	}  
	if(RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellExecuteHooks"), &hKey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, szClsid, 0, REG_SZ, NULL, 0);
		RegCloseKey(hKey);    
	}
	if(RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), &hKey) == ERROR_SUCCESS)
	{
		DWORD    dwVal = 1;
		RegSetValueEx(hKey, _T("EnableShellExecuteHooks"), 0, REG_DWORD, (LPBYTE)&dwVal, sizeof(DWORD));
		RegCloseKey(hKey);    
	}

	GetModuleFileName(g_hinstThisDll, szModule, MAX_PATH);

	return RegisterClsid(szClsid, szModule);
}

STDAPI DllUnregisterServer ()
{
	TCHAR     szClsid[64];
	HKEY        hKey;
	LPTSTR	lpszStr;
	StringFromCLSID(CLSID_ShellHook, &lpszStr);

	_stprintf_s(szClsid, _countof(szClsid), _T("%s"), lpszStr);
	CoTaskMemFree(lpszStr);
	
	RegDeleteKey(HKEY_CLASSES_ROOT, _T("Directory\\shellex\\CopyHookHandlers\\zCopyHook"));
	if(RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellExecuteHooks"), &hKey) == ERROR_SUCCESS)
	{
		RegDeleteValue(hKey, szClsid);
		RegCloseKey(hKey);    
	}

	if(RegOpenKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"), &hKey) == ERROR_SUCCESS)
	{
		RegDeleteValue(hKey, _T("EnableShellExecuteHooks"));
		RegCloseKey(hKey);    
	}

	return UnRegisterClsid(szClsid);
}
