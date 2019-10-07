#include "StdAfx.h"
#include "UIModule.h"
#include "UICommon.h"
#include <atlstr.h>

UIModule::UIModule(ModulePlatform& platform)
	: Module(platform)
{
	CleanAllHandlers();
}

UIModule::~UIModule()
{

}

BOOL UIModule::Init()
{
	CleanAllHandlers();
	return TRUE;
}

BOOL UIModule::Terminate()
{
	CleanAllHandlers();

	return TRUE;
}

BOOL UIModule::HandleRequest(const CommandType& command, LPCTSTR lpszRequest, CString& strResponse)
{
	switch (command)
	{
		case COMMAND_TYPE_INIT:
			return HandleUIModuleInit(lpszRequest, strResponse);
		break;
	}

	return TRUE;
}

void UIModule::CleanAllHandlers()
{

}

BOOL UIModule::HandleUIModuleInit(LPCTSTR lpszRequest, CString& strResponse)
{
	return TRUE;
}