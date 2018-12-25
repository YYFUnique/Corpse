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


// 由于枚举值DWMWA_EXCLUDED_FROM_PEEK只存在于v7.0的SDK中，v6aSDK不存在，故直接使用其值
// DwmSetWindowAttribute(m_hWnd, 12, &renderPolicy, sizeof(int));
// 目前将v7.0 SDK相关定义拷贝至DwmHelper帮助类中，只需要引用该文件即可
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
	DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
	DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
	DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
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

// Values designating how Flip3D treats a given window.
typedef enum tagDWMFLIP3DWINDOWPOLICY
{
	DWMFLIP3D_DEFAULT,      // Hide or include the window in Flip3D based on window style and visibility.
	DWMFLIP3D_EXCLUDEBELOW, // Display the window under Flip3D and disabled.
	DWMFLIP3D_EXCLUDEABOVE, // Display the window above Flip3D and enabled.
	DWMFLIP3D_LAST
}DWMFLIP3DWINDOWPOLICY;

typedef HRESULT (WINAPI *funcDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
typedef HRESULT (WINAPI *funcDwmEnableBlurBehindWindow)(HWND hWnd, __in const DWM_BLURBEHIND* pBlurBehind);
typedef HRESULT (WINAPI *funcDwmIsCompositionEnabled)(BOOL *pfEnabled);
typedef HRESULT (WINAPI *funcDwmGetWindowAttribute)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (WINAPI *funcDwmSetWindowAttribute)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
typedef HRESULT (WINAPI *funcDwmDefWindowProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);


class DLL_API DwmHelper
{
protected:
	DwmHelper();
	~DwmHelper();

public:
	static DwmHelper* GetInstance();
	BOOL IsEnable();
	
	void EnableTransition(HWND hWnd, BOOL bEnable);
	void SetAreoWithPeek(HWND hWnd, DWMNCRENDERINGPOLICY RenderPolicy = DWMNCRP_ENABLED);
	void SetFlip3DPolicy(HWND hWnd, DWMFLIP3DWINDOWPOLICY Flip3D = DWMFLIP3D_EXCLUDEABOVE);
protected:
	HMODULE			m_hModule;
	funcDwmExtendFrameIntoClientArea		m_pDwmExtendFrameIntoClientArea;
	funcDwmEnableBlurBehindWindow		m_pDwmEnableBlurBehindWindow;
	funcDwmIsCompositionEnabled			m_pDwmIsCompositionEnabled;
	funcDwmGetWindowAttribute				m_pDwmGetWindowAttribute;
	funcDwmSetWindowAttribute				m_pDwmSetWindowAttribute;
	funcDwmDefWindowProc						m_pDwmDefWindowProc;
};