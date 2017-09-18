#include "stdafx.h"
#include "ImageView.h"
#include "Resource.h"
#include "UIImageView.h"
#include "3DView.h"
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
	//return UILIB_ZIP;
	return UILIB_FILE;
}

void CImageView::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnInitDialog(msg);
}

void CImageView::OnInitDialog(TNotifyUI& msg)
{
	C3DViewUI* pView = (C3DViewUI*)m_PaintManager.FindControl(_T("Img"));
	
	pView->SetViewImage(_T("C:\\1234.bmp"));
	pView->Update();
	pView->StartEffect(TRUE);
}

void CImageView::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	//HBITMAP hBitmap = (HBITMAP)LoadImage(CPaintManagerUI::GetInstance(),_T("C:\\123.bmp"),IMAGE_BITMAP,0,0,LR_LOADFROMFILE );
	////HBITMAP hBitmap = CRenderEngine::GenerateBitmap(GetManager(),GetManager()->GetRoot(),rcPaint);
	///*bRet = TRUE;*/
	//CImageViewUI* pImageView = (CImageViewUI*)m_PaintManager.FindControl(_T("Img"));
	//HBITMAP hRotate = pImageView->GetRotatedBitmap(hBitmap, 1, 0x00000001);
	//BITMAP bmp;
	//::GetObject(hRotate, sizeof(bmp), &bmp);
	//m_PaintManager->AddImage(_T("test"),hRotate,bmp.bmWidth,bmp.bmWidth,TRUE);
	//pImageView->SetViewImage(_T("test"));

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
	if (_tcsicmp(pstrClass, _T("ImageView")) == 0)
		return new CImageViewUI;
	else if (_tcsicmp(pstrClass, _T("3DView")) == 0)
		return new C3DViewUI;
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
	else if (msg.pSender->GetName() == _T("BtnTurnLeft"))
	{
		C3DViewUI* pView = (C3DViewUI*)m_PaintManager.FindControl(_T("Img"));
		pView->Get3dParam().nRotateZ += 90;
		pView->Update();
	}
	else if (msg.pSender->GetName() == _T("BtnTurnRight"))
	{
		C3DViewUI* pView = (C3DViewUI*)m_PaintManager.FindControl(_T("Img"));
		pView->Get3dParam().nRotateZ -= 90;
		pView->Update();
	}
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