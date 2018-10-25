#include "StdAfx.h"
#include "HideFile.h"
#include "Utils/UIFileList.h"
#include <DllCore/Log/LogHelper.h>
#include <DllCore/Utils/ErrorInfo.h>
#include <DllCore/Authority/Process.h>
CHideFile::CHideFile()
{
	m_bLoad = FALSE;
	m_pFsFltDrv = new CFileSystemFilter;
}

CHideFile::~CHideFile()
{
	delete m_pFsFltDrv;
}

DUI_BEGIN_MESSAGE_MAP(CHideFile, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_MENU, OnMenu)
	//DUI_ON_MSGTYPE(DUI_MSGTYPE_REFRESH, OnRefresh)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	//DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMACTIVATE, OnItemActive)
DUI_END_MESSAGE_MAP()

void CHideFile::SetPaintMagager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CHideFile::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("Hide"))
	{
		CListUI* pList = (CListUI*)msg.pSender;
		//if (pList->GetCurSel() == -1)
		//	return;

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(HIDE_FILE_MENU);
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
		if (pList->GetCurSel() == -1)
		{
			CMenuUI* pHideMenu = pMenu->GetMenuUI();
			CControlUI* pDelete = pHideMenu->FindSubControl(_T("Delete"));
			pDelete->SetEnabled(FALSE);
			CControlUI* pInfo = pHideMenu->FindSubControl(_T("Info"));
			pInfo->SetEnabled(FALSE);
		}
	}
}

void CHideFile::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	LoadItem();
}

