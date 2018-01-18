#include "StdAfx.h"
#include "DwmHelper.h"

DwmHelper* DwmHelper::GetInstance()
{
	static DwmHelper dwm;
	return &dwm;
}

DwmHelper::DwmHelper()
{
	m_pDwmExtendFrameIntoClientArea = NULL;
	m_pDwmEnableBlurBehindWindow = NULL;
	m_pDwmIsCompositionEnabled = NULL;
	m_pDwmGetWindowAttribute = NULL;
	m_pDwmSetWindowAttribute = NULL;
	m_pDwmDefWindowProc = NULL;

	m_hModule = LoadLibrary(_T("dwmapi.dll"));

	if (m_hModule == NULL)
		return;

	m_pDwmExtendFrameIntoClientArea = (funcDwmExtendFrameIntoClientArea)GetProcAddress(m_hModule, "DwmExtendFrameIntoClientArea");
	m_pDwmEnableBlurBehindWindow = (funcDwmEnableBlurBehindWindow)GetProcAddress(m_hModule, "DwmEnableBlurBehindWindow");
	m_pDwmIsCompositionEnabled = (funcDwmIsCompositionEnabled)GetProcAddress(m_hModule, "DwmIsCompositionEnabled");
	m_pDwmGetWindowAttribute = (funcDwmGetWindowAttribute)GetProcAddress(m_hModule, "DwmGetWindowAttribute");
	m_pDwmSetWindowAttribute = (funcDwmSetWindowAttribute)GetProcAddress(m_hModule, "DwmSetWindowAttribute");
	m_pDwmDefWindowProc = (funcDwmDefWindowProc)GetProcAddress(m_hModule, "DwmDefWindowProc");
}

DwmHelper::~DwmHelper()
{
	if (m_hModule != NULL)
	{
		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

BOOL DwmHelper::IsEnable()
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (m_hModule == NULL)
			break;

		if (m_pDwmIsCompositionEnabled == NULL)
			break;

		m_pDwmIsCompositionEnabled(&bSuccess);
	} while (FALSE);

	return bSuccess;
}

void DwmHelper::EnableTransition(HWND hWnd, BOOL bEnable)
{
	if (hWnd == NULL)
		return;

	if (m_pDwmSetWindowAttribute == NULL)
		return;

	m_pDwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &bEnable, sizeof(bEnable));
}