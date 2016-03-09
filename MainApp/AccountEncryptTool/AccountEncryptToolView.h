
// AccountEncryptToolView.h : CAccountEncryptToolView ��Ľӿ�
//


#pragma once


class CAccountEncryptToolView : public CListView
{
protected: // �������л�����
	CAccountEncryptToolView();
	DECLARE_DYNCREATE(CAccountEncryptToolView)

// ����
public:
	CAccountEncryptToolDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CAccountEncryptToolView();
	BOOL AddGroup(LPCTSTR lpszGroupName,int nGroupId);
	BOOL AddItemToGroup(ACCOUNT_INFO& AccountInfo);
	BOOL AdjustListWidth();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // AccountEncryptToolView.cpp �еĵ��԰汾
inline CAccountEncryptToolDoc* CAccountEncryptToolView::GetDocument() const
   { return reinterpret_cast<CAccountEncryptToolDoc*>(m_pDocument); }
#endif

