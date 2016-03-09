#pragma once

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class CItemInfo;
class LS_COMMON_API CSuperGridCtrl : public CListCtrl
{
	// Construction
public:
	CSuperGridCtrl();
protected:
	//nested class forward decl.
	friend	class CTreeItem;
public:
	// Overrides
	//MUST override this to make a copy of CItemInfo..see the CMySuperGrid.cpp for implementation
	//used in drag/drop operations
	virtual CItemInfo* CopyData(CItemInfo* lpSrc);
	//sets the icon state...called from within DrawItem returns valid image index
	//You MUST override this function to set your current icon, must of course be a valid CImageList index
	virtual int GetIcon(const CTreeItem* pItem);
	//override this to set the color for current cell
	virtual COLORREF GetCellRGB(void);
	//override this to update listview items, called from within OnEndlabeledit.
	virtual void OnUpdateListViewItem(CTreeItem* lpItem, LV_ITEM *plvItem);
	//override this to activate any control when lButtonDown in a cell, called from within OnLButtonDown
	virtual void OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht);
	//override this to provide your dragimage, default: creates an image + text
	virtual CImageList *CreateDragImageEx(int nItem);
	//called before item is about to explode, return TRUE to continue, FALSE to prevent expanding
	virtual BOOL OnItemExpanding(CTreeItem *pItem, int iItem);
	//called after item has expanded
	virtual BOOL OnItemExpanded(CTreeItem* pItem, int iItem);
	//called before item are collapsed,return TRUE to continue, FALSE to prevent collapse
	virtual BOOL OnCollapsing(CTreeItem *pItem);
	//called after item has collapsed
	virtual BOOL OnItemCollapsed(CTreeItem *pItem);
	//called before item is about to be deleted,return TRUE to continue, FALSE to prevent delete item
	virtual BOOL OnDeleteItem(CTreeItem* pItem, int nIndex);
	//called before VK_MULTIPLY keydown, return TRUE to continue, FALSE to prevent expandall
	virtual BOOL OnVKMultiply(CTreeItem *pItem, int nIndex);
	//called before VK_SUBTRACT keydown, return TRUE to continue, FALSE to prevent collapse item
	virtual BOOL OnVkSubTract(CTreeItem *pItem, int nIndex);
	//called before VK_ADD keydown, return TRUE to continue, FALSE to prevent expanding item
	virtual BOOL OnVKAdd(CTreeItem *pItem, int nIndex);
	//called from PreTranslateMessage, override this to handle other controls than editctrl's
	virtual BOOL OnVkReturn(void);
	//called before from within OnlButtonDown and OnDblclk, but before anything happens, return TRUE to continue, FALSE to say not selecting the item
	virtual BOOL OnItemLButtonDown(LVHITTESTINFO& ht);
public:
	//creates a root
	CTreeItem*  InsertRootItem(CItemInfo * lpRoot);
	//from the looks of it,...it deletes a rootitem
	void DeleteRootItem(CTreeItem * lpRoot);
	//hmm
	BOOL IsRoot(CTreeItem * lpItem);
	//given the rootindex it returns the rootptr
	CTreeItem* GetRootItem(int nIndex);
	int GetRootCount() { return m_RootItems.GetCount();}
	//call this to delete all items in grid
	void DeleteAll();
	//return previous node from pItem, given a RootItem
	CTreeItem* GetPrev(CTreeItem* pRoot, CTreeItem *pItem, BOOL bInit = TRUE, BOOL bDontIncludeHidden=TRUE);
	//return next node from pItem, given a RootItem
	CTreeItem* GetNext(CTreeItem* pRoot, CTreeItem* pNode, BOOL bInit = TRUE, BOOL bDontIncludeHidden=TRUE);
	//returns the selected item :)
	int GetSelectedItem(void) const;
	//returns the itemdata associated with the supergrid
	CTreeItem* GetTreeItem(int nIndex);
	// Retrieves the number of items associated with the SuperGrid control.
	UINT GetCount(void);
	//returns number of children given the pItem node ptr.
	int NumChildren(const CTreeItem *pItem) const;
	//Determines if this tree item is a child of the specified parent
	BOOL IsChildOf(const CTreeItem* pParent, const CTreeItem* pChild) const;
	//hmm
	BOOL ItemHasChildren(const CTreeItem* pItem) const;
	// Use this to indicate that pItem has children, but has not been inserted yet.
	void SetChildrenFlag(CTreeItem *pItem,int nFlag) const;
	//are children collapsed
	BOOL IsCollapsed(const CTreeItem* pItem) const;
	//return the Indent Level of pItem
	int GetIndent(const CTreeItem* pItem) const;
	//Sets the Indentlevel of pItem
	void SetIndent(CTreeItem *pItem, int iIndent);
	//get the pItems current listview index, 
	int GetCurIndex(const CTreeItem *pItem) const;
	//set pItems current listview index
	void SetCurIndex(CTreeItem* pItem, int nIndex);
	//sets the pItem' parent
	void SetParentItem(CTreeItem*pItem, CTreeItem* pParent);
	//gets pItems parent
	CTreeItem* GetParentItem(const CTreeItem* pItem); 
	//return ptr to CItemInfo daaa
	CItemInfo* GetData(const CTreeItem* pItem); 
	//sets the CItemInfo ptr of pItem
	void UpdateData(CTreeItem* pItem, CItemInfo* lpInfo);
	//Insert item and return new parent node.
	//the bUpdate is here for performance reasons, when you insert say 100 node each having 10 children(1000 listview items)
	//the bUpdate should be set to FALSE(default) but when you want to insert an item, and you want to user to see it right away
	//set bUpdate to TRUE.(see the use of bUpdate in the HowToInsertItemsAfterTheGridHasBeenInitialized function in the CMySuperGridCtrl)
	CTreeItem* InsertItem(CTreeItem *pParent, CItemInfo* lpInfo, BOOL bUpdate=FALSE);
	//collapse all children from pItem
	void Collapse(CTreeItem *pItem);
	//expand one folder and return the last index of the expanded folder
	int Expand(CTreeItem* pSelItem, int nIndex);
	//expand all items from pItem and return last index of the expanded folder
	void ExpandAll(CTreeItem *pItem, int& nScroll);
	//expand all node in pItem and stop at pStopAt, used in SelectNode function
	void ExpandUntil(CTreeItem *pItem, CTreeItem* pStopAt);
	//use this if you want to select a node 
	//if the node is collapsed all items with in the node are expanded and the node is selected
	//it returns the listview index for the selected node
	int SelectNode(CTreeItem *pLocateNode);
	//Delete an item in the listview 
	//takes the node to be delete and its listview item index as arg.
	//note the item you delete must be visible, hence the nItem as arg. 
	void DeleteItemEx(CTreeItem *pSelItem, int nItem);
	//returns the number of columns in the listview
	int GetNumCol(void);
	//does a Quicksort of the pParents children and if bSortChildren set, 
	//all items from pParent are sorted. 
	void Sort(CTreeItem* pParent, BOOL bSortChildren);
	// simpel wrapper for the CObList in CTreeItem, same usage as in the COblist
	POSITION GetHeadPosition(CTreeItem* pItem) const;
	POSITION GetTailPosition(CTreeItem *pItem) const;
	CTreeItem* GetNextChild(CTreeItem *pItem, POSITION& pos) const;
	CTreeItem* GetPrevChild(CTreeItem *pItem, POSITION& pos) const;
	void AddTail(CTreeItem *pParent, CTreeItem *pChild);
	//simpel wrapper for the CPtrList collection of rootitems
	//feel free to add more of these simple wrappers.
	POSITION GetRootHeadPosition(void) const;
	POSITION GetRootTailPosition(void) const;
	CTreeItem* GetNextRoot(POSITION& pos) const;
	CTreeItem* GetPrevRoot(POSITION& pos) const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuperGridCtrl)
