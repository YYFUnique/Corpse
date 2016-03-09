#pragma once
#include "GlobalInfo.h"
#include "LsCommon/Json/JsonObject.h"

#ifdef LS_STATIC_LIB_CALL
#define LS_SYSTEMINFO_API 
#elif defined  LS_EXPORTS
#define LS_SYSTEMINFO_API __declspec(dllexport)
#else
#define LS_SYSTEMINFO_API __declspec(dllimport)
#endif

class LS_SYSTEMINFO_API CComputerInfo
{
public:
	CComputerInfo(void);
	virtual ~CComputerInfo(void);
public:
	CString GetComputerInfo();
protected:
	BOOL SetComputerInfo();
protected:
	CJsonObject m_Json;
};
