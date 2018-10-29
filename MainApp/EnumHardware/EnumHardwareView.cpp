
// EnumHardwareView.cpp : CEnumHardwareView 类的实现
//

#include "stdafx.h"
#include "EnumHardware.h"
#include <InitGuid.h>
#include "EnumHardwareDoc.h"
#include "EnumHardwareView.h"
#include "LsCommon/OsInfo.h"
#include "LsCommon/ErrorInfo.h"
#include <SetupAPI.h>
#include "MainFrm.h"
#include <devguid.h>
#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")
#pragma comment(lib,"setupapi.lib")
#include "LsCommon/File/LogicalDiskMgr.h"
#include "LsCommon/File/DiskVolumeInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEnumHardwareView
#define	WM_SHOW_HARDWARE		(WM_USER + 0x100)
#define INTERFACE_DETAIL_SIZE	8096

IMPLEMENT_DYNCREATE(CEnumHardwareView, CTreeView)
	
BEGIN_MESSAGE_MAP(CEnumHardwareView, CTreeView)
	ON_MESSAGE(WM_SHOW_HARDWARE,&CEnumHardwareView::OnShowHardware)
	ON_NOTIFY_REFLECT(NM_RCLICK,&CEnumHardwareView::OnRClick)
	ON_COMMAND(ID_SHOWALL, &CEnumHardwareView::OnShowall)
	ON_COMMAND(ID_DONOT_SHOW_HIDE, &CEnumHardwareView::OnDonotShowHide)
	ON_COMMAND(ID_HARDWARE_SCANF, &CEnumHardwareView::OnHardwareScanf)
	ON_COMMAND(ID_HARDWARE_PROPERTY, &CEnumHardwareView::OnHardwareProperty)
	ON_COMMAND(ID_HARDWARE_DISABLE, &CEnumHardwareView::OnHardwareDisable)
	ON_COMMAND(ID_HARDWARE_ENABLE, &CEnumHardwareView::OnHardwareEnable)
END_MESSAGE_MAP()

// CEnumHardwareView 构造/析构

CEnumHardwareView::CEnumHardwareView()
{
	// TODO: 在此处添加构造代码
	m_bShowAll = TRUE;
	m_ImgList.DeleteImageList();
	m_ImgList.Create(16,16,ILC_COLOR32|ILC_MASK,10,1);
}

CEnumHardwareView::~CEnumHardwareView()
{
}

BOOL CEnumHardwareView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);
}

void CEnumHardwareView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
	SetWindowTheme(m_hWnd,L"Explorer",NULL);

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	TreeCtrl.SetImageList(&m_ImgList,TVSIL_NORMAL);
	//m_DevInfoMap._GetBaseClass//

	UINT nIDIcon[] = {IDI_ICON_COMPUTER,IDI_ICON_DVD,IDI_ICON_IDE,IDI_ICON_CPU,IDI_ICON_DISK,IDI_ICON_DISK,IDI_ICON_DISK,
		IDI_ICON_PORT,IDI_ICON_HIDE,IDI_ICON_SYSTEM,IDI_ICON_MONITOR,IDI_ICON_KEYBORD,IDI_ICON_BODY,
		IDI_ICON_SOUND,IDI_ICON_MOUSE,IDI_ICON_USB,IDI_ICON_NETWORK,IDI_ICON_SYSTEM,
		IDI_ICON_DISPLAY,IDI_ICON_SAFEREADER,IDI_ICON_SAFEREADER,IDI_ICON_HIDE};

	LPCTSTR lpszDevName[] = {_T("DVD/CD-ROM驱动器"),_T("IDE ATA/ATAPI 控制器"),_T("处理器"),_T("便携设备"),_T("磁盘驱动器"),
								_T("磁盘卷"),_T("端口(COM和LPT)"),_T("非即插即用驱动程序"),_T("计算机"),_T("监视器"),_T("键盘"),_T("人体学输入设备"),_T("声音、视屏和游戏控制器"),
								_T("鼠标和其他指针设备"),_T("通用串行总线控制器"),_T("网络适配器"),_T("系统设备"),_T("显示设备器"),
								_T("智能卡读卡器"),_T("智能卡筛选器"),_T("未知设备")};

	GUID DevGuid[] = {GUID_DEVCLASS_CDROM,GUID_DEVCLASS_HDC,GUID_DEVCLASS_PROCESSOR,GUID_DEVCLASS_WPD,GUID_DEVCLASS_DISKDRIVE,
		GUID_DEVCLASS_VOLUME,GUID_DEVCLASS_PORTS,GUID_DEVCLASS_LEGACYDRIVER,GUID_DEVCLASS_COMPUTER,GUID_DEVCLASS_MONITOR,
		GUID_DEVCLASS_KEYBOARD,GUID_DEVCLASS_HIDCLASS,GUID_DEVCLASS_MEDIA,GUID_DEVCLASS_MOUSE,GUID_DEVCLASS_USB,GUID_DEVCLASS_NET,
		GUID_DEVCLASS_SYSTEM,GUID_DEVCLASS_DISPLAY,GUID_DEVCLASS_SMARTCARDREADER,GUID_DEVCLASS_SMARTCARDFILTER,GUID_DEVCLASS_NODRIVER};


	//for (int i=0;i<_countof(nIDIcon);++i)
	m_ImgList.Add(LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIDIcon[0])));

	m_DevNameList.RemoveAll();
	DEVNAME DevName;
	for (int i=0;i<_countof(lpszDevName);++i)
	{
		DevName.strDevName = lpszDevName[i];
		DevName.DevGuid = DevGuid[i];
		DevName.DevIcon = m_ImgList.Add(LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(nIDIcon[i+1])));
		m_DevNameList.AddTail(DevName);
	}

	PostMessage(WM_SHOW_HARDWARE,0,0);
}


