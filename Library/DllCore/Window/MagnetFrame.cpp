#include "StdAfx.h"
#include "MagnetFrame.h"

const int  KDistMin = 10;

CMagnetFrame* CMagnetFrame::m_pMagnet = NULL;

CMagnetFrame::CMagnetFrame()
{
	m_hMainWnd = NULL;
}

CMagnetFrame::~CMagnetFrame()
{

}

BOOL CMagnetFrame::Initialize()
{
	if (m_pMagnet == NULL)
		m_pMagnet = new CMagnetFrame;

	return m_pMagnet != NULL;
}

CMagnetFrame* CMagnetFrame::GetInstance()
{
	return m_pMagnet;
}

void CMagnetFrame::SetMainWnd(HWND hWnd)
{
	m_hMainWnd = hWnd;
	AddSubWnd(hWnd);
}

BOOL CMagnetFrame::AddSubWnd(HWND hWnd, ATTCH_MODE AttchMode /*= ATTCH_MODE_NULL*/, ATTCH_ALIGN AttchAlign /*= ATTCH_ALIGN_NULL*/, int nDist /*= 0*/)
{
	POSITION pos = m_SubWndProc.GetHeadPosition();
	while(pos)
	{
		const WND_DATA& WndData = m_SubWndProc.GetNext(pos);
		if (WndData.hWnd == hWnd)
			return TRUE;
	}

	BOOL bSuccess =  FALSE;
	do 
	{
		ATTCH_INFO AttchInfo = {nDist, AttchMode, AttchAlign};
		WND_DATA WndData = {hWnd, NULL, AttchInfo, FALSE};
		if (SubclassWnd(hWnd, &WndData) == FALSE)
			break;

		WndData.bNewPos = TRUE;
		UpdateSubWndPos(hWnd, &AttchInfo);
		WndData.bNewPos = FALSE;

		WndData.bSubWnd = TRUE;

		m_SubWndProc.AddTail(WndData);

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CMagnetFrame::RemoveSubWnd(HWND hWnd)
{
	POSITION pos = m_SubWndProc.GetHeadPosition();
	while(pos)
	{
		POSITION oldPos = pos;
		const WND_DATA& WndData = m_SubWndProc.GetNext(pos);
		if (WndData.hWnd == hWnd)
		{
			m_SubWndProc.RemoveAt(oldPos);
			break;
		}
	}

	return TRUE;
}

BOOL CMagnetFrame::SubclassWnd(HWND hWnd, WND_DATA* pWndData)
{
	BOOL bSuccess = FALSE;
	do 
	{
		//在吸附框架中的窗口不能使用了USERDATA，因为这个值要被吸附框架占用.
		//if (GetWindowLongPtr(hWnd, GWLP_USERDATA) != 0)
		//	break;

		WNDPROC pWndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		if (pWndProc == NULL)
			break;

		//SetWindowLongPtr(hWnd, GWLP_USERDATA, (ULONG)this);
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (ULONG_PTR)&CMagnetFrame::MagnetWndProc);
		
		if (pWndData)
			pWndData->pProc = pWndProc;

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CMagnetFrame::UnsubclassWnd(HWND hWnd)
{
	POSITION pos = m_SubWndProc.GetHeadPosition();
	while(pos)
	{
		POSITION oldPos = pos;

		const WND_DATA& WndData = m_SubWndProc.GetNext(pos);
		if (WndData.hWnd == hWnd)
		{
			m_SubWndProc.RemoveAt(oldPos);
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, (ULONG_PTR)WndData.pProc);
			//SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);

			break;
		}
	}

	return TRUE;
}

void CMagnetFrame::UpdateSubWndPos(HWND hSubWnd, const ATTCH_INFO* pAttchInfo)
{
	RECT rcSub;
	RECT rcMain;
	GetWindowRect(m_hMainWnd,&rcMain);
	GetWindowRect(hSubWnd, &rcSub);

	int x = 0, y = 0;

	switch (pAttchInfo->AttchMode)
	{
		case ATTCH_MODE_RIGHT:
			{
				switch(pAttchInfo->AttchAlign)
				{
				case ATTCH_ALIGN_TOP:
						y = rcMain.top;
					break;
				case ATTCH_ALIGN_BOTTOM:
						y = rcMain.bottom - (rcSub.bottom - rcSub.top);
					break;
				default:
						y = rcMain.top + pAttchInfo->nDist;
					break;
				}

				MoveWindow(hSubWnd, rcMain.right, y, rcSub.right - rcSub.left, rcSub.bottom - rcSub.top, TRUE);
			}
			break;
		case ATTCH_MODE_LEFT:
			{
				switch(pAttchInfo->AttchAlign)
				{
				case ATTCH_ALIGN_TOP:
						y = rcMain.top;
					break;
				case ATTCH_ALIGN_BOTTOM:
						y = rcMain.bottom - (rcSub.bottom - rcSub.top);
					break;
				default:
						y = rcMain.top + pAttchInfo->nDist;
					break;
				}

				MoveWindow(hSubWnd, rcMain.left - (rcSub.right - rcSub.left), y ,rcSub.right - rcSub.left, rcSub.bottom - rcSub.top, TRUE);
			}
			break;
		case ATTCH_MODE_TOP:
			{
				switch(pAttchInfo->AttchAlign)
				{
				case ATTCH_ALIGN_LEFT:
						x = rcMain.left;
					break;
				case ATTCH_ALIGN_RIGHT:
						x = rcMain.right - (rcSub.right - rcSub.left);
					break;
				default:
						x = rcMain.left + pAttchInfo->nDist;
					break;
				}

				MoveWindow(hSubWnd, x, rcMain.top - (rcSub.bottom - rcSub.top), rcSub.right - rcSub.left, rcSub.bottom - rcSub.top, TRUE);
			}
			break;
		case ATTCH_MODE_BOTTOM:
			{
				switch(pAttchInfo->AttchAlign)
				{
				case ATTCH_ALIGN_LEFT:
						x = rcMain.left;
					break;
				case ATTCH_ALIGN_RIGHT:
						x = rcMain.right - (rcSub.right - rcSub.left);
					break;
				default:
						x = rcMain.left + pAttchInfo->nDist;
					break;
				}

				MoveWindow(hSubWnd, x, rcMain.bottom, rcSub.right - rcSub.left, rcSub.bottom - rcSub.top, TRUE);
			}
			break;
	}
}

ATTCH_INFO CMagnetFrame::CalcAttchMode(HWND hSubWnd, LPRECT prcSubWnd)
{
	RECT rcMain;
	GetWindowRect(m_hMainWnd, &rcMain);
	if (IsRectEmpty(prcSubWnd))
		GetWindowRect(hSubWnd, prcSubWnd);

	ATTCH_INFO AttchInfo = {0, ATTCH_MODE_NULL, ATTCH_ALIGN_NULL};

	if (IsNeighbour(rcMain.left, prcSubWnd->right))
	{
		AttchInfo.AttchMode = ATTCH_MODE_LEFT;
		int nDist = CalcDistance(rcMain.bottom, prcSubWnd->bottom);
		
		if (nDist == 0)
		{
			AttchInfo.AttchAlign = ATTCH_ALIGN_BOTTOM;
		}
		else
		{
			nDist = CalcDistance(rcMain.top, prcSubWnd->top);
			if (nDist == 0)
				AttchInfo.AttchAlign = ATTCH_ALIGN_TOP;
			else
				AttchInfo.AttchAlign = ATTCH_ALIGN_NULL;
		}
		AttchInfo.nDist = nDist;

	}else if (IsNeighbour(rcMain.right, prcSubWnd->left))
	{
		AttchInfo.AttchMode = ATTCH_MODE_RIGHT;

		int nDist = CalcDistance(rcMain.bottom, prcSubWnd->bottom);

		if (nDist == 0)
			AttchInfo.AttchAlign = ATTCH_ALIGN_BOTTOM;
		else
		{
			nDist = CalcDistance(rcMain.top, prcSubWnd->top);
			if (nDist)
				AttchInfo.AttchAlign = ATTCH_ALIGN_TOP;
			else
				AttchInfo.AttchAlign = ATTCH_ALIGN_NULL;
		}
		AttchInfo.nDist = nDist;
	}else if (IsNeighbour(rcMain.top, prcSubWnd->bottom))
	{
		AttchInfo.AttchMode = ATTCH_MODE_TOP;
		int nDist = CalcDistance(rcMain.right, prcSubWnd->right);
		if (nDist == 0)
			AttchInfo.AttchAlign = ATTCH_ALIGN_RIGHT;
		else
		{
			nDist = CalcDistance(rcMain.left, prcSubWnd->left);
			if (nDist == 0)
				AttchInfo.AttchAlign = ATTCH_ALIGN_LEFT;
			else
				AttchInfo.AttchAlign = ATTCH_ALIGN_NULL;
		}
		AttchInfo.nDist = nDist;
	}
	else if (IsNeighbour(rcMain.bottom, prcSubWnd->top))
	{
		AttchInfo.AttchMode = ATTCH_MODE_BOTTOM;
		int nDist = CalcDistance(rcMain.right, prcSubWnd->right);
		if (nDist == 0)
			AttchInfo.AttchAlign = ATTCH_ALIGN_RIGHT;
		else
		{
			nDist = CalcDistance(rcMain.left, prcSubWnd->left);
			if (nDist == 0)
				AttchInfo.AttchAlign = ATTCH_ALIGN_LEFT;
			else
				AttchInfo.AttchAlign = ATTCH_ALIGN_NULL;
		}
		AttchInfo.nDist = nDist;
	}

	return AttchInfo;
}

BOOL CMagnetFrame::IsNeighbour(int nV1, int nV2)
{
	return abs(nV1 - nV2) < KDistMin;
}

int CMagnetFrame::CalcDistance(int nV1, int nV2)
{
	int nDist = nV2 - nV1;
	if (abs(nDist) < KDistMin)
		nDist = 0;
	return nDist;
}

LRESULT CALLBACK CMagnetFrame::MagnetWndProc(HWND hWnd, UINT uMsg, WPARAM wParma, LPARAM lParam)
{
	CMagnetFrame *pMagnet = GetInstance();

	if (pMagnet == NULL)
		return FALSE;

	WND_DATA WndData;
	ZeroMemory(&WndData, sizeof(WND_DATA));
	// 修改吸附窗体需要引用链表中的对象，故保存节点位置
	//	可能存在先获取到节点位置，使用时节点已经移除的BUG，暂时未解决

	BOOL bFindItem = FALSE;
	POSITION WndDataPos = NULL;
	POSITION pos = pMagnet->m_SubWndProc.GetHeadPosition();
	while(pos)
	{
		WndDataPos = pos;
		WndData = pMagnet->m_SubWndProc.GetNext(pos);
		if (WndData.hWnd == hWnd)
		{
			bFindItem = TRUE;
			break;
		}
	}

	if (bFindItem == FALSE)
		WndDataPos = NULL;

	if (uMsg == WM_MOVE || uMsg == WM_SIZE)
	{
		if (pMagnet->m_hMainWnd == hWnd)
		{
			RECT rcMain;
			GetWindowRect(hWnd,&rcMain);

			POSITION posWnd = pMagnet->m_SubWndProc.GetHeadPosition();
			while(posWnd)
			{
				WND_DATA& SubWndData = pMagnet->m_SubWndProc.GetNext(posWnd);

				if (SubWndData.bSubWnd == FALSE)
					continue;

				if (SubWndData.hWnd == hWnd)
					continue;

				SubWndData.bNewPos = TRUE;
				pMagnet->UpdateSubWndPos(SubWndData.hWnd, &SubWndData.AttchInfo);
				SubWndData.bNewPos = FALSE;
			}
		}
		else if (WndDataPos != NULL)
		{
			WND_DATA& SubWndData = pMagnet->m_SubWndProc.GetAt(WndDataPos);
			RECT rcSubWnd = {0};
			ATTCH_INFO AttchInfo = pMagnet->CalcAttchMode(hWnd, &rcSubWnd);
			if (AttchInfo.AttchMode != ATTCH_MODE_NULL && AttchInfo.AttchAlign != ATTCH_ALIGN_NULL)
			{
				SubWndData.bNewPos = TRUE;
				pMagnet->UpdateSubWndPos(hWnd, &AttchInfo);
				SubWndData.bNewPos = FALSE;

				SubWndData.AttchInfo.AttchMode = AttchInfo.AttchMode;
				SubWndData.AttchInfo.AttchAlign = AttchInfo.AttchAlign;
				SubWndData.bSubWnd = TRUE;
			}
			else
			{
				SubWndData.bSubWnd = FALSE;
			}
			WND_DATA& SubWndData2 = pMagnet->m_SubWndProc.GetAt(WndDataPos);
			BOOL bWnd = SubWndData2.bSubWnd;
		}
	}
	
	if (WndData.pProc != NULL)
		return WndData.pProc(hWnd, uMsg, wParma, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParma, lParam);
}