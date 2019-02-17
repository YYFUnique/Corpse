#include "StdAfx.h"
#include "UIScintilla.h"

CScintillaUI::CScintillaUI()
{

}

CScintillaUI::~CScintillaUI()
{

}

LPCTSTR CScintillaUI::GetClass() const
{
	return _T("ScintillaUI");
}

LPVOID CScintillaUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, _T("Scintilla")) == 0)
		return static_cast<CScintillaUI*>(this);
	else
		return CLabelUI::GetInterface(pstrName);
}

UINT CScintillaUI::GetControlFlags()
{
	if( !IsEnabled() ) return CLabelUI::GetControlFlags();

	return UIFLAG_SETCURSOR | UIFLAG_TABSTOP | UIFLAG_IME_COMPOSITION;
}