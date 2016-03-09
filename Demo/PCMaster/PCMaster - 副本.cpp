#include "stdafx.h"
#include "resource.h"
#include "PCMaster.h"
#include "Wnd/ColorSkinWindow.h"
#include "Wnd/MessageTip.h"
#include "Wnd/SkinChange.h"
#include "Wnd/SkinDescribe.h"
#include	"ControlEx/ListCtrl.h"
#include	"ControlEx/SkinFace.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <shlobj.h>
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"shlwapi.lib")

typedef enum _MONITORPOWER
{
	MONITORPOWER_OPEN = 1,	//开启显示器
	MONITORPOWER_CLOSE,		//关闭显示器
};

typedef enum _tagTHUMBBUTTON_INDEX
{
	THUMBBUTTON_FIRST = 1,
	THUMBBUTTON_TWO,
	THUMBBUTTON_THREE,
};

#define		WM_ADD_TOOLBAR				(WM_USER+0x100)

CPCMaster::CPCMaster()
:m_pTaskbarList(NULL)
{
	WM_TASKBARBUTTONCREATED = :: RegisterWindowMessage (TEXT ("TaskbarButtonCreated" ));
	//设置窗口属性，打开自定义缩略图和AeroPeek预览
	// 	BOOL truth = TRUE;
	// 	DwmSetWindowAttribute(g_hWnd, DWMWA_HAS_ICONIC_BITMAP, &truth, sizeof(truth));
	// 	DwmSetWindowAttribute(g_hWnd, DWMWA_FORCE_ICONIC_REPRESENTATION, &truth, sizeof(truth));
}

CPCMaster::~CPCMaster()
{

}

// DUI_BEGIN_MESSAGE_MAP(CPCMaster, WindowImplBase)
// 	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
// DUI_END_MESSAGE_MAP()

LRESULT CPCMaster::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_DROPFILES:
				OnDropFiles(wParam,lParam);
			break;
		case WM_COMMAND:
			if (HIWORD(wParam) == THBN_CLICKED)
				OnThumbnailButtonClicked(LOWORD(wParam));
			break;
		case WM_COPYDATA:
			{
				COPYDATASTRUCT* pCopyData = reinterpret_cast<COPYDATASTRUCT *>(lParam);  
				if(pCopyData->dwData == 1)  
				{  
					LPCTSTR szCmdLine = reinterpret_cast<LPCTSTR>(pCopyData->lpData);  
					if(szCmdLine != NULL)  
						HandleCmdLine(szCmdLine);  
				} 
			}
			break;
		default:
			return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
	}
	return 0;
}

LPCTSTR CPCMaster::GetWindowClassName() const
{
	return _T("DuilibPCMaster");
}

LPCTSTR CPCMaster::GetResourceID() const
{
	//return MAKEINTRESOURCE(IDR_ZIP_SKIN);
	return _T("");
}

CDuiString CPCMaster::GetZIPFileName() const
{
	return _T("PCMaster.zip");
}

CDuiString CPCMaster::GetSkinFile()
{
	return _T("PCMaster.xml");
}

CDuiString CPCMaster::GetSkinFolder()
{
	return _T("PCMaster");
}

UILIB_RESOURCETYPE CPCMaster::GetResourceType() const
{
	return UILIB_FILE;
}

CControlUI* CPCMaster::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass,_T("ListCtrl")) == 0)
		return new CListCtrlUI(m_PaintManager);
	else if (_tcsicmp(pstrClass, _T("ListCtrlItem")) == 0)
		return new CListCtrlItemUI;
	else if (_tcsicmp(pstrClass, _T("SkinFace")) == 0)
		return new CSkinFaceUI;
	return NULL;
}

void CPCMaster::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
		OnItemClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		OnItemSelect(msg);
}