// CEnumHardwareView 诊断

#ifdef _DEBUG
void CEnumHardwareView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CEnumHardwareView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CEnumHardwareDoc* CEnumHardwareView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEnumHardwareDoc)));
	return (CEnumHardwareDoc*)m_pDocument;
}
#endif //_DEBUG


// CEnumHardwareView 消息处理程序
LRESULT CEnumHardwareView::OnShowHardware(WPARAM wParam,LPARAM lParam)
{
	m_DevInfoList.RemoveAll();
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	TreeCtrl.DeleteAllItems();

	HTREEITEM hRoot = TreeCtrl.InsertItem(GetComputerName(),0);
	
	POSITION pos = m_DevNameList.GetHeadPosition();
	
	CDevInfoList DevInfoList;
	while(pos)
	{
		DevInfoList.RemoveAll();
		DEVNAME& DevName = m_DevNameList.GetNext(pos);
		if (GetDevInfo(DevName.DevGuid,DevInfoList,m_bShowAll) != FALSE)
		{
			if (DevInfoList.IsEmpty() != TRUE)
			{
				HTREEITEM hItem = TreeCtrl.InsertItem(TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM,
							DevName.strDevName,DevName.DevIcon,DevName.DevIcon,0,0,-1,hRoot,TVI_LAST);

				POSITION pos = DevInfoList.GetHeadPosition();
				while(pos)
				{
					DEV_INFO& DevInfo = DevInfoList.GetNext(pos);
					POSITION CurrentPos = m_DevInfoList.AddTail(DevInfo);

					CString strDevName;
					strDevName.Format(_T("%s[%d]"), DevInfo.strDevName, DevInfo.DevInst);
					if (strDevName.IsEmpty())
						strDevName = DevInfo.strDevDesc;

					TreeCtrl.InsertItem(TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM,strDevName,
													DevName.DevIcon,DevName.DevIcon,0,0,(LPARAM)CurrentPos,hItem,TVI_SORT);
				}
			}
		}
	}

	TreeCtrl.SelectItem(hRoot);
	return TreeCtrl.Expand(hRoot,TVE_EXPAND);
}

