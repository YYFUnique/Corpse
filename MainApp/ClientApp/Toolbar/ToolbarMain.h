#pragma once
#include "../Resource.h"

class CToolbarMain
{
public:
	CToolbarMain(void);
	virtual ~CToolbarMain(void);
	/************************************************************************/
	/* 函数名称：Launch				                                                                    */
	/* 函数功能：启动派生类中的相应功能															*/
	/* 参       数：无输入参数																				*/
	/*	返  回 值：无返回值																					*/
	/************************************************************************/
	virtual BOOL Launch() = 0;
	UINT GetCtrlId();
	CString& GetCtrlTipInfo();
protected:
	UINT m_nID;
	CString m_strTipInfo;
};
