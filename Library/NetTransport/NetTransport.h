// NetTransport.h : NetTransport DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNetTransportApp
// �йش���ʵ�ֵ���Ϣ������� NetTransport.cpp
//

class CNetTransportApp : public CWinApp
{
public:
	CNetTransportApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
