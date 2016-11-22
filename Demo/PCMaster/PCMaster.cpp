#include "stdafx.h"
#include "resource.h"
#include "PCMaster.h"
#include "Wnd/ColorSkinWindow.h"
#include "Wnd/MessageTip.h"
#include "Wnd/SkinChange.h"
#include "Wnd/SkinDescribe.h"
#include	"ControlEx/UIListCtrl.h"
#include	"ControlEx/SkinFace.h"
#include "ControlEx/UIToolBar.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <shlobj.h>
#include "DllCore/Utils/ErrorInfo.h"
#include "DllCore/Utils/APIs.h"
#include "DllCore/Utils/TextTools.h"

#include "QRCode/qr.h"

#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Msimg32.lib")

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

#define		WM_ADD_TOOLBAR							(WM_USER+0x100)
#define		UI_FEATUREEFFECT_CONTROL			_T("Tools")
#define		PRIVATE_CONFIG_SECTION_NAME	_T("App")

CPCMaster::CPCMaster()
:m_pTaskbarList(NULL)
{
	WM_TASKBARBUTTONCREATED = :: RegisterWindowMessage (TEXT ("TaskbarButtonCreated" ));
	//m_pDraw = NULL;
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
		case WM_ADD_TOOLBAR:
				OnLoadToobar();
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
	else if (_tcsicmp(pstrClass,_T("ToolBar")) == 0)
		return new CToolBarUI;
	else if (_tcsicmp(pstrClass, _T("SkinFace")) == 0)
		return new CSkinFaceUI;
	return NULL;
}

void CPCMaster::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMDBCLICK)
		OnItemClick(msg);
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
		OnItemSelect(msg);
	else if (msg.sType == DUI_MSGTYPE_MENU)
		OnMenu(msg);
}

void CPCMaster::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CPCMaster::InitWindow()
{
	//将主窗口句柄放入内存映射文件
	//SetMainWndToMapping();
	::DragAcceptFiles(m_hWnd,TRUE);
	//设置显示图标
	SetIcon(IDI_MAINFRAME);
	//设置用户名

	/*int nCode = QR_ERR_NONE;
	//qrInit的5个参数分别是version,mode,纠错等级和掩码，
	//使用过程中使用除了QR_EM_8BIT以外的mode会生成不正常的二维码，暂时不知道原因。
	//如果不确定使用什么模式，请直接使用QR_EM_AUTO模式
	QRCode* pCode = qrInit(5, QR_EM_8BIT, QR_ECL_M, 7, &nCode);

	if (pCode == NULL)
		return;

	LPCSTR lpszText = "http://www.baidu.com/s?wd=我说中国人!";
	LPSTR lpszUtf8 = Gb32ToUtf8(lpszText);
	if (qrAddData(pCode, (const qr_byte_t* )lpszUtf8, strlen(lpszUtf8)) == FALSE)
		return;
	//注意需要调用qrFinalize函数
	if (qrFinalize(pCode) == FALSE)
		return;

	int size = 0;
	//两个5分别表示：像素之间的距离(是否有边框）和二维码图片的放大倍数，范围都是1-16
	//目前由于该函数返回BMP深度为1位，恰巧在Duilib图片解析引擎不能识别深度为1的BMP
	//，故
	qr_byte_t * buffer = qrSymbolToBMP2(pCode, 1, 6, &size);
	if (buffer == NULL)
		return;
*/
	//由于目前Duilib没有做从内存中打开BMP文件方式，暂时存放在缓存目录，待后续修改接口完成后处理
	/*TCHAR szTmpPathName[MAX_PATH],szTmpName[MAX_PATH];
	GetTempPath(_countof(szTmpPathName), szTmpPathName);
	GetTempFileName(szTmpPathName, _T("QRCode"), 0, szTmpName);
*/
	/*HANDLE hFile = CreateFile(_T("C:\\test123.bmp"),GENERIC_WRITE|GENERIC_READ,
				FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	DWORD dwSize = 0;
	if (WriteFile(hFile,buffer,size,&dwSize,NULL) == FALSE)
		return;
	//暂时没有处理写入文件失败的关闭句柄
	CloseHandle(hFile);*/
	//释放BMP分配的内存
	//delete[] buffer;

	/*m_PaintManager.AddImage(_T("BtnFace.bmp"), (LPBYTE)buffer, size);

	qrFreeBMP(buffer);

	CButtonUI* pBtnFace = (CButtonUI*)m_PaintManager.FindControl(_T("BtnFace"));
	if (pBtnFace)
		pBtnFace->SetNormalImage(_T("BtnFace.bmp"));*/
	//
	//free(buffer);
	/*CMessageTip* pTip = new CMessageTip(_T(""),0xFFFFFFFF,_T("测试"),_T("测试内容"));
	pTip->ShowWindow(true);*/

	/*TCHAR szUserName[MAX_PATH];

 	CTextUI* pText = (CTextUI*)m_PaintManager.FindControl(_T("LabelUserName"));
 	if (pText)
 	{
 		DWORD dwSize = _countof(szUserName);
 		GetUserName(szUserName,&dwSize);
 		pText->SetText(szUserName);
		pText->SetToolTip(szUserName);
 	}
*/
	//由于获取的图片是16位色，显示存在问题，故暂时废弃
	/*TCHAR szUserPicturePath[MAX_PATH];
	if (GetUserPicturePath(szUserName,szUserPicturePath,_countof(szUserPicturePath)) == ERROR_SUCCESS)
	{
		CButtonUI* pFace = (CButtonUI*)m_PaintManager.FindControl(_T("BtnFace"));
		if (pFace)
		{
			CDuiString strUserPictrue;
			strUserPictrue.Format(_T("file='%s' dest='1,1,81,81'"),szUserPicturePath);
			pFace->SetNormalImage(strUserPictrue);
		}
	}*/

	//加载工具栏图标
	PostMessage(WM_ADD_TOOLBAR, NULL, NULL);
}

