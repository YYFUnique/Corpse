#include "StdAfx.h"
#include "MenuHandler.h"
#include "Define.h"
#include "TrayClock.h"

CMenuHandler::CMenuHandler()
{

}

CMenuHandler::~CMenuHandler()
{

}

LRESULT CMenuHandler::OnCMD(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = FALSE;
	switch(LOWORD(wParam))
	{
		case IDM_ITEM_EXIT:
				FreeRemoteLibrary(hWnd);
			break;
	}

	return lRet;
}