void CEnumHardwareView::OnRClick(NMHDR* pNMHdr,LPARAM* lResult)
{
	NMTREEVIEW* pTreeView = (NMTREEVIEW*)pNMHdr;
	
	CMenu Menu,*pRClickMenu;
	Menu.LoadMenu(IDR_MENU_TREEVIEW);
	pRClickMenu = Menu.GetSubMenu(0);

	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	CPoint pt;
	GetCursorPos(&pt);
	TreeCtrl.ScreenToClient(&pt);
	UINT uFlag;
	HTREEITEM ht = TreeCtrl.HitTest(pt,&uFlag);

	if (ht == NULL)
		return;

	if (TreeCtrl.ItemHasChildren(ht))
	{
		pRClickMenu->DeleteMenu(ID_HARDWARE_UPDATE_DRIVERS,MF_BYCOMMAND);

		pRClickMenu->DeleteMenu(ID_HARDWARE_DISABLE,MF_BYCOMMAND);

		pRClickMenu->DeleteMenu(ID_HARDWARE_ENABLE,MF_BYCOMMAND);

		pRClickMenu->DeleteMenu(ID_HARDWARE_UNISTALL,MF_BYCOMMAND);
		pRClickMenu->DeleteMenu(ID_HARDWARE_PROPERTY,MF_BYCOMMAND);
		pRClickMenu->DeleteMenu(0,MF_BYPOSITION);
	}
	else
	{
		POSITION pos = (POSITION)TreeCtrl.GetItemData(ht);
		DEV_INFO& DevInfo = m_DevInfoList.GetAt(pos);

		if (DevInfo.dwStatus == DEV_DISABLE)
			pRClickMenu->DeleteMenu(ID_HARDWARE_DISABLE,MF_BYCOMMAND);
		else
			pRClickMenu->DeleteMenu(ID_HARDWARE_ENABLE,MF_BYCOMMAND);
	}
	SetForegroundWindow();

	GetCursorPos(&pt);
	pRClickMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}

void CEnumHardwareView::OnShowall()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowAll = TRUE;
	PostMessage(WM_SHOW_HARDWARE,0,0);
}

void CEnumHardwareView::OnDonotShowHide()
{
	// TODO: 在此添加命令处理程序代码
	m_bShowAll = FALSE;
	PostMessage(WM_SHOW_HARDWARE,0,0);
}

void CEnumHardwareView::OnHardwareScanf()
{
	// TODO: 在此添加命令处理程序代码
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	HTREEITEM ht = TreeCtrl.GetSelectedItem();
	if (ht == NULL)
		return;

	ScanfHardwareInfo();
}

void CEnumHardwareView::OnHardwareProperty()
{
	// TODO: 在此添加命令处理程序代码
	//ShellExecute(m_hWnd,)
}

void CEnumHardwareView::OnHardwareEnable()
{
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	HTREEITEM ht = TreeCtrl.GetSelectedItem();
	if (ht == NULL)
		return;

	POSITION pos = (POSITION)TreeCtrl.GetItemData(ht);
	DEV_INFO& DevInfo = m_DevInfoList.GetAt(pos);

	OUTER_DEV_MANAGER DeviceRuleManage;
	DeviceRuleManage.DevChangeGuid = GUID_DEVINTERFACE_DISK;
	DeviceRuleManage.DevClassGuid = DevInfo.ClassGuid;
	DeviceRuleManage.dwOperator = DEV_OP_ENABLE;
	DeviceRuleManage.dwSubClass = 0;
	DeviceRuleManage.strDevClassDisplayName = DevInfo.strDisplayName;
	DeviceRuleManage.strDevClassName = DevInfo.strClassName;

	SetDeviceStatus(DeviceRuleManage);
}

void CEnumHardwareView::OnHardwareDisable()
{
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	HTREEITEM ht = TreeCtrl.GetSelectedItem();
	if (ht == NULL)
		return;

	POSITION pos = (POSITION)TreeCtrl.GetItemData(ht);
	DEV_INFO& DevInfo = m_DevInfoList.GetAt(pos);

	OUTER_DEV_MANAGER DeviceRuleManage;
	DeviceRuleManage.DevChangeGuid = GUID_DEVINTERFACE_DISK;
	DeviceRuleManage.DevClassGuid = DevInfo.ClassGuid;
	DeviceRuleManage.dwOperator = DEV_OP_DISABLE;
	DeviceRuleManage.dwSubClass = 0;
	DeviceRuleManage.strDevClassDisplayName = DevInfo.strDisplayName;
	DeviceRuleManage.strDevClassName = DevInfo.strClassName;

	SetDeviceStatus(DeviceRuleManage);
}

