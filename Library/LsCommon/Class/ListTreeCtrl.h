#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

#include "SuperGridCtrl.h"

typedef enum tgaCONTROLTYPE {
	CONTROLTYPE_Static = 0,
	CONTROLTYPE_Edit, 
	CONTROLTYPE_ComboBox, 
	CONTROLTYPE_DateControl, 
	CONTROLTYPE_SpinButton
}CONTROLTYPE;

class LS_COMMON_API CItemInfo
{
public:
	CItemInfo();
	~CItemInfo();

public:
	//���ʹ�� LVS_EX_CHECKBOXES ���
	void SetCheck(BOOL bCheck){ m_bCheck = bCheck;}
	BOOL GetCheck(void) const {return m_bCheck;}

	//��ȡͼ������
	void SetImage(UINT iImage){m_iImage = iImage;}
	UINT GetImage(void) const {return m_iImage;}//return icon

	//��������Ϣ
	void SetItemText(const CString& strItem){ m_strItemName = strItem; }
	void SetItemText(const CString& strItem, COLORREF Color) { m_strItemName = strItem; m_Color = Color;}
	void AddSubItemText(const CString& strSubItem){ m_SubItems.Add(strSubItem); }
	void AddSubItemText(const CString& strSubItem, COLORREF Color)
	{ 
		int nIndex = m_SubItems.Add(strSubItem); 
		m_MapColor.SetAt(nIndex, Color);
	}
	void SetSubItemText(int iSubItem, const CString& strSubItem){ m_SubItems.SetAtGrow(iSubItem, strSubItem); }
	void SetSubItemText(int iSubItem, const CString& strSubItem, COLORREF Color)
	{
		m_SubItems.SetAtGrow(iSubItem, strSubItem);
		m_MapColor.SetAt(iSubItem, Color);
	}

	//��ȡ����Ϣ
	const CString& GetItemText(void){ return m_strItemName; }
	CString GetSubItem(int iSubItem){ return m_SubItems.GetAt(iSubItem); }
	int GetItemCount(void) const { return m_SubItems.GetSize(); }

	//��ȡItem����
	void SetListData(int iSubItem, CStringList *strInitArr)
	{
		CStringList* ListData = new CStringList;	//�����������Զ�ɾ��
		ListData->AddTail(strInitArr);
		m_ListData.SetAt(iSubItem, ListData);
	}	

	BOOL GetListData(int iSubItem, CStringList*& pListData) const
	{	
		return m_ListData.Lookup(iSubItem, pListData);
	}

	//���ÿؼ�����
	void SetControlType(CONTROLTYPE EnumCtrlType, int nCol=-1){ m_ControlType.SetAt(nCol, EnumCtrlType); }

	BOOL GetControlType(int nCol,CONTROLTYPE& ControlType) const
	{
		ControlType = CONTROLTYPE_Static;
		m_ControlType.Lookup(nCol,ControlType);

		return ControlType;
	}

	COLORREF GetItemColor(void) const {return m_Color;}
	COLORREF GetBkColor(int iSubItem) const
	{
		COLORREF BkColor;
		m_MapColor.Lookup(iSubItem,BkColor);

		return BkColor;
	}

	void CopyObjects(CItemInfo* pItemInfo);

	void CopyControls(CItemInfo* pItemInfo);

	void CopyColors(CItemInfo* pItemInfo);

private:
	CONTROLTYPE m_EnumCtrlType; 
	CMap<int,int, CONTROLTYPE, CONTROLTYPE&> m_ControlType;//hmm
	CMap<int,int, COLORREF, COLORREF&> m_MapColor;//colors
	CMap<int,int, CStringList*, CStringList*> m_ListData;//listbox
	CString m_strItemName;					//��һ������
	CStringArray m_SubItems;				//�ǵ�һ������
	BOOL m_bCheck;
	UINT m_iImage;									//ͼ������
	COLORREF m_Color;
	LPARAM m_lParam;
};

