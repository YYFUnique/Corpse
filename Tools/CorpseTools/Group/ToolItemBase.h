#pragma once
#include "../Resource.h"
class CToolItemBase
{
public:
	CToolItemBase(void);
	virtual ~CToolItemBase(void);
	/************************************************************************/
	// �������ƣ�OnLButtonDbClick
	// �������ܣ�����ͼ�н���˫��ʱ�����˺�����������Ҫ��д�ú�����ʵ�ֹ���
	// �����������ú���û�в���
	// ��������ֵ�����ִ�гɹ�������TRUE�����򷵻�FALSE������ʹ��
	//						GetThreadErrorInfo��ȡ������Ϣ
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
