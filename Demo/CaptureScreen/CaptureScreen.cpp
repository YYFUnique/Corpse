#include "stdafx.h"
#include "CaptureScreen.h"
#include "Resource.h"
#include <Windows.h>
//#include "YCLibCore/Utils/ErrorInfo.h"

#define	SCREEN_WIDTH		GetSystemMetrics(SM_CXSCREEN)
#define   SCREEN_HEIGHT		GetSystemMetrics(SM_CYSCREEN)

#define	CAPTURE_SCREEN	_T("DesktopScreen")
#define	CAPTURE_COLOR	_T("Capture")

CCaptureScreen::CCaptureScreen()
:m_hDesktopBitMap(NULL)
,m_hMask(NULL)
{
	m_hColorCursor = ::LoadCursor(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDC_CURSOR));

	//ZeroMemory(&m_RectSelected,0);
	m_PaintManager.SetMaxInfo(SCREEN_WIDTH,SCREEN_HEIGHT);

	RECT rcWnd;
	GetWindowRect(GetDesktopWindow(), &rcWnd);
	
	CopyScreenToBitmap(&rcWnd,m_hDesktopBitMap);
}

CCaptureScreen::~CCaptureScreen()
{

}

LPCTSTR CCaptureScreen::GetWindowClassName() const
{
	return _T("TxCaptureScreen");
}

CDuiString CCaptureScreen::GetZIPFileName() const
{
	return _T("CaptureScreen.zip");
}

CDuiString CCaptureScreen::GetSkinFile()
{
	return _T("CaptureScreen.xml");
}

CDuiString CCaptureScreen::GetSkinFolder()
{
	return _T("CaptureScreen");
}

UILIB_RESOURCETYPE CCaptureScreen::GetResourceType() const
{
	return UILIB_FILE;
}

CControlUI* CCaptureScreen::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CCaptureScreen::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_SETCURSOR:
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(m_hWnd, &pt);
				if (m_pRcTracker->HitTest(pt) >= 0)
					m_pRcTracker->SetCursor(m_hWnd, HTCLIENT);
				else
					SetCursor(m_hColorCursor);
				return TRUE;
			}
			break;
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CCaptureScreen::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnInitDialog(msg);
	else if (msg.sType == DUI_MSGTYPE_POSCHANGED)
		OnPosChanged(msg);
}

void CCaptureScreen::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CCaptureScreen::NotifyDeal(FILE_ACTION Action, LPCTSTR lpszFileName, LPVOID lParam)
{
	CDuiString strTipInfo;
	strTipInfo.Format(_T("ACTION:%d,NAME:%s,LParam:%d"),Action,lpszFileName,lParam);
	OutputDebugString(strTipInfo);
}

void CCaptureScreen::InitWindow()
{
	//设置显示图标
	SetIcon(IDI_MAINFRAME);
	
	m_pRcTracker = (CRectTrackerUI*)m_PaintManager.FindControl(_T("capture"));

	/*m_pRcTracker->se*/
	
	//DWORD dwNotifyFilter = FILTER_FILE_NAME|FILTER_DIR_NAME;
	//FileObject.Run(_T("C:\\test"), TRUE, dwNotifyFilter, (LPDEALFUNCTION)NotifyDeal, 0);

#ifdef _DEBUG
	SetWindowPos(m_hWnd,HWND_NOTOPMOST,0,0,800,600,SWP_SHOWWINDOW);
	CenterWindow();
#else
	SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,SWP_SHOWWINDOW);
#endif // _DEBUG
	SetForegroundWindow(m_hWnd);

	//HBITMAP hDotBmp = (HBITMAP)LoadImage(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(IDB_BITMAP_MASK),IMAGE_BITMAP,5,5,LR_DEFAULTCOLOR);
	HBITMAP hDotBmp = LoadBitmap(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(IDB_BITMAP_MASK));

	CoverDesktopBmp(m_hDesktopBitMap,hDotBmp,m_hMask);

	m_PaintManager.AddImage(CAPTURE_SCREEN,m_hMask,SCREEN_WIDTH,SCREEN_HEIGHT,FALSE);

	m_PaintManager.AddImage(CAPTURE_COLOR,m_hDesktopBitMap,SCREEN_WIDTH,SCREEN_HEIGHT,FALSE);
}

void CCaptureScreen::OnClick(TNotifyUI& msg)
{
	
}

void CCaptureScreen::OnInitDialog(TNotifyUI& msg)
{
	CVerticalLayoutUI* pScreen = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("Screen"));
	if (pScreen != NULL)
	{
		CDuiString strBkImage;
		RECT rcPos = pScreen->GetPos();
		CDuiPoint pt(rcPos.left,rcPos.top);
		ClientToScreen(m_hWnd,&pt);
		strBkImage.Format(_T("file='%s' source='%d,%d,%d,%d'"),CAPTURE_SCREEN,pt.x,pt.y,pt.x+rcPos.right-rcPos.left,pt.y+rcPos.bottom-rcPos.top);

		pScreen->SetBkImage(strBkImage);
	}
}

