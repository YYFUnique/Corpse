
// ParseDNS.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CParseDNSApp:
// �йش����ʵ�֣������ ParseDNS.cpp
//

class CParseDNSApp : public CWinApp
{
public:
	CParseDNSApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CParseDNSApp theApp;