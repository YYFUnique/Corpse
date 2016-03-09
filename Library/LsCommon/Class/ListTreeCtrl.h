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
	//如果使用 LVS_EX_CHECKBOXES 风格
	void SetCheck(BOOL bCheck){ m_bCheck = bCheck;}
	BOOL GetCheck(void) const {return m_bCheck;}

	//获取图标索引
	void SetImage(UINT iImage){m_iImage = iImage;}
	UINT GetImage(void) const {return m_iImage;}//return icon

	//设置项信息
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

	//获取项信息
	const CString& GetItemText(void){ return m_strItemName; }
	CString GetSubItem(int iSubItem){ return m_SubItems.GetAt(iSubItem); }
	int GetItemCount(void) const { return m_SubItems.GetSize(); }

	//获取Item数据
	void SetListData(int iSubItem, CStringList *strInitArr)
	{
		CStringList* ListData = new CStringList;	//析构函数会自动删除
		ListData->AddTail(strInitArr);
		m_ListData.SetAt(iSubItem, ListData);
	}	

	BOOL GetListData(int iSubItem, CStringList*& pListData) const
	{	
		return m_ListData.Lookup(iSubItem, pListData);
	}

	//设置控件类型
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
	CString m_strItemName;					//第一列内容
	CStringArray m_SubItems;				//非第一列内容
	BOOL m_bCheck;
	UINT m_iImage;									//图标索引
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
	//函 数  名：GetParent
	//函数功能：获取该节点对应的父节点
	//函数参数：无
	//返 回  值：返回父节点指针，如果没有父节点则为空
	//////////////////////////////////////////////////////////////////////////
	CTreeItem* GetParent() const;
	CTreeItem* GetParent();

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：SetParent
	//函数功能：设置该节点对应的父节点
	//函数参数：父节点指针
	//返 回  值：返回设置之前的父节点指针
	//////////////////////////////////////////////////////////////////////////
	CTreeItem* SetParent(CTreeItem* pParent);

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：SetListIndex
	//函数功能：设置结点在ListCtrl控件中的编号
	//函数参数：结点在ListCtrl中的编号
	//返 回  值：无
	//////////////////////////////////////////////////////////////////////////
	void SetListIndex(UINT uListIndex);

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：GetListIndex
	//函数功能：取得结点在ListCtrl控件中的编号
	//函数参数：无
	//返 回  值：返回结点在ListCtrl控件中的编号
	//////////////////////////////////////////////////////////////////////////
	UINT GetListIndex() const;
	UINT GetListIndex();

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：SetListIndent
	//函数功能：设置结点的层级
	//函数参数：结点的层级
	//返 回  值：无
	//////////////////////////////////////////////////////////////////////////
	void SetListIndent(UINT uListIndent);

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：GetListIndent
	//函数功能：获取结点的层级
	//函数参数：无
	//返 回  值：返回结点的层级
	//////////////////////////////////////////////////////////////////////////
	UINT GetListIndent() const;
	UINT GetListIndent();

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：ItemHasChild
	//函数功能：获取该节点时候有子节点
	//函数参数：无
	//返 回  值：返回TRUE表示该结点有子节点
	//////////////////////////////////////////////////////////////////////////
	BOOL ItemHasChild() const;
	BOOL ItemHasChild();

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：SetItemChildFlag
	//函数功能：设置该结点是否有子节点
	//函数参数：是否有子节点标记
	//返 回  值：无
	//////////////////////////////////////////////////////////////////////////
	void SetItemChildFlag(BOOL bHasChlid = TRUE);

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：SetHideChild
	//函数功能：子节点是否需要隐藏
	//函数参数：是否隐藏标记
	//返 回  值：无
	//////////////////////////////////////////////////////////////////////////
	void SetHideChild(BOOL bHideChild = TRUE );

	//////////////////////////////////////////////////////////////////////////
	//函 数  名：IsChildHided
	//函数功能：子节点是否隐藏
	//函数参数：无
	//返 回  值：返回子节点是否隐藏
	//////////////////////////////////////////////////////////////////////////
	BOOL IsChildHided() const;
	BOOL IsChildHided();

protected:
	CObList m_ListChild;	
	CTreeItem* m_pParent;			//当前结点的父节点指针
	CItemInfo* m_lpNodeInfo;		//当前结点的结点内容信息
	BOOL m_bHideChildren;			//结点是否展开
	UINT m_nIndex;						//结点在ListCtrl中的编号
	UINT m_nIndent;						//结点的层级
	BOOL m_bHasChild;				//结点是否有子节点
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
