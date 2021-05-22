#include "StdAfx.h"
#include "FolderTable.h"

CaseFolder::~CaseFolder()
{

}

CaseFolderTable::CaseFolderTable()
{
	for (size_t iChar = 0; iChar < sizeof(chMapping); ++ iChar){
		chMapping[iChar] = static_cast<CHAR>(iChar);
	}
}

CaseFolderTable::~CaseFolderTable()
{

}

size_t CaseFolderTable::Fold(CHAR* pFolded, size_t nFolded, const CHAR* chMixed, size_t nMixed)
{
	if (nMixed > nFolded)
		return 0;

	for (size_t iIndex = 0; iIndex < nFolded; ++iIndex)
		pFolded[iIndex] = chMapping[static_cast<BYTE>(chMixed[iIndex])];
	return nMixed;
}

void CaseFolderTable::SetTranslation(CHAR ch, CHAR chTranslation)
{
	chMapping[static_cast<BYTE>(ch)] = chTranslation;
}

void CaseFolderTable::StandardASCII()
{
	for (size_t iChar = 0; iChar< sizeof(chMapping); ++iChar) {
		if (iChar >= 'A' && iChar <= 'Z')
			chMapping[iChar] = static_cast<CHAR>(iChar - 'A' + 'a');
		else
			chMapping[iChar] = static_cast<CHAR>(iChar);
	}
}

