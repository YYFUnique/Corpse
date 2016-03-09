
// ScreenSpy.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "ScreenSpy.h"
#include "ScreenSpyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenSpyApp

BEGIN_MESSAGE_MAP(CScreenSpyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CScreenSpyApp ����

CScreenSpyApp::CScreenSpyApp()
{
	// ֧����������������
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	m_hwndDlg = NULL;
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CScreenSpyApp ����

CScreenSpyApp theApp;


// CScreenSpyApp ��ʼ��

BOOL CScreenSpyApp::InitInstance()
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

	GdiplusStartupInput gdi;  
	GdiplusStartup(&m_gdiToken,&gdi,NULL);   

	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CScreenSpyDlg dlg;
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

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


#define SHIFTED 0x8000 
BOOL CScreenSpyApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	if ( NULL!= m_hwndDlg)
	{
		if((lpMsg->hwnd==m_hwndDlg) || ::IsChild(m_hwndDlg,lpMsg->hwnd))
		{
			//�����Ϣ��WM_KEYDOWN
			//�÷��������λ��
			if(lpMsg->message==WM_KEYDOWN)
			{
				CRect rect(0,0,0,0);
				CScreenSpyDlg * pDlg=(CScreenSpyDlg *)AfxGetMainWnd();
				rect=pDlg->m_RectTracker.m_rect;

				//���Shift�����������������С
				BOOL isShifeDowm=FALSE;
				int nVirtKey;
				nVirtKey = GetKeyState(VK_SHIFT); 
				if (nVirtKey & SHIFTED) 
					isShifeDowm=TRUE;

				switch(lpMsg->wParam)
				{
				case VK_UP:
					//�������Shift,��ֻ����һ��
					if(!isShifeDowm)
						rect.top-=1;
					rect.bottom-=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				case VK_DOWN:
					rect.top+=1;
					if(!isShifeDowm)
						rect.bottom+=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				case VK_LEFT:
					if(!isShifeDowm)
						rect.left-=1;
					rect.right-=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				case VK_RIGHT:
					rect.left+=1;
					if(!isShifeDowm)
						rect.right+=1;
					pDlg->m_RectTracker.m_rect=rect;
					//pDlg->PaintWindow();
					break;
				}
			}
		}
	}

	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
