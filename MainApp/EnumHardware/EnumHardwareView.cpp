
// EnumHardwareView.cpp : CEnumHardwareView ���ʵ��
//

#include "stdafx.h"
#include "EnumHardware.h"
#include <InitGuid.h>
#include "EnumHardwareDoc.h"
#include "EnumHardwareView.h"
#include "LsCommon/OsInfo.h"
#include "LsCommon/ErrorInfo.h"

#include "MainFrm.h"
#include <devguid.h>
#include <Uxtheme.h>
#pragma comment(lib,"uxtheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEnumHardwareView
#define	WM_SHOW_HARDWARE		(WM_USER + 0x100)

IMPLEMENT_DYNCREATE(CEnumHardwareView, CTreeView)
	
BEGIN_MESSAGE_MAP(CEnumHardwareView, CTreeView)
	ON_MESSAGE(WM_SHOW_HARDWARE,&CEnumHardwareView::OnShowHardware)
	ON_NOTIFY_REFLECT(NM_RCLICK,&CEnumHardwareView::OnRClick)
	ON_COMMAND(ID_SHOWALL, &CEnumHardwareView::OnShowall)
	ON_COMMAND(ID_DONOT_SHOW_HIDE, &CEnumHardwareView::OnDonotShowHide)
	ON_COMMAND(ID_HARDWARE_SCANF, &CEnumHardwareView::OnHardwareScanf)
	ON_COMMAND(ID_HARDWARE_PROPERTY, &CEnumHardwareView::OnHardwareProperty)
END_MESSAGE_MAP()

// CEnumHardwareView ����/����

CEnumHardwareView::CEnumHardwareView()
{
	// TODO: �ڴ˴���ӹ������
	m_bShowAll = TRUE;
	m_ImgList.DeleteImageList();
	m_ImgList.Create(16,16,ILC_COLOR32|ILC_MASK,10,1);
}

CEnumHardwareView::~CEnumHardwareView()
{
}

BOOL CEnumHardwareView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
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

	LPCTSTR lpszDevName[] = {_T("DVD/CD-ROM������"),_T("IDE ATA/ATAPI ������"),_T("������"),_T("��Я�豸"),_T("����������"),
								_T("���̾�"),_T("�˿�(COM��LPT)"),_T("�Ǽ��弴����������"),_T("�����"),_T("������"),_T("����"),_T("����ѧ�����豸"),_T("��������������Ϸ������"),
								_T("��������ָ���豸"),_T("ͨ�ô������߿�����"),_T("����������"),_T("ϵͳ�豸"),_T("��ʾ�豸��"),
								_T("���ܿ�������"),_T("���ܿ�ɸѡ��"),_T("δ֪�豸")};

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


// CEnumHardwareView ���

#ifdef _DEBUG
void CEnumHardwareView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CEnumHardwareView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CEnumHardwareDoc* CEnumHardwareView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEnumHardwareDoc)));
	return (CEnumHardwareDoc*)m_pDocument;
}
#endif //_DEBUG


// CEnumHardwareView ��Ϣ�������
LRESULT CEnumHardwareView::OnShowHardware(WPARAM wParam,LPARAM lParam)
{
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
					POSITION CurrentPos = pos;
					DEV_INFO& DevInfo = DevInfoList.GetNext(pos);
					m_DevInfoList.AddTail(DevInfo);

					CString strDevName(DevInfo.strDevName);
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
	// TODO: �ڴ���������������
	m_bShowAll = TRUE;
	PostMessage(WM_SHOW_HARDWARE,0,0);
}

void CEnumHardwareView::OnDonotShowHide()
{
	// TODO: �ڴ���������������
	m_bShowAll = FALSE;
	PostMessage(WM_SHOW_HARDWARE,0,0);
}

void CEnumHardwareView::OnHardwareScanf()
{
	// TODO: �ڴ���������������
	CTreeCtrl& TreeCtrl = GetTreeCtrl();
	HTREEITEM ht = TreeCtrl.GetSelectedItem();
	if (ht == NULL)
		return;

	ScanfHardwareInfo();
}

void CEnumHardwareView::OnHardwareProperty()
{
	// TODO: �ڴ���������������
	//ShellExecute(m_hWnd,)
}
