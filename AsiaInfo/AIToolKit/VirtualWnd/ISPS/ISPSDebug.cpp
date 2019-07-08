#include "StdAfx.h"
#include "ISPSDebug.h"
#include "DllCore/Utils/OsInfo.h"

#include <shlobj.h>

#define REG__KEY_SUB_PATH						_T("SOFTWARE\\TrendMicro\\PortableSecurity")
#define ST_REG_OUTPUT_KEY_NAME			_T("STKeySwitch")
#define ST_REG_OUTPUT_FILTER_NAME		_T("STdwFlag")
#define MP_REG_OUTPUT_KEY_NAME			_T("MPKeySwitch")
#define MP_REG_OUTPUT_FILTER_NAME		_T("MPdwFlag")


typedef enum tagISPS_LOG_LEVEL
{
	LOG_OFF				= 0x00000000,
	LOG_FATAL			= 0x00000001,
	LOG_ERROR		= 0x00000002,
	LOG_WARN			= 0x00000004,
	LOG_INFO			= 0x00000008,
	LOG_DEBUG		= 0x00000010,
}ISPS_LOG_LEVEL;

typedef enum tagISPS_MP_LOG_LEVEL
{
	LOG_NONE			= 0x00000000,
	LOG_CHECK			= 0x00000001,
	LOG_NOTIFY		= 0x00000010,
	LOG_EXCEP			= 0x00000100,
}ISPS_MP_LOG_LEVEL;

CISPSDebug::CISPSDebug()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
}

CISPSDebug::~CISPSDebug()
{
	m_pPaintManager = NULL;
	m_bLoad = FALSE;
}

DUI_BEGIN_MESSAGE_MAP(CISPSDebug, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

void CISPSDebug::SetPaintManager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CISPSDebug::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnAll"))
		SelectSTAll(TRUE);
	else if (msg.pSender->GetName() == _T("BtnSave"))
		StoreSTCfg();
	else if (msg.pSender->GetName() == _T("BtnMPSave"))
		StoreMPCfg();
	else if (msg.pSender->GetName() == _T("BtnMPAll"))
		SelectMPAll(TRUE);
}

void CISPSDebug::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad == TRUE)
		return;
	m_bLoad = TRUE;

	LoadSTCfg();
	LoadMPCfg();
}

void CISPSDebug::OnSelectChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("STEnable"))
		EnableSTWindows(TRUE);
	else if (msg.pSender->GetName() == _T("STDisable"))
		EnableSTWindows(FALSE);
	else if (msg.pSender->GetName() == _T("MPEnable"))
		EnableMPWindows(TRUE);
	else if (msg.pSender->GetName() == _T("MPDisable"))
		EnableMPWindows(FALSE);
}

void CISPSDebug::SelectSTAll(BOOL bSelect /*= TRUE*/)
{
	COptionUI* pFatal = (COptionUI*)m_pPaintManager->FindControl(_T("LogFatal"));
	pFatal->Selected(bSelect != FALSE);

	COptionUI* pError = (COptionUI*)m_pPaintManager->FindControl(_T("LogError"));
	pError->Selected(bSelect != FALSE);

	COptionUI* pWarn = (COptionUI*)m_pPaintManager->FindControl(_T("LogWarn"));
	pWarn->Selected(bSelect != FALSE);

	COptionUI* pInfo = (COptionUI*)m_pPaintManager->FindControl(_T("LogInfo"));
	pInfo->Selected(bSelect != FALSE);

	COptionUI* pDebug = (COptionUI*)m_pPaintManager->FindControl(_T("LogDebug"));
	pDebug->Selected(bSelect != FALSE);
}

void CISPSDebug::SelectMPAll(BOOL bSelect /*= TRUE*/)
{
	COptionUI* pCheck = (COptionUI*)m_pPaintManager->FindControl(_T("LogCheck"));
	pCheck->Selected(bSelect != FALSE);

	COptionUI* pNotify = (COptionUI*)m_pPaintManager->FindControl(_T("LogNotify"));
	pNotify->Selected(bSelect != FALSE);

	COptionUI* pExcep = (COptionUI*)m_pPaintManager->FindControl(_T("LogExcep"));
	pExcep->Selected(bSelect != FALSE);
}

