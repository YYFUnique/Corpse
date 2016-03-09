#include "StdAfx.h"
#include "360Control.h"

C360Dui::C360Dui()
{

}

CControlUI* C360Dui::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass,_T("ComputerExamine")) == 0)
	{
		CDialogBuilder DialogExtend;
		CControlUI* pControlUI = DialogExtend.Create(_T("ComputerExamine.xml")); 
		return pControlUI;
	}
	return NULL;
}