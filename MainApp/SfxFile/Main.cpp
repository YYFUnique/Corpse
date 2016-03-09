#include "StdAfx.h"
#include "SfxModuleFile.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE , LPSTR szCmdLine, int nCmdShow)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (CSfxModuleFile::OnInitWindow(hInstance,400,300) == FALSE)
			break;

		CSfxModuleFile::MessageLoop();

		bSuccess = TRUE;
	} while (FALSE);
	
	return bSuccess;
}
