#include "stdafx.h"
#include "LsFunc.h"

#include "Dlg/CreateNewConnectDlg.h"
#include "Dlg/CleanProject.h"
#include "Dlg/ShutDownSystemDlg.h"

BOOL ShowNewConnectDlg(CString& strIPAddr,CString& strPort)
{
#ifdef LS_EXPORTS
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

	BOOL bRet = FALSE;
	CCreateNewConnectDlg NewConnectDlg;
	if (NewConnectDlg.DoModal() == IDOK)
	{
		strIPAddr = NewConnectDlg.GetIPAddr();
		strPort = NewConnectDlg.GetPort();

		bRet = TRUE;
	}

	return bRet;
}

INT_PTR ShowShutDownDlg()
{
#ifdef LS_EXPORTS
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	CShutDownSystemDlg ShutDownDlg;

	return ShutDownDlg.DoModal();
}