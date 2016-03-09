
// CopyFileDoc.h : CCopyFileDoc 类的接口
//


#pragma once


class CCopyFileDoc : public CDocument
{
protected: // 仅从序列化创建
	CCopyFileDoc();
	DECLARE_DYNCREATE(CCopyFileDoc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	virtual void Serialize(CArchive& ar);
	//virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
// 实现
public:
	virtual ~CCopyFileDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAddItemToList();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
public:

};


