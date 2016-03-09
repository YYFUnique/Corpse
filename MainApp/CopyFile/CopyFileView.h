
// CopyFileView.h : CCopyFileView 类的接口
//


#pragma once
#include "MainFrm.h"
#include "LsCommon/ErrorInfo.h"
#include "DragFile.h"
#include "SelfStruct.h"

#define  WM_UPDATE_STSTUS (WM_USER + 0x110)

#define  NO_NEED_COPY		0			//不需要复制文件
#define  NEED_COPY_FILE	    1			//需要复制文件
#define  FIRST_IS_NEW			2			//源文件是最新文件
#define  SECOND_IS_NEW     4			//目标文件是最新文件

class CCopyFileView : public CListView
{
protected: // 仅从序列化创建
	CCopyFileView();
	DECLARE_DYNCREATE(CCopyFileView)

// 属性
public:
	CCopyFileDoc* GetDocument() const;

// 操作
public:
	void SetOriginalItemText(int iItemCount , LPCTSTR strFilePath);
	void FileExists( LPCTSTR strCurrentDir , LPCTSTR strDesFilePath , CString& strCopyInfo);
	BOOL CopyFileToDirectory(LPCTSTR strSrcPath , LPCTSTR strDesFilePath , CString& strCopyInfo , int iItemIndex);
	void GetIniFilePath(TCHAR* szFilePath);
	BOOL GetFileStatus(LPCTSTR strFilePath , CFileStatus& FileStatusInfo);
	void SetTargetItemText(int iItemCount , LPCTSTR strFilePath);
	//void SetImageList();
	CImageList m_ImgList;
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


	void CopyFileToDestation();
	void ShowUpdate(LPCTSTR lpszTargetFilePath);
	//void SetStatusBar(LPCTSTR strInfo);
	BOOL InsertListItem(LPCTSTR lpszSrcFilePath , LPCTSTR lpszTargetFilePath);
protected:
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(NMHDR* pNMHdr,LPARAM* pResult);
	afx_msg void OnRClick(NMHDR* pNMHdr,LPARAM* lResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnInitialUpdate(); // 构造后第一次调用
	BOOL OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
	void OnDropFiles(HDROP hDropInfo);

	int GetCheckLine(UINT nFlags, CPoint point);
	BOOL OpenDesFilePath(int iItemIndex);
	/************************************************************************/
	/* 将图像添加到ImageList中                                                            */
	/************************************************************************/
	UINT InsertIconToImageList(LPCTSTR lpszSrcFilePath);
	void Refresh();
	void	FindBox();
// 实现
public:
	virtual ~CCopyFileView();
	/************************************************************************/
	/* 功能：判断是否需要拷贝文件                                                        */
	/* 参数一：lpszSrcFile源文件路径													*/
	/* 参数二：lpszDesFile目标文件路径												 */
	/* 返回值：如果目标文件更新且有效则不覆盖返回FALSE,否则返回TRUE*/
	/************************************************************************/
	BOOL CopyFileCondition(LPCTSTR lpszSrcFile , LPCTSTR lpszDesFile);

	/************************************************************************/
	/* 功能：比较文件属性是否相同                                                        */
	/* 参数一：lpszSrcFile源文件路径													*/
	/* 参数二：lpszDesFile目标文件路径												 */
	/* 返回值：如果目标文件更新且有效则不覆盖返回FALSE,否则返回TRUE*/
	/************************************************************************/
	BOOL CompareFileProperty(LPCTSTR lpszSrcFile,LPCTSTR lpszDesFile);

	BOOL GetSrcAndTargetFilePath(int iIndex,LPTSTR lpSrcFilePath,LPTSTR lpTargetFilePath,CString& strCopyInfo);
	BOOL ShowFileInfo(int iIndex);
	void UpdateFileInfo();
	BOOL InitListView();
	BOOL OpenFolderAndSelectFile(TCHAR* szFilePath);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int m_nLastSortSubItem;
	BOOL m_bRefreshByKey;
public:
	
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnListviewEdit();
	afx_msg void OnListviewDelete();
	//afx_msg void OnMenuOpenPath();
	afx_msg void OnMenuLookupTargetFile();
	afx_msg void OnMenuFileProperty();
	afx_msg void OnMenuLookupFile();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMenuRefresh();
	afx_msg LRESULT FindItemName(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCopyAllSelected();
};

#ifndef _DEBUG  // CopyFileView.cpp 中的调试版本
inline CCopyFileDoc* CCopyFileView::GetDocument() const
   { return reinterpret_cast<CCopyFileDoc*>(m_pDocument); }
#endif