public:
	//handle VK_xxxx
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	// Implementation
public:
	virtual ~CSuperGridCtrl();
protected:
	//given the rootptr it returns the rootindex.
	int GetRootIndex(CTreeItem * lpRoot);
	//delete pItem and all of its children
	BOOL Delete(CTreeItem *pItem, BOOL bClean=TRUE/*delete itemdata*/);
	//used in drag/drop operation..
	void CopyChildren(CTreeItem* pDest, CTreeItem* pSrc);
	//drag/drop thing....clipboard not supported!
	BOOL DoDragDrop(CTreeItem* pTarget, CTreeItem* pSelItem);
	//updates internal nodes, called when ever insert,delete on listview
	void InternaleUpdateTree(void);
	//Get ListView Index from pNode
	int NodeToIndex(CTreeItem *pNode);
	//see if user clicked the [+] [-] sign, also handles LVS_EX_CHECKBOXES.
	BOOL HitTestOnSign(CPoint point, LVHITTESTINFO& ht);
	//positions an edit-control and creates it
	CEdit* EditLabelEx(int nItem, int nCol);
	int m_cxImage, m_cyImage;//icon size
	//translates column index value to a column order value.
	int IndexToOrder(int iIndex);
	//set hideflag for pItem
	void Hide(CTreeItem* pItem, BOOL bFlag);
	//hmm
	int GetCurSubItem(void){return m_CurSubItem;}
	int GetDragItem(void) const {return m_nDragItem; }
	int GetDropTargetItem(void) const {return m_nDragTarget; }
private:
	//list of rootItems
	CPtrList m_RootItems;
	//internal helpers
	BOOL _Delete(CTreeItem* pStartAt, CTreeItem *pItem, BOOL bClean=TRUE/*delete itemdata*/);
	UINT _GetCount(CTreeItem* pItem, UINT& nCount);
	//hmm
	void DrawTreeItem(CDC* pDC, CTreeItem* pSelItem, int nListItem, const CRect& rcBounds);
	//makes the dot ... thing
	LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	//set the hideflag from pItem and all its children
	void HideChildren(CTreeItem *pItem, BOOL bHide, int iIndent);
	//checks whether a column is visible, if not scrolls to it
	void MakeColumnVisible(int nCol);
	//hmm
	void DrawFocusCell(CDC *pDC, int nItem, int iSubItem);
	//draw the down arrow if any combobox precent in the listview item
	void DrawComboBox(CDC *pDC, CTreeItem *pSelItem, int nItem, int nColumn);
	int m_CurSubItem;//keyboard handling..it is what it says
	//hmm these represents the state of my expresso machine
	int m_nDragItem, m_nDragTarget;
	BOOL m_bIsDragging;
	//helper compare fn used with Quicksort
	static int CompareChildren(const void* p1, const void* p2);
	void CleanMe(CTreeItem *pItem);
	int _NodeToIndex(CTreeItem *pStartpos, CTreeItem* pNode, int& nIndex, BOOL binit = TRUE);
	CPen m_psTreeLine;
	CPen m_psRectangle;
	CPen m_psPlusMinus;
	CBrush m_brushErase;
	friend class CRectangle;
	HIMAGELIST m_himl;
	// Generated message map functions
protected:
	//{{AFX_MSG(CSuperGridCtrl)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);	
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};