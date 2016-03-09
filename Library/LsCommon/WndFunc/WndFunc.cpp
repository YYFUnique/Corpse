#include "Stdafx.h"
#include "WndFunc.h"
#include <Windowsx.h>

#pragma comment(lib,"gdi32.lib")

HWND GetTrayWndHwnd()
{
	HWND hWnd = FindWindow(_T("Shell_TrayWnd"),NULL);	//��ȡ���������
	if (hWnd != NULL)
	{
		hWnd = FindWindowEx(hWnd,NULL,_T("TrayNotifyWnd"),NULL);	//��ȡ���½�����
		if (hWnd != NULL)
		{
			HWND hPageWnd = FindWindowEx(hWnd,NULL,_T("SysPager"),NULL);	//
			if (hPageWnd != NULL)
				hWnd = FindWindowEx(hPageWnd,NULL,_T("ToolBarWindow32"),NULL);
			else
				hWnd = FindWindowEx(hWnd,NULL,_T("ToolBarWindow32"),NULL);

			return hWnd;
		}
	}
	return NULL;
}

HWND GetOverflowHwnd()
{
	HWND hWnd =FindWindow(_T("NotifyIconOverflowWindow"),NULL);  //�õ����֪ͨ������
	if (hWnd!=NULL)
		hWnd=FindWindowEx(hWnd,NULL,_T("ToolBarWindow32"),NULL);
	return hWnd;
}

BOOL GetWndInformation(HWND hWnd,CWindowInfoList& WindowInfoList)
{
	if (IsWindow(hWnd) == FALSE)
		return FALSE;

	BOOL bSuccess = FALSE;
	do 
	{
		WINDOW_INFO WndInfo;
		
		TCHAR szWndTitle[MAX_PATH];
		GetWindowText(hWnd,szWndTitle,_countof(szWndTitle));
		WndInfo.strWndTitle = szWndTitle;

		WndInfo.hWnd = hWnd;
		WndInfo.bVisible = IsWindowVisible(hWnd);
		WndInfo.IsUnicodeWnd = IsWindowUnicode(hWnd);
		WndInfo.dwThreadId = GetWindowThreadProcessId(hWnd,NULL);
		WndInfo.dwWndId = GetDlgCtrlID(hWnd);
		WndInfo.bEnable = IsWindowEnabled(hWnd);

		TCHAR szClassName[MAX_CLASS_NAME];
		GetClassName(hWnd,szClassName,_countof(szClassName));
		WndInfo.strClassName = szClassName;

		WndInfo.dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		WndInfo.dwStyleEx = GetWindowLong(hWnd,GWL_EXSTYLE);

		//PostThreadMessage(WndInfo.dwThreadId,WM_MYMESSAGE,(WPARAM)WndInfo.hWnd,(LPARAM)WndInfo.pWndProc);
		WndInfo.pWndProc = 0;

		HDESK hDesk = GetThreadDesktop(WndInfo.dwThreadId);
		if (hDesk)
		{
			TCHAR szDeskName[MAX_PATH];
			if (GetUserObjectInformation(hDesk,UOI_NAME,szDeskName,sizeof(szDeskName),NULL) == TRUE)
				WndInfo.strDesktop = szDeskName;
		}
		

		WindowInfoList.AddTail(WndInfo);
	} while (FALSE);

	return bSuccess;
}

