#pragma once

#include <atlstr.h>

class CDynamicPackage 
{
public:
	CDynamicPackage();
	~CDynamicPackage();

public:
	BOOL Init();

protected:
	CString		m_strSignToolPath;			// ǩ�����������������·��
	CString		m_strCrossCertifyPath;	// ����ǩ��֤�������������·��
	CString		m_strScriptPath;				// ����ű������������·��

	static CString m_strMakeNsisPath;	//	�������װĿ¼
};