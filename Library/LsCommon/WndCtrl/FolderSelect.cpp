#include "StdAfx.h"
#include "FolderSelect.h"


CFolderPickerDialog::CFolderPickerDialog(LPCTSTR lpszFolder, DWORD dwFlags, CWnd* pParentWnd, DWORD dwSize, BOOL fNonFileSystemFolders) :
CFileDialog(TRUE, NULL, lpszFolder, dwFlags, NULL, pParentWnd, dwSize, TRUE)
{
	m_bPickFoldersMode = TRUE;
	m_bPickNonFileSysFoldersMode = fNonFileSystemFolders;
}

CFolderPickerDialog::~CFolderPickerDialog()
{
}

IMPLEMENT_DYNAMIC(CFolderPickerDialog, CFileDialog)