#include "StdAfx.h"
#include "ControlBuilder.h"

CControlBuilder::CControlBuilder()
{

}

CControlBuilder::~CControlBuilder()
{

}

CControlUI* CControlBuilder::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pControl = NULL;
	if (_tcsicmp(pstrClass, DUI_CTR_FILELIST) == 0)
		pControl = new CFileListUI;
	else if (_tcsicmp(pstrClass, DUI_CTR_FILELISTITEM) == 0)
		pControl = new CFileListItemUI;
	else if (_tcsicmp(pstrClass, DUI_CTR_MUILTLAYOUT) == 0)
		pControl = new CMuiltLayoutUI;
	//else if (_tcsicmp(pstrClass, DUI_CTR_TABOPTION) == 0)
	//	pControl = new CTabOptionUI;
	return pControl;
}