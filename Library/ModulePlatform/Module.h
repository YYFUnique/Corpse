#pragma once

#ifdef LS_STATIC_LIB_CALL
#define MODULE_PLATFORM_API 
#elif defined  MODULE_PLATFORM_EXPORTS
#define MODULE_PLATFORM_API __declspec(dllexport)
#else
#define MODULE_PLATFORM_API __declspec(dllimport)
#endif

#include "ModulePlatform/ModulePlatform.h"

class MODULE_PLATFORM_API Module
{
public:
	Module(ModulePlatform& platform) : m_Platform(platform) {}
	virtual ~Module() {}
private:
	Module(const Module&);
	Module& operator= (const Module&);

public:
	virtual BOOL Init() = 0;
	virtual BOOL Terminate() = 0;
	virtual BOOL HandleRequest(const CommandType& command, LPCTSTR lpszRequest, CString& strResponse) = 0;

protected:
	ModulePlatform& GetPlatform() { return m_Platform; }

private:
	ModulePlatform& m_Platform;
};