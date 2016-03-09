#pragma once

#include <afxdlgs.h>


class CFolderPickerDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CFolderPickerDialog)
public:
	CFolderPickerDialog(LPCTSTR lpszFolder = NULL, 
									DWORD dwFlags = 0, 
									CWnd* pParentWnd = NULL, 
									DWORD dwSize = NULL, 
									BOOL fNonFileSystemFolders = FALSE);
	virtual ~CFolderPickerDialog();
};

