// CommonDlg.h : CommonDlg DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCommonDlgApp
// �йش���ʵ�ֵ���Ϣ������� CommonDlg.cpp
//

class CCommonDlgApp : public CWinApp
{
public:
	CCommonDlgApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
