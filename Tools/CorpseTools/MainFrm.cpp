
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "CorpseTools.h"

#include "MainFrm.h"
#include "Common/Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()
//
//static UINT indicators[] =
//{
//	ID_SEPARATOR,           // 状态行指示器
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
//};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("未能创建状态栏\n");
	//	return -1;      // 未能创建
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	//SetTitle(_T("飞逸无限专用工具"));
	::SetProp(m_hWnd, _T("ToolSemaphore"), (HANDLE)1);//设置信号量（只能使用一个实例使用）
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	int dwWidth,dwHeight;
	//读取最后一次使用的窗口大小
	if ( !( ReadRegFromLanToolNode(_T("ToolWidth"),(DWORD& )dwWidth) &&
		ReadRegFromLanToolNode(_T("ToolHeight"),(DWORD& )dwHeight)) 
		|| dwHeight<70)//防止程序以最小化方式关闭
	{
		dwWidth=894;
		dwHeight=640;
	}

	::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, dwWidth, dwHeight, SWP_NOMOVE);
	CenterWindow();

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序
// BOOL CMainFrame::SendData(CONNID ConnID,PMSG_HEAD pMsg)
// {
// 	m_Server.Send()
/*}*/

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	RECT rcWnd;
	GetWindowRect(&rcWnd);
	DWORD dwWidth = rcWnd.right - rcWnd.left;
	DWORD dwHeight = rcWnd.bottom - rcWnd.top;
	WriteRegFromLanToolNode(_T("ToolWidth"), (DWORD&)dwWidth);
	WriteRegFromLanToolNode(_T("LToolHeight"), (DWORD&)dwHeight);
	::RemoveProp(m_hWnd, _T("ToolSemaphore")); //移除信号量（只能使用一个实例使用）
	CFrameWnd::OnClose();
}

//读取	HKEY_LOCAL_MACHINE\\SOFTWARE\\Lonsin\\LanGuardTool下的	的值
BOOL CMainFrame::ReadRegFromLanToolNode(LPCTSTR lpKeyName,DWORD &dwValue)
{
	DWORD dwValueT = 0; 
	DWORD dwErrorCode;
	BOOL bRet=FALSE;
	dwErrorCode = RegGetToolDwordValue(lpKeyName, dwValueT);
	if (dwErrorCode == ERROR_SUCCESS)
	{
		dwValue = dwValueT;
		bRet=TRUE;
	}

	return bRet;	
}

//写入	HKEY_LOCAL_MACHINE\\SOFTWARE\\Lonsin\\LanGuardTool下的	的值
BOOL CMainFrame::WriteRegFromLanToolNode(LPCTSTR lpKeyName, DWORD dwValue)
{
	DWORD dwErrorCode = RegSetToolDwordValue(lpKeyName, dwValue);
	if (dwErrorCode == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}