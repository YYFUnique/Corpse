#include "stdafx.h"
#include "RichEdit.h"
#include "resource.h"
#include "RichEditUtil.h"
#include "Utils.h"
#include "FileDialogEx.h"
#include "LsList.h"
#include "ImageOleCtrl/ImageOle.h"
CFontInfo g_BuddyFontInfo;

typedef CLsList<DWORD,DWORD> CListDWord;

CRichEdit::CRichEdit()
:m_pSendRichEdit(NULL)
,m_pRecvRichEdit(NULL)
,m_Inited(FALSE)
// ,m_pRichEditOleCallBackSendEdit(NULL)
// ,m_pRichEditOleCallBackRecvEdit(NULL)
{

}

CRichEdit::~CRichEdit()
{
// 	if (m_pRichEditOleCallBackSendEdit != NULL)
// 		m_pRichEditOleCallBackSendEdit->Release();
// 	if (m_pRichEditOleCallBackRecvEdit != NULL)
// 		m_pRichEditOleCallBackRecvEdit->Release();
}

LPCTSTR CRichEdit::GetWindowClassName() const
{
	return _T("DuilibFoundation");
}

UILIB_RESOURCETYPE CRichEdit::GetResourceType() const
{
	return UILIB_FILE;
}

LPCTSTR CRichEdit::GetResourceID() const
{
	return _T("");
}

CDuiString CRichEdit::GetZIPFileName() const
{
	return _T("RichEdit.zip");
}

CDuiString CRichEdit::GetSkinFile()
{
	return _T("MainWindow.xml");
}

CDuiString CRichEdit::GetSkinFolder()
{
	return _T("RichEdit");
}

void CRichEdit::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		OnItemSelect(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectChanged(msg);
}

void CRichEdit::InitWindow()
{
	m_bReturnSendMsg = TRUE;
	m_Inited = TRUE;
	SetIcon(IDI_MAINFRAME);

	m_pSendRichEdit = (CRichEditUI*)m_PaintManager.FindControl(_T("RichSend"));
	m_pRecvRichEdit = (CRichEditUI*)m_PaintManager.FindControl(_T("RichRecv"));
	

	CListDWord LsListDWord;
	DWORD dwIndex = 5;
	LsListDWord.AddTail(dwIndex);
	LsListDWord.AddTail(dwIndex);
// 	IRichEditOleCallback2* pRichEditOleCallback2 = NULL;
// 	HRESULT hRet = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
// 		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
// 	if (SUCCEEDED(hRet))
// 	{
// 		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
// 		ITextServices * pTextServices = m_pRecvRichEdit->GetTextServices();
// 		pRichEditOleCallback2->SetTextServices(pTextServices);
// 		pTextServices->Release();
// 		m_pRecvRichEdit->SetOleCallback(pRichEditOleCallback2);
// 		pRichEditOleCallback2->Release();
// 	}
// 
// 	pRichEditOleCallback2 = NULL;
// 	hRet = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
// 		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
// 	if (SUCCEEDED(hRet))
// 	{
// 		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
// 		ITextServices * pTextServices = m_pSendRichEdit->GetTextServices();
// 		pRichEditOleCallback2->SetTextServices(pTextServices);
// 		pTextServices->Release();
// 		m_pSendRichEdit->SetOleCallback(pRichEditOleCallback2);
// 		pRichEditOleCallback2->Release();
// 	}

// 	IDropTarget *pDropTarget = m_pSendRichEdit->GetTxDropTarget();
// 	hRet = ::RegisterDragDrop(m_hWnd, pDropTarget);
// 	pDropTarget->Release();
}

LRESULT CRichEdit::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
		case WM_MENUCLICK:
				OnMenuClick((CControlUI*)lParam);
			break;
		default:
			bHandled = FALSE;
	}
	return TRUE;
}

