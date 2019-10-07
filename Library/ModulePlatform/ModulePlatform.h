#pragma once

#ifdef LS_STATIC_LIB_CALL
#define MODULE_PLATFORM_API 
#elif defined  MODULE_PLATFORM_EXPORTS
#define MODULE_PLATFORM_API __declspec(dllexport)
#else
#define MODULE_PLATFORM_API __declspec(dllimport)
#endif

#pragma warning (disable : 4251)
#include "Modules.h"
#include "CommunicationCenter.h"

class Module;
typedef std::map<MODULE_TYPE, Module*> ModuleMapType;

class MODULE_PLATFORM_API ModulePlatform : public IRequestHandler
{
public:
	ModulePlatform(LPCTSTR lpszPipeprefix = _T(""));
	virtual ~ModulePlatform();
private:
	ModulePlatform(const ModulePlatform&);
	ModulePlatform& operator= (const ModulePlatform&);

public:
	BOOL RunAsync();
	void WaitUntilDone();
	BOOL Stop();
	BOOL StopForced();
	LPCTSTR GetPipePreFix() const { return m_strPipePrefix; }
	const Module* GetModule(const MODULE_TYPE moduleId) const;
	BOOL Request(const MODULE_TYPE module, const CommandType& command, LPCTSTR strRequest, CString& strResponse);

	virtual BOOL HandleRequest(LPCTSTR strRequest, CString& strResponse);

protected:
	virtual BOOL InitModules() = 0;
	virtual BOOL TerminateModules() = 0;
	virtual COMMUNICATION_NODE GetNode() = 0;
	virtual BOOL RequestRemote(const MODULE_TYPE module, LPCTSTR strRequest, CString& strResponse) = 0;
	virtual BOOL ShouldSuppressCommandLog(const MODULE_TYPE module, const CommandType& command) { return FALSE; }
	
	BOOL LoadModule(const MODULE_TYPE id, Module* module); // Module released when init failed
	BOOL UnloadModule(const MODULE_TYPE id); // Module released when unload success
	CommunicationCenter& GetCommunicationCenter();	

private:
	CString m_strPipePrefix;
	ModuleMapType m_Modules;
	CommunicationCenter m_CommunicationCenter;
	HANDLE m_DoneEvent;
};
