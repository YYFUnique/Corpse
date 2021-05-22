#pragma once

class CScintillaUI : public CLabelUI
{
public:
	CScintillaUI();
	~CScintillaUI();
public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags();

public:

};