void CPCMaster::InitWindow()
{
	//将主窗口句柄放入内存映射文件 
	SetMainWndToMapping();
	::DragAcceptFiles(m_hWnd,TRUE);
	//设置显示图标
	SetIcon(IDI_MAINFRAME);
	//设置用户名

 	CTextUI* pText = (CTextUI*)m_PaintManager.FindControl(_T("LabelUserName"));
 	if (pText)
 	{
 		TCHAR szUserName[MAX_PATH];
 		DWORD dwSize = _countof(szUserName);
 		GetUserName(szUserName,&dwSize);
 		pText->SetText(szUserName);
 	}
	
	/*
	CreateJumpList();
 	CVerticalLayoutUI* pVLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("VLayoutTotal"));
 	LPCTSTR lpszBkFile = pVLayout->GetBkImage();
 	CMessageTip* pMessageTip = new CMessageTip(lpszBkFile,pVLayout->GetBkColor(),_T("启动消息"),_T("程序需要开机启动，是否允许？"));
 	skin_changed_observer_.AddReceiver(pMessageTip);
	*/

	// 	CString strUserPicturePath;
	// 	if (GetUserPicturePath(szUserName,strUserPicturePath) == TRUE)
	// 	{
	// 		CButtonUI* pBtnFace = (CButtonUI*)m_PaintManager.FindControl(_T("BtnFace"));
	// 		CDuiString strBtnFace;
	// 		strBtnFace.Format(_T("file='%s' dest='1,1,81,81'"),strUserPicturePath);
	// 		pBtnFace->SetNormalImage(strBtnFace);
	// 	}

	//AddToolBoard();


// 	CButtonUI* pFaceBtn = (CButtonUI*)m_PaintManager.FindControl(_T("BtnFace"));
// 
// 	CDuiString strFaceImage;
// 	strFaceImage.Format(_T("file='%s' dest='1,1,81,81' mask='0x00FF03FF'"),szFilePath);
// 
// 	pFaceBtn->SetNormalImage(strFaceImage);

	PostMessage(WM_ADD_TOOLBAR);
}

void CPCMaster::OnMenuClick(CControlUI* pControl)
{
	ASSERT(pControl);
	if (pControl != NULL)
	{
		CDuiString strBkImage = pControl->GetBkImage();
		UINT nStart = strBkImage.Find(_T("'"));
		UINT nEnd = strBkImage.Find(_T("'"),nStart+1);
		CDuiString strBkFileFormat;
		strBkFileFormat.Format(_T("file='%s' dest='4,4,16,16'"),strBkImage.Mid(nStart+1,nEnd-nStart-1));
		CButtonUI* pBtnStatus = (CButtonUI*)m_PaintManager.FindControl(_T("BtnStatus"));
		pBtnStatus->SetBkImage(strBkFileFormat);
		pBtnStatus->SetNormalImage(strBkFileFormat);
	}
}

void CPCMaster::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnClose")))
		PostQuitMessage(0);
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnMin")))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnSkin")))
	{
	 	CVerticalLayoutUI* pVLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("VLayoutTotal"));
	 	LPCTSTR lpszBkFile = pVLayout->GetBkImage();
		CSkinChange* pSkinChange = new CSkinChange(lpszBkFile,pVLayout->GetBkColor());
		skin_changed_observer_.AddReceiver(pSkinChange);
	}
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnStatus")))
	{
		CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
		CPoint pt = msg.ptMouse;
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("MenuStatus.xml"));
		pMenu->Init(NULL,strXmlFile,_T("xml"), pt);
	}
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnCloseDisplay")))
		SendMessage(WM_SYSCOMMAND,SC_MONITORPOWER,MONITORPOWER_CLOSE);
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnLock")))
		LockWorkStation();
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnIconCache")))
		SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST,NULL,NULL);
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnChangeSkin")))
	{
		/*MessageBox(m_hWnd,_T("CatchBtn"),_T("提示"),MB_OK);*/
	}
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnChangeColor")))
	{
		CDuiRect rcWindow;
		GetWindowRect(m_hWnd, &rcWindow);
		rcWindow.top = rcWindow.top + msg.pSender->GetPos().bottom;
		new CColorSkinWindow(this, rcWindow);
	}
}

void CPCMaster::OnItemClick(TNotifyUI& msg)
{

}

void CPCMaster::OnItemSelect(TNotifyUI& msg)
{
	CListCtrlItemUI* pListCtrlItem = (CListCtrlItemUI*)msg.pSender->GetInterface(_T("ListCtrlItem"));
	//if (pListCtrlItem)
	//	ShellExecute(m_hWnd,_T("open"),pListCtrlItem->GetItemPath(),NULL,NULL,SW_SHOW);
}

LRESULT CPCMaster::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
	TCHAR szFilePathName[MAX_PATH];
	HDROP hDropFile = (HDROP)wParam;

	UINT nCount = DragQueryFile(hDropFile, 0, szFilePathName, sizeof(szFilePathName)); //得到文件路径
	if (PathFileExists(szFilePathName))
	{
		CControlUI* pControlUI = m_PaintManager.FindControl(_T("VLayoutTotal"));
		ASSERT(pControlUI);
		CDuiString strBkImage;
		strBkImage.Format(_T("file='%s' corner='600,200,1,1'"),szFilePathName);
		pControlUI->SetBkImage(strBkImage);
	}
	DragFinish(hDropFile);
	return 0;
}

