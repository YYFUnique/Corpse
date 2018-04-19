#include "StdAfx.h"
#include "ClipBoardMonitor.h"
#include "Utils/TextTools.h"
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Log/LogHelper.h"

CClipBoardMonitor::CClipBoardMonitor()
{
	m_bFirstRun = TRUE;
	m_pPaintManager = NULL;
	m_hNextClipboard = NULL;
}

CClipBoardMonitor::~CClipBoardMonitor()
{
	m_pPaintManager = NULL;
	ChangeClipboardChain(m_pPaintManager->GetPaintWindow(), m_hNextClipboard);
}

DUI_BEGIN_MESSAGE_MAP(CClipBoardMonitor, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CClipBoardMonitor::SetPaintMagager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CClipBoardMonitor::OnLoadItem(TNotifyUI& msg)
{
	if (m_hNextClipboard != NULL)
		return;

	// Visit 及以后操作系统可以使用 AddClipboardFormatListener
	m_hNextClipboard = SetClipboardViewer(m_pPaintManager->GetPaintWindow());
}

void CClipBoardMonitor::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter)
{
	if (m_hNextClipboard == NULL)
		m_hNextClipboard = hWndAfter;

	if (hWndRemove == m_hNextClipboard)
		m_hNextClipboard = hWndAfter;
	else
		::PostMessage(m_hNextClipboard, WM_CHANGECBCHAIN, (WPARAM)hWndRemove, (LPARAM)hWndAfter);
}

void CClipBoardMonitor::OnDrawClipboard()
{
	if (m_hNextClipboard)  
		::SendMessage(m_hNextClipboard, WM_DRAWCLIPBOARD, 0, 0); 

	if (GetClipboardOwner() != m_pPaintManager->GetPaintWindow())
		ShowClipBoardData();
}

void CClipBoardMonitor::ShowClipBoardData()
{
	////////////设置SetClipboardViewer系统会发送WM_DRAWCLIPBOARD消息//////////////
	if (m_bFirstRun == TRUE)
	{
		m_bFirstRun = FALSE;
		return;
	}

	if (OpenClipboard(m_pPaintManager->GetPaintWindow()) == FALSE)
		return ;

	UINT nFormat[] = {CF_BITMAP, CF_TEXT, CF_UNICODETEXT, CF_HDROP};
	UINT nFormatType = GetPriorityClipboardFormat(nFormat, sizeof(nFormat)); 

	switch (nFormatType)
	{
	case CF_TEXT:
		{
			HGLOBAL hClipboardData = GetClipboardData(CF_TEXT);
			LPCSTR lpStr = (LPCSTR)GlobalLock(hClipboardData); 
			GlobalUnlock(hClipboardData); 
			CString strAsniCode(lpStr);
			InsertItemToList(strAsniCode,CLIPBOARDTYPE_Text);
		}
		break;
	case CF_UNICODETEXT:
		{
			HGLOBAL hClipboardData = GetClipboardData(CF_UNICODETEXT);
			LPCTSTR lpStr = (LPCTSTR)GlobalLock(hClipboardData); 
			GlobalUnlock(hClipboardData); 
			InsertItemToList(lpStr,CLIPBOARDTYPE_Text);
		}
		break;
	case CF_BITMAP:
		//hClipboardData = GetClipboardData(CF_BITMAP);
		break;
	case CF_HDROP:
		{
			HDROP hMem = (HDROP)::GetClipboardData(CF_HDROP);
			int iFiles = DragQueryFile(hMem, -1, NULL, 0);
			for (int n=0; n<iFiles; ++n)
			{
				UINT uch = DragQueryFile(hMem, n, NULL, 0);
				LPTSTR lpFilePath = new TCHAR[uch + 1];

				DragQueryFile(hMem, n, lpFilePath, uch + 1);
				InsertItemToList(lpFilePath, CLIPBOADRTYPE_File);
				delete[] lpFilePath;
			}
		}
		break;
	}

	CloseClipboard();
}

