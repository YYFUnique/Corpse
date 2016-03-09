
// CaptureScreen.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "CaptureScreen.h"
#include "CaptureScreenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCaptureScreenApp

BEGIN_MESSAGE_MAP(CCaptureScreenApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCaptureScreenApp 构造

CCaptureScreenApp::CCaptureScreenApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CCaptureScreenApp 对象

CCaptureScreenApp theApp;


// CCaptureScreenApp 初始化

BOOL CCaptureScreenApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CCaptureScreenDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CCaptureScreenApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	CCaptureScreenDlg * pDlg=(CCaptureScreenDlg *)AfxGetMainWnd();
	do 
	{
		if (pDlg->m_hWnd != lpMsg->hwnd || IsChild(pDlg->m_hWnd,lpMsg->hwnd))
			return FALSE;

		if(lpMsg->message==WM_KEYDOWN)
		{
			LPRECT lpRect = pDlg->GetSelectRect();

			//如果Shift键按下则方向键调整大小
			BOOL IsShiftDown = FALSE,IsCtrlDown = FALSE;
			SHORT  nVirtKey = GetKeyState(VK_SHIFT); 
			if (nVirtKey & CONTROL_KEY_DOWN) 
				IsShiftDown = TRUE;

			nVirtKey = GetKeyState(VK_CONTROL);
			if (nVirtKey & CONTROL_KEY_DOWN)
				IsCtrlDown = TRUE;

			switch(lpMsg->wParam)
			{
				//默认为平移
				//按住SHIFT表示缩小，按住CTRL表示增大，同时按住表示平移
			case VK_UP:
				if (IsShiftDown == IsCtrlDown)
				{
					lpRect->top -= 1;
					lpRect->bottom -= 1;
				}
				else if (IsShiftDown != FALSE)
					lpRect->bottom -=1;
				else if (IsCtrlDown != FALSE)
					lpRect->top -= 1;
				break;
			case VK_DOWN:
				if (IsShiftDown == IsCtrlDown)
				{
					lpRect->top += 1;
					lpRect->bottom += 1;
				}
				else if (IsShiftDown != FALSE)
					lpRect->top -=1;
				else if (IsCtrlDown != FALSE)
					lpRect->bottom += 1;
				break;
			case VK_LEFT:
				if (IsShiftDown == IsCtrlDown)
				{
					lpRect->left -= 1;
					lpRect->right -= 1;
				}
				else if (IsShiftDown != FALSE)
					lpRect->right -=1;
				else if (IsCtrlDown != FALSE)
					lpRect->left -= 1;	
				break;
			case VK_RIGHT:
				if (IsShiftDown == IsCtrlDown)
				{
					lpRect->left += 1;
					lpRect->right += 1;
				}
				else if (IsShiftDown != FALSE)
					lpRect->left +=1;
				else if (IsCtrlDown != FALSE)
					lpRect->right += 1;
				break;
			}
			pDlg->PaintScreenCapture();
		}

	} while (FALSE);

	return CWinApp::ProcessMessageFilter(code,lpMsg);
}