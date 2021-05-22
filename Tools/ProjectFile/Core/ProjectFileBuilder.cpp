#include "StdAfx.h"
#include "ProjectFileBuilder.h"

CProjectFileBuilder::CProjectFileBuilder()
{
	m_pProjectFileMark = new CProjectFileMarkup;
}

CProjectFileBuilder::~CProjectFileBuilder()
{
	if (m_pProjectFileMark != NULL)
		delete m_pProjectFileMark;
}

IMarkup* CProjectFileBuilder::Create(LPCTSTR lpszProjectFile, CProjectFileManager* pManager)
{
	m_pProjectFileMark->LoadProjectFile(lpszProjectFile);

	if (pManager != NULL)
		pManager->SetProjectFilePath(lpszProjectFile);

	return Create(pManager);
}

IMarkup* CProjectFileBuilder::Create(CProjectFileManager* pManager)
{
	CMarkupNode root = m_pProjectFileMark->GetRoot();
	if( !root.IsValid() ) return NULL;

	LPCTSTR pstrClass = NULL;
	int nAttributes = 0;
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;

	for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) {
		pstrClass = node.GetName();
		if (_tcsicmp(pstrClass, _T("NoSuchFile")) == 0) {
			nAttributes = node.GetAttributeCount();
			for( int i = 0; i < nAttributes; i++ ) {
				pstrName = node.GetAttributeName(i);
				pstrValue = node.GetAttributeValue(i);
				if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
					while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
					if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
					LPTSTR pstr = NULL;
					DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
					pManager->SetNoSuchFileColor(clrColor);
				}
			}
		} else if (_tcsicmp(pstrClass, _T("ErrorAttribute")) == 0) {
			nAttributes = node.GetAttributeCount();
			for( int i = 0; i < nAttributes; i++ ) {
				pstrName = node.GetAttributeName(i);
				pstrValue = node.GetAttributeValue(i);
				if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
					while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
					if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
					LPTSTR pstr = NULL;
					DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
					pManager->SetErrorAttributeColor(clrColor);
				}
			}
		}
	}

	pstrClass = root.GetName();
	if (_tcsicmp(pstrClass, _T("ProjectFile")) == 0) {
		int nAttributes = root.GetAttributeCount();
		for( int i = 0; i < nAttributes; i++ ) {
			pstrName = root.GetAttributeName(i);
			pstrValue = root.GetAttributeValue(i);
			if (_tcsicmp(pstrName, _T("FilePath")) == 0) {
				pManager->SetProjectFileType((PROJECT_FILE_PATH_TYPE)_ttoi(pstrValue));
			} else if (_tcsicmp(pstrName, _T("Name")) == 0) {
				pManager->SetProjectName(pstrValue);
			}
		}
	}
	
	return _Parse(&root, pManager);
}

IMarkup* CProjectFileBuilder::_Parse(CMarkupNode* pRoot, CProjectFileManager* pManager)
{
	IMarkup* pReturen = NULL;
	IMarkup* pMarkupPre = NULL;
	for (CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling())
	{
		LPCTSTR pstrClass = node.GetName();
		if (_tcscmp(pstrClass, _T("NoSuchFile")) == 0 || _tcsicmp(pstrClass, _T("ErrorAttribute")) == 0 ||
			_tcsicmp(pstrClass, _T("FileInfo")) == 0) 
			continue;

		IMarkup* pMarkup = NULL;
		SIZE_T cchLen = _tcslen(pstrClass);
		switch (cchLen)
		{
			case 4:
					if (_tcsicmp(pstrClass, MARK_FILE) == 0)
						pMarkup = new CProjectFileInfo;
				break;
		}

		if (pMarkup == NULL)
			continue;

		if (pMarkupPre != NULL)
			pMarkupPre->SetNext(pMarkup);

		pMarkupPre = pMarkup;

		// 设置节点属性
		if (node.HasAttributes()) {
			TCHAR szValue[500] = { 0 };
			SIZE_T cchLen = _countof(szValue) - 1;
			// Set ordinary attributes
			int nAttributes = node.GetAttributeCount();
			for (int i = 0; i < nAttributes; i++)
				pMarkup->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
		}

		if (pReturen == NULL)
			pReturen = pMarkup;
	}

	return pReturen;
}