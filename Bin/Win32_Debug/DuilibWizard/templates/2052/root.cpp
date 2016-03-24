[!if PRE_COMPILED_HEADER]
#include "stdafx.h"
[!endif]
#include "[!output PROJECT_NAME].h"

C[!output SAFE_PROJECT_IDENTIFIER_NAME]::C[!output SAFE_PROJECT_IDENTIFIER_NAME]()
{

}

C[!output SAFE_PROJECT_IDENTIFIER_NAME]::~C[!output SAFE_PROJECT_IDENTIFIER_NAME]()
{

}

LPCTSTR C[!output SAFE_PROJECT_IDENTIFIER_NAME]::GetWindowClassName() const
{
	return _T("DuilibFoundation[!output SAFE_PROJECT_IDENTIFIER_NAME]");
}

CDuiString C[!output SAFE_PROJECT_IDENTIFIER_NAME]::GetSkinFolder()
{
	return _T("[!output DUILIB_XML_FILE_FOLDER]");
}

CDuiString C[!output SAFE_PROJECT_IDENTIFIER_NAME]::GetZIPFileName() const
{
	return _T("[!output DUILIB_XML_ZIPFILE]");
}

CDuiString C[!output SAFE_PROJECT_IDENTIFIER_NAME]::GetSkinFile()
{
	return _T("[!output DUILIB_XML_FILE_NAME]");
}

UILIB_RESOURCETYPE C[!output SAFE_PROJECT_IDENTIFIER_NAME]::GetResourceType() const
{
	return UILIB_ZIP;
}

void C[!output SAFE_PROJECT_IDENTIFIER_NAME]::Notify(TNotifyUI& msg)
{
	//TODO
	//在这里添加响应事件

}

void C[!output SAFE_PROJECT_IDENTIFIER_NAME]::InitWindow()
{
	//TODO
	//在这里添加窗口初始化代码

}

CControlUI* C[!output SAFE_PROJECT_IDENTIFIER_NAME]::CreateControl(LPCTSTR pstrClass)
{
	//TODO
	//在这里添加创建自定义控件代码
	CControlUI* pControl = NULL;
	return NULL;
}