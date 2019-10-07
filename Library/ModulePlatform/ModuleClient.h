#pragma once

#include "ModulePlatform.h"
class ModuleClient
{
public:
	ModuleClient(ModulePlatform& platform) : m_Platform(platform) {}
	ModuleClient(const ModuleClient& _rhs) : m_Platform(_rhs.m_Platform) {}
	virtual ~ModuleClient() {}
private:
	ModuleClient& operator= (const ModuleClient&);

protected:
	ModulePlatform& GetPlatform() { return m_Platform; }

private:
	ModulePlatform& m_Platform;
};