void CPCMaster::OnMenuClick(CControlUI* pControl)
{
	ASSERT(pControl);
	CDuiString strName = pControl->GetManager()->GetRoot()->GetName();
	if (strName == _T("PCStatus"))
	{
		CDuiString strBkImage = pControl->GetBkImage();
		MessageBox(m_hWnd,pControl->GetText(),_T("您点击了菜单项"),MB_YESNO|MB_ICONINFORMATION);
		UINT nStart = strBkImage.Find(_T("'"));
		UINT nEnd = strBkImage.Find(_T("'"),nStart+1);
		CDuiString strBkFileFormat;
		strBkFileFormat.Format(_T("file='%s' dest='4,4,16,16'"),strBkImage.Mid(nStart+1,nEnd-nStart-1));
		CButtonUI* pBtnStatus = (CButtonUI*)m_PaintManager.FindControl(_T("BtnStatus"));
		pBtnStatus->SetBkImage(strBkFileFormat);
		pBtnStatus->SetNormalImage(strBkFileFormat);
	}
	else if (strName == _T("ListMenu"))
	{
		CListCtrlUI* pListCtrl = (CListCtrlUI*)m_PaintManager.FindControl(_T("Tools"));
		if (pControl->GetName() == _T("MenuDel"))
			pListCtrl->RemoveAt(pListCtrl->GetCurSel());
	}else{
		//二级菜单及更多级菜单跟结点没有名称，在这里统一处理
		MessageBox(m_hWnd,pControl->GetText(),_T("您点击了菜单项"),MB_YESNO|MB_ICONINFORMATION);
	}
}

