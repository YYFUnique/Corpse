// SystemInfo.h : SystemInfo DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSystemInfoApp
// �йش���ʵ�ֵ���Ϣ������� SystemInfo.cpp
//

class CSystemInfoApp : public CWinApp
{
public:
	CSystemInfoApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
