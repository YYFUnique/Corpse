#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CSignHelper
{
public:
	CSignHelper();
	~CSignHelper();

public:
	/*
	使用方式
	if (CSign::VerifySignTrust(_T("C:\\Aquarius.exe")) != ERROR_SUCCESS)
	{
		获取错误号，格式化输出验证结果
	}*/
	static LONG VerifySignTrust(LPCTSTR lpszFilePath);
};