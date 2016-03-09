
// NetShareMgrDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NetShareMgr.h"
#include "NetShareMgrDlg.h"

#include <lm.h>
#pragma comment(lib,"Netapi32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "LsCommon/ErrorInfo.h"
#include "LsCommon/CommonFunc.h"
// CNetShareMgrDlg �Ի���




CNetShareMgrDlg::CNetShareMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetShareMgrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetShareMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNetShareMgrDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK,IDC_LIST_NETSHARE,&CNetShareMgrDlg::OnRClick)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CNetShareMgrDlg::OnBnClickedBtnRefresh)
	ON_COMMAND(ID_MENU_REFRESH, &CNetShareMgrDlg::OnMenuRefresh)
	ON_COMMAND(ID_MENU_DELETE, &CNetShareMgrDlg::OnMenuDelete)
END_MESSAGE_MAP()


// CNetShareMgrDlg ��Ϣ�������

BOOL CNetShareMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_NETSHARE);
	int nIndex=0;
	pListCtrl->InsertColumn(nIndex++,_T("���繲����"));
	pListCtrl->InsertColumn(nIndex++,_T("����·��"));
	pListCtrl->InsertColumn(nIndex++,_T("����"));
	pListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT);
	if (ShowNetShare() == FALSE)
	{
		MessageBox(GetThreadErrorInfoString(),_T("��ʾ"),MB_OK);
		PostQuitMessage(0);
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNetShareMgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CNetShareMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CNetShareMgrDlg::EnumLocalNetShare(CLsNetShareInfoArray& NetShareInfoArray)
{
	DWORD dwReadItemCount = 0;
	DWORD dwAllItemCount = 0;

	LPBYTE lpDataBuffer = NULL;
	NET_API_STATUS dwRet = NetShareEnum (NULL , 502, &lpDataBuffer , MAX_PREFERRED_LENGTH , &dwReadItemCount, &dwAllItemCount, NULL);
	if (dwRet != ERROR_SUCCESS)
	{
		SetErrorInfo(SYSTEM_ERROR , dwRet , _T("ö�ٵ�ǰ�������繲����Ϣʧ��"));
		return FALSE;
	}

	PSHARE_INFO_502 pShareInfo = (PSHARE_INFO_502)lpDataBuffer;
	for(DWORD i=0;i<dwReadItemCount;i++,++pShareInfo)
	{
		//����IPC$��XPϵͳ���޷�ʹ������ɾ��������ʱ���Ըù���Ŀ¼���д���
		//if (_wcsicmp(pShareInfo->shi502_netname,L"IPC$") == 0)
		//	continue;

		LS_NET_SHARE_INFO LsNetShareInfo;
		LsNetShareInfo.strNetName = pShareInfo->shi502_netname;
		LsNetShareInfo.dwType = pShareInfo->shi502_type;
		LsNetShareInfo.strRemark = pShareInfo->shi502_remark;
		LsNetShareInfo.dwPermissions = pShareInfo->shi502_permissions;
		LsNetShareInfo.dwMaxUses = pShareInfo->shi502_max_uses;
		LsNetShareInfo.dwCurrentUses = pShareInfo->shi502_current_uses;
		LsNetShareInfo.strFilePath = pShareInfo->shi502_path;
		LsNetShareInfo.strPassword = pShareInfo->shi502_passwd;
		NetShareInfoArray.Add(LsNetShareInfo);
	}
	NetApiBufferFree(lpDataBuffer);

	return TRUE;
}

BOOL CNetShareMgrDlg::ShowNetShare()
{
	CLsNetShareInfoArray LsNetShareInfoArray;
	if (EnumLocalNetShare(LsNetShareInfoArray) != TRUE)
	{
		SetErrorTitle(_T("�������繲��ʧ��"));
		return FALSE;
	}

	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_NETSHARE);

	pListCtrl->DeleteAllItems();
	for (int i=0;i<LsNetShareInfoArray.GetSize();++i)
	{
		LS_NET_SHARE_INFO& LsNetShareInfo = LsNetShareInfoArray.ElementAt(i);
		int nItem = pListCtrl->InsertItem(0x7FFFFFFF,LsNetShareInfo.strNetName);
		pListCtrl->SetItemText(nItem,1,LsNetShareInfo.strFilePath);
		pListCtrl->SetItemText(nItem,2,LsNetShareInfo.strRemark);
	}

	AdjustListWidth(*pListCtrl);
	return TRUE;
}
void CNetShareMgrDlg::OnBnClickedBtnRefresh()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (ShowNetShare() == FALSE)
		MessageBox(GetThreadErrorInfoString(),_T("��ʾ"),MB_OK);
}

void CNetShareMgrDlg::OnRClick(NMHDR* pNMHDR,LRESULT* pResult)
{
	NMLISTVIEW* pLVCD = (NMLISTVIEW*)pNMHDR;

	if (pLVCD->iItem <0)
		return;

	CMenu Menu;
	Menu.LoadMenu(IDR_LIST_MENU);
	CMenu* pMenu;
	pMenu = Menu.GetSubMenu(0);

	SetForegroundWindow();
	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}

void CNetShareMgrDlg::OnMenuRefresh()
{
	// TODO: �ڴ���������������
	OnBnClickedBtnRefresh();
}

void CNetShareMgrDlg::OnMenuDelete()
{
	// TODO: �ڴ���������������
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_NETSHARE);
	int nSelectItem = pListCtrl->GetSelectionMark();
	if (nSelectItem<0)
		return;

	//CStringW strNetShareName = CStringW(pListCtrl->GetItemText(nSelectItem,0));
	WCHAR wszNetShareName[MAX_PATH];
	wcscpy_s(wszNetShareName , _countof(wszNetShareName) , CStringW(pListCtrl->GetItemText(nSelectItem,0)));
	NET_API_STATUS dwRet = NetShareDel(NULL,wszNetShareName,0);
	if (dwRet != NERR_Success)
	{
		SetErrorInfo(SYSTEM_ERROR,dwRet,_T("ɾ�����繲���ļ���%sʧ��"),wszNetShareName);
		MessageBox(GetThreadErrorInfoString(),_T("��ʾ"),MB_OK);
	}
	else
		pListCtrl->DeleteItem(nSelectItem);
}
