#pragma once

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// Blur behind data structures
#define DWM_BB_ENABLE										0x00000001  // fEnable has been specified
#define DWM_BB_BLURREGION							0x00000002  // hRgnBlur has been specified
#define DWM_BB_TRANSITIONONMAXIMIZED	0x00000004  // fTransitionOnMaximized has been specified

typedef struct tagDWM_BLURBEHIND
{
	DWORD	dwFlags;
	BOOL		bEnable;
	HRGN		hRgnBlur;
	BOOL		bTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

typedef struct tagMARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
} MARGINS, *PMARGINS;

// Window attributes
typedef enum tagDWMWINDOWATTRIBUTE
{
	DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
	DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
	DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
	DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
	DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
	DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
	DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
	DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
	DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
	DWMWA_LAST
}DWMWINDOWATTRIBUTE;

// Non-client rendering policy attribute values
typedef enum tagDWMNCRENDERINGPOLICY
{
	DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
	DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
	DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
	DWMNCRP_LAST
}DWMNCRENDERINGPOLICY;

typedef HRESULT (__stdcall *funcDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
typedef HRESULT (__stdcall *funcDwmEnableBlurBehindWindow)(HWND hWnd, __in const DWM_BLURBEHIND* pBlurBehind);
typedef HRESULT (__stdcall *funcDwmIsCompositionEnabled)(BOOL *pfEnabled);
typedef HRESULT (__stdcall *funcDwmGetWindowAttribute)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (__stdcall *funcDwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (__stdcall *funcDwmDefWindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);


class DLL_API DwmHelper
{
protected:
	DwmHelper();
	~DwmHelper();

public:
	static DwmHelper* GetInstance();
	BOOL IsEnable();
	void EnableTransition(HWND hWnd, BOOL bEnable);
protected:
	HMODULE			m_hModule;
	funcDwmExtendFrameIntoClientArea		m_pDwmExtendFrameIntoClientArea;
	funcDwmEnableBlurBehindWindow		m_pDwmEnableBlurBehindWindow;
	funcDwmIsCompositionEnabled			m_pDwmIsCompositionEnabled;
	funcDwmGetWindowAttribute				m_pDwmGetWindowAttribute;
	funcDwmSetWindowAttribute				m_pDwmSetWindowAttribute;
	funcDwmDefWindowProc						m_pDwmDefWindowProc;
};