LRESULT CRichEdit::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ((m_pSendRichEdit != NULL) && m_pSendRichEdit->IsFocused() 
		&& (uMsg == WM_KEYDOWN) && (wParam == 'V') && (lParam & VK_CONTROL))	// 发送消息框的Ctrl+V消息
	{
		m_pSendRichEdit->PasteSpecial(CF_TEXT);
		return TRUE;
	}

	//if (uMsg == WM_KEYDOWN && wParam == VK_RETURN && m_bReturnSendMsg == TRUE)
	//{
	//	//OnRichSendMessage();
	//	m_pSendRichEdit->SetFocus();
	//	return 0;
	//}

	if (uMsg == WM_NOTIFY && EN_PASTE == ((LPNMHDR)lParam)->code)
	{
		ITextServices * pTextServices = m_pSendRichEdit->GetTextServices();
		if ((UINT)pTextServices == ((LPNMHDR)lParam)->idFrom)
			AddMsgToSendEdit(((NMRICHEDITOLECALLBACK *)lParam)->lpszText);
		if (pTextServices != NULL)
			pTextServices->Release();
	}

	if (uMsg == WM_MENUCLICK)
	{
		CControlUI * pControl = (CControlUI *)lParam;
		if (pControl != NULL)
			m_PaintManager.SendNotify(pControl, _T("menu_msg"), wParam, lParam);
	}

	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

CControlUI* CRichEdit::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT CRichEdit::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::OnSysCommand(uMsg, wParam, lParam,bHandled);
}

LRESULT CRichEdit::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ( IsWindowVisible(m_hWnd) && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED))
	{
		CButtonUI* pMax = (CButtonUI*)m_PaintManager.FindControl(_T("BtnMax"));
		CButtonUI* pRestor = (CButtonUI*)m_PaintManager.FindControl(_T("BtnRestore"));
		pMax->SetVisible(wParam == SIZE_RESTORED);
		pRestor->SetVisible(wParam != SIZE_RESTORED);
	}

	return WindowImplBase::OnSize(uMsg,wParam,lParam,bHandled);
}

LRESULT CRichEdit::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return TRUE;
}

