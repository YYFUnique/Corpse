#pragma once

class CShellDesktop : public WindowImplBase
{
public:
	CShellDesktop(IFinalCallback* pFinalCallback);
	~CShellDesktop();

public:
	void Init(HWND hWnd);
	void OnFinalMessage(HWND hWnd);

protected:
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	LONG   m_lRef;
	IFinalCallback* m_pFinalCallback;
};