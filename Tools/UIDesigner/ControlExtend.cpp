#include "StdAfx.h"
#include "ControlExtend.h"

CControlExtend::CControlExtend()
{

}

CControlUI* CControlExtend::CreateControl(LPCTSTR pstrClass)
{
	ASSERT(pstrClass);
	CStdString strXmlFilePath;
	strXmlFilePath.Format(_T("%s.xml"),pstrClass);
	CDialogBuilder DialogExtend;
	CControlUI* pControlUI = DialogExtend.Create((LPCTSTR)strXmlFilePath);
	return pControlUI;
}