BOOL CopyScreenToBitmap(LPRECT lpRect,HBITMAP& hDesktopMap,BOOL bSave /*= FALSE*/)
{
	// ��Ļ���ڴ��豸������
	HDC      hScrDC, hMemDC;      
	
	// λͼ���
	HBITMAP    hBitmap, hOldBitmap; 

	// ѡ����������
	int       nLeftPt, nTopPt, nRightPt, nBottomPt;      
	
	int      nWidth, nHeight;

	//��ȡ��Ļ����
	static int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	static int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//ȷ��ѡ������Ϊ�վ���
	if (IsRectEmpty(lpRect))
		return NULL;

	//Ϊ��Ļ�����豸������
	hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	hMemDC = CreateCompatibleDC(hScrDC);
	nLeftPt = lpRect->left;nTopPt = lpRect->top;nRightPt = lpRect->right;nBottomPt = lpRect->bottom;

	//ȷ��ѡ������ɼ���
	if (nLeftPt<0)
		nLeftPt = 0;
	if (nTopPt<0)
		nLeftPt = 0;
	if (nRightPt>nScreenWidth)
		nRightPt = nScreenWidth;
	if (nBottomPt>nScreenHeight)
		nBottomPt = nScreenHeight;

	//ѡ������Ĵ�С
	nWidth = nRightPt - nLeftPt;
	nHeight = nBottomPt - nTopPt;
	//��������Ļ�豸���ݵ�λͼ
	hBitmap = CreateCompatibleBitmap(hScrDC , nWidth , nHeight);
	// ����λͼѡ���ڴ��豸��������
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	if (bSave)
		//��ӱ��湦��
		BitBlt(hMemDC, 0, 0, nWidth, nHeight ,hScrDC, nLeftPt, nTopPt, SRCCOPY);
	else
		BitBlt(hMemDC, 0, 0, nWidth, nHeight ,hScrDC, nLeftPt, nTopPt, SRCCOPY);

	//��ԭ��ǰ��λͼ
	hDesktopMap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

	//�ͷ���Դ
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);

	if (bSave)
	{
		//��ӱ��湦��
	}

	return TRUE;
}

CBitmap* CoverDesktopBmp(LPRECT lpRect,HBITMAP hDesktopMap,HBITMAP hCoverMap)
{
	// ��Ļ���ڴ��豸������
	HDC      hScrDC,hMemDC,hTmpDC;      
	HBITMAP hCoverDesktopMap, hTmpOldMap,hOldMap;

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

	int nRet = AlphaBlend(hMemDC,0,0,nScreenWidth,nScreenHeight,hTmpDC,0,0,4,4,BlendFunc);
	hCoverMap = (HBITMAP)SelectObject(hTmpDC,hTmpOldMap);

	hCoverDesktopMap = (HBITMAP)SelectObject(hMemDC,hOldMap);

	return CBitmap::FromHandle(hCoverDesktopMap);
}

void EnsureRectVisible(LPRECT lpRect,int nRectWidth)
{
	if (lpRect->right - lpRect->left < nRectWidth)
		lpRect->right = lpRect->left + nRectWidth;
	if (lpRect->bottom - lpRect->top < nRectWidth)
		lpRect->bottom = lpRect->top + nRectWidth;
}

void EnsureRectIsVisible(LPRECT lpRect,LPRECT lpRectContainer)
{
	CRect Rect(lpRect);
	CRect RectContainer(lpRectContainer);
	Rect.NormalizeRect();
	int nHeight = Rect.Height();
	int nWidth = Rect.Width();
	if (lpRect->top<0)
	{
		lpRect->top = RectContainer.top + 10;
		lpRect->bottom = nHeight + lpRect->top;
		lpRect->right = RectContainer.left - 10;
		lpRect->left = lpRect->right - nWidth;
	}

	if (lpRect->left<0)
	{
		lpRect->left = RectContainer.right + 10;
		lpRect->right = lpRect->left + nWidth;
	}
	
	static int nScreenX = GetSystemMetrics(SM_CXSCREEN);
	static int nScreenY = GetSystemMetrics(SM_CYSCREEN);

	if (lpRect->right>nScreenX)
	{
		lpRect->left = RectContainer.left + 10;
		lpRect->right = lpRect->left + nWidth;
		
		lpRect->top = RectContainer.bottom + 10;
		lpRect->bottom = lpRect->top + nHeight;
	}

	if (lpRect->bottom>nScreenY)
	{
		lpRect->top = RectContainer.top + 10;
		lpRect->left = RectContainer.left + 10;
		lpRect->right = lpRect->left + nWidth;
		lpRect->bottom = lpRect->top + nHeight;
	}

}