LRESULT CCaptureScreen::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	if (m_pRcTracker->HitTest(pt) < 0)
		m_pRcTracker->TrackRubberBand(m_hWnd, pt, TRUE);
	else
		m_pRcTracker->Track(m_hWnd, pt, TRUE);	
	return TRUE;
}


void CCaptureScreen::OnPosChanged(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("capture")))
		ShowScreen();
}

void CCaptureScreen::ShowScreen()
{
	CDuiRect rcWndPos;
	RECT rcShow;
	GetWindowRect(m_hWnd, &rcWndPos);

	//因为控件是相对于窗口左上角
	//将窗口矩形恢复到屏幕左上角(并未将窗口移动到屏幕左上角)
	RECT rcWnd = {0, 0, rcWndPos.GetWidth(), rcWndPos.GetHeight()};
	RECT rcTracker = m_pRcTracker->GetPos();
	::IntersectRect(&rcShow, &rcWnd, &rcTracker);

	int nWidth = rcShow.right-rcShow.left;
	int nHeight =rcShow.bottom-rcShow.top;

	if (m_pRcTracker->GetBkImage() != NULL)
	{
		//开始截图位置
		POINT pt = {rcShow.left,rcShow.top};
		ClientToScreen(m_hWnd,&pt);
		CDuiString strCapture;
		strCapture.Format(_T("file='%s' source='%d,%d,%d,%d'"), CAPTURE_COLOR, pt.x, pt.y, pt.x+nWidth, pt.y+nHeight);
		m_pRcTracker->SetBkImage(strCapture);
	}
}

BOOL CCaptureScreen::CopyScreenToBitmap(LPRECT lpRect,HBITMAP& hDesktopMap,BOOL bSave /*= FALSE*/)
{
	// 屏幕和内存设备描述表
	HDC      hScrDC, hMemDC;      
	// 位图句柄
	HBITMAP    hBitmap, hOldBitmap; 
	// 选定区域坐标
	int       nLeftPt, nTopPt, nRightPt, nBottomPt;      
	int      nWidth, nHeight;
	//获取屏幕长宽
	static int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	static int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//确保选定区域不为空矩形
	if (IsRectEmpty(lpRect))
		return NULL;

	//为屏幕创建设备描述表
	hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	hMemDC = CreateCompatibleDC(hScrDC);
	nLeftPt = lpRect->left;nTopPt = lpRect->top;nRightPt = lpRect->right;nBottomPt = lpRect->bottom;

	//确保选中区域可见性
	if (nLeftPt<0)
		nLeftPt = 0;
	if (nTopPt<0)
		nLeftPt = 0;
	if (nRightPt>nScreenWidth)
		nRightPt = nScreenWidth;
	if (nBottomPt>nScreenHeight)
		nBottomPt = nScreenHeight;

	//选定区域的大小
	nWidth = nRightPt - nLeftPt;
	nHeight = nBottomPt - nTopPt;
	//创建与屏幕设备兼容的位图
	hBitmap = CreateCompatibleBitmap(hScrDC , nWidth , nHeight);
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	if (bSave)
		//添加保存功能
		BitBlt(hMemDC, 0, 0, nWidth, nHeight ,hScrDC, nLeftPt, nTopPt, SRCCOPY);
	else
		BitBlt(hMemDC, 0, 0, nWidth, nHeight ,hScrDC, nLeftPt, nTopPt, SRCCOPY);

	//还原以前的位图
	hDesktopMap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	//释放资源
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	if (bSave)
	{
		//添加保存功能
	}

	return TRUE;
}

BOOL CCaptureScreen::CoverDesktopBmp(HBITMAP hDesktopMap,HBITMAP hCoverMap,HBITMAP& hCoverDesktopMap)
{
	// 屏幕和内存设备描述表
	HDC      hScrDC,hMemDC,hTmpDC;      
	HBITMAP hTmpOldMap,hOldMap;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	hMemDC = CreateCompatibleDC(hScrDC);
	hTmpDC = CreateCompatibleDC(hScrDC);

	hCoverDesktopMap = CreateCompatibleBitmap(hScrDC,nScreenWidth,nScreenHeight);
	hOldMap = (HBITMAP)SelectObject(hMemDC,hCoverDesktopMap);

	hTmpOldMap = (HBITMAP)SelectObject(hTmpDC,hDesktopMap);
	BitBlt(hMemDC,0,0,nScreenWidth,nScreenHeight,hTmpDC,0,0,SRCCOPY);

	SelectObject(hTmpDC,hCoverMap);

	BLENDFUNCTION BlendFunc;
	BlendFunc.BlendOp=AC_SRC_OVER;
	BlendFunc.BlendFlags=0;
	BlendFunc.SourceConstantAlpha=0x50;
	BlendFunc.AlphaFormat=0 ;

	typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

	int nRet = lpAlphaBlend(hMemDC,0,0,nScreenWidth,nScreenHeight,hTmpDC,0,0,4,4,BlendFunc);
	hCoverMap = (HBITMAP)SelectObject(hTmpDC,hTmpOldMap);

	hCoverDesktopMap = (HBITMAP)SelectObject(hMemDC,hOldMap);

	DeleteDC(hMemDC);
	DeleteDC(hTmpDC);

	return TRUE;
}