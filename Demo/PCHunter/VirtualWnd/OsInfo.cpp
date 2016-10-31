#include "stdafx.h"
#include "../Utils/WMITools.h"
#include <shlobj.h>
#pragma comment(lib,"shell32.lib")


COsInfo::COsInfo()
{

}

COsInfo::~COsInfo()
{

}

void COsInfo::OnPaint()
{
	CTreeViewUI* pTreeView = (CTreeViewUI*)m_pPaintManager->FindControl(_T("OsInfo"));

	if (pTreeView->GetCount())
		pTreeView->RemoveAll();

	//获取系统信息
	if (GetSystemInfo(m_SystemInfoList) == FALSE)
		return;

	//设置标签名称
	POSITION pos = m_SystemInfoList.GetHeadPosition();
	CTreeNodeUI* pTreeNode = NULL;
	CTreeNodeUI* pSubNode = new CTreeNodeUI;
	int n=0;
	while(pos)
	{
		const SYSTEM_NODE_INFO& SystemNodeInfo = m_SystemInfoList.GetNext(pos);
		if (SystemNodeInfo.NodeType == NODE_TYPE_FIRST)
		{
			pTreeNode	= new CTreeNodeUI;
			if (pTreeNode == NULL)
				continue;
			pTreeView->Add(pTreeNode);
			pTreeNode->SetItemText(SystemNodeInfo.strTitle);
			pTreeNode->GetItemButton()->SetFont(0);
			pTreeNode->SetFixedHeight(33);
			pSubNode = NULL;
		}
		else if (SystemNodeInfo.NodeType == NODE_TYPE_SECOND && (pSubNode == NULL || n%3==0))
		{
			pSubNode = new CTreeNodeUI;
			if (pSubNode == NULL || pTreeNode == NULL)
				continue;

			pTreeNode->Add(pSubNode);
			pSubNode->SetItemText(SystemNodeInfo.strTitle);
			pSubNode->SetFixedHeight(25);
			pSubNode->SetItemFont(0);
			++n;
		}
		else if (SystemNodeInfo.NodeType == NODE_TYPE_SECOND && pSubNode != NULL)
		{
			CLabelUI* pLable = new CLabelUI;
			pLable->SetText(SystemNodeInfo.strTitle);
			pLable->SetFont(0);
			CHorizontalLayoutUI* pHor = pSubNode->GetTreeNodeHoriznotal();
			pHor->SetFixedWidth(100);
			pHor->Add(pLable);
			++n;
		}
	}
}