BOOL CEnumHardwareView::SetDeviceStatus(OUTER_DEV_MANAGER& DeviceRuleManage)
{
	BOOL bSuccess = FALSE;
	do 
	{
		CDevInfoList DevInfoList; //设备状态链表

		//如果是需要启用该类下的设备，需要枚举所有设备
		BOOL bAllDevice = DeviceRuleManage.dwOperator == DEV_OP_ENABLE;

		//获取指定类型下的的设备
		if (GetDevInfoFromClassGuid(DeviceRuleManage.DevClassGuid, DevInfoList, bAllDevice) == FALSE)
			break;

		//记录设备状态
		//WriteOutDevInfo(DeviceRuleManage);

		POSITION pos = DevInfoList.GetHeadPosition();
		while(pos)
		{
			const DEV_INFO& DeviceInfo = DevInfoList.GetNext(pos);

			BOOL bChangeRet = FALSE;

			//如果是硬盘设备
			if (DeviceInfo.ClassGuid == GUID_DEVCLASS_DISKDRIVE)
			{
				//由于设备状态类型值与规则动作值不对应，需要进行相应的处理，详见值的定义
				if (DeviceInfo.dwStatus == DeviceRuleManage.dwOperator - 1 ||
					DeviceInfo.dwStatus == DEV_NOT_EXIST)
					continue;

				//如果是设备需要禁用，则判断该设备是否是需要禁用的设备
				if (DeviceRuleManage.dwOperator != DEV_OP_ENABLE)
				{
					//获取当前系统所在硬盘序号
					CLogicalDiskMgr LogicalDisk;
					DWORD dwIndex = LogicalDisk.GetSystemDiskIndex();
					DWORD dwPhysicalItem = GetPhysicalDiskIndex(DeviceInfo);
					if (dwIndex == dwPhysicalItem || dwPhysicalItem == -1)
						continue;

					//过滤IDE总线设备和STAT总线设备
					STORAGE_BUS_TYPE StorageBusType;
					BOOL bRet = CLogicalDiskMgr::GetStorageDeviceBusType(dwPhysicalItem, StorageBusType);
					if (bRet != FALSE && StorageBusType != BusTypeUnknown)
					{
						if (StorageBusType == BusTypeSata || StorageBusType == BusTypeAta)	//IDE (ATA) Bus
							continue;
					}

					//枚举当前移动存储设备上所有卷设备，并且锁定卷，然后卸载设备
					CDiskVolumeInfo DiskVolume;
					CStringArray strVolumeArray;

					LS_PHYSICAL_DISK_SIZE LsPhysicalDisk;
					if (CLogicalDiskMgr::GetPhysicalDiskSize(dwPhysicalItem, LsPhysicalDisk) == FALSE)
						continue;

					LARGE_INTEGER DiskSize;
					DiskSize.QuadPart = LsPhysicalDisk.DiskSectors.QuadPart*LsPhysicalDisk.dwBytesPerSector;

					if (DiskVolume.EnumVolumeForDisk(dwPhysicalItem, 0, DiskSize.QuadPart, strVolumeArray) != FALSE)
					{
						int nIndex = 0;
						DWORD dwVolumeCount = (DWORD)strVolumeArray.GetCount();
						CString strVolumeName;
						for(DWORD i=0;i<dwVolumeCount;i++)
						{
							CLogicalDiskMgr LogicalDisk;
							strVolumeName.Format(_T("%s"), strVolumeArray.GetAt(i));
							DefineDosDevice(DDD_REMOVE_DEFINITION, strVolumeName.TrimRight(_T("\\")),NULL);
						}
					}
				}				

				//DevStatusChange(DeviceInfo.ClassGuid, DeviceInfo.strDevInstId, DeviceRuleManage.dwOperator);
				//上面删除了设备的符号链接，虽然函数会返回失败，但是功能已经生效
				//SaveDeviceResult(DeviceRuleManage,DeviceInfo, TRUE);
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

DWORD CEnumHardwareView::GetPhysicalDiskIndex(const DEV_INFO& DeviceInfo)
{
	DWORD dwDiskIndex = -1;
	HDEVINFO hDevInfoSet = INVALID_HANDLE_VALUE;
	HANDLE hPhysicalDisk = INVALID_HANDLE_VALUE;

	do 
	{
		hDevInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK,	// class GUID 
			NULL,										// 无关键字
			NULL,										// 不指定父窗口句柄
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);	// 目前存在的设备

		if (hDevInfoSet == INVALID_HANDLE_VALUE)
		{
			SetErrorInfo(SYSTEM_ERROR, 0, _T("获取系统中所有磁盘失败"));
			break;
		}

		SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
		ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
		DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);		

		for(int n=0; n<0xff ;++n)
		{
			// 枚举符合该GUID的设备接口
			BOOL bRet = SetupDiEnumDeviceInterfaces(hDevInfoSet,NULL,&GUID_DEVINTERFACE_DISK,n,&DeviceInterfaceData);
			if (bRet == FALSE)
				break;

			SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)}; //设备信息结构体

			bRet = SetupDiEnumDeviceInfo(hDevInfoSet, n ,&DeviceInfoData);
			if (bRet == FALSE)
				break;

			//判断是否为当前设备
			if (DeviceInfoData.DevInst != DeviceInfo.DevInst)
				continue;

			BYTE byDetailBuffer[INTERFACE_DETAIL_SIZE];
			ZeroMemory(byDetailBuffer, INTERFACE_DETAIL_SIZE);
			PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)byDetailBuffer;
			pDeviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			// 取得该设备接口的细节(设备路径)
			if (SetupDiGetInterfaceDeviceDetail(hDevInfoSet,									// 设备信息集句柄
				&DeviceInterfaceData,					// 设备接口信息
				pDeviceInterfaceDetailData,			// 设备接口细节(设备路径)
				INTERFACE_DETAIL_SIZE,				// 输出缓冲区大小
				NULL,												// 不需计算输出缓冲区大小(直接用设定值)
				&DeviceInfoData) == FALSE)				// 不需额外的设备描述								
				break;

			hPhysicalDisk = CreateFile(pDeviceInterfaceDetailData->DevicePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0 ,NULL);
			if (hPhysicalDisk == INVALID_HANDLE_VALUE)
				break;

			//获取当前磁盘设备硬盘序号
			STORAGE_DEVICE_NUMBER StorageDeviceNumber;
			DWORD dwSize = sizeof(STORAGE_DEVICE_NUMBER);
			bRet = DeviceIoControl(hPhysicalDisk, IOCTL_STORAGE_GET_DEVICE_NUMBER, 
				NULL, NULL, &StorageDeviceNumber, dwSize, &dwSize ,NULL);

			if (bRet == FALSE)
				break;

			if (StorageDeviceNumber.DeviceNumber != -1)
			{
				dwDiskIndex = StorageDeviceNumber.DeviceNumber;
				break;
			}
		}

	} while (FALSE);

	if (hPhysicalDisk != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hPhysicalDisk);
		hPhysicalDisk = INVALID_HANDLE_VALUE;
	}

	if (hDevInfoSet != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(hDevInfoSet);
		hDevInfoSet = INVALID_HANDLE_VALUE;
	}

	return dwDiskIndex;
}

