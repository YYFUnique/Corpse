#pragma once

class CBlockNet : public WindowImplBase
{
public:
	CBlockNet(HWND hWnd);
	~CBlockNet();

public:
	virtual void InitWindow();
	void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);

	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual LPCTSTR GetWindowClassName(void) const;

	LRESULT ResponseDefaultKeyEvent(WPARAM wParam);

public:
	void SetAttackTargetIP(LPCTSTR lpszTargetIP);
	void SetAttackTargetMAC(LPCTSTR lpszTargetMAC);
protected:
	void OnClick(TNotifyUI& msg);

protected:
	CDuiString m_strAttackTargetIP;
	CDuiString m_strAttackTargetMAC;	
};