void CHideFile::CreateAndShowListItem(CListUI* pList, DWORD dwIndex, USER_FILE_PATH_RULE* pUserFilePathRule)
{
	if (pUserFilePathRule->dwRuleItemCount< dwIndex)
		return;

	CFileListItemUI* pFileItem = NULL;
	if (m_DialogBuilder.GetMarkup()->IsValid() == false)
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(HIDE_FILE_LIST_ITEM_INFO, 0, &m_RootBuilder, m_pPaintManager);
	else
		pFileItem = (CFileListItemUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

	USER_FILE_PATH_RULE_ITEM* pUserFilePathRuleItem = &pUserFilePathRule->FilePathRuleItem[dwIndex];

	TListInfoUI* pListInfo = pList->GetListInfo();
	pList->Add(pFileItem);
	pFileItem->SetFixedHeight(27);
	CLabelUI* pIndex = (CLabelUI*)pFileItem->FindSubControl(_T("Index"));
	if (pIndex != NULL)
	{
		CString strTipInfo;
		strTipInfo.Format(_T("%d"), dwIndex);
		pIndex->SetText(strTipInfo);
		pIndex->SetFont(pListInfo->nFont);
		pIndex->SetForeColor(pListInfo->dwTextColor);
		pIndex->AppendTextStyle(DT_END_ELLIPSIS);
	}

	CLabelUI* pName = (CLabelUI*)pFileItem->FindSubControl(_T("Name"));
	if (pName != NULL)
	{
		CString strFilePathInfo;
		if (DosPathToNtPath(pUserFilePathRuleItem->wszFilePath, strFilePathInfo) != FALSE)
			pName->SetText(strFilePathInfo);
		else
			pName->SetText(pUserFilePathRuleItem->wszFilePath);
		pName->SetFont(pListInfo->nFont);
		pName->SetForeColor(pListInfo->dwTextColor);
		pName->AppendTextStyle(DT_END_ELLIPSIS);
	}

	CLabelUI* pMode = (CLabelUI*)pFileItem->FindSubControl(_T("Mode"));
	if (pMode != NULL)
	{
		pMode->SetText(GetHideFileMode(pUserFilePathRuleItem->HideMode));
		pMode->SetFont(pListInfo->nFont);
		pMode->SetForeColor(pListInfo->dwTextColor);
		pMode->AppendTextStyle(DT_END_ELLIPSIS);
	}

	CLabelUI* pState = (CLabelUI*)pFileItem->FindSubControl(_T("State"));
	if (pState != NULL)
	{
		pState->SetText(pUserFilePathRuleItem->bEnable ? _T("启用") : _T("未启用"));
		pState->SetFont(pListInfo->nFont);
		pState->SetForeColor(pListInfo->dwTextColor);
		pState->AppendTextStyle(DT_END_ELLIPSIS);
	}
}

CString CHideFile::GetHideFileMode(HIDE_FILE_MODE HideFileMode)
{
	LPCTSTR lpszHideFileMode = NULL;
	switch(HideFileMode)
	{
		case HIDE_FILE_MODE_NAME:
				lpszHideFileMode = _T("匹配文件名");
			break;
		case HIDE_FILE_MODE_ALL:
				lpszHideFileMode = _T("匹配全路径");
			break;
		case HIDE_FILE_MODE_PRE:
				lpszHideFileMode = _T("匹配文件名前缀");
			break;
		default:
			lpszHideFileMode = NULL;
	}

	CString strTipInfo;
	if (lpszHideFileMode == NULL)
		strTipInfo.Format(_T("未知类型[%d]"), HideFileMode);
	else
		strTipInfo = lpszHideFileMode;
	return strTipInfo;
}

void CHideFile::OnAppMenu(CControlUI* pControl)
{
	CDuiString strItemName = pControl->GetName();

	if (strItemName == _T("Add"))
		AddItem();
	else if (strItemName == _T("Refresh"))
		LoadItem();
	else
		MessageBox(NULL,_T("开发中，请继续留意^_^。"), MAIN_APP_NOTIFY_TITLE, MB_OK | MB_ICONINFORMATION);
}

void CHideFile::AddItem()
{
	// 请求底层驱动获取文件过滤规则
	if (m_pFsFltDrv->IsOpen() == FALSE)
		m_pFsFltDrv->OpenDrv();

	BYTE bData[1024];
	PSET_USER_FILE_PATH_RULE pSetUserFilePathRule = (PSET_USER_FILE_PATH_RULE)bData;
	pSetUserFilePathRule->FltCommand = FS_FLT_SET_HIDE_FILE_INFO;
	pSetUserFilePathRule->dwRuleItemCount = 1;
	pSetUserFilePathRule->FilePathRuleItem[0].bEnable = TRUE;
	pSetUserFilePathRule->FilePathRuleItem[0].HideMode = HIDE_FILE_MODE_PRE;
	/*DWORD*/
	//ZeroMemory(pSetUserFilePathRule->FilePathRuleItem[0].wszFilePath, sizeof(pSetUserFilePathRule->FilePathRuleItem[0].wszFilePath));
	wcscpy_s(pSetUserFilePathRule->FilePathRuleItem[0].wszFilePath, MAX_PATH, L"Ls");
	DWORD dwLen = offsetof(SET_USER_FILE_PATH_RULE, FilePathRuleItem) + sizeof(USER_FILE_PATH_RULE_ITEM) * pSetUserFilePathRule->dwRuleItemCount;
	
	m_pFsFltDrv->DeviceIoControl((LPVOID)pSetUserFilePathRule, dwLen, NULL, 0, NULL);
	LoadItem();
}

void CHideFile::LoadItem()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Hide"));
	if (pList == NULL)
		return;

	if (pList->GetCount())
		pList->RemoveAll();

	// 请求底层驱动获取文件过滤规则
	if (m_pFsFltDrv->IsOpen() == FALSE)
		m_pFsFltDrv->OpenDrv();

	FS_FLT_COMMAND FsFltCommand = FS_FLT_GET_HIDE_FILE_INFO;
	DWORD dwReturnLen = 0;
	BOOL bRet = m_pFsFltDrv->DeviceIoControl((LPVOID)&FsFltCommand, sizeof(FS_FLT_COMMAND), NULL, 0, &dwReturnLen);
	if (bRet != FALSE )
	{
		BYTE* pFsFltRule = new BYTE[dwReturnLen];
		if (pFsFltRule == NULL)
			return;

		bRet = m_pFsFltDrv->DeviceIoControl((LPVOID)&FsFltCommand, sizeof(FS_FLT_COMMAND), pFsFltRule, dwReturnLen, NULL);
		if (bRet != FALSE)
		{
			TListInfoUI* pListInfo = pList->GetListInfo();
			//去掉DrawText函数中&被转义为_,比如&a会显示为'下划线a'
			pList->SetItemTextStyle(pListInfo->uTextStyle & ~DT_NOPREFIX);

			USER_FILE_PATH_RULE* pUserFilePathRule = (USER_FILE_PATH_RULE*)pFsFltRule;
			for (UINT n=0; n<pUserFilePathRule->dwRuleItemCount; ++n)
				CreateAndShowListItem(pList, n, pUserFilePathRule);
		}
		delete[] pFsFltRule;
	}	
}