BOOL COsInfo::GetSystemInfo(CSystemInfoList& SystemInfoList)
{
	BOOL bSuccess = FALSE;
	do 
	{
		SystemInfoList.RemoveAll();
		//获取操作系统信息
		GetOSInfo(SystemInfoList);
		//获取CPU信息
		GetCpuInfo(SystemInfoList);
		//获取硬盘信息
		GetHardDiskInfo(SystemInfoList);
		//获取显卡信息
		//Get
		//获取主板信息
		//获取BIOS信息
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void COsInfo::GetOSInfo(CSystemInfoList& SystemInfoList)
{
	//添加操作系统结点信息
	SYSTEM_NODE_INFO SystemNodeInfo;
	SystemNodeInfo.NodeType = NODE_TYPE_FIRST;
	SystemNodeInfo.strTitle = _T("操作系统信息");
	SystemNodeInfo.lParam = NULL;

	SystemInfoList.AddTail(SystemNodeInfo);

	//以下全部是操作系统信息叶子结点，故直接修改NodeType = NODE_TYPE_SECOND
	SystemNodeInfo.NodeType = NODE_TYPE_SECOND;
	//获取操作系统名称
	CWMITools WmiTools;
	CDuiString strSystemCaption;
	if (WmiTools.GetWin32_OperatingSystem(_T("Caption"),strSystemCaption))
	{
		SystemNodeInfo.strTitle.Format(_T("操作系统名称 : %s"), strSystemCaption);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strSystemVersion;
	if (WmiTools.GetWin32_OperatingSystem(_T("Version"), strSystemVersion))
	{
		SystemNodeInfo.strTitle.Format(_T("系统版本 : %s"), strSystemVersion);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strSystemBuildType;
	if (WmiTools.GetWin32_OperatingSystem(_T("BuildType"), strSystemBuildType))
	{
		SystemNodeInfo.strTitle.Format(_T("BuildType : %s"), strSystemBuildType);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strSystemPatchVersion;
	if (WmiTools.GetWin32_OperatingSystem(_T("CSDVersion"), strSystemPatchVersion))
	{
		SystemNodeInfo.strTitle.Format(_T("补丁版本 : %s"), strSystemPatchVersion);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	TCHAR szWindowsPath[MAX_PATH];
	if (GetWindowsDirectory(szWindowsPath,_countof(szWindowsPath)))
	{
		SystemNodeInfo.strTitle.Format(_T("系统目录 : %s"), szWindowsPath);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	DWORD dwTickCount = GetTickCount();
	if (dwTickCount>=0)
	{
		TCHAR szRunTime[MAX_PATH];
		StrFromTimeInterval(szRunTime,_countof(szRunTime), dwTickCount, 4);
	
		SystemNodeInfo.strTitle.Format(_T("已运行时间 : %s"), szRunTime);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strSystemSerialNumber;
	if (WmiTools.GetWin32_OperatingSystem(_T("SerialNumber"), strSystemSerialNumber))
	{
		SystemNodeInfo.strTitle.Format(_T("序列号 : %s"), strSystemSerialNumber);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strCountryCode;
	if (WmiTools.GetWin32_OperatingSystem(_T("CountryCode"), strCountryCode))
	{
		SystemNodeInfo.strTitle.Format(_T("国家代码 : %s"), strCountryCode);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	SYSTEM_INFO SystemInfo;
	GetNativeSystemInfo(&SystemInfo);

	TCHAR szPageSize[MAX_PATH];
	LPTSTR lpszPageSize = StrFormatKBSize(SystemInfo.dwPageSize, szPageSize, _countof(szPageSize));
	SystemNodeInfo.strTitle.Format(_T("页大小 : %s"), lpszPageSize);
	SystemInfoList.AddTail(SystemNodeInfo);

	SystemNodeInfo.strTitle.Format(_T("最小应用程序地址 : 0x%08X"), SystemInfo.lpMinimumApplicationAddress);
	SystemInfoList.AddTail(SystemNodeInfo);

	SystemNodeInfo.strTitle.Format(_T("最大应用程序地址 : 0x%X"), SystemInfo.lpMaximumApplicationAddress);
	SystemInfoList.AddTail(SystemNodeInfo);

	//主机名称
	TCHAR szPCName[MAX_PATH];
	DWORD dwSize = _countof(szPCName);
	if (GetComputerName(szPCName, &dwSize))
	{
		SystemNodeInfo.strTitle.Format(_T("主机名称 : %s"), szPCName);
		SystemInfoList.AddTail(SystemNodeInfo);
	}
	TCHAR szUserName[MAX_PATH];
	dwSize = _countof(szUserName);
	if (GetUserName(szUserName, &dwSize))
	{
		SystemNodeInfo.strTitle.Format(_T("用户名 : %s"), szUserName);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	SystemNodeInfo.strTitle.Format(_T("是否有管理员权限 : %s"), IsUserAnAdmin() ? _T("是"): _T("否"));
	SystemInfoList.AddTail(SystemNodeInfo);
}

void COsInfo::GetCpuInfo(CSystemInfoList& SystemInfoList)
{
	//添加CPU结点信息
	SYSTEM_NODE_INFO SystemNodeInfo;
	SystemNodeInfo.NodeType = NODE_TYPE_FIRST;
	SystemNodeInfo.strTitle = _T("CPU信息");
	SystemNodeInfo.lParam = NULL;

	SystemInfoList.AddTail(SystemNodeInfo);
}

void COsInfo::GetHardDiskInfo(CSystemInfoList& SystemInfoList)
{
	//添加硬盘结点信息
	SYSTEM_NODE_INFO SystemNodeInfo;
	SystemNodeInfo.NodeType = NODE_TYPE_FIRST;
	SystemNodeInfo.strTitle = _T("硬盘信息");
	SystemNodeInfo.lParam = NULL;

	SystemInfoList.AddTail(SystemNodeInfo);
}