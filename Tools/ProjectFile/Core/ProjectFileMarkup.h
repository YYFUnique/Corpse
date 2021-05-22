#pragma once

class CProjectFileMarkup
{
public:
	CProjectFileMarkup();
public:
	BOOL LoadProjectFile(LPCTSTR lpszProjectFile);
	void LoadFromFile(LPCTSTR lpszProjectFile);
	CMarkupNode GetRoot();
protected:
private:
	//CControlUI* _Parse(CMarkupNode* parent, CControlUI* pParent = NULL, CPaintManagerUI* pManager = NULL);

	CMarkup m_xml;
};