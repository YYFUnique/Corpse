// LsCUrl.h : LsCUrl DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLsCUrlApp
// �йش���ʵ�ֵ���Ϣ������� LsCUrl.cpp
//

class CLsCUrlApp : public CWinApp
{
public:
	CLsCUrlApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
