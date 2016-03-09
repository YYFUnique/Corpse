#pragma once
#include "../Resource.h"
class CToolItemBase
{
public:
	CToolItemBase(void);
	virtual ~CToolItemBase(void);
	/************************************************************************/
	// 函数名称：OnLButtonDbClick
	// 函数功能：在视图中进行双击时激发此函数，派生类要重写该函数以实现功能
	// 函数参数：该函数没有参数
	// 函数返回值：如果执行成功，返回TRUE，否则返回FALSE，可以使用
	//						GetThreadErrorInfo获取出错信息
	/************************************************************************/
	virtual BOOL OnLButtonDbClick() = 0;
	virtual BOOL OnRButtonClick();
	INT GetSubItemMenu();
	UINT GetIconResourceId();
	CString& GetItemName();
protected:
	CString m_strItemText;
	UINT m_hItemID;
	INT m_nSubMenu;
};
