#pragma once

BOOL DelServiceByName(LPCTSTR lpServiceName);
BOOL GetServiceFilePath(LPCTSTR lpszServiceName, LPTSTR lpszPathFile);
BOOL GetServiceFilePath(SC_HANDLE schService, LPTSTR lpszPathFile);
BOOL IsServiceRunning(LPCTSTR lpServiceName);
BOOL RestartServiceByName(LPCTSTR lpServiceName,DWORD dwTimeout = 0);
BOOL ServiceIsExist(LPCTSTR lpszServiceName);
BOOL StartServiceByName(LPCTSTR lpServiceName,BOOL bWait);
BOOL StopServiceByName(LPCTSTR lpServiceName , BOOL bWait);
BOOL StopDependentsServiceByName(LPCTSTR lpServiceName , BOOL bWait);