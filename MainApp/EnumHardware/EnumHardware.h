
// EnumHardware.h : EnumHardware Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CEnumHardwareApp:
// �йش����ʵ�֣������ EnumHardware.cpp
//

class CEnumHardwareApp : public CWinApp
{
public:
	CEnumHardwareApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEnumHardwareApp theApp;