void CPCMaster::SetBkColor(DWORD dwBackColor)
{
	CControlUI* pBackground = m_PaintManager.FindControl(_T("VLayoutTotal"));
	if (pBackground != NULL)
	{
		pBackground->SetBkImage(_T(""));
		pBackground->SetBkColor(dwBackColor);
		pBackground->NeedUpdate();

		SkinChangedParam param;
		param.bkcolor = pBackground->GetBkColor();
		param.bgimage = pBackground->GetBkImage();
		skin_changed_observer_.Broadcast(param);
	}
}

LRESULT CPCMaster::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*if (uMsg == WM_TASKBARBUTTONCREATED)
	{
		CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList, (void **)&m_pTaskbarList);
		if (m_pTaskbarList == NULL)
			return FALSE;

		m_hIcon = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(IDI_CORRECT));
		m_hRed = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(IDI_BTN_RED));
		m_hGreen = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(IDI_BTN_GREEN));
		m_hBlue = LoadIcon(CPaintManagerUI::GetInstance(),MAKEINTRESOURCE(IDI_BTN_BLUE));

		m_pTaskbarList->SetOverlayIcon(m_hWnd,m_hIcon,L"在线状态");

		AddThumbnailButtons();
	}*/

	switch(uMsg)
	{
		case WM_MENUCLICK:
				OnMenuClick((CControlUI*)lParam);
			break;
		case WM_ADD_TOOLBAR:
				OnAddToobar();
			break;
		default:
			bHandled = FALSE;
	}
	return TRUE;
}

void CPCMaster::OnAddToobar()
{
	CListCtrlUI* pListCtrl = (CListCtrlUI*)m_PaintManager.FindControl(_T("Tools"));
	if (pListCtrl == NULL)
		return;
	for (int n=0;n<5;++n)
	{
		CPictureUI* pPicture = new CPictureUI;
		if (pPicture == NULL)
			return;

		pPicture->SetManager(&m_PaintManager, NULL, false);
		LPCTSTR pDefaultAttributes = m_PaintManager.GetDefaultAttributeList(DUI_CTR_PICTURE);
		if( pDefaultAttributes ) {
			pPicture->ApplyAttributeList(pDefaultAttributes);
		}

		pPicture->SetFilePath(_T("C:\\LSGuardClientSetup.exe"));
		pPicture->SetToolTip(_T("这是代码添加的"));
		pListCtrl->Add(pPicture);
	}
}

void CPCMaster::AddToolBoard()
{
	CListCtrlUI* pListCtrl = (CListCtrlUI*)m_PaintManager.FindControl(_T("Tools"));
	if (pListCtrl == NULL)
		return;
	/*ListItemInfo ListCtrlIconItem;
	for (int i=0;i<20;++i)
	{
		ListCtrlIconItem.strListItemIcon = _T("file='Government.ico' nIcon='2'");
		ListCtrlIconItem.strListItemName = _T("木马防火墙");
		pListCtrl->AddNode((LPARAM)&ListCtrlIconItem);
	}*/
}

void CPCMaster::AddThumbnailButtons()
{
	THUMBBUTTONMASK dwMask = THB_ICON | THB_TOOLTIP;
	THUMBBUTTON ThumbBtn[3];

	ThumbBtn[0].dwMask = dwMask;
	ThumbBtn[0].iId = THUMBBUTTON_FIRST;
	ThumbBtn[0].hIcon = m_hRed;
	_tcsncpy_s(ThumbBtn[0].szTip,TEXT("Red"), sizeof(ThumbBtn[0].szTip));

	ThumbBtn[1].dwMask = dwMask;
	ThumbBtn[1].iId = THUMBBUTTON_TWO;
	ThumbBtn[1].hIcon = m_hGreen;
	_tcsncpy_s(ThumbBtn[1].szTip, TEXT("Green"), sizeof(ThumbBtn[1].szTip));

	ThumbBtn[2].dwMask = dwMask;
	ThumbBtn[2].iId = THUMBBUTTON_THREE;
	ThumbBtn[2].hIcon = m_hBlue;
	_tcsncpy_s(ThumbBtn[2].szTip,TEXT("Blue"),sizeof(ThumbBtn[2].szTip));

	m_pTaskbarList->ThumbBarAddButtons(m_hWnd,ARRAYSIZE(ThumbBtn) , ThumbBtn);
}

