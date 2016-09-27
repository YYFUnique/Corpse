#include "StdAfx.h"
#include "FileTools.h"

#include <shellapi.h>

BOOL SHDeleteDirectory(LPCTSTR lpszPathName)
{
	SHFILEOPSTRUCT    shFileOp = {0};

	TCHAR szPathName[MAX_PATH+1];
	ZeroMemory(szPathName, _countof(szPathName));

	//����pFrom��Ҫ˫�ս�����ʲ��ô˷�ʽ
	_tcsncpy_s(szPathName, _countof(szPathName), lpszPathName, _tcslen(lpszPathName));

	shFileOp.wFunc    = FO_DELETE;
	shFileOp.pFrom   = szPathName;
	shFileOp.pTo		 = NULL;
	shFileOp.fFlags    = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;

	//���SHFileOperation����0��ʾ�����ɹ�
	return SHFileOperation(&shFileOp) == 0;
}