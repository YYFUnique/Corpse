// ModifyItem.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuGroup.h"
#include "ModifyItem.h"
#include "LsCommon/ErrorInfo.h"

typedef WINSHELLAPI BOOL (WINAPI *fnPickIconDlg)(HWND hWndParent, LPWSTR pszFilename, LPDWORD pdwBufferSize, LPDWORD pdwIndex);
// CModifyItem 对话框
#define			WM_LOAD_ITEM_INFO			WM_USER+0x1000
IMPLEMENT_DYNAMIC(CModifyItem, CDialog)

CModifyItem::CModifyItem(LPCTSTR lpszItemName,LPCTSTR lpszItemIcon /*=NULL*/,
									LPCTSTR lpszRunCmd /*=NULL*/,LPCTSTR lpszRegName /*=NULL*/,CWnd* pParent /*=NULL*/)
	: CDialog(CModifyItem::IDD, pParent)
	,m_strItemName(lpszItemName)
	,m_strIconPath(lpszItemIcon)
	,m_strRunCmd(lpszRunCmd)
	,m_strRegName(lpszRegName)
	,m_bOpen(FALSE)
{

}

CModifyItem::~CModifyItem()
{
}

void CModifyItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModifyItem, CDialog)
	ON_MESSAGE(WM_LOAD_ITEM_INFO,OnLoadItemInfo)
	ON_BN_CLICKED(IDC_BTN_LOOKUP, OnBnClickedBtnLookup)
	ON_BN_CLICKED(IDC_BTN_PREV_SET, OnBnClickedBtnPrevSet)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_COMPUTER,IDC_RADIO_OUT,OnRadioClick)
	//ON_RIDAO_RANGE()
END_MESSAGE_MAP()


// CModifyItem 消息处理程序
BOOL CModifyItem::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnBnClickedBtnPrevSet();
	GetDlgItem(IDC_EDIT_ITEM_NAME)->SetWindowText(m_strItemName);
	GetDlgItem(IDC_EDIT_ITEM_ICON)->SetWindowText(m_strIconPath);
	GetDlgItem(IDC_EDIT_RUN_CMD)->SetWindowText(m_strRunCmd);

	PostMessage(WM_LOAD_ITEM_INFO);
	return TRUE;
}

CString& CModifyItem::GetItemName()
{
	return m_strItemName;
}

BOOL CModifyItem::GetItemInfo(TREEITEMINFO& TreeItemInfo)
{
	TreeItemInfo.strIconPath = m_strIconPath;
	TreeItemInfo.strRunCmd = m_strRunCmd;
	TreeItemInfo.strName = m_strRegName;
	return TRUE;
}

