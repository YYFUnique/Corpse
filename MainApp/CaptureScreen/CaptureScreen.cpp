
// CaptureScreen.cpp : ����Ӧ�ó��������Ϊ��
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


// CCaptureScreenApp ����

CCaptureScreenApp::CCaptureScreenApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CCaptureScreenApp ����

CCaptureScreenApp theApp;


// CCaptureScreenApp ��ʼ��

BOOL CCaptureScreenApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CCaptureScreenDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
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

			//���Shift�����������������С
			BOOL IsShiftDown = FALSE,IsCtrlDown = FALSE;
			SHORT  nVirtKey = GetKeyState(VK_SHIFT); 
			if (nVirtKey & CONTROL_KEY_DOWN) 
				IsShiftDown = TRUE;

			nVirtKey = GetKeyState(VK_CONTROL);
			if (nVirtKey & CONTROL_KEY_DOWN)
				IsCtrlDown = TRUE;

			switch(lpMsg->wParam)
			{
				//Ĭ��Ϊƽ��
				//��סSHIFT��ʾ��С����סCTRL��ʾ����ͬʱ��ס��ʾƽ��
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