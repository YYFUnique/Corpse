#pragma once

#include "../Utils/DuiList.h"

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

typedef CDuiList<EventHookWnd,const EventHookWnd&> CEventMainWnd;
//typedef std::vector<EventHookWnd> CEventMainWnd;

class DLL_API CFloatWindow
{
public:
	CFloatWindow();
	~CFloatWindow();

public:
	/************************************************************************/
	/* 函数名称 : ReleaseHookWinEvent                                                          */
	/* 函数说明：释放事件Hook句柄																   */
	/* 参数说明：无																						   */
	/* 返  回 值：无																						   */
	/************************************************************************/
	void ReleaseHookWinEvent();

	/************************************************************************/
	/* 函数名称 : StickWndToDesktop                                                              */
	/* 函数说明：用于将窗口钉在桌面																   */
	/* 参数说明：hMainWnd 需要钉在桌面的窗口句柄									   */
	/* 返  回 值：操作成功返回TRUE，否则返回FALSE									   */
	/*           注：该操作需要窗口具有WS_EX_TOOLWINDOW 扩展属性		   */
	/************************************************************************/
	BOOL StickWndToDesktop(HWND hMainWnd);

	/************************************************************************/
	/* 函数名称 : CannelWndSticked		                                                           */
	/* 函数说明：取消钉在桌面																		   */
	/* 参数说明：hMainWnd 钉在桌面的窗口句柄											   */
	/************************************************************************/
	void    CannelWndSticked(HWND hMainWnd);
protected:
	static void CALLBACK WinEventHookProc(HWINEVENTHOOK hEventHook, DWORD dwEvent, HWND hWnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
private:
	HWINEVENTHOOK m_hEventHook;
	static CEventMainWnd m_sHookMainWnd;
};