#include "stdafx.h"
#include "ComputerSetting.h"

CComputerSetting g_ComputerSetting;

CComputerSetting::CComputerSetting()
{
	m_nID = ID_TOOL_COMPUTER;
	m_strTipInfo = _T("���������");
}

CComputerSetting::~CComputerSetting()
{

}

BOOL CComputerSetting::Launch()
{
	MessageBox(AfxGetMainWnd()->m_hWnd,_T("������"),GetCtrlTipInfo(),MB_OK);
	return TRUE;
}