// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

typedef CList<CString,CString&> CTreeItem;

class CResourceViewBar : public CDockablePane
{
// Construction
public:
	CResourceViewBar();

// Attributes
protected:
	CViewTree m_wndResourceView;
	CImageList m_ResourceViewImages;

public:
	void OnChangeVisualStyle();

	void InsertImageTree(LPCTSTR strTitle, CString strPath);
	void RemoveImageTree(const CString& strTree);
	void  RenameImageTree(LPCTSTR pstrTree, LPCTSTR pstrNewName);
	void InsertImage(const CString& strImage, const CString& strTree);
	const CStringArray* GetImageTree(const CString& strTree) const;
	void CopyImageToSkinDir(LPCTSTR pstrSkinDir, LPCTSTR pstrTree);
	BOOL GetMapTree(CStringArray& strMapTree);
	BOOL GetSkinFilePath(LPCTSTR lpszTreeName,CString& strTreeSkinPath);
protected:
	void InitResourceView();

private:
	CMapStringToPtr m_mapTree;
	CTreeItem m_TreeItem;
	CMapStringToPtr m_mapImageArray;

// Implementation
public:
	virtual ~CResourceViewBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
};

