#include "StdAfx.h"
#include "UIClient.h"
#include "UICommon.h"

UIClient::UIClient(ModulePlatform& platform)
	: ModuleClient(platform)
{

}

UIClient::~UIClient()
{

}

BOOL UIClient::IsUIModuleInit()
{
	CString strResponse;
	return this->GetPlatform().Request(UI_MODULE, COMMAND_TYPE_INIT, NULL, strResponse);
}