void CPCMaster::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnClose")))
		PostQuitMessage(0);//Close(0);
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnMin")))
	{		
		PostMessage(WM_SYSCOMMAND,SC_MINIMIZE,0);
 		/*m_pEffect = GetAnimation();
 		if (m_pEffect == NULL)
 			return;
		//hDc = CreateCompatibleDC(m_PaintManager.GetPaintDC());
		CListCtrlUI* pListCtrl = (CListCtrlUI*)m_PaintManager.FindControl(UI_FEATUREEFFECT_CONTROL);

		m_pDraw = new CDrawingBoard;
		if (m_pDraw == NULL)
			return;
		
		RECT rcWnd;
		GetWindowRect(m_hWnd,&rcWnd);

		RECT rcPos = pListCtrl->GetPos();
		m_pDraw->Create(rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,false,true);

		BITMAPINFOHEADER bih;
		memset(&bih, 0, sizeof(BITMAPINFOHEADER));
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biHeight = rcPos.bottom-rcPos.top;
		bih.biWidth = rcPos.right-rcPos.left;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
	
		CDrawingBoard DrawTmp;
		DrawTmp.Create(rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top,false,true);
		RECT rcItem = {0,0,rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};
		pListCtrl->DoPaint(DrawTmp.GetSafeHdc(),rcItem);

		BLENDFUNCTION bf={AC_SRC_OVER,0,255,AC_SRC_ALPHA};

		AlphaBlend(m_pDraw->GetSafeHdc(),0,0,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
							DrawTmp.GetSafeHdc(),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,bf);

		AnimationParam Animation;
 		Animation.animationEffect = 36;
 		Animation.animationFrequency = 30;
 		Animation.bShow = FALSE;
 		Animation.effectKey = (WPARAM)pListCtrl;
 		Animation.hdc = m_pDraw->GetSafeHdc();
		Animation.pBmpData = m_pDraw->GetBits();
		Animation.bmpSize = m_pDraw->GetDcSize();
		Animation.hBitmap = m_pDraw->GetBmpHandle();

		m_pEffect->AppendAnimation(Animation);

		m_pEffect->Animation(this,-1);*/
	}
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnSkin")))
	{
	 	CVerticalLayoutUI* pVLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("VLayoutTotal"));
	 	LPCTSTR lpszBkFile = pVLayout->GetBkImage();
		CSkinChange* pSkinChange = new CSkinChange(lpszBkFile,pVLayout->GetBkColor());
		skin_changed_observer_.AddReceiver(pSkinChange);
	}
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnStatus")))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("MenuStatus.xml"));
		pMenu->Init(NULL,strXmlFile, pt,&m_PaintManager);
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
	else if(msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnSpy")))
	{

	}
}

void CPCMaster::OnItemClick(TNotifyUI& msg)
{
	if (msg.pSender->GetClass() == _T("CToolBarUI")){
		CToolBarUI* pToolBar = static_cast<CToolBarUI*>(msg.pSender->GetInterface(_T("ToolBar")));
		if (pToolBar)
			ShellExecute(m_hWnd,_T("open"),pToolBar->GetFilePath(),NULL,NULL,SW_SHOW);
	}
}

void CPCMaster::OnItemSelect(TNotifyUI& msg)
{

}

void CPCMaster::OnMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetParent()->GetName() == _T("Tools"))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		CDuiPoint pt = msg.ptMouse;
		ClientToScreen(m_hWnd, &pt);
		STRINGorID strXmlFile(_T("ListMenu.xml"));
		pMenu->Init(NULL,strXmlFile,pt,&m_PaintManager);
	}
}

LRESULT CPCMaster::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
	TCHAR szFilePathName[MAX_PATH];
	HDROP hDropFile = (HDROP)wParam;

	UINT nCount = DragQueryFile(hDropFile, 0, szFilePathName, sizeof(szFilePathName)); //得到文件路径
	SHFILEINFO ShFileInfo;
	SHGetFileInfo(szFilePathName,0,&ShFileInfo,sizeof(SHFILEINFO),SHGFI_ICON|SHGFI_LARGEICON|SHGFI_TYPENAME|SHGFI_DISPLAYNAME);
	if (StrStrI(ShFileInfo.szTypeName,_T("图像")) != NULL)
		SetBkImage(szFilePathName);
	else
		AddToolBar(szFilePathName,ShFileInfo.szDisplayName);
	DragFinish(hDropFile);
	return 0;
}

void CPCMaster::SetBkImage(LPCTSTR lpszPathFileName)
{
	if (PathFileExists(lpszPathFileName) == FALSE)
		return;

	CControlUI* pControlUI = m_PaintManager.FindControl(_T("VLayoutTotal"));
	ASSERT(pControlUI);
	CDuiString strBkImage;
	strBkImage.Format(_T("file='%s' corner='600,200,1,1'"),lpszPathFileName);
	pControlUI->SetBkImage(strBkImage);
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
				OnLoadToobar();
			break;
		default:
			bHandled = FALSE;
	}
	return TRUE;
}

