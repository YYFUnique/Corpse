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

	//��ȡϵͳ��Ϣ
	if (GetSystemInfo(m_SystemInfoList) == FALSE)
		return;

	//���ñ�ǩ����
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
		//��ȡ����ϵͳ��Ϣ
		GetOSInfo(SystemInfoList);
		//��ȡCPU��Ϣ
		GetCpuInfo(SystemInfoList);
		//��ȡӲ����Ϣ
		GetHardDiskInfo(SystemInfoList);
		//��ȡ�Կ���Ϣ
		//Get
		//��ȡ������Ϣ
		//��ȡBIOS��Ϣ
		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

void COsInfo::GetOSInfo(CSystemInfoList& SystemInfoList)
{
	//��Ӳ���ϵͳ�����Ϣ
	SYSTEM_NODE_INFO SystemNodeInfo;
	SystemNodeInfo.NodeType = NODE_TYPE_FIRST;
	SystemNodeInfo.strTitle = _T("����ϵͳ��Ϣ");
	SystemNodeInfo.lParam = NULL;

	SystemInfoList.AddTail(SystemNodeInfo);

	//����ȫ���ǲ���ϵͳ��ϢҶ�ӽ�㣬��ֱ���޸�NodeType = NODE_TYPE_SECOND
	SystemNodeInfo.NodeType = NODE_TYPE_SECOND;
	//��ȡ����ϵͳ����
	CWMITools WmiTools;
	CDuiString strSystemCaption;
	if (WmiTools.GetWin32_OperatingSystem(_T("Caption"),strSystemCaption))
	{
		SystemNodeInfo.strTitle.Format(_T("����ϵͳ���� : %s"), strSystemCaption);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strSystemVersion;
	if (WmiTools.GetWin32_OperatingSystem(_T("Version"), strSystemVersion))
	{
		SystemNodeInfo.strTitle.Format(_T("ϵͳ�汾 : %s"), strSystemVersion);
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
		SystemNodeInfo.strTitle.Format(_T("�����汾 : %s"), strSystemPatchVersion);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	TCHAR szWindowsPath[MAX_PATH];
	if (GetWindowsDirectory(szWindowsPath,_countof(szWindowsPath)))
	{
		SystemNodeInfo.strTitle.Format(_T("ϵͳĿ¼ : %s"), szWindowsPath);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	DWORD dwTickCount = GetTickCount();
	if (dwTickCount>=0)
	{
		TCHAR szRunTime[MAX_PATH];
		StrFromTimeInterval(szRunTime,_countof(szRunTime), dwTickCount, 4);
	
		SystemNodeInfo.strTitle.Format(_T("������ʱ�� : %s"), szRunTime);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strSystemSerialNumber;
	if (WmiTools.GetWin32_OperatingSystem(_T("SerialNumber"), strSystemSerialNumber))
	{
		SystemNodeInfo.strTitle.Format(_T("���к� : %s"), strSystemSerialNumber);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	CDuiString strCountryCode;
	if (WmiTools.GetWin32_OperatingSystem(_T("CountryCode"), strCountryCode))
	{
		SystemNodeInfo.strTitle.Format(_T("���Ҵ��� : %s"), strCountryCode);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	SYSTEM_INFO SystemInfo;
	GetNativeSystemInfo(&SystemInfo);

	TCHAR szPageSize[MAX_PATH];
	LPTSTR lpszPageSize = StrFormatKBSize(SystemInfo.dwPageSize, szPageSize, _countof(szPageSize));
	SystemNodeInfo.strTitle.Format(_T("ҳ��С : %s"), lpszPageSize);
	SystemInfoList.AddTail(SystemNodeInfo);

	SystemNodeInfo.strTitle.Format(_T("��СӦ�ó����ַ : 0x%08X"), SystemInfo.lpMinimumApplicationAddress);
	SystemInfoList.AddTail(SystemNodeInfo);

	SystemNodeInfo.strTitle.Format(_T("���Ӧ�ó����ַ : 0x%X"), SystemInfo.lpMaximumApplicationAddress);
	SystemInfoList.AddTail(SystemNodeInfo);

	//��������
	TCHAR szPCName[MAX_PATH];
	DWORD dwSize = _countof(szPCName);
	if (GetComputerName(szPCName, &dwSize))
	{
		SystemNodeInfo.strTitle.Format(_T("�������� : %s"), szPCName);
		SystemInfoList.AddTail(SystemNodeInfo);
	}
	TCHAR szUserName[MAX_PATH];
	dwSize = _countof(szUserName);
	if (GetUserName(szUserName, &dwSize))
	{
		SystemNodeInfo.strTitle.Format(_T("�û��� : %s"), szUserName);
		SystemInfoList.AddTail(SystemNodeInfo);
	}

	SystemNodeInfo.strTitle.Format(_T("�Ƿ��й���ԱȨ�� : %s"), IsUserAnAdmin() ? _T("��"): _T("��"));
	SystemInfoList.AddTail(SystemNodeInfo);
}

void COsInfo::GetCpuInfo(CSystemInfoList& SystemInfoList)
{
	//���CPU�����Ϣ
	SYSTEM_NODE_INFO SystemNodeInfo;
	SystemNodeInfo.NodeType = NODE_TYPE_FIRST;
	SystemNodeInfo.strTitle = _T("CPU��Ϣ");
	SystemNodeInfo.lParam = NULL;

	SystemInfoList.AddTail(SystemNodeInfo);
}

void COsInfo::GetHardDiskInfo(CSystemInfoList& SystemInfoList)
{
	//���Ӳ�̽����Ϣ
	SYSTEM_NODE_INFO SystemNodeInfo;
	SystemNodeInfo.NodeType = NODE_TYPE_FIRST;
	SystemNodeInfo.strTitle = _T("Ӳ����Ϣ");
	SystemNodeInfo.lParam = NULL;

	SystemInfoList.AddTail(SystemNodeInfo);
}