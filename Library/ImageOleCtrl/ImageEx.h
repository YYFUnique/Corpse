#pragma once

class CImageEx;
typedef struct _LS_IMAGEOLECTRL 
{
	HWND hMainWnd;
	CImageEx* pImageEx;
}IMAGEOLECTRL,*PIMAGEOLECTRL;

class CImageEx
{
public:
	CImageEx(HWND hWnd);
	~CImageEx();
public:
	HRESULT GetFileName(BSTR* lpszFileName);
	HRESULT LoadFromFile(BSTR& bstrFileName);
	void ShowImage(HDC HdcDraw,LPRECT lpRect);
	LPRECT GetImageRect();
	SIZE GetImageSize();
	void OnNextFrame();
	UINT GetFrameDelay(UINT nFrame = 0);
	static UINT CALLBACK TimerRoutine(LPVOID lParam,BOOLEAN TimerOrWaitFired);
	HANDLE      m_hTimer;
	HANDLE      m_hQueueTimer;
protected:

protected:
	GUID			m_PageGuid;
	int				m_iCurrentImage;
	Image*		    m_pImage;
	RECT			m_rcPos;
	HDC*			m_hDCs;
	HBITMAP*   m_phBitmaps;
	HBITMAP*   m_phOldBitmaps;
	/*HWND			m_hWndContainer;*/
	int			    m_nFrmCount;
	int				m_nWidth;
	int				m_nHeight;
	PropertyItem* m_pPropertyItem;
	IMAGEOLECTRL* m_pImageOleCtrl;
};