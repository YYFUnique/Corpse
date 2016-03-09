#include "stdafx.h"
#include "layeredanimatewindow.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowanimatebase.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(ILayeredAnimateWindow, LayeredAnimateWindow, Message);

LayeredAnimateWindow::LayeredAnimateWindow()
{
    m_pWindowAnimate = NULL;
    m_lOldExStyle = -1;
    m_pILayeredAnimateWindow = NULL;
}
LayeredAnimateWindow::~LayeredAnimateWindow()
{
    // ��ԭ�ֲ�����
    if (-1 != m_lOldExStyle)
    {
        SetWindowLong(m_hHostWnd, GWL_EXSTYLE, m_lOldExStyle);

        // ���ȡ���ֲ㴰�����Ժ󣬴��ڱ�ڵ�����
        HDC hDC = ::GetDC(m_hHostWnd);
        RECT rc = {0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight()};
        m_pWindow->DrawMemBitmap(hDC, &rc, false);
        ReleaseDC(m_hHostWnd, hDC);
    }

    //	m_pWindow->UpdateObject(); // �п����Ƕ����ĵ�һ����������˲���������
}
void LayeredAnimateWindow::Soul_Attach(WindowBase* pWnd)
{
    if (NULL == pWnd)
        return;

    m_pWindow = pWnd;
    m_hHostWnd = pWnd->GetHWND();

    // ��ֹ���������������������Ӱ�춯����ʾ
    m_pWindow->GetWindowRender()->SetCanCommit(false);

    pWnd->AddHook(this->GetIMessage(), 0, 0);

    LONG lStyleEx = ::GetWindowLong(m_hHostWnd, GWL_EXSTYLE);
    if (m_pWindowAnimate->GetWndTransparentType() & WINDOW_TRANSPARENT_MODE_LAYERED)
    {

    }
    else
    {
        m_lOldExStyle = lStyleEx;
        SetWindowLong(m_hHostWnd, GWL_EXSTYLE, WS_EX_LAYERED | lStyleEx);
        InitLayered();
    }
    ReSize();

    // 	LONG lStyleEx = GetWindowLong(hWnd, GWL_EXSTYLE);
    // 	if (lStyleEx & WS_EX_LAYERED)
    // 	{
    // 
    // 	}
    // 	else
    // 	{
    // 
    // 	}
    // 	
    // 	m_thunk.Init((WNDPROC)LayeredAnimateWindow::TempWndProc, this);
    // 	m_pOldProc = (WNDPROC)::GetWindowLong(hWnd, GWL_WNDPROC);
    // 	::SetWindowLong(hWnd, GWL_WNDPROC, (LONG)m_thunk.GetWNDPROC());
}
// LRESULT LayeredAnimateWindow::TempWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
// 	LayeredAnimateWindow* pThis = (LayeredAnimateWindow*)hWnd;
// 	return 0;
// }

// �ڶ�������������������Ϣ�������ж��������Ϣ
BOOL LayeredAnimateWindow::nvProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
{
    if (pMsg->message <= WM_MOUSEFIRST && pMsg->message >= WM_MOUSELAST)
        return TRUE;
    if (pMsg->message <= WM_KEYFIRST && pMsg->message >= WM_KEYLAST)
        return TRUE;

    switch (pMsg->message)
    {
    case UI_WM_ANIMATE_TICK:
        {
            int nCount = pMsg->wParam;
            IStoryboard** ppStoryboardArray = (IStoryboard**)pMsg->lParam;

            m_pWindowAnimate->OnTick(nCount, ppStoryboardArray);
            this->Commit2LayeredWindow();

            // ������������ˣ��򾡿��ͷŶԴ�����Ϣ�����ء����ⴰ������ӦTick Finishʱ�����µĶ�����
            // ����ܶ���Ϣ�������ˣ���QueryInterface IID_ICustomWindow��ȡ����͸�����ͣ�
            
            bool bAllFinish = true;
            for (int i = 0; i < nCount; i++)
            {
                if (!ppStoryboardArray[i]->IsFinish())
                {
                    bAllFinish = false;
                    break;
                }
            }
            if (bAllFinish)
            {
                m_pWindow->RemoveHook(this->GetIMessage());
                m_pWindow->GetWindowRender()->SetCanCommit(true);
            }
            return FALSE;
        }
        break;

    case UI_WM_ANIMATE_OVER:
        {
            return FALSE;
        }
        break;

        // �п��ܶ���ʱ������һЩ������ˢ�£�������������Ϣ�����´��ڻ��治����
//     case WM_ERASEBKGND:
//     case WM_PAINT:
//         return FALSE;
    }
    return FALSE;
}


}