BOOL CEnumHardwareView::GetDevInfoFromClassGuid(IN GUID DevClassGuid, OUT CDevInfoList& DevInfoList,BOOL bAllDevice)
{
	BOOL bRet = TRUE;

	DWORD dwFlags = 0;
	if(bAllDevice == FALSE)
		dwFlags = DIGCF_PRESENT | DIGCF_PROFILE; //默认标志量

	BOOL bHiddenClass = IsClassHidden(DevClassGuid); //判断是否隐藏的设备类

	HDEVINFO hDev = SetupDiGetClassDevs(&DevClassGuid,NULL,NULL,dwFlags);
	if(INVALID_HANDLE_VALUE == hDev)
	{
		CString strTipInfo;
		strTipInfo.Format(_T("获取设备信息时获取硬件列表信息失败,错误码:%d."),GetLastError());
		OutputDebugString(strTipInfo);
		return FALSE;
	}

	DWORD dwError = 0;
	DWORD dwRequiredSize = 0;

	TCHAR pDataBuffer[10*4*MAX_PATH];
	DWORD dwSize = _countof(pDataBuffer);
	SP_DEVINFO_DATA DeviceInfoData = {sizeof(SP_DEVINFO_DATA)}; //设备信息结构体

	for(int i=0; SetupDiEnumDeviceInfo(hDev,i,&DeviceInfoData); ++i)
	{
		DEV_INFO DevInfo;

		LPBYTE pDevClassGuid = NULL;
		DevInfo.DevInst = DeviceInfoData.DevInst;
		DevInfo.ClassGuid = DeviceInfoData.ClassGuid;
		DevInfo.dwBluetoothFlag = 0;
		if(DevInfo.ClassGuid==GUID_DEVCLASS_BLUETOOTH)
			DevInfo.dwBluetoothFlag = MICROSOFT_BLUETOOTH;

		DEVINST ParentInst;
		TCHAR szParentInstId[MAX_DEVICE_ID_LEN] = {0};
		if (CR_SUCCESS != (dwError = CM_Get_Parent(&ParentInst,DevInfo.DevInst,0)))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("获取设备信息时获取设备父节点出错,错误号%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		if(CR_SUCCESS != (dwError = CM_Get_Device_ID(ParentInst,szParentInstId,MAX_DEVICE_ID_LEN,0)))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("获取设备信息时获取设备父节点实例ID出错,错误号%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		DevInfo.strParentInstId = szParentInstId;

		if (FALSE == SetupDiClassNameFromGuid(&DevInfo.ClassGuid,pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("获取设备信息时获取设备父节点出错,错误号%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		DevInfo.strClassName = pDataBuffer;

		if (FALSE == SetupDiGetDeviceInstanceId(hDev,&DeviceInfoData,pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("获取设备信息时获取设备实例ID出错,错误号%d"),dwError);
			OutputDebugString(strTipInfo);
			continue;
		}

		DevInfo.strDevInstId = pDataBuffer;

		if (FALSE == SetupDiGetDeviceRegistryProperty(hDev,&DeviceInfoData,SPDRP_ENUMERATOR_NAME,NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("获取设备信息时获取设备总线类型出错,错误号%d"),dwError);
			OutputDebugString(strTipInfo);
		}
		DevInfo.strDevEnumerator = pDataBuffer;

		if (FALSE == SetupDiGetDeviceRegistryProperty(hDev,&DeviceInfoData,SPDRP_DEVICEDESC,NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize)) //设备描述
		{
			dwError = GetLastError();
			CString strTipInfo;
			strTipInfo.Format(_T("获取设备信息时获取设备描述出错,错误号%d"),dwError);
			OutputDebugString(strTipInfo);
		}

		DWORD dwDescLen = (DWORD)_tcslen(pDataBuffer);
		if (dwDescLen)
			DevInfo.strDevDesc = pDataBuffer;

		if (FALSE == SetupDiGetDeviceRegistryProperty(hDev,&DeviceInfoData,SPDRP_FRIENDLYNAME,NULL,(LPBYTE)pDataBuffer,dwSize,&dwRequiredSize))
		{
			dwError = GetLastError();
			//OutputDebugStringWhenDebugMode("获取设备信息时获取友好名称出错,错误号%d",dwError);
		}

		DevInfo.strDevName = pDataBuffer;

		if (dwDescLen == 0)
			DevInfo.strDevDesc = DevInfo.strDevName;

		DevInfo.strServiceName = _T("");
		DevInfo.strDisplayName = _T("");
		DevInfo.dwFirstStartType = 0;
		DevInfo.dwBluetoothFlag = 0;
		if(DevClassGuid==GUID_DEVCLASS_BLUETOOTH)
			DevInfo.dwBluetoothFlag = MICROSOFT_BLUETOOTH;
		//获取硬件状态
		DWORD dwStatus; //设备状态
		DWORD dwProblem; //问题编号
		dwError = CM_Get_DevNode_Status(&dwStatus,&dwProblem,DeviceInfoData.DevInst,0);
		if(CR_SUCCESS != dwError)
		{
			if(DevInfo.ClassGuid==GUID_DEVCLASS_DISKDRIVE)
			{	
				DevInfo.dwStatus = DEV_NOT_EXIST;
				DevInfo.bHidden = FALSE;
				DevInfoList.AddTail(DevInfo);
			}
			else
			{
				CString strTipInfo;
				strTipInfo.Format(_T("获取设备信息时获取设备状态出错,错误号%d"),dwError);
				OutputDebugString(strTipInfo);
			}
			continue;
		}

		if ((dwStatus&DN_NO_SHOW_IN_DM) || bHiddenClass) //是否是隐藏的设备或者设备类
			DevInfo.bHidden = TRUE;
		else
			DevInfo.bHidden = FALSE;

		if ((dwStatus&DN_STARTED) && (dwProblem==0))
		{
			DevInfo.dwStatus = DEV_ENABLE;
		}
		else if ((dwStatus&DN_HAS_PROBLEM) && (dwProblem==CM_PROB_DISABLED))
		{
			DevInfo.dwStatus = DEV_DISABLE;
		}
		else if ((dwStatus&DN_HAS_PROBLEM) && (dwProblem!=CM_PROB_DISABLED))
		{
			DevInfo.dwStatus = DEV_PROBLEM;
		}
		else
		{
			DevInfo.dwStatus = DEV_NOT_EXIST;
		}

		DevInfoList.AddTail(DevInfo);
	}

	dwError = GetLastError();

	if(hDev != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(hDev);
		hDev = INVALID_HANDLE_VALUE;
	}

	return bRet;
}