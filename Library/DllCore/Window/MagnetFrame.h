#pragma once

#include "../Utils/DuiList.h"

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef enum tagATTCH_MODE
{
	ATTCH_MODE_NULL		= 0,
	ATTCH_MODE_LEFT		=	1,
	ATTCH_MODE_TOP			=	2,
	ATTCH_MODE_RIGHT		=	3,
	ATTCH_MODE_BOTTOM= 4,
}ATTCH_MODE;

typedef enum tagATTCH_ALIGN
{
	ATTCH_ALIGN_NULL		= 0,
	ATTCH_ALIGN_LEFT		=	1,
	ATTCH_ALIGN_TOP			=	2,
	ATTCH_ALIGN_RIGHT		= 3,
	ATTCH_ALIGN_BOTTOM= 4,
}ATTCH_ALIGN;

typedef struct tagATTCH_INFO
{
	int		nDist;
	ATTCH_MODE AttchMode;
	ATTCH_ALIGN	AttchAlign;
}ATTCH_INFO,*PATTCH_INFO;

typedef struct tagWND_DATA
{
	HWND				hWnd;
	WNDPROC		pProc;
	ATTCH_INFO	AttchInfo;
	BOOL				bNewPos;
	BOOL				bSubWnd;
}WND_DATA,*PWND_DATA;

typedef CDuiList<WND_DATA,WND_DATA&> CWndDataMap;

class DLL_API CMagnetFrame
{
protected:
	CMagnetFrame();
	~CMagnetFrame();

public:
	/************************************************************************/
	/* 函  数  名：Initialize						                                                           */
	/* 函数功能：初始化吸附模块对象，使用模块前必须调用，否则功能不正常  */
	/* 输入参数：无																						   */
	/* 返  回 值：对象创建成功返回TRUE，否则返回FALSE								   */
	/************************************************************************/
	static BOOL Initialize();

	void Release();

	static CMagnetFrame* GetInstance();
	/************************************************************************/
	/* 函  数  名：SetMainWnd				                                                           */
	/* 函数功能：设置吸附主窗口																	   */
	/* 输入参数：hWnd 主窗口句柄																   */
	/* 返  回 值：无																						   */
	/************************************************************************/
	void SetMainWnd(HWND hWnd);

	/************************************************************************/
	/* 函  数  名：AddSubWnd		                                                                  */
	/* 函数功能：添加一个吸附子窗口																   */
	/* 输入参数：hWnd 需要移除的子窗口														   */
	/* 返  回 值：成功返回TRUE，失败返回FALSE											   */
	/************************************************************************/
	BOOL AddSubWnd(HWND hWnd, ATTCH_MODE AttchMode = ATTCH_MODE_NULL, ATTCH_ALIGN AttchAlign = ATTCH_ALIGN_NULL, int nDist = 0);

	/************************************************************************/
	/* 函  数  名：RemoveSubWnd                                                                  */
	/* 函数功能：移除一个吸附子窗口																   */
	/* 输入参数：hWnd 需要移除的子窗口														   */
	/* 返  回 值：成功返回TRUE，失败返回FALSE											   */
	/************************************************************************/
	BOOL RemoveSubWnd(HWND hWnd);

protected:
	BOOL SubclassWnd(HWND hWnd, WND_DATA* pWndData);
	BOOL UnsubclassWnd(HWND hWnd);

	ATTCH_INFO CalcAttchMode(HWND hSubWnd, LPRECT rcWnd);
	void UpdateSubWndPos(HWND hSubWnd, const ATTCH_INFO* pAttchInfo);

protected:
	int CalcDistance(int nV1, int nV2);
	BOOL IsNeighbour(int nV1, int nV2);
	static LRESULT CALLBACK MagnetWndProc(HWND hWnd, UINT uMsg, WPARAM wParma, LPARAM lParam);

protected:
	HWND m_hMainWnd;		//吸附主窗口
	static CMagnetFrame* m_pMagnet;
	CWndDataMap m_SubWndProc;
};