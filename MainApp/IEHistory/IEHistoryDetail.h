#pragma once

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CHtmlView��"
#endif 

// CIEHistoryDetail Html ��ͼ

class CIEHistoryDetail : public CHtmlView
{
	DECLARE_DYNCREATE(CIEHistoryDetail)
public:
	void SetContent(LPCTSTR lpszDescribe);
protected:
	CIEHistoryDetail();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CIEHistoryDetail();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};


