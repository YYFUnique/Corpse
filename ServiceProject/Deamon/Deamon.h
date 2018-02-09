#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DEAMON_API __declspec(dllexport)
#elif defined  DEAMON_EXPORTS
#define DEAMON_API __declspec(dllexport)
#else
#define DEAMON_API __declspec(dllimport)
#endif

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

//#pragma comment(linker,"/export:[Exports Name]=[Mangling Name]")

extern "C" DEAMON_API void WINAPI ServiceMain(int nArgc, LPWSTR lpszArgv[]);
extern "C" DEAMON_API void CALLBACK RundllInstallA(HWND hWnd, HINSTANCE hInstance, LPCSTR szCmdLine, INT nCmdShow);
extern "C" DEAMON_API void CALLBACK RundllUninstallA(HWND hWnd, HINSTANCE hInstance, LPCSTR szCmdLine, INT nCmdShow);