void CModifyItem::OnOK()
{
	GetDlgItem(IDC_EDIT_ITEM_NAME)->GetWindowText(m_strItemName);
	GetDlgItem(IDC_EDIT_ITEM_ICON)->GetWindowText(m_strIconPath);
	GetDlgItem(IDC_EDIT_RUN_CMD)->GetWindowText(m_strRunCmd);

	BOOL bSuccess = FALSE;
	do 
	{
		if (m_strItemName.IsEmpty() == TRUE)
		{
			MessageBox(_T("请输入项名称"),_T("提示"),MB_OK);
			GetDlgItem(IDC_EDIT_ITEM_NAME)->SetFocus();
			break;
		}
		if (m_strIconPath.IsEmpty() == TRUE && m_strRegName.CompareNoCase(_T("|")) )
		{
			MessageBox(_T("请选择项图标"),_T("提示"),MB_OK);
			GetDlgItem(IDC_EDIT_ITEM_ICON)->SetFocus();
			break;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (bSuccess == FALSE)
		return;
	
	CDialog::OnOK();
}
void CModifyItem::OnBnClickedBtnLookup()
{
	// TODO: 在此添加控件通知处理程序代码
	HMODULE hShell32 = ::LoadLibrary(_T("shell32.dll"));
	if (hShell32)
	{
		fnPickIconDlg PickIconDlg = (fnPickIconDlg)::GetProcAddress(hShell32, (LPCSTR)62);
		if (PickIconDlg)
		{
			DWORD dwSize = 1024;
			DWORD dwIcon = 0;
			WCHAR szFilePath[1024];
			szFilePath[0] = L'\0';
			m_strIconPath.Replace(_T("\""),_T(""));
			swscanf_s(m_strIconPath,L"%[^,],%d",szFilePath,dwSize,&dwIcon);
			if (PathFileExists(szFilePath) == FALSE)
				szFilePath[0] = L'\0';
			if (PickIconDlg(m_hWnd, szFilePath, &dwSize, &dwIcon) == TRUE)
			{
				m_strIconPath.Format(_T("\"%s\",%d"),szFilePath,dwIcon);
				GetDlgItem(IDC_EDIT_ITEM_ICON)->SetWindowText(m_strIconPath);
			}
		}
		FreeLibrary(hShell32);
	}
}

void CModifyItem::OnBnClickedBtnPrevSet()
{
	CRect Rect;
	GetWindowRect(&Rect);
	UINT nyOffset = Rect.Height();
	if (m_bOpen)
	{
		nyOffset += 178;
		m_bOpen = FALSE;
	}
	else
	{
		nyOffset -= 178;
		m_bOpen = TRUE;
	}
	SetWindowPos(NULL,0,0,Rect.Width(),nyOffset,SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
}

void CModifyItem::OnRadioClick(UINT nID)
{
	CString strItemName;
	GetDlgItem(nID)->GetWindowText(strItemName);
	GetDlgItem(IDC_EDIT_ITEM_NAME)->SetWindowText(strItemName);

	TREEITEMINFO TreeItemInfo;
	if (m_TreeItemInfo.Lookup(nID,TreeItemInfo) == TRUE)
	{
		GetDlgItem(IDC_EDIT_ITEM_ICON)->SetWindowText(TreeItemInfo.strIconPath);
		GetDlgItem(IDC_EDIT_RUN_CMD)->SetWindowText(TreeItemInfo.strRunCmd);
		m_strRegName = TreeItemInfo.strName;
	}
}

LRESULT CModifyItem::OnLoadItemInfo(WPARAM wParam,LPARAM lParam)
{
	static LPCTSTR lpszTreeItemInfo[][3] = 
	{
		{_T("explorer.exe /e,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"),_T("imageres.dll,-109"),_T("My Computer")},	//计算机
		{_T("rundll32.exe shell32.dll,Control_RunDLL"),_T("shell32.dll,21"),_T("ControlPan")},		//控制面板
		{_T("cmd.exe"),_T("cmd.exe"),_T("CMD")},		//控制面板
		{_T("mspaint.exe"),_T("mspaint.exe"),_T("Paint")},		//控制面板
		{_T("calc.exe"),_T("calc.exe"),_T("Calc")},		//控制面板
		{_T("Regedit.exe"),_T("Regedit.exe"),_T("Register")},		//控制面板
		{_T("Shutdown -r -f -t 0"),_T("shell32.dll,112"),_T("Reboot")},		//控制面板
		{_T("Shutdown -s -f -t 0"),_T("shell32.dll,215"),_T("Shutdown")},		//控制面板
		{_T(""),_T(""),_T("")},
		{_T("无"),_T(""),_T("|")},
		{_T("Magnify.exe"),_T("SHELL32.dll,15"),_T("Magnify")},
		{_T("compmgmt.msc /s"),_T("mmc.exe"),_T("Compmgmt")},
		{_T("services.msc"),_T("mmc.exe"),_T("Services")},
		{_T("taskmgr.exe"),_T("taskmgr.exe"),_T("taskmgr")},
		{_T("devmgmt.msc"),_T("mmc.exe"),_T("Devmgmt")}
	};
	static UINT uID[] = {IDC_RADIO_COMPUTER,IDC_RADIO_CONTROL,IDC_RADIO_CMD,
									IDC_RADIO_PAINT,IDC_RADIO_CACL,IDC_RADIO_REG,IDC_RADIO_REBOOT,
									IDC_RADIO_SHUTDOWN,IDC_RADIO_NET,IDC_RADIO_OUT,IDC_RADIO_BIG,
									IDC_RADIO_MANAGE,IDC_RADIO_SERVICE,IDC_RADIO_TASK,IDC_RADIO_DEV};
	for (UINT i=0;i<_countof(lpszTreeItemInfo);++i)
	{
		TREEITEMINFO TreeItemInfo;
		TreeItemInfo.strRunCmd = lpszTreeItemInfo[i][0];
		TreeItemInfo.strIconPath = lpszTreeItemInfo[i][1];
		TreeItemInfo.strName = lpszTreeItemInfo[i][2];
		m_TreeItemInfo.SetAt(uID[i],TreeItemInfo);
	}

	return TRUE;
}