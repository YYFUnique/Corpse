// LsCommon.h : LsCommon DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLsCommonApp
// �йش���ʵ�ֵ���Ϣ������� LsCommon.cpp
//

class CLsCommonApp : public CWinApp
{
public:
	CLsCommonApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
