
// TestSendRecvByte.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestSendRecvByteApp:
// �йش����ʵ�֣������ TestSendRecvByte.cpp
//

class CTestSendRecvByteApp : public CWinApp
{
public:
	CTestSendRecvByteApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestSendRecvByteApp theApp;