class LS_COMMON_API CTreeItem : public CObject
{
public:
	CTreeItem();
	~CTreeItem();
public:

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����GetParent
	//�������ܣ���ȡ�ýڵ��Ӧ�ĸ��ڵ�
	//������������
	//�� ��  ֵ�����ظ��ڵ�ָ�룬���û�и��ڵ���Ϊ��
	//////////////////////////////////////////////////////////////////////////
	CTreeItem* GetParent() const;
	CTreeItem* GetParent();

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����SetParent
	//�������ܣ����øýڵ��Ӧ�ĸ��ڵ�
	//�������������ڵ�ָ��
	//�� ��  ֵ����������֮ǰ�ĸ��ڵ�ָ��
	//////////////////////////////////////////////////////////////////////////
	CTreeItem* SetParent(CTreeItem* pParent);

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����SetListIndex
	//�������ܣ����ý����ListCtrl�ؼ��еı��
	//���������������ListCtrl�еı��
	//�� ��  ֵ����
	//////////////////////////////////////////////////////////////////////////
	void SetListIndex(UINT uListIndex);

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����GetListIndex
	//�������ܣ�ȡ�ý����ListCtrl�ؼ��еı��
	//������������
	//�� ��  ֵ�����ؽ����ListCtrl�ؼ��еı��
	//////////////////////////////////////////////////////////////////////////
	UINT GetListIndex() const;
	UINT GetListIndex();

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����SetListIndent
	//�������ܣ����ý��Ĳ㼶
	//�������������Ĳ㼶
	//�� ��  ֵ����
	//////////////////////////////////////////////////////////////////////////
	void SetListIndent(UINT uListIndent);

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����GetListIndent
	//�������ܣ���ȡ���Ĳ㼶
	//������������
	//�� ��  ֵ�����ؽ��Ĳ㼶
	//////////////////////////////////////////////////////////////////////////
	UINT GetListIndent() const;
	UINT GetListIndent();

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����ItemHasChild
	//�������ܣ���ȡ�ýڵ�ʱ�����ӽڵ�
	//������������
	//�� ��  ֵ������TRUE��ʾ�ý�����ӽڵ�
	//////////////////////////////////////////////////////////////////////////
	BOOL ItemHasChild() const;
	BOOL ItemHasChild();

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����SetItemChildFlag
	//�������ܣ����øý���Ƿ����ӽڵ�
	//�����������Ƿ����ӽڵ���
	//�� ��  ֵ����
	//////////////////////////////////////////////////////////////////////////
	void SetItemChildFlag(BOOL bHasChlid = TRUE);

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����SetHideChild
	//�������ܣ��ӽڵ��Ƿ���Ҫ����
	//�����������Ƿ����ر��
	//�� ��  ֵ����
	//////////////////////////////////////////////////////////////////////////
	void SetHideChild(BOOL bHideChild = TRUE );

	//////////////////////////////////////////////////////////////////////////
	//�� ��  ����IsChildHided
	//�������ܣ��ӽڵ��Ƿ�����
	//������������
	//�� ��  ֵ�������ӽڵ��Ƿ�����
	//////////////////////////////////////////////////////////////////////////
	BOOL IsChildHided() const;
	BOOL IsChildHided();

protected:
	CObList m_ListChild;	
	CTreeItem* m_pParent;			//��ǰ���ĸ��ڵ�ָ��
	CItemInfo* m_lpNodeInfo;		//��ǰ���Ľ��������Ϣ
	BOOL m_bHideChildren;			//����Ƿ�չ��
	UINT m_nIndex;						//�����ListCtrl�еı��
	UINT m_nIndent;						//���Ĳ㼶
	BOOL m_bHasChild;				//����Ƿ����ӽڵ�
	friend class CSuperGridCtrl;
};

class LS_COMMON_API CListTreeCtrl : public CSuperGridCtrl
{
public:
		CListTreeCtrl();
		virtual ~CListTreeCtrl();
public:
	void SortData(void);
	void _DeleteAll(void);
	CImageList *CreateDragImageEx(int nItem);
	BOOL m_bDrag;
	//search item and subitems 
	//usage:
	//	CTreeItem *pResult = Search("Item 1","subitem x","Subitem y","subitem 0", NULL); //must finish with NULL terminator thank you
	//  if(pResult!=NULL) .....bla. bla. bla.
	// returns the pointer to node that matches the search criteria or NULL if not found
	CTreeItem *Search(CString strItem, ...);
	CTreeItem* SearchEx(CTreeItem *pStartPosition, CString strItem);
	// Overrides
	void OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht);	
	void OnUpdateListViewItem(CTreeItem* lpItem, LV_ITEM *plvItem);
	CItemInfo* CopyData(CItemInfo* lpSrc);
	int GetIcon(const CTreeItem* pItem);
	COLORREF GetCellRGB(void);
	BOOL OnItemExpanding(CTreeItem *pItem, int iItem);
	BOOL OnItemExpanded(CTreeItem* pItem, int iItem);
	BOOL OnCollapsing(CTreeItem *pItem);
	BOOL OnItemCollapsed(CTreeItem *pItem);
	BOOL OnDeleteItem(CTreeItem* pItem, int nIndex);
	BOOL OnVkReturn(void);
	BOOL OnItemLButtonDown(LVHITTESTINFO& ht);
protected:
	//CImageList m_image;//Must provide an imagelist
	//positions and creates/initalize a combobox control
	CComboBox* ShowList(int nItem, int nCol, CStringList *lstItems);
	//helper function called from ShowList...calcs the lists max horz extent
	int CalcHorzExtent(CWnd* pWnd, CStringList *pList);
};
