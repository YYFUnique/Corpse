#pragma once
#include "../Utils/YjList.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef struct tagEventHookWnd
{
	HWND							hMainWnd;		//安装消息钩子的窗口
	HWINEVENTHOOK		hEventHook;	//当前消息钩子句柄
}EventHookWnd,*PEventHookWnd;

typedef CYjList<EventHookWnd,const EventHookWnd&> CEventMainWnd;

class DLL_API CFloatWindow
{
public:
	CFloatWindow();
	~CFloatWindow();

public:
	void ReleaseHookWinEvent();
	BOOL StickWndToDesktop(HWND hMainWnd);
	void    CannelWndSticked(HWND hMainWnd);
protected:
	static void CALLBACK WinEventHookProc(HWINEVENTHOOK hEventHook, DWORD dwEvent, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
private:
	HWINEVENTHOOK m_hEventHook;
	static CEventMainWnd m_sHookMainWnd;
};