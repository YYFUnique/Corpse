#pragma once
#include "../Resource.h"

class CToolbarMain
{
public:
	CToolbarMain(void);
	virtual ~CToolbarMain(void);
	/************************************************************************/
	/* �������ƣ�Launch				                                                                    */
	/* �������ܣ������������е���Ӧ����															*/
	/* ��       �������������																				*/
	/*	��  �� ֵ���޷���ֵ																					*/
	/************************************************************************/
	virtual BOOL Launch() = 0;
	UINT GetCtrlId();
	CString& GetCtrlTipInfo();
protected:
	UINT m_nID;
	CString m_strTipInfo;
};
