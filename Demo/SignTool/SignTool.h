#pragma once

class CSignTool : public WindowImplBase
{
public:
	CSignTool();
	~CSignTool();

public:
	virtual LPCTSTR GetWindowClassName() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);

	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	void OnSave(TNotifyUI& msg);
	void OnClick(TNotifyUI& msg);
	void OnBtnSignCert(TNotifyUI& msg);		// 签发证书
	void OnCertSelfSign(TNotifyUI& msg);		// 签发证书（自签名证书）

	void OnPrivateKeyPath(TNotifyUI& msg, LPCTSTR lpszPrivateKeyName);
	void OnReqFilePath(TNotifyUI& msg, LPCTSTR lpszReqName);
	void OnCertFilePath(TNotifyUI& msg, LPCTSTR lpszCertName);

	void OnBtnCreateReqFile(TNotifyUI& msg);
	void OnUpdateCertName(TNotifyUI& msg);

	void OnItemSelect(TNotifyUI& msg);
	void OnMenu(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);

protected:
	LRESULT OnMenuClick(WPARAM wParam, LPARAM lParam);
protected:
	TCHAR m_szOPENSSLPath[MAX_PATH];
	TCHAR m_szOpenSSLConfigPath[MAX_PATH];
};