void CPCMaster::AddToolBar(LPCTSTR lpszToolBar,LPCTSTR lpszDisplayName)
{
	CListCtrlUI* pListCtrl = (CListCtrlUI*)m_PaintManager.FindControl(_T("Tools"));
	if (pListCtrl == NULL)
		return;

	CToolBarUI* pToolBar = new CToolBarUI;
	if (pToolBar == NULL)
		return;

	pToolBar->SetFilePath(lpszToolBar);
	pToolBar->SetToolTip(lpszDisplayName);
	pToolBar->SetText(lpszDisplayName);
	RECT rcTextPadding = pListCtrl->GetPadding();
	pToolBar->SetTextPadding(rcTextPadding);
	RECT rcChildPadding = {0,10,0,0};
	pToolBar->SetPadding(rcChildPadding);
	pListCtrl->Add(pToolBar);
}

void CPCMaster::OnLoadToobar()
{
	TCHAR szModuleFile[MAX_PATH];
	GetModuleFileName(NULL,szModuleFile,_countof(szModuleFile));
	PathRemoveFileSpec(szModuleFile);
	PathAppend(szModuleFile,_T("Config.ini"));

	//if (PathFileExists(szModuleFile) == FALSE)
	//{
	//	SetErrorInfo(SYSTEM_ERROR, 0, _T("查找运行目录下的Config.ini配置文件失败"));
	//	MessageBox(m_hWnd, GetThreadErrorInfoString(), _T("提示") ,MB_OK|MB_ICONSTOP);
	//	return;
	//}

	TCHAR szKeys[5*1024];
	TCHAR szAppName[MAX_PATH];
	TCHAR szAppPath[MAX_PATH];
	GetPrivateProfileString(NULL,NULL,NULL,szKeys,_countof(szKeys),szModuleFile);
	LPCTSTR lpszKeys = szKeys;
	while(*lpszKeys)
	{
		GetPrivateProfileString(lpszKeys,_T("AppName"),NULL,szAppName,_countof(szAppName),szModuleFile);
		GetPrivateProfileString(lpszKeys,_T("AppPath"),NULL,szAppPath,_countof(szAppPath),szModuleFile);

		AddToolBar(szAppPath,szAppName);

		lpszKeys += _tcslen(lpszKeys) + 1;
	}

	//需要添加一个添加按钮，暂时未做
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

void CPCMaster::OnUiEffectBegin(WPARAM effectKey, DWORD animaType)
{
 	CListCtrlUI* pListCtrl = (CListCtrlUI*)effectKey;
 	if (pListCtrl)
 		pListCtrl->SetVisible(false);
	CString strTipInfo;
	strTipInfo.Format(_T("展示动画编号:%d"),animaType);
	OutputDebugString(strTipInfo);
}

void CPCMaster::OnUiEffectEnd(WPARAM effectKey, DWORD animaType)
{
 	CListCtrlUI* pListCtrl = (CListCtrlUI*)effectKey;
 	if (pListCtrl)
 		pListCtrl->SetVisible(true);
		
	/*CTabLayoutUI* pDemo = (CTabLayoutUI*)m_PaintManager.FindControl(_T("demo"));
	if (pDemo)
		pDemo->SelectItem(1);*/
}

void CPCMaster::OnUiEffectDraw()
{
	/*HDC hDc = m_PaintManager.GetPaintDC();
	CListCtrlUI* pListCtrl = (CListCtrlUI*)m_PaintManager.FindControl(UI_FEATUREEFFECT_CONTROL);
	RECT rcPos = pListCtrl->GetPos();
	BLENDFUNCTION bf={AC_SRC_OVER,0,255,AC_SRC_ALPHA};

	RECT rcWnd;
	GetWindowRect(m_hWnd,&rcWnd);

	CDrawingBoard WndMemDc;
	WndMemDc.Create(rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top,false,true);

	RECT rcItem = {0,0,rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};
	if (pListCtrl != m_PaintManager.GetRoot())
		m_PaintManager.GetRoot()->DoPaint(WndMemDc.GetSafeHdc(),rcItem);

	AlphaBlend(WndMemDc.GetSafeHdc(),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
					m_pDraw->GetSafeHdc(),0,0,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,bf);
	
	POINT ptWinPos = {rcWnd.left, rcWnd.top};
	POINT ptSrc = {0, 0};
	SIZE sizeWindow = {rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};

	::UpdateLayeredWindow(m_hWnd, hDc, &ptWinPos, &sizeWindow, WndMemDc.GetSafeHdc(), &ptSrc, 0, &bf, ULW_ALPHA);

	WndMemDc.Delete();*/
}