#pragma once

#include "ModulePlatform/ModuleClient.h"
#include "ModulePlatform/ModulePlatform.h"

class UIClient : public ModuleClient
{
public:
	UIClient(ModulePlatform& platform);
	virtual ~UIClient();

public:
	BOOL IsUIModuleInit();
};