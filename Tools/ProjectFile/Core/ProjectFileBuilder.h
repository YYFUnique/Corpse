#pragma once

class CProjectFileBuilder
{
public:
	CProjectFileBuilder();
	~CProjectFileBuilder();
public:
	IMarkup* Create(LPCTSTR lpszProjectFile, CProjectFileManager* pManager);
	IMarkup* Create(CProjectFileManager* pManager);
protected:
	IMarkup* _Parse(CMarkupNode* pRoot, CProjectFileManager* pManager);
protected:
	CProjectFileMarkup* m_pProjectFileMark;
};