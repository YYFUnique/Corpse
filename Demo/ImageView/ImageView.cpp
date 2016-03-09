#include "stdafx.h"
#include "ImageView.h"
#include "Resource.h"
#include <Wingdi.h>

CImageView::CImageView()
	:m_nLastState(SIZE_RESTORED)
{
	CPaintManagerUI::SetResourceZip(_T("small.zip"));
}

LPCTSTR CImageView::GetWindowClassName() const
{
	return _T("ImageView");
}

CDuiString CImageView::GetSkinFile()
{
	return _T("main.xml");
}

CDuiString CImageView::GetZIPFileName() const
{
	return CPaintManagerUI::GetResourceZip();
}

CDuiString CImageView::GetSkinFolder()
{
	return _T("ImageView");
}

UILIB_RESOURCETYPE CImageView::GetResourceType() const
{
	return UILIB_ZIP;
}

void CImageView::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

void CImageView::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
	::DragAcceptFiles(m_hWnd,TRUE);
}

LRESULT CImageView::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DROPFILES:
				OnDropFiles(wParam,lParam);
			break;
		/*case WM_WINDOWPOSCHANGED:
			return OnWindowPosChanged(wParam,lParam);*/
		default:
			return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
	}
	return 0;
}

CControlUI* CImageView::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CImageView::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
	TCHAR szFilePathName[MAX_PATH];
	HDROP hDropFile = (HDROP)wParam;

	UINT nCount = DragQueryFile(hDropFile, 0, szFilePathName, _countof(szFilePathName)); //得到文件路径
	CVerticalLayoutUI* pMain = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("VLayoutShow"));
	if (pMain)
	{
		CDuiString strBkImage;
		RECT rcPos = pMain->GetPos();

		TImageInfo* pData = CRenderEngine::LoadImage(szFilePathName);
		int nWidth=pData->nX;
		int nHeight = pData->nY;
		int nLeft = (rcPos.right - rcPos.left - nWidth)/2 ;
		int nTop = (rcPos.bottom - rcPos.top - nHeight)/2;
		strBkImage.Format(_T("file='%s' dest='%d,%d,%d,%d' xtiled='false' ytiled='false'"),szFilePathName,nLeft,nTop,nLeft+nWidth,nTop+nHeight);
		pMain->SetBkImage(strBkImage);
	}
	DragFinish(hDropFile);
	return 0;
}

LRESULT CImageView::OnWindowPosChanged(WPARAM wParam, LPARAM lParam)
{
	//WINDOWPOS* pWIndowPos = (WINDOWPOS*)lParam;
	//CPaintManagerUI::ReloadSkin();

	return TRUE;
}

void CImageView::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("CloseBtn"))
		PostQuitMessage(0);
	else if (msg.pSender->GetName() == _T("MinBtn"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (msg.pSender->GetName() == _T("BtnFullScreen"))
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	else if (msg.pSender->GetName() == _T("BtnDesktop"))
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
}

LRESULT CImageView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet = WindowImplBase::OnSize(uMsg,wParam,lParam,bHandled);
 	if (IsWindowVisible(m_hWnd))
	{
		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			m_PaintManager.RemoveAllImages();

		if (wParam == SIZE_MAXIMIZED)
 		{
			m_nLastState = SIZE_MAXIMIZED;
			CPaintManagerUI::SetResourceZip(_T("full.zip"));
		}else if(IsWindowVisible(m_hWnd) && wParam == SIZE_RESTORED && m_nLastState != SIZE_RESTORED)
 		{
			m_nLastState = SIZE_RESTORED;
			CPaintManagerUI::SetResourceZip(_T("small.zip")); // 这里写入新的皮肤包的文件名就行了   
 		}

		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			ReloadResource();
	}
	
	return lRet;
}