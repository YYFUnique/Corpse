#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API CSign
{
public:
	CSign();
	~CSign();

public:
	/*
	ʹ�÷�ʽ
	if (CSign::VerifySignTrust(_T("C:\\Aquarius.exe")) != ERROR_SUCCESS)
	{
		��ȡ����ţ���ʽ�������֤���
	}*/
	static LONG VerifySignTrust(LPCTSTR lpszFilePath);
};