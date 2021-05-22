#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

typedef struct tagTRAYDATAFIREWALL
{
	HWND		hMainWnd;
	UINT		uID;
	UINT		uCallbackMessage;	
	DWORD	Reserved[2];		
	HICON		hIcon;								//托盘图标的句柄	
	DWORD	Reserved2[3];
	TCHAR		szExePath[MAX_PATH];
	TCHAR		szTip[128];
}TRAYDATAFIREWALL,*PTRAYDATAFIREWALL;

typedef struct tagTRAYDATAFIREWALL64
{
	INT64		hMainWnd;
	UINT		uID;
	UINT		uCallbackMessage;	
	DWORD	Reserved[2];
	HICON		hIcon;								//托盘图标的句柄	
	DWORD	Reserved2[5];
	TCHAR		szExePath[MAX_PATH];
	TCHAR		szTip[128];
}TRAYDATAFIREWALL64,*PTRAYDATAFIREWALL64;

typedef enum tagTRAY_AREA
{
	Tray = 0,
	Overflow,
}TRAY_AREA;

typedef struct tagTRAYDATAINFO
{
	CString		strTitle;						//托盘提示文字
	CString		strFilePath;					//	文件路径
	HICON		hTrayIcon;					//托盘图标
	HWND		hWnd;							//窗口句柄
	UINT		uID;								//图标ID
	UINT		uBtnID;
	UINT		uCallbackMessage;		//回调函数消息号
	BOOL		bShowTray;					//是否显示图标
	UINT		bTrayIcon;					//托盘图标
	TRAY_AREA TrayArea;
}TRAYDATAINFO,*PTRAYDATAINFO;

typedef CDuiList<TRAYDATAINFO,const TRAYDATAINFO&> CTrayDataInfo;

typedef struct tagTBBUTTON64 
{
	int iBitmap;
	int idCommand;
	BYTE fsState;
	BYTE fsStyle;
	BYTE bReserved[6];          // padding for alignment
	DWORD_PTR dwData;
	INT_PTR iString;
} TBBUTTON64, NEAR* PTBBUTTON64;

/************************************************************************/
/* 函  数  名：GetTrayWndHwnd                                                                */
/* 函数功能：获取托盘图标窗口句柄															   */
/* 输入参数：无																						   */
/* 返  回 值：获取成功返回窗口句柄，否则返回NULL								   */
/************************************************************************/
DLL_API HWND GetTrayWndHwnd();

/************************************************************************/
/* 函  数  名：GetOverflowHwnd                                                               */
/* 函数功能：获取托盘图标溢出区域窗口句柄											   */
/* 输入参数：无																						   */
/* 返  回 值：获取成功返回窗口句柄，否则返回NULL								   */
/************************************************************************/
DLL_API HWND GetOverflowHwnd();

/************************************************************************/
/* 函  数  名：GetTrayIcon			                                                               */
/* 函数功能：获取32位版本对应的系统图标												   */
/* 输入参数：hWndTray	托盘窗口句柄													   */
/*					 TrayDataInfoList 用于保存托盘图标信息链表   					   */
/* 返  回 值：成功返回TRUE，否则返回FALSE											   */
/************************************************************************/
DLL_API BOOL GetTrayIcon(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);

/************************************************************************/
/* 函  数  名：GetTrayIcon			                                                               */
/* 函数功能：获取64位版本对应的系统图标												   */
/* 输入参数：hWndTray	托盘窗口句柄													   */
/*					 TrayDataInfoList 用于保存托盘图标信息链表   					   */
/* 返  回 值：成功返回TRUE，否则返回FALSE											   */
/************************************************************************/
DLL_API BOOL GetTrayIcon64(HWND hWndTray, CTrayDataInfo& TrayDataInfoList);