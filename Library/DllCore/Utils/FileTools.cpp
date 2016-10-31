#include "StdAfx.h"
#include "FileTools.h"

#include <shellapi.h>

BOOL SHDeleteDirectory(LPCTSTR lpszPathName)
{
	SHFILEOPSTRUCT    shFileOp = {0};

	TCHAR szPathName[MAX_PATH+1];
	ZeroMemory(szPathName, _countof(szPathName));

	//由于pFrom需要双终结符，故采用此方式
	_tcsncpy_s(szPathName, _countof(szPathName), lpszPathName, _tcslen(lpszPathName));

	shFileOp.wFunc    = FO_DELETE;
	shFileOp.pFrom   = szPathName;
	shFileOp.pTo		 = NULL;
	shFileOp.fFlags    = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;

	//如果SHFileOperation返回0表示操作成功
	return SHFileOperation(&shFileOp) == 0;
}