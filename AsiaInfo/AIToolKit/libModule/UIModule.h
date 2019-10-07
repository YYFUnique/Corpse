#pragma once

#include "ModulePlatform/Module.h"

class UIModule : public Module
{
public:
	UIModule(ModulePlatform& platform);
	~UIModule();

public:
	virtual BOOL Init();
	virtual BOOL Terminate();
	virtual BOOL HandleRequest(const CommandType& command, LPCTSTR lpszRequest, CString& strResponse);
protected:
	BOOL HandleUIModuleInit(LPCTSTR lpszRequest, CString& strResponse);
protected:
	void CleanAllHandlers();
};