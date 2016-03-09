#include "stdafx.h"
#include "windowupdownalphashowanimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\layeredanimatewindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"
#include "UISDK\Kernel\Src\Util\dwm\dwmhelper.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"

namespace UI
{

WindowUpDownAlphaShowAnimate::WindowUpDownAlphaShowAnimate()
{
    m_pIWindowUpDownAlphaShowAnimate = NULL;
    m_nStartLine = 0;
    m_nScanlineRange = 200;
    m_bShow = false;
    m_pAeroWindow = NULL;
    m_rcBlurRegion.SetRectEmpty();
}

WindowUpDownAlphaShowAnimate::~WindowUpDownAlphaShowAnimate()
{
    if (m_bShow)
    {

    }
    else
    {
        ShowWindow(m_pLayeredWindow->m_hHostWnd, SW_HIDE);
    }


    if (m_pAeroWindow)
    {
        // 还原Aero参数 
        m_pAeroWindow->UpdateRgn();
        m_pAeroWindow = NULL;
    }
}

void  WindowUpDownAlphaShowAnimate::Initialize()
{
    m_pWindow->GetWindowRender()->SetCanCommit(false);
    ::ShowWindow(m_pWindow->m_hWnd, SW_SHOW);
    m_pWindow->PaintWindow(NULL);
    m_pWindow->GetWindowRender()->SetCanCommit(true);

    if (m_nWndTranslateType & WINDOW_TRANSPARENT_MODE_AERO)
    {
        DwmHelper*  pDwm = DwmHelper::GetInstance();
        if (pDwm->IsEnable())
        {
            IID iid = uiiidof(IAeroWindowWrap);
            m_pAeroWindow = (IAeroWindowWrap*)UISendMessage(m_pWindow, UI_WM_QUERYINTERFACE, (WPARAM)&iid);
            if (m_pAeroWindow)
            {
                if (AERO_MODE_TRANSPARENT == m_pAeroWindow->GetAeroMode())
                {
                    m_pAeroWindow = NULL;
                }
                else
                {
                    m_pAeroWindow->GetBlurRegion(&m_rcBlurRegion);
                }
            }
        }
        
    }
    __super::Initialize();

    
}

void  WindowUpDownAlphaShowAnimate::OnTick(int nCount, IStoryboard** ppTimerArray)
{
    int nCurY = 0;
    ppTimerArray[0]->FindTimeline(0)->GetCurrentValue((void*)&nCurY);

    byte* pSrc = (byte*)m_SrcImage.GetBits();
    byte* p = m_pLayeredWindow->m_pBits;
    int nBytesPerLine = m_pLayeredWindow->m_nWidth<<2;

	pSrc += m_rcWindowInBuffer.top * m_nSrcPitch;
	pSrc += m_rcWindowInBuffer.left * 4;

    // 1. 不需要处理的部分
    if (nCurY > 0)
    {
        for (int y = 0; y < nCurY; y++)
        {
            memcpy(p, pSrc, m_pLayeredWindow->m_nWidth<<2);

            pSrc += m_SrcImage.GetPitch();
            p += m_pLayeredWindow->m_nPitch;
        }
    }


    // 2. 需要渐变的部分，每一行减1个alpha比例，直到减够255行
    int alpha = 0;
    int y = 0;

    if (nCurY > 0)  // nCurY小于0表示hide的最后阶段或者show的开始
    {
        alpha = 255;
        y = nCurY;
    }
    else
    {
        alpha = 255 + nCurY;
        y = 0;
    }

    for (; y < m_pLayeredWindow->m_nHeight && alpha > 0; y++, alpha--)
    {
        for (int x = 0; x < nBytesPerLine; x++)
        {
            p[x] = pSrc[x]*alpha >> 8;
        }
        
        pSrc += m_SrcImage.GetPitch();
        p += m_pLayeredWindow->m_nPitch;
    }

    // 3. 剩下的部分表示不显示了
    for (; y < m_pLayeredWindow->m_nHeight; y++)
    {
        memset(p, 0, nBytesPerLine);

        pSrc += m_SrcImage.GetPitch();
        p += m_pLayeredWindow->m_nPitch;
    }
    
    if (m_pAeroWindow)  // 说明是aero透明窗口
    {
        CRect  rcClient;
        m_pWindow->GetClientRect(&rcClient);
        rcClient.DeflateRect(m_rcBlurRegion.left, m_rcBlurRegion.top, m_rcBlurRegion.right, m_rcBlurRegion.bottom);

        int y = nCurY + 255;
        if (y < 0)
            y = 0;
        if (y > rcClient.bottom)
            y = rcClient.bottom;
        rcClient.bottom = y;

        HRGN hRgn = CreateRectRgnIndirect(&rcClient);
        DWM_BLURBEHIND blurbehind = {0};
        blurbehind.dwFlags = DWM_BB_ENABLE|DWM_BB_BLURREGION;
        blurbehind.hRgnBlur = hRgn;
        blurbehind.fEnable = TRUE;
        DwmHelper::GetInstance()->pDwmEnableBlurBehindWindow(m_pWindow->m_hWnd, &blurbehind);
        DeleteObject(hRgn);
    }
    m_pLayeredWindow->Commit2LayeredWindow();
}

bool  WindowUpDownAlphaShowAnimate::Show(int nId, int nDuration)
{
    return DoAction(nId, nDuration, true);
}
bool  WindowUpDownAlphaShowAnimate::Hide(int nId, int nDuration)
{
    return DoAction(nId, nDuration, false);
}

bool  WindowUpDownAlphaShowAnimate::DoAction(int nId, int nDuration, bool bShow)
{
    if (NULL == m_pAnimateMgr || NULL == m_pWindow)
        return false;

    int nHeight = m_pWindow->GetHeight();
    if (0 == nHeight)
        return false;

    IIntTimeline*  pTimeline = CreateIntTimeline(0);
    if (NULL == pTimeline)
        return false;

    m_bShow = bShow;
    if (bShow)
    {
        pTimeline->SetLinerParam1(-255, nHeight, nDuration); // 第一次响应OnTick的时候可能不是-255，例如-37，导致已经显示了很大一截了。因此将From设大一些
    }
    else
    {
        pTimeline->SetLinerParam1(nHeight-128, -255, nDuration);
    }

    m_nStartLine = m_pLayeredWindow->m_nHeight-1;
    m_SrcImage.BitBlt(m_pLayeredWindow->m_hLayeredMemDC,0,0, m_pLayeredWindow->m_nWidth, m_pLayeredWindow->m_nHeight, 0 , 0);


    this->SetId(nId);
    this->SetNotifyObj(m_pWindow->GetIMessage());
    this->BeginBlock();
    return true;
}

}