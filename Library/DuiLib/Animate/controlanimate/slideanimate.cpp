#include "stdafx.h"
#include "slideanimate.h"
#include "UISDK/Kernel/Src/Animate/animatemgr.h"
#include "UISDK/Kernel/Inc/Interface/iobject.h"

namespace UI
{

SlideAnimate::SlideAnimate()
{
    m_pISlideAnimate = NULL;
    m_pWindow = NULL;
    m_rcCommit.SetRectEmpty();
    m_pObjLeft = m_pObjRight = NULL;
}
SlideAnimate::~SlideAnimate()
{

}

bool  SlideAnimate::Slide(IObject* pObjLeft, IObject* pObjRight, CRect* prcCommitInWindow, int nFlags)
{
    if (NULL == pObjLeft || NULL == pObjRight || NULL == prcCommitInWindow)
        return false;

    m_pObjLeft = pObjLeft;
    m_pObjRight = pObjRight;
    m_pWindow = pObjLeft->GetImpl()->GetWindowObject();
    m_rcCommit = *prcCommitInWindow;
    m_nFlags = nFlags;

    HBITMAP hBitmap1 = pObjLeft->TakeSnapshot();
    HBITMAP hBitmap2 = pObjRight->TakeSnapshot();
    if (NULL == hBitmap1 || NULL == hBitmap2)
    {
        if (hBitmap1)
            DeleteObject(hBitmap1);
        if (hBitmap2)
            DeleteObject(hBitmap2);

        return false;
    }
    HBITMAP hBitmapBkgnd = pObjLeft->TakeBkgndSnapshot();

    m_imageObjLeft.Attach(hBitmap1, Image::DIBOR_BOTTOMUP);
    m_imageObjRight.Attach(hBitmap2, Image::DIBOR_BOTTOMUP);
    m_imageObjBkgnd.Attach(hBitmapBkgnd, Image::DIBOR_BOTTOMUP);

    IIntTimeline* pTimeline = CreateIntTimeline(0);
	pTimeline->SetEaseParam(0, prcCommitInWindow->Width(), 300, UI::ease);
    this->SetWParam((WPARAM)this);

    static SlideAnimateMessageHandler _Handler;
    this->SetNotifyObj(_Handler.GetIMessage());

    this->Begin();
    
    return true;
}

void  SlideAnimateMessageHandler::OnAnimateTick(int nCount, IStoryboard** ppArray)
{
    for (int i = 0; i < nCount; i++)
    {
        IStoryboard* p = ppArray[i];
        ITimeline* pTimeline = p->FindTimeline(0);
        if (NULL == pTimeline)
            continue;
        
        SlideAnimate* pSlideAnimate = (SlideAnimate*)p->GetWParam();
        if (pSlideAnimate)
        {
            int nValue = 0;
            pTimeline->GetCurrentValue(&nValue);
            pSlideAnimate->OnAnimateTick(nValue);

            if (pSlideAnimate->IsFinish())
            {
                pSlideAnimate->OnAnimateOver();
            }
        }
    }
}

void  SlideAnimate::OnAnimateTick(int nValue)
{
    UIASSERT(0);// todo
#if 0
    if (!m_pObjLeft)
        return;

    HDC hMemDC = m_pObjLeft->GetRenderLayer()->GetMemoryLayerDC();

    HRGN hRgn = CreateRectRgnIndirect(&m_rcCommit);
    ::SelectClipRgn(hMemDC, hRgn);

    ::FillRect(hMemDC, &m_rcCommit, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    POINT ptOffset = {m_rcCommit.left, m_rcCommit.top};

    if (m_nFlags == SLIDE_LEFT2RIGHT)
    {
        ptOffset.x += nValue-m_rcCommit.Width();
    }
    else if (m_nFlags == SLIDE_RIGHT2LEFT)
    {
        ptOffset.x -= nValue;
    }

    m_imageObjBkgnd.Draw(hMemDC, m_rcCommit.left, m_rcCommit.top, m_imageObjLeft.GetWidth(), m_imageObjLeft.GetHeight(), 
        0, 0, m_imageObjLeft.GetWidth(), m_imageObjLeft.GetHeight());

    ::SetViewportOrgEx(hMemDC, ptOffset.x, ptOffset.y, NULL);
    
    m_imageObjLeft.Draw(hMemDC, 0, 0, m_imageObjLeft.GetWidth(), m_imageObjLeft.GetHeight(), 
        0, 0, m_imageObjLeft.GetWidth(), m_imageObjLeft.GetHeight());
    m_imageObjRight.Draw(hMemDC, m_rcCommit.Width(), 0, m_imageObjRight.GetWidth(), m_imageObjRight.GetHeight(), 
        0, 0, m_imageObjRight.GetWidth(), m_imageObjRight.GetHeight());

    ::SelectClipRgn(hMemDC, NULL);
    ::SetViewportOrgEx(hMemDC, 0, 0, NULL);

    m_pWindow->GetRenderChain()->CombineAllLayer(hRgn);
    DeleteObject(hRgn);
    m_pWindow->CommitDoubleBuffet2Window(NULL, NULL, &m_rcCommit, 1);
#endif
}
void  SlideAnimate::OnAnimateOver()
{
    if (m_nFlags == SLIDE_RIGHT2LEFT)
    {
        if (m_pObjLeft)
        {
            m_pObjLeft->SetVisible(false, false, false);
        }
        if (m_pObjRight)
        {
            m_pObjRight->SetVisible(true, false, false);
        }
    }
    else if (m_nFlags == SLIDE_LEFT2RIGHT)
    {
        if (m_pObjLeft)
        {
            m_pObjRight->SetVisible(false, false, false);
        }
        if (m_pObjRight)
        {
            m_pObjLeft->SetVisible(true, false, false);
        }
    }
}

}