void CISPSDebug::EnableSTWindows(BOOL bEnable /*= TRUE*/)
{
	COptionUI* pFatal = (COptionUI*)m_pPaintManager->FindControl(_T("LogFatal"));
	pFatal->SetEnabled(bEnable != FALSE);

	COptionUI* pError = (COptionUI*)m_pPaintManager->FindControl(_T("LogError"));
	pError->SetEnabled(bEnable != FALSE);

	COptionUI* pWarn = (COptionUI*)m_pPaintManager->FindControl(_T("LogWarn"));
	pWarn->SetEnabled(bEnable != FALSE);

	COptionUI* pInfo = (COptionUI*)m_pPaintManager->FindControl(_T("LogInfo"));
	pInfo->SetEnabled(bEnable != FALSE);

	COptionUI* pDebug = (COptionUI*)m_pPaintManager->FindControl(_T("LogDebug"));
	pDebug->SetEnabled(bEnable != FALSE);

	CButtonUI* pBtnAll = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnAll"));
	pBtnAll->SetEnabled(bEnable != FALSE);
}

void CISPSDebug::EnableMPWindows(BOOL bEnable /*= TRUE*/)
{
	COptionUI* pCheck = (COptionUI*)m_pPaintManager->FindControl(_T("LogCheck"));
	pCheck->SetEnabled(bEnable != FALSE);

	COptionUI* pNotify = (COptionUI*)m_pPaintManager->FindControl(_T("LogNotify"));
	pNotify->SetEnabled(bEnable != FALSE);

	COptionUI* pExcep = (COptionUI*)m_pPaintManager->FindControl(_T("LogExcep"));
	pExcep->SetEnabled(bEnable != FALSE);

	CButtonUI* pBtnMPAll = (CButtonUI*)m_pPaintManager->FindControl(_T("BtnMPAll"));
	pBtnMPAll->SetEnabled(bEnable != FALSE);
}


void CISPSDebug::LoadSTCfg()
{
	DWORD dwKeyValue = KEY_READ;
	if (OsIsWow64Process())
		dwKeyValue = KEY_READ_WOW64_VALUE;

	DWORD dwEnable = 0;
	LsRegQueryValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, ST_REG_OUTPUT_KEY_NAME, dwEnable, dwKeyValue);

	if (dwEnable)
	{
		COptionUI* pEnable = (COptionUI*)m_pPaintManager->FindControl(_T("STEnable"));
		pEnable->Selected(true);
	}
	else
	{
		COptionUI* pDisable = (COptionUI*)m_pPaintManager->FindControl(_T("STDisable"));
		pDisable->Selected(true);
	}

	DWORD dwFilter = 0;
	LsRegQueryValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, ST_REG_OUTPUT_FILTER_NAME, dwFilter, dwKeyValue);

	SetSTFilterType(dwFilter);
}

void CISPSDebug::LoadMPCfg()
{
	DWORD dwKeyValue = KEY_READ;
	if (OsIsWow64Process())
		dwKeyValue = KEY_READ_WOW64_VALUE;

	DWORD dwEnable = 0;
	LsRegQueryValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, MP_REG_OUTPUT_KEY_NAME, dwEnable, dwKeyValue);

	if (dwEnable)
	{
		COptionUI* pEnable = (COptionUI*)m_pPaintManager->FindControl(_T("MPEnable"));
		pEnable->Selected(true);
	}
	else
	{
		COptionUI* pDisable = (COptionUI*)m_pPaintManager->FindControl(_T("MPDisable"));
		pDisable->Selected(true);
	}

	DWORD dwFilter = 0;
	LsRegQueryValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, MP_REG_OUTPUT_FILTER_NAME, dwFilter, dwKeyValue);

	SetMPFilterType(dwFilter);
}

