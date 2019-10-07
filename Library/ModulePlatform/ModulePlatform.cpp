#include "StdAfx.h"
#include "CommunicationCenter.h"
#include "Module.h"
#include "ModulePlatform.h"
#include "DllCore/Log/LogHelper.h"
#include "DllCore/Json/JsonObject.h"

#define MODULE_NAME	_T("module")
#define MODULE_CMD		_T("cmd")
#define MODULE_DATA		_T("data")
#define MODULE_OMIT		_T("result")

ModulePlatform::ModulePlatform(LPCTSTR lpszPipeprefix /*= _T("")*/)
	: m_strPipePrefix(lpszPipeprefix)
{	
	m_CommunicationCenter.SetPipePrefix(m_strPipePrefix);
}

ModulePlatform::~ModulePlatform()
{
}

BOOL ModulePlatform::RunAsync()
{
	m_CommunicationCenter.Init(GetNode(), m_strPipePrefix, this);
	if (m_CommunicationCenter.RunAsync() == FALSE)
	{
		QLOG_ERR(_T("Unable to run communication center."));
		return FALSE;
	}

	if (InitModules() == FALSE)
	{
		QLOG_ERR(_T("Unable to init modules."));
		m_CommunicationCenter.Stop();
		return FALSE;
	}

	m_DoneEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
	::ResetEvent(m_DoneEvent);

	QLOG_INFO(_T("Platform[%d] now running"), GetNode());

	return TRUE;
}

void ModulePlatform::WaitUntilDone()
{
	::WaitForSingleObject(m_DoneEvent, INFINITE);
}

BOOL ModulePlatform::Stop()
{
	QLOG_INFO(_T("Platform[%d] now stopping"), GetNode());

	m_CommunicationCenter.Stop();
	if (TerminateModules() == FALSE)
	{
		QLOG_ERR(_T("Unable to terminate modules."));
		return false;
	}
	QLOG_INFO(_T("Modules terminated."));
	::SetEvent(m_DoneEvent);
	return true;
}

BOOL ModulePlatform::StopForced()
{
	QLOG_INFO(_T("Platform[%d] now forcing to stop"), GetNode());
	m_CommunicationCenter.Stop();
	::SetEvent(m_DoneEvent);
	return TRUE;
}

const Module* ModulePlatform::GetModule(const MODULE_TYPE moduleId) const
{
	ModuleMapType::const_iterator it = m_Modules.find(moduleId);
	if (it == m_Modules.end())
		return NULL;

	return it->second;
}

BOOL ModulePlatform::Request(const MODULE_TYPE module, const CommandType& command, LPCTSTR strRequest, CString& strResponse)
{
	if (m_Modules.find(module) != m_Modules.end())
	{
		if (this->ShouldSuppressCommandLog(module, command) == FALSE)
		{
			QLOG_INFO(_T("(Local) %s Command=%d"), STRING_MODULE_ID(module), command);
		}

		return m_Modules[module]->HandleRequest(command, strRequest, strResponse);
	}
	else
	{
		if (this->ShouldSuppressCommandLog(module, command) == FALSE)
		{
			QLOG_INFO(_T("(Remote) %s Command=%d"), STRING_MODULE_ID(module), command);
		}

		CJsonObject JsonPacket;
		JsonPacket.SetValue(MODULE_NAME, module);
		JsonPacket.SetValue(MODULE_CMD, command);
		JsonPacket.SetValue(MODULE_DATA, strRequest);
		JsonPacket.SetValue(MODULE_OMIT, strResponse.IsEmpty());

		if (RequestRemote(module, JsonPacket.ToRawString(), strResponse) == FALSE)
		{
			QLOG_ERR(_T("Unable to request remote node."));
			return FALSE;
		}

		if (strResponse.IsEmpty())
		{
			QLOG_ERR(_T("Remote node responds failed."));
			return FALSE;
		}
		
		return TRUE;
	}
}

BOOL ModulePlatform::HandleRequest(LPCTSTR strRequest, CString& strResponse)
{
	CJsonObject JsonPacket;
	if (JsonPacket.FromString(strRequest) != TRUE)
	{
		QLOG_ERR(_T("Unable to parse request."));
		return FALSE;
	}

	MODULE_TYPE ModuleID =MODULE_TYPE_UNKNOW;
	if (JsonPacket.IsMember(MODULE_NAME))
		JsonPacket.GetValue(MODULE_NAME, (UINT*)&ModuleID);

	if (m_Modules.find(ModuleID) == m_Modules.end())
	{
		QLOG_ERR(_T("%s is not loaded."), STRING_MODULE_ID(ModuleID));
		return FALSE;
	}

	CommandType cmd;
	if (JsonPacket.IsMember(MODULE_CMD))
		JsonPacket.GetValue(MODULE_CMD, (UINT*)&cmd);

	if (this->ShouldSuppressCommandLog(ModuleID, cmd) == FALSE)
	{
		QLOG_INFO(_T("%s Command=%d"), STRING_MODULE_ID(ModuleID), cmd);
	}

	/*std::string module_response;
	std::string* module_response_pointer;
	if (request_packet.omit_response())
		module_response_pointer = NULL;
	else
		module_response_pointer = &module_response;*/
	
	CString strRequestData = JsonPacket.GetValue(MODULE_DATA);
	if (m_Modules[ModuleID]->HandleRequest(cmd, strRequestData, strResponse) == FALSE)
	{
		QLOG_ERR(_T("%s failed to handle request."), STRING_MODULE_ID(cmd));
		return FALSE;
	}
	
	/*response_packet.set_data(module_response);
	if (!response_packet.SerializeToString(response))
	{
		QLOG_ERR(_T("Unable to serialize response."));
		return FALSE;
	}*/
	return TRUE;
}

BOOL ModulePlatform::LoadModule(const MODULE_TYPE id, Module* module)
{
	QLOG_INFO(_T("Platform[%d] - Loading %s"), GetNode(), STRING_MODULE_ID(id));

	if (m_Modules.find(id) != m_Modules.end())
	{
		QLOG_ERR(_T("Module %s is already loaded."), STRING_MODULE_ID(id));
		return FALSE;
	}

	if (module->Init() == FALSE)
	{
		QLOG_ERR(_T("Module %s is unable to init."), STRING_MODULE_ID(id));
		delete module;
		return FALSE;
	}

	m_Modules[id] = module;
	return TRUE;
}

BOOL ModulePlatform::UnloadModule(const MODULE_TYPE id)
{
	QLOG_INFO(_T("Platform[%d] - Unloading %s"), GetNode(), STRING_MODULE_ID(id));

	if (m_Modules.find(id) == m_Modules.end())
	{
		QLOG_INFO(_T("Module %s is not loaded."), STRING_MODULE_ID(id));
		return TRUE;
	}
	if (m_Modules[id]->Terminate() == FALSE)
	{
		QLOG_ERR(_T("Module %s is unable to terminate."), STRING_MODULE_ID(id));
		return false;
	}

	delete m_Modules[id];
	m_Modules.erase(id);

	return TRUE;
}

CommunicationCenter& ModulePlatform::GetCommunicationCenter()
{	
	return m_CommunicationCenter;
}