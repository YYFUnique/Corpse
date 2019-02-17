#include "StdAfx.h"
#include "PlatWin.h"

// Declarations needed for functions dynamically loaded as not available on all Windows versions.
typedef BOOL (WINAPI *AlphaBlendSig)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
typedef HMONITOR (WINAPI *MonitorFromPointSig)(POINT, DWORD);
typedef HMONITOR (WINAPI *MonitorFromRectSig)(LPCRECT, DWORD);
typedef BOOL (WINAPI *GetMonitorInfoSig)(HMONITOR, LPMONITORINFO);

/*static*/ CRITICAL_SECTION crPlatformLock;
static HINSTANCE hinstPlatformRes = 0;
static bool onNT = false;

static HMODULE hDLLImage = 0;
static AlphaBlendSig AlphaBlendFn = 0;

static HMODULE hDLLUser32 = 0;
static HMONITOR (WINAPI *MonitorFromPointFn)(POINT, DWORD) = 0;
static HMONITOR (WINAPI *MonitorFromRectFn)(LPCRECT, DWORD) = 0;
static BOOL (WINAPI *GetMonitorInfoFn)(HMONITOR, LPMONITORINFO) = 0;

static HCURSOR reverseArrowCursor = NULL;


Platfrom::Platfrom()
{

}

Platfrom::~Platfrom()
{

}

Platfrom::Platfrom(const Platfrom& )
{

}

Platfrom& Platfrom::operator =(const Platfrom&)
{
	return *this;
}

int Platfrom::Clamp(int nVla, int nMinVal, int nMaxVal)
{
	if (nVla > nMaxVal)
		nVla = nMaxVal;
	if (nVla < nMinVal)
		nVla = nMinVal;
	return nVla;
}


void Platform_Initialise(void *hInstance)
{
	OSVERSIONINFO osv = {sizeof(OSVERSIONINFO),0,0,0,0,TEXT("")};
	::GetVersionEx(&osv);
	onNT = osv.dwPlatformId == VER_PLATFORM_WIN32_NT;
	::InitializeCriticalSection(&crPlatformLock);
	hinstPlatformRes = reinterpret_cast<HINSTANCE>(hInstance);
	// This may be called from DllMain, in which case the call to LoadLibrary
	// is bad because it can upset the DLL load order.
	if (!hDLLImage) {
		hDLLImage = ::LoadLibrary(TEXT("Msimg32"));
	}
	if (hDLLImage) {
		AlphaBlendFn = (AlphaBlendSig)::GetProcAddress(hDLLImage, "AlphaBlend");
	}
	if (!hDLLUser32) {
		hDLLUser32 = ::LoadLibrary(TEXT("User32"));
	}
	if (hDLLUser32) {
		MonitorFromPointFn = (MonitorFromPointSig)::GetProcAddress(hDLLUser32, "MonitorFromPoint");
		MonitorFromRectFn = (MonitorFromRectSig)::GetProcAddress(hDLLUser32, "MonitorFromRect");
		GetMonitorInfoFn = (GetMonitorInfoSig)::GetProcAddress(hDLLUser32, "GetMonitorInfoA");
	}

	//ListBoxX_Register();
}

void Platform_Finalise() 
{
	if (reverseArrowCursor != NULL)
		::DestroyCursor(reverseArrowCursor);
	//ListBoxX_Unregister();
	::DeleteCriticalSection(&crPlatformLock);
}