void CPCMaster::OnThumbnailButtonClicked(UINT nBtnId)
{
	switch(nBtnId)
	{
	case THUMBBUTTON_FIRST:
		SetForegroundWindow(m_hWnd);
		MessageBox(m_hWnd, TEXT("Red Button Clicked!"), _T("提示"), MB_OK);
		break;
	case THUMBBUTTON_TWO:
		SetForegroundWindow(m_hWnd);
		MessageBox(m_hWnd, TEXT("Green Button Clicked!"), _T("提示"), MB_OK);
		break;
	case THUMBBUTTON_THREE:
		SetForegroundWindow(m_hWnd);
		MessageBox(m_hWnd, TEXT("Blue Button Clicked!"), _T("提示"), MB_OK);
		break;
	}
}

void CPCMaster::SetMainWndToMapping()
{
	HANDLE hMMFFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,   
										PAGE_READWRITE, 0, sizeof(m_hWnd), _T("Local//MMFTestJumpList"));  
	LPVOID lpVoid = MapViewOfFile(hMMFFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);  
	memcpy_s(lpVoid, sizeof(m_hWnd), &m_hWnd, sizeof(m_hWnd));  
	UnmapViewOfFile(hMMFFile);  
}

void CPCMaster::HandleCmdLine(LPCTSTR lpszCmdLine)
{
	if(NULL != StrStr(lpszCmdLine, _T("/Task1")))
		MessageBox(m_hWnd,lpszCmdLine,_T("提示"),MB_OK);
}

void CPCMaster::CreateJumpList()
{
	//由于SDK开发版本问题，VS2008不支持Win7任务开发，可以直接拷贝过去也可以
	//创建List
	ICustomDestinationList *pList = NULL;
	HRESULT hRet = CoCreateInstance(__uuidof(DestinationList), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pList));
	if(FAILED(hRet))
		return;
	IObjectArray *pOARemoved = NULL;
	IObjectArray *pOATasks = NULL;
	IObjectCollection *pOCTasks = NULL;
	do 
	{
		//BeginList
		UINT uMinSlots;
		hRet = pList->BeginList(&uMinSlots, IID_PPV_ARGS(&pOARemoved));
		if(FAILED(hRet))
			break;
		//ObjectCollection
		hRet = CoCreateInstance(__uuidof(EnumerableObjectCollection), NULL,CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pOCTasks));
		if (FAILED(hRet))
			break;
		hRet = AddShellLink(pOCTasks);
		if (FAILED(hRet))
			break;
		//ObjectArray
		
		hRet = pOCTasks->QueryInterface(IID_PPV_ARGS(&pOATasks));
		if(FAILED(hRet))
			break;
		hRet = pList->AddUserTasks(pOATasks);
		if(FAILED(hRet))
			break;
		hRet = pList->CommitList();
		pOATasks->Release();

	} while (FALSE);
	if (pOCTasks != NULL)
		pOCTasks->Release();
	if (pOATasks != NULL)
		pOATasks->Release();
	if (pList != NULL)
		pList->Release();

	return;
}

HRESULT CPCMaster::AddShellLink(IObjectCollection *pOCTasks)
{
	//创建ShellLink
	IShellLink *pSLAutoRun = NULL;
	HRESULT hRet = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pSLAutoRun));
	if(FAILED(hRet))
		return hRet;

	do 
	{
		TCHAR szPath[MAX_PATH];  
		GetModuleFileName(GetModuleHandle(NULL), szPath, MAX_PATH);  
		hRet = pSLAutoRun->SetPath(szPath); 
		if (FAILED(hRet))
			break;
		hRet = pSLAutoRun->SetIconLocation(szPath, 0);  
		if (FAILED(hRet))
			break;
		hRet = pSLAutoRun->SetArguments(_T("/Task1"));
		if (FAILED(hRet))
			break;
		hRet = SetTitle(pSLAutoRun, _T("User Task 1"));  
		if (FAILED(hRet))
			break;
	} while (FALSE);
	if (pSLAutoRun != NULL)
		pSLAutoRun->Release();

	return hRet;
}

HRESULT CPCMaster::SetTitle( IShellLink * pShellLink, LPCTSTR lpszTitle )
{
	//标题
	IPropertyStore *pPS = NULL;
	HRESULT hRet = pShellLink->QueryInterface(IID_PPV_ARGS(&pPS));
	if(FAILED(hRet))
		return hRet;
	do 
	{
		PROPVARIANT pvTitle;
		hRet = InitPropVariantFromString(lpszTitle,&pvTitle);
		if (FAILED(hRet))
			break;
		hRet = pPS->SetValue(PKEY_Title, pvTitle);
		if(FAILED(hRet))
			break;
		hRet = pPS->Commit();
		if (FAILED(hRet))
			break;
		PropVariantClear(&pvTitle);
	} while (FALSE);

	if (pPS != NULL)
		pPS->Release();

	return hRet;
}