void CISPSDebug::SetSTFilterType(DWORD dwFilter)
{
	COptionUI* pTatal = (COptionUI*)m_pPaintManager->FindControl(_T("LogFatal"));
	pTatal->Selected((dwFilter & LOG_FATAL) == LOG_FATAL);

	COptionUI* pError = (COptionUI*)m_pPaintManager->FindControl(_T("LogError"));
	pError->Selected((dwFilter & LOG_ERROR) == LOG_ERROR);

	COptionUI* pWarn = (COptionUI*)m_pPaintManager->FindControl(_T("LogWarn"));
	pWarn->Selected((dwFilter & LOG_WARN) == LOG_WARN);

	COptionUI* pInfo = (COptionUI*)m_pPaintManager->FindControl(_T("LogInfo"));
	pInfo->Selected((dwFilter & LOG_INFO) == LOG_INFO);

	COptionUI* pDebug = (COptionUI*)m_pPaintManager->FindControl(_T("LogDebug"));
	pDebug->Selected((dwFilter & LOG_DEBUG) == LOG_DEBUG);
}

void CISPSDebug::SetMPFilterType(DWORD dwFilter)
{
	COptionUI* pCheck = (COptionUI*)m_pPaintManager->FindControl(_T("LogCheck"));
	pCheck->Selected((dwFilter & LOG_CHECK) == LOG_CHECK);

	COptionUI* pNotify = (COptionUI*)m_pPaintManager->FindControl(_T("LogNotify"));
	pNotify->Selected((dwFilter & LOG_NOTIFY) == LOG_NOTIFY);

	COptionUI* pExcep = (COptionUI*)m_pPaintManager->FindControl(_T("LogExcep"));
	pExcep->Selected((dwFilter & LOG_EXCEP) == LOG_EXCEP);
}

void CISPSDebug::StoreSTCfg()
{
	DWORD dwKeyValue = KEY_SET_VALUE;
	if (OsIsWow64Process())
		dwKeyValue = KEY_SET_WOW64_VALUE;

	COptionUI* pEnable = (COptionUI*)m_pPaintManager->FindControl(_T("STEnable"));
	DWORD dwEnable = pEnable->IsSelected();

	DWORD dwFilter = 0;
	COptionUI* pFatal = (COptionUI*)m_pPaintManager->FindControl(_T("LogFatal"));
	if (pFatal->IsSelected())
		dwFilter |= LOG_FATAL;

	COptionUI* pError = (COptionUI*)m_pPaintManager->FindControl(_T("LogError"));
	if (pError->IsSelected())
		dwFilter |= LOG_ERROR;

	COptionUI* pWarn = (COptionUI*)m_pPaintManager->FindControl(_T("LogWarn"));
	if (pWarn->IsSelected())
		dwFilter |= LOG_WARN;

	COptionUI* pInfo = (COptionUI*)m_pPaintManager->FindControl(_T("LogInfo"));
	if (pInfo->IsSelected())
		dwFilter |= LOG_INFO;

	COptionUI* pDebug = (COptionUI*)m_pPaintManager->FindControl(_T("LogDebug"));
	if (pDebug->IsSelected())
		dwFilter |= LOG_DEBUG;

	LsRegSetValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, ST_REG_OUTPUT_KEY_NAME, dwEnable, dwKeyValue);

	LsRegSetValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, ST_REG_OUTPUT_FILTER_NAME, dwFilter, dwKeyValue);
}

void CISPSDebug::StoreMPCfg()
{
	DWORD dwKeyValue = KEY_SET_VALUE;
	if (OsIsWow64Process())
		dwKeyValue = KEY_SET_WOW64_VALUE;

	COptionUI* pEnable = (COptionUI*)m_pPaintManager->FindControl(_T("MPEnable"));
	DWORD dwEnable = pEnable->IsSelected();

	DWORD dwFilter = 0;
	COptionUI* pCheck = (COptionUI*)m_pPaintManager->FindControl(_T("LogCheck"));
	if (pCheck->IsSelected())
		dwFilter |= LOG_CHECK;

	COptionUI* pNotify = (COptionUI*)m_pPaintManager->FindControl(_T("LogNotify"));
	if (pNotify->IsSelected())
		dwFilter |= LOG_NOTIFY;

	COptionUI* pExcep = (COptionUI*)m_pPaintManager->FindControl(_T("LogExcep"));
	if (pExcep->IsSelected())
		dwFilter |= LOG_EXCEP;

	LsRegSetValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, MP_REG_OUTPUT_KEY_NAME, dwEnable, dwKeyValue);

	LsRegSetValue(HKEY_LOCAL_MACHINE, REG__KEY_SUB_PATH, MP_REG_OUTPUT_FILTER_NAME, dwFilter, dwKeyValue);
}