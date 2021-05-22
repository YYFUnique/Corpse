#include "StdAfx.h"
#include "HideFile.h"
#include "Utils/UIFileList.h"
#include "Wnd/AddPathFileItem.h"
#include <DllCore/Log/LogHelper.h>
#include <DllCore/Utils/ErrorInfo.h>
#include <DllCore/Authority/Process.h>

CHideFile::CHideFile()
	: m_pFsFltDrv(NULL)
{
	m_bLoad = FALSE;
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

		CMenuWnd* pMenu = new CMenuWnd;
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_pPaintManager->GetPaintWindow(), &pt);
		STRINGorID strXmlFile(HIDE_FILE_MENU);
		pMenu->Init(NULL,strXmlFile, pt,m_pPaintManager);
		/*if (pList->GetCurSel() == -1)
		{
			CMenuUI* pHideMenu = pMenu->GetMenuUI();
			CControlUI* pDelete = pHideMenu->FindSubControl(_T("Delete"));
			pDelete->SetEnabled(FALSE);
			CControlUI* pInfo = pHideMenu->FindSubControl(_T("Info"));
			pInfo->SetEnabled(FALSE);
		}*/
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
		strTipInfo.Format(_T("%d"), dwIndex + 1);
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
		pMode->SetText(GetHideFileMode(pUserFilePathRuleItem->MatchMode));
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

CString CHideFile::GetHideFileMode(MATCH_MODE MatchMode)
{
	LPCTSTR lpszMatchMode = NULL;
	switch(MatchMode)
	{
		case MATCH_MODE_PRE:
				lpszMatchMode = _T("匹配名称前缀");
			break;
		case MATCH_MODE_SUB:
				lpszMatchMode = _T("匹配包含名称");
			break;
		case MATCH_MODE_NAME:
				lpszMatchMode = _T("匹配文件名");
			break;
		case MATCH_MODE_ALL:
				lpszMatchMode = _T("匹配全路径");
			break;
		default:
			lpszMatchMode = NULL;
	}

	CString strTipInfo;
	if (lpszMatchMode == NULL)
		strTipInfo.Format(_T("未知类型[%d]"), MatchMode);
	else
		strTipInfo = lpszMatchMode;
	return strTipInfo;
}

void CHideFile::OnAppMenu(CControlUI* pControl)
{
	CDuiString strItemName = pControl->GetName();

	if (strItemName == _T("Add"))
		AddItem();
	else if (strItemName == _T("Refresh"))
		LoadItem();
	else if (strItemName == _T("Delete"))
		DeleteItem();
	else if (strItemName == _T("Stop"))
		StopFilter();
	else
		MessageBox(NULL,_T("开发中，请继续留意^_^。"), MAIN_APP_NOTIFY_TITLE, MB_OK | MB_ICONINFORMATION);
}

void CHideFile::AddItem()
{
	if (m_pFsFltDrv == NULL)
	{
		m_pFsFltDrv = new CFileSystemFilter;
		m_pFsFltDrv->OpenDrv();
	}

	CAddPathFileItem* pAddFileItem = new CAddPathFileItem(m_pPaintManager->GetPaintWindow());
	if (pAddFileItem != NULL)
	{
		pAddFileItem->ShowModal();

		BYTE bData[1024];
		PSET_USER_FILE_PATH_RULE pSetUserFilePathRule = (PSET_USER_FILE_PATH_RULE)bData;
		pSetUserFilePathRule->dwRuleItemCount = 1;
		pSetUserFilePathRule->FltCommand = FS_FLT_SET_PATH_FILE_INFO;
		pSetUserFilePathRule->FilePathRuleItem[0].bEnable = pAddFileItem->IsMatchRuleEnabled();
		pSetUserFilePathRule->FilePathRuleItem[0].MatchMode = (MATCH_MODE)pAddFileItem->GetMatchMode();
		CStringW strFilePath(pAddFileItem->GetMatchFilePath());
		wcscpy_s(pSetUserFilePathRule->FilePathRuleItem[0].wszFilePath, MAX_PATH, strFilePath);

		if (m_pFsFltDrv->DeviceIoControl(pSetUserFilePathRule, _countof(bData)) == FALSE)
		{
			m_pFsFltDrv->CloseDrv();
			MessageBox(m_pPaintManager->GetPaintWindow(), GetThreadErrorInfoString(), _T("提示"), MB_OK|MB_ICONINFORMATION);
		}

		delete pAddFileItem;
	}
}

void CHideFile::LoadItem()
{
	CListUI* pList = (CListUI*)m_pPaintManager->FindControl(_T("Hide"));
	if (pList == NULL)
		return;

	if (pList->GetCount())
		pList->RemoveAll();

	if (m_pFsFltDrv == NULL)
		m_pFsFltDrv = new CFileSystemFilter;

	// 请求底层驱动获取文件过滤规则
	if (m_pFsFltDrv->IsOpen() == FALSE)
		m_pFsFltDrv->OpenDrv();

	FS_FLT_COMMAND FsFltCommand = FS_FLT_GET_PATH_FILE_INFO;
	DWORD dwReturnLen = 0;
	BOOL bRet = m_pFsFltDrv->DeviceIoControl((LPVOID)&FsFltCommand, sizeof(FS_FLT_COMMAND), NULL, 0, &dwReturnLen);
	if (bRet != FALSE)
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
	else
		m_pFsFltDrv->CloseDrv();	// 如果调用过滤驱动设置信息失败，那么有可能是过滤驱动重新启动，导致句柄无效
}

void CHideFile::StopFilter()
{
	if (m_pFsFltDrv != NULL)
		m_pFsFltDrv->StopDrvService();
}

void CHideFile::DeleteItem()
{

}