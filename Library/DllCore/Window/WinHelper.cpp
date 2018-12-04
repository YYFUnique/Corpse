#include "StdAfx.h"
#include "WinHelper.h"
#include <ShlDisp.h>

void ShowDesktop()
{
	IShellDispatch4* pDisp = NULL;
	CoCreateInstance(CLSID_Shell, NULL, CLSCTX_ALL, __uuidof(IShellDispatch4), (void **)&pDisp);
	if (pDisp != NULL)
	{
		// ÏÔÊ¾×ÀÃæ
		pDisp->ToggleDesktop();
		pDisp->Release();
	}
}