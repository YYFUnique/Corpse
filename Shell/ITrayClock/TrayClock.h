#pragma once

void FreeRemoteLibrary(HWND hWnd); 

class CTrayClock : public WinImplBase
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

	void ClearClockDC();
	void CreateClockDC(HWND hWnd);
	void FillClock(HWND hwnd, HDC hdc, const RECT *prc);
	void DrawClock(HWND hWnd, HDC hdc, const SYSTEMTIME* pt);
	void CopyClockBack(HWND hwnd, HDC hdcDest, HDC hdcSrc, int w, int h);
	void CopyParentSurface(HWND hwnd, HDC hdcDest, int xdst, int ydst, int w, int h, int xsrc, int ysrc);
	BOOL CreateOffScreenDC(HDC hdc, HDC *phdcMem, HBITMAP *phbmp, int width, int height);
protected:
	void FillClockBG(HDC hdcPaint);
	HBITMAP LoadImageFromRes(UINT pResourceID, HMODULE hInstance, LPCTSTR pResourceType);
	void GetClockTextSize(HDC hdc, const TEXTMETRIC* ptm, const wchar_t* str, int *wout, int *hout);
protected:
	BOOL				m_bFocus;
	BOOL				m_bFillTray;
	HDC					m_hdcClock;
	HDC					m_hdcClockBack;
	HBITMAP			m_hbmpClockBack;
	HBITMAP			m_hbmpClock;
	ULONG_PTR	m_gdiplusStartupToken; 
};