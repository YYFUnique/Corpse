#pragma once

class CaseFolder
{
public:
	virtual ~CaseFolder() ;
	virtual size_t Fold(CHAR* pFolded, size_t nFolded, const CHAR* chMixed, size_t nMixed) = 0;
};

class CaseFolderTable : public CaseFolder
{
public:
	CaseFolderTable();
	virtual ~CaseFolderTable();
public:
	virtual size_t Fold(CHAR* pFolded, size_t nFolded, const CHAR* chMixed, size_t nMixed) = 0;
	void SetTranslation(CHAR ch, CHAR chTranslation);
	void StandardASCII();
protected:
	CHAR chMapping[256];
};