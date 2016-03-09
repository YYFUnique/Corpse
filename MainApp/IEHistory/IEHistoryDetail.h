#pragma once

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CHtmlView。"
#endif 

// CIEHistoryDetail Html 视图

class CIEHistoryDetail : public CHtmlView
{
	DECLARE_DYNCREATE(CIEHistoryDetail)
public:
	void SetContent(LPCTSTR lpszDescribe);
protected:
	CIEHistoryDetail();           // 动态创建所使用的受保护的构造函数
	virtual ~CIEHistoryDetail();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};