void CRichEdit::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnClose"))
		Close();
	else if (msg.pSender->GetName() == _T("BtnMax"))
	{
		m_PaintManager.FindControl(_T("BtnMax"))->SetVisible(false);
		m_PaintManager.FindControl(_T("BtnRestore"))->SetVisible(true);
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else if (msg.pSender == (CControlUI*)m_PaintManager.FindControl(_T("BtnMin")))
	{
		RestoreBtnStatus(msg.pSender);
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (msg.pSender == (CControlUI*)m_PaintManager.FindControl(_T("BtnRestore")))
	{
		m_PaintManager.FindControl(_T("BtnMax"))->SetVisible(true);
		m_PaintManager.FindControl(_T("BtnRestore"))->SetVisible(false);
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	}
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnImage")))
		OnSelectBtnImage(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnColor")))
		OnSelectBtnColor(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnShock")))
		OnSelectBtnShock(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnSendOption")))
		OnBtnSendOption(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnSend")))
		OnRichSendMessage();
}

void CRichEdit::OnItemSelect(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("FontName"))
		OnSelectFont(msg);
	else if (msg.pSender->GetName() == _T("FontSize"))
		OnSelectFontSize(msg);
}

void CRichEdit::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender == (CControlUI*)m_PaintManager.FindControl(_T("BtnFont")))
		OnSelectBtnFont(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnBold")))
		OnSelectBtnBold(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnItalic")))
		OnSelectBtnItalic(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnUnderLine")))
		OnSelectBtnUnderLine(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnFace")))
		OnSelectBtnFace(msg);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnScreenShots")))
		OnSelectBtnScreenShots(msg);
}

void CRichEdit::SetRichEditDefaultFont(const CFontInfo* pFontInfo,CRichEditUI* pRichEdit)
{
	if (g_BuddyFontInfo.IsEqualFont(pFontInfo) == FALSE)
		g_BuddyFontInfo.Copy(pFontInfo);

	ITextServices * pTextServices = pRichEdit->GetTextServices();
	RichEdit_SetDefFont(pTextServices, pFontInfo->m_strName,
		pFontInfo->m_nSize, pFontInfo->m_clrText, pFontInfo->m_bBold,
		pFontInfo->m_bItalic, pFontInfo->m_bUnderLine, FALSE);
	pTextServices->Release();
}

void CRichEdit::OnSelectFont(TNotifyUI& msg)
{
	CDuiString strFont = msg.pSender->GetText();
	if (m_FontInfo.m_strName != strFont)
	{
		m_FontInfo.m_strName = strFont;
		SetRichEditDefaultFont(&m_FontInfo,m_pSendRichEdit);
	}
}

void CRichEdit::OnSelectFontSize(TNotifyUI& msg)
{
	int nFontSize = 0;
	_stscanf_s(msg.pSender->GetText(),_T("%d"),&nFontSize);
	
	if (m_FontInfo.m_nSize != nFontSize)
	{
		m_FontInfo.m_nSize = nFontSize;
		SetRichEditDefaultFont(&m_FontInfo,m_pSendRichEdit);
	}
}

void CRichEdit::OnSelectBtnFont(TNotifyUI& msg)
{
	CControlUI* pFontBar = m_PaintManager.FindControl(_T("FontBar"));
	COptionUI* pBtnFont = (COptionUI*)msg.pSender;
	pFontBar->SetVisible(pBtnFont->IsSelected());
}

void CRichEdit::OnSelectBtnColor(TNotifyUI& msg)
{
	
}

void CRichEdit::OnSelectBtnBold(TNotifyUI& msg)
{
	COptionUI* pBtnBold = (COptionUI*)m_PaintManager.FindControl(_T("BtnBold"));
	if (m_FontInfo.m_bBold != (BOOL)pBtnBold->IsSelected())
	{
		m_FontInfo.m_bBold = pBtnBold->IsSelected();
		SetRichEditDefaultFont(&m_FontInfo,m_pSendRichEdit);
	}
}

void CRichEdit::OnSelectBtnItalic(TNotifyUI& msg)
{
	COptionUI* pBtnItalic = (COptionUI*)m_PaintManager.FindControl(_T("BtnItalic"));
	if (m_FontInfo.m_bItalic != (BOOL)pBtnItalic->IsSelected())
	{
		m_FontInfo.m_bItalic = pBtnItalic->IsSelected();
		SetRichEditDefaultFont(&m_FontInfo,m_pSendRichEdit);
	}
}

void CRichEdit::OnSelectBtnUnderLine(TNotifyUI& msg)
{
	COptionUI* pBtnUnderLine = (COptionUI*)m_PaintManager.FindControl(_T("BtnUnderLine"));
	if (m_FontInfo.m_bUnderLine != (BOOL)pBtnUnderLine->IsSelected())
	{
		m_FontInfo.m_bUnderLine = pBtnUnderLine->IsSelected();
		SetRichEditDefaultFont(&m_FontInfo,m_pSendRichEdit);
	}
}

void CRichEdit::OnSelectBtnFace(TNotifyUI& msg)
{

}

void	CRichEdit::OnSelectBtnImage(TNotifyUI& msg)
{
	if (NULL == m_pSendRichEdit)
		return;

	LPCTSTR lpszFilter = _T("图像文件(*.bmp;*.jpg;*.jpeg;*.gif;*.png)\0*.bmp;*.jpg;*.jpeg;*.gif;*.png\0\0");;

	CFileDialogEx FileDlg;
	FileDlg.SetFilter(lpszFilter);
	FileDlg.SetParentWnd(m_hWnd);
	if (FileDlg.ShowOpenFileDlg())
	{
		_RichEdit_InsertFace(m_pSendRichEdit, FileDlg.GetPathName(), -1, -1);
		m_pSendRichEdit->SetFocus();
	}
}

void	CRichEdit::OnSelectBtnShock(TNotifyUI& msg)
{
	RestoreBtnStatus(msg.pSender);
	CDuiRect RcRect;
	GetWindowRect(m_hWnd,&RcRect);
	int nLeft = 0,nTop = 0,nOffset;
	AddTipMsgToRecvEdit(_T("您向对方发送了一个窗口抖动"));
	for (int i=0;i<10;++i)
	{
		nOffset = i%2 == 0 ? 5 : -5;
		nLeft = RcRect.left + nOffset;
		nTop = RcRect.top + nOffset;
		SetWindowPos(m_hWnd,NULL,nLeft,nTop,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
		Sleep(50);
	}
	SetWindowPos(m_hWnd,NULL,RcRect.left,RcRect.top,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW);
}

void	CRichEdit::OnSelectBtnScreenShots(TNotifyUI& msg)
{

}

void CRichEdit::OnBtnSendOption(TNotifyUI& msg)
{
	CMenuWnd* pMenu = new CMenuWnd;
	const RECT& rcPos = msg.pSender->GetPos();
	CPoint pt(rcPos.left,rcPos.bottom);
	ClientToScreen(m_hWnd, &pt);
	STRINGorID strXmlFile(_T("MenuStatus.xml"));
	pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
}

void CRichEdit::OnRichSendMessage()
{
	ITextServices* pTextServices = m_pSendRichEdit->GetTextServices();

	CDuiString strSendText;
	RichEdit_GetText(pTextServices, strSendText);
	
	pTextServices->Release();

	if (strSendText.GetLength() <= 0)
		return;

	AddMsgToRecvEdit(strSendText);

	m_pSendRichEdit->SetText(_T(""));
	m_pSendRichEdit->SetFocus();
}

void CRichEdit::AddMsgToSendEdit(LPCTSTR lpText)
{
	AddMsg(m_pSendRichEdit, lpText);
	m_pSendRichEdit->EndDown();
}

void CRichEdit::AddTipMsgToRecvEdit(LPCTSTR lpText)
{
	if (NULL == lpText || NULL == *lpText)
		return;

	ITextServices * pTextServices = m_pRecvRichEdit->GetTextServices();
	RichEdit_SetSel(pTextServices, -1, -1);

	RichEdit_ReplaceSel(pTextServices,lpText,_T("微软雅黑"),9,0x696969,FALSE,FALSE,FALSE,FALSE,-1);

	RichEdit_ReplaceSel(pTextServices, _T("\r\n"));
	RichEdit_SetStartIndent(pTextServices, 0);
	m_pRecvRichEdit->EndDown();
}

void CRichEdit::AddMsgToRecvEdit(LPCTSTR lpText)
{
	if (NULL == lpText || NULL == *lpText)
		return;

	m_pRecvRichEdit->SetAutoURLDetect(true);

	CDuiString strRecvTime;
	strRecvTime = FormatTime(time(NULL), _T("%H:%M:%S"));

	ITextServices * pTextServices = m_pRecvRichEdit->GetTextServices();
	RichEdit_SetSel(pTextServices, -1, -1);

	TCHAR cText[2048] = {0};
	wsprintf(cText, _T("%s("), _T("倚天"));

	RichEdit_ReplaceSel(pTextServices, cText, 
		_T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE, FALSE, 0);

	wsprintf(cText, _T("%u"), 43156150);
	RichEdit_ReplaceSel(pTextServices, cText, 
		_T("宋体"), 9, RGB(0, 114, 193), FALSE, FALSE, TRUE, TRUE, 0);

	wsprintf(cText, _T(")  %s\r\n"), strRecvTime);
	RichEdit_ReplaceSel(pTextServices, cText, 
		_T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE, FALSE, 0);

	AddMsg(m_pRecvRichEdit, lpText);

	RichEdit_ReplaceSel(pTextServices, _T("\r\n"));
	RichEdit_SetStartIndent(pTextServices, 0);
	m_pRecvRichEdit->EndDown();

	pTextServices->Release();
}

// "/f["系统表情id"]/s["系统表情index"]/c["自定义图片路径"]"
void CRichEdit::AddMsg(CRichEditUI* pRichEdit, LPCTSTR lpText)
{
	if (NULL == pRichEdit || NULL == lpText || NULL == *lpText)
		return;

	CDuiString strTmp;

	for (LPCTSTR pRichSendText = lpText; *pRichSendText != _T('\0'); pRichSendText++)
	{
		if (*pRichSendText == _T('/'))
		{
			if (*(pRichSendText+1) == _T('/'))
			{
				strTmp += *pRichSendText;
				pRichSendText++;
				continue;
			}
// 			else if (*(p+1) == _T('f'))
// 			{
// 				if (HandleSysFaceId(pRichEdit, p, strTmp))
// 					continue;
// 			}
// 			else if (*(p+1) == _T('s'))
// 			{
// 				if (HandleSysFaceIndex(pRichEdit, p, strTmp))
// 					continue;
// 			}
			else if (*(pRichSendText+1) == _T('c'))
			{
				if (HandleCustomPic(pRichEdit, pRichSendText, strTmp))
					continue;
			}
		}
		strTmp += *pRichSendText;
	}

	if (strTmp.IsEmpty() == FALSE)
		_RichEdit_ReplaceSel(pRichEdit, strTmp);
}

void CRichEdit::_RichEdit_ReplaceSel(CRichEditUI * pRichEdit, LPCTSTR lpszNewText)
{
	ITextServices * pTextServices = pRichEdit->GetTextServices();
	if (pRichEdit == m_pRecvRichEdit)
	{
		RichEdit_ReplaceSel(pTextServices, lpszNewText, 
			m_FontInfo.m_strName, m_FontInfo.m_nSize, 
			m_FontInfo.m_clrText, m_FontInfo.m_bBold, m_FontInfo.m_bItalic, 
			m_FontInfo.m_bUnderLine, FALSE, 300);
	}
	else
	{
		RichEdit_ReplaceSel(pTextServices, lpszNewText);
	}
	pTextServices->Release();
}


BOOL CRichEdit::_RichEdit_InsertFace(CRichEditUI * pRichEdit, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex)
{
	BOOL bRet = FALSE;

	if (NULL == pRichEdit || NULL == lpszFileName || NULL == *lpszFileName)
		return FALSE;

	ITextServices * pTextServices = pRichEdit->GetTextServices();
	ITextHost * pTextHost = pRichEdit->GetTextHost();
	if (pTextServices != NULL && pTextHost != NULL)
	{
		if (pRichEdit == m_pRecvRichEdit)
			RichEdit_SetStartIndent(pTextServices, 300);
		bRet = RichEdit_InsertFace(pTextServices, pTextHost, 
			lpszFileName, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40);
	}

	if (pTextServices != NULL)
		pTextServices->Release();
	if (pTextHost != NULL)
		pTextHost->Release();

	return bRet;
}


BOOL CRichEdit::HandleCustomPic(CRichEditUI * pRichEdit, LPCTSTR& pRichSendText, CDuiString& strRicvText)
{
	CDuiString strSendText(pRichSendText);
	int nPoss = strSendText.Find(_T("[\""),2);
	int nPose = strSendText.Find(_T("\"]"),2);
	if (nPoss == NULL || nPose == NULL)
		return FALSE;

	nPoss += 1;  //去掉开头的引号
	nPose -= 1;   //去掉结尾的引号

	CDuiString strFileName(strSendText.Mid(nPoss+1,nPose-nPoss));	//由于查询的_T("[\"")还包含有一个'['，故需要+1
	if (strFileName.IsEmpty() == FALSE)
	{
		if (strRicvText.IsEmpty() == FALSE)
		{
			_RichEdit_ReplaceSel(pRichEdit, strRicvText); 
			strRicvText = _T("");
		}

		_RichEdit_InsertFace(pRichEdit, strFileName, -1, -1);

		pRichSendText = _tcsstr(pRichSendText+2, _T("\"]"));
		pRichSendText++;
		return TRUE;
	}
	return FALSE;
}

void CRichEdit::OnMenuClick(CControlUI* pControl)
{
	
}

void CRichEdit::RestoreBtnStatus(CControlUI* pControl)
{
	TEventUI event = {0};
	event.Type = UIEVENT_MOUSELEAVE;
	event.pSender = pControl;
	pControl->DoEvent(event);
}