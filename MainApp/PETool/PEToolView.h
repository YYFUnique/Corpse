
// PEToolView.h : CPEToolView ��Ľӿ�
//


#pragma once
class CPELibrary;

typedef enum tagITEM_DATA_INFO
{
	ITEM_DATA_INFO_DOS_HEADER = 0,			//PE�ļ�DOSͷ
	ITEM_DATA_INFO_FILE_HEADER = 0,			//PE�ļ�DOSͷ
	ITEM_DATA_INFO_OPTION_HEADER = 0,			//PE�ļ�DOSͷ
}ITEM_DATA_INFO;

class CPEToolView : public CListView
{
protected: // �������л�����
	CPEToolView();
	DECLARE_DYNCREATE(CPEToolView)

// ����
public:
	CPEToolDoc* GetDocument() const;
	void OnShowItemData(ITEM_DATA_INFO ItemDataInfo,CPELibrary* pPELibrary);

	void ShowItemDosHeader(CPELibrary* pPELibrary);
	void ShowItemFileHeader(CPELibrary* pPELibrary);
// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CPEToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // PEToolView.cpp �еĵ��԰汾
inline CPEToolDoc* CPEToolView::GetDocument() const
   { return reinterpret_cast<CPEToolDoc*>(m_pDocument); }
#endif

