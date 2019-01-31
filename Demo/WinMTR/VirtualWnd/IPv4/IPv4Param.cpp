#include "StdAfx.h"
#include "IPv4Param.h"
#include "DllCore/Json/JsonObject.h"

CIPv4Param::CIPv4Param()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
	m_pVirtualWndMgr = NULL;
}

CIPv4Param::~CIPv4Param()
{
	m_bLoad = FALSE;
	m_pPaintManager = NULL;
	m_pVirtualWndMgr = NULL;
}

DUI_BEGIN_MESSAGE_MAP(CIPv4Param, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_LOADITEM, OnLoadItem)
DUI_END_MESSAGE_MAP()

void CIPv4Param::SetPaintMagager(CPaintManagerUI *pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CIPv4Param::SetOwner(IVirtualWndMgr* pVirtualWndMgr)
{
	m_pVirtualWndMgr = pVirtualWndMgr;
}

void CIPv4Param::OnClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("BtnOk"))
	{
		// 获取主机地址或域名
		CEditUI2* pHostName = (CEditUI2*)m_pPaintManager->FindControl(_T("Addr"));
		CString strHostName = pHostName->GetText();

		if (strHostName.IsEmpty())
		{
			MessageBox(m_pPaintManager->GetPaintWindow(), _T("主机地址或域名不能为空，请确认后重新输入！"), _T("提示"), MB_OK | MB_ICONINFORMATION);
			return;
		}

		// 获取Tab页指针
		CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pPaintManager->FindControl(_T("IPv4Wizard"));
		if (pTabLayout == NULL)
			return;

		// 构造TabLayout页面
		CVerticalLayoutUI* pResult = NULL;
		if (m_DialogBuilder.GetMarkup()->IsValid() == false)
			pResult = (CVerticalLayoutUI*)m_DialogBuilder.Create(_T("PingResult.xml"), 0, &m_RootBuilder, m_pPaintManager);
		else
			pResult = (CVerticalLayoutUI*)m_DialogBuilder.Create(&m_RootBuilder, m_pPaintManager);

		pTabLayout->Add(pResult);

		CString strVirtualWnd;
		strVirtualWnd.Format(_T("ipv4param%d"), pTabLayout->GetCount());
		pResult->SetVirtualWnd(strVirtualWnd);

		CHorizontalLayoutUI* pLayoutUI = (CHorizontalLayoutUI*)m_pPaintManager->FindControl(_T("VLayoutIPv4"));
		if (pLayoutUI == NULL)
			return;

		CControlUI* pSpaceUI = new CControlUI;
		pLayoutUI->Add(pSpaceUI);
		pSpaceUI->SetFixedWidth(5);

		COptionUI* pOptionUI = new COptionUI;
		pLayoutUI->Add(pOptionUI);
		pOptionUI->SetText(strHostName);
		pOptionUI->SetToolTip(strHostName);
		pOptionUI->SetAttribute(_T("font"), _T("2"));

		CDuiString strTipInfo;
		strTipInfo.Format(_T("%d"), pTabLayout->GetCount() - 1);
		pOptionUI->SetAttribute(_T("style"), _T("IPv4Option"));
		pOptionUI->SetAttribute(_T("userdata"), strTipInfo);

		if (m_pVirtualWndMgr != NULL)
			m_pVirtualWndMgr->AddVirtualWnd(strVirtualWnd);

		CEditUI2* pInterval = (CEditUI2*)m_pPaintManager->FindControl(_T("Interval"));
		CEditUI2* pCount = (CEditUI2*)m_pPaintManager->FindControl(_T("Times"));
		CEditUI2* pPacketSize = (CEditUI2*)m_pPaintManager->FindControl(_T("Size"));

		// 构造Ping测试参数信息
		CJsonObject JsonObject;
		JsonObject.SetValue(JSON_HOSTNAME, strHostName);
		JsonObject.SetValue(JSON_INTERVAL, _ttoi(pInterval->GetText()));
		JsonObject.SetValue(JSON_COUNT, _ttoi(pCount->GetText()));

		DWORD dwSize = _ttoi(pPacketSize->GetText());
		if (dwSize == 0)
			dwSize = 32;
		if (dwSize >65550)
		{
			MessageBox(m_pPaintManager->GetPaintWindow(), _T("网络包大小不正确，请重新输入！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
			pPacketSize->SetFocus();
			return;
		}
		
		JsonObject.SetValue(JSON_PACKETSIZE, (UINT)dwSize);
		// 设置超时时间
		CEditUI2* pTimeout = (CEditUI2*)m_pPaintManager->FindControl(_T("Timeout"));
		DWORD dwTimeout = _ttoi(pTimeout->GetText());
		if (dwTimeout == 0)
			dwTimeout = 2;
		if (dwTimeout > 10)
		{
			MessageBox(m_pPaintManager->GetPaintWindow(), _T("数据包超时时间不正确，请重新输入！"), _T("提示"), MB_OK|MB_ICONINFORMATION);
			pTimeout->SetFocus();
			return;
		}

		JsonObject.SetValue(JSON_TIMEOUT, _ttoi(pTimeout->GetText())*1000);
		// 设置TimeToLive 数据包最大转发次数
		CEditUI2* pTTL = (CEditUI2*)m_pPaintManager->FindControl(_T("TTL"));
		JsonObject.SetValue(JSON_TIMETOLIVE, _ttoi(pTTL->GetText()));

		NTCHDR NotifyHDR;
		NotifyHDR.nWizardId		= WIZARD_ID_IPV4;
		NotifyHDR.strTabTo		= strVirtualWnd;
		NotifyHDR.strTabFrom	= VIRTUAL_WND_IPv4Param;
		NotifyHDR.strData			= JsonObject.ToString();

		SendMessage(m_pPaintManager->GetPaintWindow(), WM_NOTIFY_TAB_CHANGE, NULL, (LPARAM)&NotifyHDR);
	}
}

void CIPv4Param::OnLoadItem(TNotifyUI& msg)
{
	if (m_bLoad != FALSE)
		return;
	m_bLoad = TRUE;

	// 加载历史记录
	//	OnRefresh(msg);
}