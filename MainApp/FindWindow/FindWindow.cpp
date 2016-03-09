
// FindWindow.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FindWindow.h"
#include "FindWindowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFindWindowApp

BEGIN_MESSAGE_MAP(CFindWindowApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFindWindowApp ����

CFindWindowApp::CFindWindowApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFindWindowApp ����

CFindWindowApp theApp;


// CFindWindowApp ��ʼ��

BOOL CFindWindowApp::InitInstance()
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

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	/*
	CFindWindowDlg dlg;
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
	*/

	HWND hWndExist = FindWindow(NULL ,  _T("ԭ�α�¶ By Unique"));
	if(hWndExist)
	{
		PostMessage(hWndExist , WM_NOTIFYICON , 0 , WM_LBUTTONDBLCLK);
		return FALSE;
	}
	CDialog* pMainDlg = new CFindWindowDlg;
	pMainDlg->Create(CFindWindowDlg::IDD);

	m_pMainWnd=pMainDlg;
	return TRUE;
}

int CFindWindowApp::ExitInstance() 
{
	if(m_pMainWnd)
	{
		m_pMainWnd->DestroyWindow();
		m_pMainWnd = NULL;
	}

	return CWinApp::ExitInstance();
}
