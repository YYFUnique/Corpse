
// TaskMarDoc.h : CTaskMarDoc 类的接口
//


#pragma once


class CTaskMarDoc : public CDocument
{
protected: // 仅从序列化创建
	CTaskMarDoc();
	DECLARE_DYNCREATE(CTaskMarDoc)

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);

// 实现
public:
	virtual ~CTaskMarDoc();
	CView* GetView(CRuntimeClass* pClass);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};


