#pragma once

void FreeRemoteLibrary(HWND hWnd); 

class CTrayClock : public WindowImplBase
{
public:
	CTrayClock();
	~CTrayClock();

public:
	void Init(HWND hWnd);
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
protected:
	LRESULT OnCalcRect(HWND hWnd);

	void DestoryClock();
	void CreateClockDC(HWND hWnd);
	void FillClockBG(HWND hWnd);
	void FillClockBGHover(HWND hWnd, HDC hdcPaint, const RECT* prc);
	void DrawClock(HWND hWnd, HDC hdc, const SYSTEMTIME* pt);
	void CopyClockBack(HWND hwnd, HDC hdcDest, HDC hdcSrc, int w, int h);
	void CopyParentSurface(HWND hwnd, HDC hdcDest, int xdst, int ydst, int w, int h, int xsrc, int ysrc);
	BOOL CreateDBISectionDC(HDC hdc, HDC *phdcMem, HBITMAP *phbmp, int width, int height);
protected:
	void DrawClockParentBG(HWND hWnd, HDC hdcPaint, const RECT* prc);
	HBITMAP LoadImageFromRes(UINT pResourceID, HMODULE hInstance, LPCTSTR pResourceType);
	void GetClockTextSize(HDC hdc, const TEXTMETRIC* ptm, const wchar_t* str, int *wout, int *hout);
protected:
	HDC					m_hdcClock;
	HDC					m_hdcClockBack;
	
	HBITMAP			m_hbmpClock;
	HBITMAP			m_hbmpClockBack;

	CThemeHelper*		m_pThemeHelper;
	TCLOCK_HIGHLIGHT_WPARAM		m_ClockStates;
};