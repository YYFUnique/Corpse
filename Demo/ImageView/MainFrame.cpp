#include "stdafx.h"
#include "resource.h"
#include "ImageView.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	BOOL bSuccess = FALSE;
	HRESULT hRet = S_FALSE;
	do 
	{
		WORD wPort = MAKEWORD(0,1);
		hRet = ::CoInitialize(NULL);
		if (FAILED(hRet)) 
			break;
		CImageView* pImageView = new CImageView();
		if (pImageView != NULL)
		{
			pImageView->Create(NULL, _T("图片信息查看工具"),UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
			pImageView->CenterWindow();
			pImageView->ShowWindow();

			CPaintManagerUI::MessageLoop();
			delete pImageView;
		}

		CPaintManagerUI::Term();
		WindowImplBase::Cleanup();

		bSuccess = TRUE;
	} while (FALSE);

	if (SUCCEEDED(hRet))
		::CoUninitialize();

	return bSuccess;
}