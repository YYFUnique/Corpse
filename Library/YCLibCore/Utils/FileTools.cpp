#include "StdAfx.h"
#include "FileTools.h"

#include <shellapi.h>

BOOL SHDeleteDirectory(LPCTSTR lpszPathName)
{
	SHFILEOPSTRUCT    shFileOp;

	shFileOp.wFunc    = FO_DELETE;
	shFileOp.pFrom    = lpszPathName;
	shFileOp.pTo    = NULL;
	shFileOp.fFlags    = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR;

	//���SHFileOperation����0��ʾ�����ɹ�
	return SHFileOperation(&shFileOp) == 0;

}