void CClipBoardMonitor::InsertItemToList(LPCTSTR lpszClipData, CLIPBOADRTYPE ClipBoardType)
{
	CFileListUI* pMonitorList = (CFileListUI*)m_pPaintManager->FindControl(_T("ClipBord"));
	TListInfoUI* pListInfo = pMonitorList->GetListInfo();

	CFileListItemUI* pClipBoardMonitor = NULL;
	if (m_DialogBuilder.GetMarkup()->IsValid() == false)
		pClipBoardMonitor = (CFileListItemUI*)m_DialogBuilder.Create(CLIP_BOARD_ITEM_INFO, NULL, &m_RootBuilder, m_pPaintManager);
	else
		pClipBoardMonitor = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

	ASSERT(pClipBoardMonitor);
	pMonitorList->Add(pClipBoardMonitor);
	pClipBoardMonitor->SetFixedHeight(27);
	pClipBoardMonitor->SetTag((UINT_PTR)ClipBoardType);

	CLabelUI* pLabelIndex = (CLabelUI*)pClipBoardMonitor->FindSubControl(_T("Index"));
	pLabelIndex->SetText(GetFormatNum(pMonitorList->GetCount()));
	pLabelIndex->SetFont(pListInfo->nFont);
	pLabelIndex->SetForeColor(pListInfo->dwTextColor);

	/*CLabelUI* pLabelType = (CLabelUI*)pClipBoardMonitor->FindSubControl(_T("Type"));
	pLabelType->SetText(ConvertClipBoardTypeToString(ClipBoardType));
	pLabelType->SetFont(pListInfo->nFont);
	pLabelType->SetForeColor(pListInfo->dwTextColor);*/

	HICON hFileIcon = NULL;
	if (ClipBoardType == CLIPBOADRTYPE_File)
		hFileIcon = GetFileTypeIcon(lpszClipData);

	CPictureUI* pIcon = (CPictureUI*)pClipBoardMonitor->FindSubControl(_T("Pic"));
	pIcon->SetVisible(true);
	if (hFileIcon)
	{
		pIcon->SetIcon(hFileIcon);
		DestroyIcon(hFileIcon);
	}
	else
	{
		pIcon->SetForeImage(_T("txt.png"));
	}
	
	CLabelUI* pLabelText = (CLabelUI*)pClipBoardMonitor->FindSubControl(_T("Content"));
	pLabelText->SetText(lpszClipData);
	pLabelText->SetFont(pListInfo->nFont);
	//pLabelText->
	pLabelText->SetForeColor(pListInfo->dwTextColor);
	//pLabelText->SetAttribute(_T("endellipsis"), _T("true"));
	pLabelText->AppendTextStyle(DT_END_ELLIPSIS|DT_NOPREFIX);

	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	CDuiString strLocalTime;
	strLocalTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
									 SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	CLabelUI* pLabelTime = (CLabelUI*)pClipBoardMonitor->FindSubControl(_T("Time"));
	pLabelTime->SetText(strLocalTime);
	pLabelTime->SetFont(pListInfo->nFont);
	pLabelTime->SetForeColor(pListInfo->dwTextColor);
}

CString CClipBoardMonitor::ConvertClipBoardTypeToString(CLIPBOADRTYPE ClipBoardType)
{
	CString strClipBoardType = _T("未知类型");
	switch(ClipBoardType)
	{
	case CLIPBOARDTYPE_Text:
		strClipBoardType = _T("文本");
		break;
	case CLIPBOARDTYPE_Img:
		strClipBoardType = _T("图像文件");
		break;
	case CLIPBOADRTYPE_File:
		strClipBoardType = _T("文件");
		break;
	case CLIPBOADRTYPE_Execute:
		strClipBoardType = _T("可执行文件");	
		break;
	case CLIPBOADRTYPE_Folder:
		strClipBoardType = _T("文件夹");
		break;
	case CLIPBOADRTYPE_Disk:
		strClipBoardType = _T("磁盘");
		break;
	}

	return strClipBoardType;
}

HICON CClipBoardMonitor::GetFileTypeIcon(LPCTSTR lpszSrcFilePath)
{
	HICON hIcon = NULL;
	do 
	{
		if (PathFileExists(lpszSrcFilePath) == FALSE)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 ,_T("文件%s不存在"),lpszSrcFilePath);
			break;
		}

		SHFILEINFO ShFileInfo;
		memset(&ShFileInfo , 0 , sizeof(SHFILEINFO));
		DWORD dwRet = SHGetFileInfo(lpszSrcFilePath , NULL , &ShFileInfo , sizeof(SHFILEINFO) , SHGFI_ICON|SHGFI_SMALLICON);
		if (dwRet == 0)
		{
			SetErrorInfo(SYSTEM_ERROR , 0 , _T("获取文件图标失败"));
			break;
		}

		//HICONDestroyIcon(ShFileInfo.hIcon);
		hIcon = ShFileInfo.hIcon;

	} while (FALSE);

	return hIcon;
}