#include "stdafx.h"
#include "RichEdit.h"
#include "Utils.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CDuiString strFileName = CPaintManagerUI::GetInstancePath() + _T("ImageOleCtrl.dll");
	if (DllRegisterServer(strFileName) == FALSE)	// ע��COM���
	{
		::MessageBox(NULL, _T("COM���ע��ʧ�ܣ�Ӧ�ó����޷���ɳ�ʼ��������"), _T("��ʾ"), MB_OK);
		return 0;
	}

	HRESULT hRet = ::OleInitialize(NULL);
	HMODULE hRichEditDll = NULL;
	BOOL bSuccess = FALSE;
	do 
	{
		if (FAILED(hRet))
			break;

		CPaintManagerUI::SetInstance(hInstance);
		CRichEdit* pRichEdit = new CRichEdit();
		if (pRichEdit != NULL)
		{
			pRichEdit->Create(NULL, _T("��������������"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
			pRichEdit->CenterWindow();
			pRichEdit->ShowWindow();

			CPaintManagerUI::MessageLoop();
			delete pRichEdit;
		}

		CPaintManagerUI::Term();
		WindowImplBase::Cleanup();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::OleUninitialize();

	return 0;
}