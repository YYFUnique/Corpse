#pragma once

#include <windows.h>

#ifdef LS_STATIC_LIB_CALL
#define LIBDLNA_API 
#elif defined  LIBDLNA_EXPORTS
#define LIBDLNA_API __declspec(dllexport)
#else
#define LIBDLNA_API __declspec(dllimport)
#endif

// File IO
typedef struct
{
	void*	(*Open)	(const char* urlpath);
	int		(*Close)(void* handle);
	int		(*Read)	(void* handle, unsigned char* buffer, long len, long* read);
	int		(*Seek)	(void* handle, long offset, int method);
	int		(*Tell)	(void* handle, unsigned long long* offset);
} FileIO;


typedef enum tagDLNAEVENT_TYPE
{
	DLNAEVENT_TYPE_ERROR						= 0,		//	DLNA发生错误
	DLNAEVENT_TYPE_DEVICE_CHANGED	= 10,	//	DLNA发生变化
}DLNAEVENT_TYPE;

// DLNA event
typedef struct tagDLNA_EVENT_INFO
{
	DWORD	dwEventCode;
	TCHAR		szEventParameter[1024];
}DLNA_EVENT_INFO,*PDLNA_EVENT_INFO;

typedef void (*LPDLNA_EVENT_ROUTINE)(PDLNA_EVENT_INFO pDlnaEventInfo, LPVOID pCallbackParameter);

// DLNA API
LIBDLNA_API BOOL APlayerDlna_Initialize(FileIO* pIO, LPDLNA_EVENT_ROUTINE pfnEventCallBack, LPVOID pCallBackParameter);
LIBDLNA_API BOOL APlayerDlna_UnInitialize();
LIBDLNA_API BOOL APlayerDlna_Open(LPCTSTR lpszUrl, HANDLE * phSession);
LIBDLNA_API BOOL APlayerDlna_Close(HANDLE hSession);
LIBDLNA_API	BOOL APlayerDlna_SelectDevice(LPCTSTR lpszDeviceID);
LIBDLNA_API BOOL APlayerDlna_Play();
LIBDLNA_API BOOL APlayerDlna_Pause();
LIBDLNA_API BOOL APlayerDlna_Stop();
LIBDLNA_API BOOL APlayerDlna_SetPosition(LONG nTimePosition);
LIBDLNA_API BOOL APlayerDlna_GetPosition(LONG* pnTimePosition);
