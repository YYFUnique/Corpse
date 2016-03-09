
#include "StdAfx.h"
#include "IPAddress.h"
#include <iphlpapi.h>
#include <icmpapi.h>
#include <SetupAPI.h>
#pragma comment(lib,"Iphlpapi.lib")

#define PING_TIMES 4
// CIpDlg 对话框

IMPLEMENT_DYNAMIC(CIpAddressDlg, CDialog)

CIpAddressDlg::CIpAddressDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIpAddressDlg::IDD, pParent)
{

}

CIpAddressDlg::~CIpAddressDlg()
{
}

void CIpAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_IPAddr);
	DDX_Control(pDX, IDC_EDIT_IP_ADDR, m_EditAddr);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CIpAddressDlg, CDialog)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS, &CIpAddressDlg::OnIpnFieldchangedIpaddress)
	ON_BN_CLICKED(IDC_BTN_CHECK_NETWORK, &CIpAddressDlg::OnBnClickedBtnCheckNetwork)
	ON_EN_CHANGE(IDC_EDIT_IP_ADDR, &CIpAddressDlg::OnEnChangeEditIpAddr)
END_MESSAGE_MAP()


// CIpDlg 消息处理程序
BOOL CIpAddressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(_T("IP地址转换工具"));
	CString strIpAddr;
	//m_IPAddr.SetFocus();
	m_IPAddr.GetWindowText(strIpAddr);
	GetDlgItem(IDC_BTN_CHECK_NETWORK)->EnableWindow(strIpAddr.IsEmpty());
	return TRUE;
}

void CIpAddressDlg::ShowMemberValue(DWORD dwIp,int nControlId)
{
	if (INADDR_NONE == dwIp)
		return ;
	GetDlgItem(IDC_BTN_CHECK_NETWORK)->EnableWindow(dwIp);
	if (nControlId != IDC_IPADDRESS)
	{
		char* szIP;
		IN_ADDR Addr;
		Addr.S_un.S_addr=dwIp;
		szIP = inet_ntoa(Addr);
		CString strIp(szIP);
		m_IPAddr.SetWindowText(strIp);
	}
	if (nControlId != IDC_EDIT_IP_ADDR)
	{
		SetDlgItemInt(IDC_EDIT_IP_ADDR,dwIp,FALSE);
	}
}


void CIpAddressDlg::OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString strIpAddr;
	m_IPAddr.GetWindowText(strIpAddr);
	DWORD dwIp;
	CStringA strIpAddrA(strIpAddr);
	dwIp = inet_addr(strIpAddrA);
	ShowMemberValue(dwIp,IDC_IPADDRESS);
	*pResult = 0;
}

void CIpAddressDlg::OnBnClickedBtnCheckNetwork()
{
	GetDlgItem(IDC_BTN_CHECK_NETWORK)->EnableWindow(FALSE);
	// TODO: 在此添加控件通知处理程序代码
	HANDLE hThread;
	hThread = AfxBeginThread(CheckNetWork,this);
	GetDlgItem(IDC_BTN_CHECK_NETWORK)->EnableWindow(TRUE);
}

void CIpAddressDlg::OnEnChangeEditIpAddr()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	int dwIP = GetDlgItemInt(IDC_EDIT_IP_ADDR,NULL,FALSE);
	ShowMemberValue(dwIP,IDC_EDIT_IP_ADDR);
}

UINT CIpAddressDlg::CheckNetWork(LPVOID lparam)
{
	BOOL bSuccess = FALSE;
	CIpAddressDlg* pIPdlg = (CIpAddressDlg*)lparam;
	pIPdlg->GetDlgItem(IDC_BTN_CHECK_NETWORK)->EnableWindow(FALSE);
	TCHAR szTitle[MAX_TITLE_LEN];
	LRESULT lResult;
	int nCopied=MAX_TITLE_LEN;
	do 
	{
		lResult = ::SendMessage(pIPdlg->m_IPAddr.m_hWnd,WM_GETTEXT,(WPARAM)nCopied,(LPARAM)szTitle);  
		CStringA strIPA(szTitle);
		DWORD dwIp = inet_addr(strIPA);
		if (INADDR_NONE == dwIp)
			break;

		HANDLE hIcmpFile = INVALID_HANDLE_VALUE;
		hIcmpFile = IcmpCreateFile();
		if (hIcmpFile == INVALID_HANDLE_VALUE)
			break;

		DWORD dwReplySize = 0;
		CString strSendData(_T("Check NetWork"));
		LPVOID pReplyBuffer = NULL;
		DWORD dwRetVal = 0;
		int iSendDataSize = strSendData.GetLength()*sizeof(TCHAR);
		dwReplySize = sizeof(ICMP_ECHO_REPLY)+iSendDataSize;
		pReplyBuffer = new BYTE[dwReplySize];
		pIPdlg->m_ListCtrl.ResetContent();
		for (UINT nIndex=0; nIndex<PING_TIMES; ++nIndex)
		{
			memset(pReplyBuffer,0,dwReplySize);
			dwRetVal = IcmpSendEcho(hIcmpFile,dwIp,(LPTSTR)(LPCTSTR)strSendData,iSendDataSize,NULL,pReplyBuffer,dwReplySize,1000);
			CString str;
			PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)pReplyBuffer;
			if (dwRetVal)
			{
				in_addr ReplyAddr;
				ReplyAddr.S_un.S_addr = pEchoReply->Address;
				CString strRecvFrom(inet_ntoa(ReplyAddr));
				CString strConnect;
				strConnect.Format(_T("来自 %s 的回复: 字节=%d 时间=%dms \r\n"),strRecvFrom,
					pEchoReply->DataSize,
					pEchoReply->RoundTripTime);
				pIPdlg->m_ListCtrl.InsertString(-1,strConnect);
				::PostMessage(pIPdlg->m_ListCtrl,WM_VSCROLL,SB_BOTTOM,0);
				Sleep(200);
			}
		}
		pIPdlg->m_ListCtrl.InsertString(-1,_T("=============================\r\n"));

		IcmpCloseHandle(hIcmpFile);
		if (pReplyBuffer)
			delete[] pReplyBuffer;

		bSuccess = TRUE;
	} while (FALSE);
	
	pIPdlg->GetDlgItem(IDC_BTN_CHECK_NETWORK)->EnableWindow(TRUE);

	return bSuccess;
}

void CIpAddressDlg::OnCancel()
{

}

void CIpAddressDlg::OnOK()
{

}