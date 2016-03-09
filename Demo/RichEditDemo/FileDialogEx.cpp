#include "StdAfx.h"
#include "FileDialogEx.h"

CFileDialogEx::CFileDialogEx(void)
{
	memset(&m_szDefExt, 0, sizeof(m_szDefExt));
	memset(&m_szFilter, 0, sizeof(m_szFilter));
	memset(&m_szFileName, 0, sizeof(m_szFileName));
	memset(&m_stOFN, 0, sizeof(OPENFILENAME));

	m_lpszFileName = NULL;

	m_stOFN.lStructSize = sizeof(OPENFILENAME);
	m_stOFN.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_EXPLORER|OFN_NOCHANGEDIR|OFN_EXTENSIONDIFFERENT;
	m_stOFN.lpstrDefExt = m_szDefExt;
	m_stOFN.lpstrFilter = m_szFilter;
	m_stOFN.lpstrFile = m_szFileName;
	m_stOFN.nMaxFile = MAX_PATH;
}

CFileDialogEx::~CFileDialogEx(void)
{
	if (m_lpszFileName != NULL)
	{
		delete []m_lpszFileName;
		m_lpszFileName = NULL;
	}
}

void CFileDialogEx::SetDefExt(LPCTSTR lpszDefExt)
{
	_tcsncpy(m_szDefExt, lpszDefExt, sizeof(m_szDefExt) / sizeof(TCHAR));
}

void CFileDialogEx::SetFileName(LPCTSTR lpszFileName)
{
	_tcsncpy(m_szFileName, lpszFileName, sizeof(m_szFileName) / sizeof(TCHAR));
}

void CFileDialogEx::SetFlags(DWORD dwFlags)
{
	m_stOFN.Flags = dwFlags;
}

// Ê¾Àý_T("Text Files(*.txt)\0*.txt\0ÍøÒ³ÎÄ¼þ\0*.htm;*.html\0All Files(*.*)\0*.*\0\0")
void CFileDialogEx::SetFilter(LPCTSTR lpszFilter)
{
	LPTSTR lpsz = (LPTSTR)lpszFilter;
	LPTSTR lpszOld;
	int i = 0;

	while(1)
	{
		if (i >= MAX_PATH - 1)
			break;

		m_szFilter[i] = *lpsz;
		i++;
		lpszOld = lpsz;
		lpsz = _tcsinc(lpsz);

		if (*lpszOld == '\0' && *lpsz == '\0')
		{
			m_szFilter[i] = *lpsz;
			break;
		}
	}
}

void CFileDialogEx::SetMultiSel(BOOL bMultiSel/* = TRUE*/)
{
	if (bMultiSel)
		m_stOFN.Flags |= OFN_ALLOWMULTISELECT;
	else
		m_stOFN.Flags &= ~OFN_ALLOWMULTISELECT;
}

void CFileDialogEx::SetParentWnd(HWND hParentWnd)
{
	m_stOFN.hwndOwner = hParentWnd;
}

void CFileDialogEx::SetTitle(LPCTSTR lpszTitle)
{
	m_stOFN.lpstrTitle = lpszTitle;
}

void CFileDialogEx::SetFileNameBufferSize(DWORD dwSize)
{
	if (m_lpszFileName != NULL)
	{
		delete []m_lpszFileName;
		m_lpszFileName = NULL;
	}

	if (dwSize <= (sizeof(m_szFileName) / sizeof(TCHAR)))
	{
		m_stOFN.lpstrFile = m_szFileName;
		m_stOFN.nMaxFile = MAX_PATH;
	}
	else
	{
		m_lpszFileName = new TCHAR[dwSize];
		if (m_lpszFileName != NULL)
		{
			memset(m_lpszFileName, 0, dwSize * sizeof(TCHAR));
			m_stOFN.lpstrFile = m_lpszFileName;
			m_stOFN.nMaxFile = dwSize;
		}
		else
		{
			m_stOFN.lpstrFile = m_szFileName;
			m_stOFN.nMaxFile = MAX_PATH;
		}
	}
}

BOOL CFileDialogEx::ShowOpenFileDlg()
{
	return ::GetOpenFileName(&m_stOFN);
}

BOOL CFileDialogEx::ShowSaveFileDlg()
{
	return ::GetSaveFileName(&m_stOFN);
}

CDuiString CFileDialogEx::GetPathName()
{
	return m_stOFN.lpstrFile;
}

CDuiString CFileDialogEx::GetFileName()
{
	return _T("");
}

CDuiString CFileDialogEx::GetFileExt()
{
	return _T("");
}

CDuiString CFileDialogEx::GetFileTitle()
{
	return _T("");
}

CDuiString CFileDialogEx::GetFolderPath()
{
	return _T("");
}


POSITION CFileDialogEx::GetStartPosition()
{
	return (POSITION)m_stOFN.lpstrFile;
}

CDuiString CFileDialogEx::GetNextPathName(POSITION& pos)
{
	LPTSTR lpsz = (LPTSTR)pos;
	if (lpsz == m_stOFN.lpstrFile) // first time
	{
		if ((m_stOFN.Flags & OFN_ALLOWMULTISELECT) == 0)
		{
			pos = NULL;
			return m_stOFN.lpstrFile;
		}

		// find char pos after first Delimiter
		while(*lpsz != '\0')
			lpsz = _tcsinc(lpsz);
		lpsz = _tcsinc(lpsz);

		// if single selection then return only selection
		if (*lpsz == '\0')
		{
			pos = NULL;
			return m_stOFN.lpstrFile;
		}
	}

	CDuiString strBasePath = m_stOFN.lpstrFile;
	CDuiString strFileName = lpsz;
	if (strBasePath.Right(1).CompareNoCase(_T("\\")) != 0)
		strBasePath += _T("\\");
	CDuiString strPath = strBasePath + strFileName;

	// find char pos at next Delimiter
	while(*lpsz != '\0')
		lpsz = _tcsinc(lpsz);
	lpsz = _tcsinc(lpsz);
	if (*lpsz == '\0') // if double terminated then done
		pos = NULL;
	else
		pos = (POSITION)lpsz;

	return strPath;
}