
// CopyFileView.h : CCopyFileView ��Ľӿ�
//


#pragma once
#include "MainFrm.h"
#include "LsCommon/ErrorInfo.h"
#include "DragFile.h"
#include "SelfStruct.h"

#define  WM_UPDATE_STSTUS (WM_USER + 0x110)

#define  NO_NEED_COPY		0			//����Ҫ�����ļ�
#define  NEED_COPY_FILE	    1			//��Ҫ�����ļ�
#define  FIRST_IS_NEW			2			//Դ�ļ��������ļ�
#define  SECOND_IS_NEW     4			//Ŀ���ļ��������ļ�

class CCopyFileView : public CListView
{
protected: // �������л�����
	CCopyFileView();
	DECLARE_DYNCREATE(CCopyFileView)

// ����
public:
	CCopyFileDoc* GetDocument() const;

// ����
public:
	void SetOriginalItemText(int iItemCount , LPCTSTR strFilePath);
	void FileExists( LPCTSTR strCurrentDir , LPCTSTR strDesFilePath , CString& strCopyInfo);
	BOOL CopyFileToDirectory(LPCTSTR strSrcPath , LPCTSTR strDesFilePath , CString& strCopyInfo , int iItemIndex);
	void GetIniFilePath(TCHAR* szFilePath);
	BOOL GetFileStatus(LPCTSTR strFilePath , CFileStatus& FileStatusInfo);
	void SetTargetItemText(int iItemCount , LPCTSTR strFilePath);
	//void SetImageList();
	CImageList m_ImgList;
// ��д
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
	virtual void OnInitialUpdate(); // ������һ�ε���
	BOOL OnHeaderClick(NMHDR* pNMHDR, LRESULT* pResult);
	void OnDropFiles(HDROP hDropInfo);

	int GetCheckLine(UINT nFlags, CPoint point);
	BOOL OpenDesFilePath(int iItemIndex);
	/************************************************************************/
	/* ��ͼ����ӵ�ImageList��                                                            */
	/************************************************************************/
	UINT InsertIconToImageList(LPCTSTR lpszSrcFilePath);
	void Refresh();
	void	FindBox();
// ʵ��
public:
	virtual ~CCopyFileView();
	/************************************************************************/
	/* ���ܣ��ж��Ƿ���Ҫ�����ļ�                                                        */
	/* ����һ��lpszSrcFileԴ�ļ�·��													*/
	/* ��������lpszDesFileĿ���ļ�·��												 */
	/* ����ֵ�����Ŀ���ļ���������Ч�򲻸��Ƿ���FALSE,���򷵻�TRUE*/
	/************************************************************************/
	BOOL CopyFileCondition(LPCTSTR lpszSrcFile , LPCTSTR lpszDesFile);

	/************************************************************************/
	/* ���ܣ��Ƚ��ļ������Ƿ���ͬ                                                        */
	/* ����һ��lpszSrcFileԴ�ļ�·��													*/
	/* ��������lpszDesFileĿ���ļ�·��												 */
	/* ����ֵ�����Ŀ���ļ���������Ч�򲻸��Ƿ���FALSE,���򷵻�TRUE*/
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
	
// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // CopyFileView.cpp �еĵ��԰汾
inline CCopyFileDoc* CCopyFileView::GetDocument() const
   { return reinterpret_cast<CCopyFileDoc*>(m_pDocument); }
#endif

