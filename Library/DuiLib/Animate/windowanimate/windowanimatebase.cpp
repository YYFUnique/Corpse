#include "stdafx.h"
#include "windowanimatebase.h"
#include "UISDK\Kernel\Src\Animate\storyboard.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\layeredanimatewindow.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

namespace UI
{

WindowAnimateBase::WindowAnimateBase()
{
    m_pSrcBits = NULL;
    m_nSrcPitch = 0;
    m_nSrcWndWidth = 0;
    m_nSrcWndHeight = 0;
    m_pWindow = NULL;
    m_pAnimateMgr = NULL;
	m_rcWindowInBuffer.SetRectEmpty();

    m_pLayeredWindow = NULL;
    m_pIWindowAnimateBase = NULL;

    m_nWndTranslateType = WINDOW_TRANSPARENT_MODE_NORMAL;
    m_pStroyboard = NULL;
}
WindowAnimateBase::~WindowAnimateBase() 
{
    if (m_pLayeredWindow)
    {
        ILayeredAnimateWindow*  pI = m_pLayeredWindow->GetILayeredAnimateWindow();
        SAFE_DELETE_Ixxx(pI);
        m_pLayeredWindow = NULL;
    }
    SAFE_DELETE(m_pStroyboard);
};


void  WindowAnimateBase::UpdateWindowSourceImage()
{
    if (NULL == m_pWindow)
        return;
 
    if (m_nWndTranslateType & WINDOW_TRANSPARENT_MODE_AERO || 
        m_nWndTranslateType & WINDOW_TRANSPARENT_MODE_LAYERED)
        GetSrcBitmap_Layered();
    else
        GetSrcBitmap_Normal();
}

void WindowAnimateBase::SetAnimateWindow(IWindowBase* pWindow)
{
	if (NULL == pWindow)
		return;

    m_pWindow = pWindow->GetImpl();
    m_nWndTranslateType = UISendMessage(m_pWindow, UI_WM_GET_WINDOW_TRANSPARENT_MODE);
}
void  WindowAnimateBase::Initialize()
{
	CRect rcWndOrigin;
	::GetWindowRect(m_pWindow->GetHWND(), &rcWndOrigin);
	m_nSrcWndWidth  = rcWndOrigin.Width();
	m_nSrcWndHeight = rcWndOrigin.Height();

	// ��ȡ��������ʱ�Ĵ��ڻ��棬ʹ�þ�ֹ������ж�����ʾ
	this->UpdateWindowSourceImage();

	// ���ö�������
    ILayeredAnimateWindow*  pILayeredAnimateWindow = NULL;
    ILayeredAnimateWindow::CreateInstance(m_pWindow->GetUIApplication(), &pILayeredAnimateWindow);   

	m_pLayeredWindow = pILayeredAnimateWindow->GetImpl();
    m_pLayeredWindow->SetWindowAnimate(this);
	m_pLayeredWindow->Soul_Attach(m_pWindow);

	// ��ʾ�������� -- ������ʾ����Ϊ�п��ܵ�ǰΪ���������������ڶ�������ʱ�����п����Ѿ�
	//                 ��ת��90�ȣ���һ��ʾԭ���ھͻ���
// 	HDC hMemDC = m_SrcImage.BeginDrawToMyself();
// 	::BitBlt(m_pLayeredWindow->m_hLayeredMemDC, 0,0, m_rcWndOrigin.Width(), m_rcWndOrigin.Height(), hMemDC, 0,0, SRCCOPY);
// 	m_SrcImage.EndDrawToMyself();
// 	m_pLayeredWindow->Commit2LayeredWindow();
}

// ע����Щʱ�򣬲���������Ҫ�ڶ�����ʼʱ��ȡһ�´��ڿ��գ��ڶ���������Ҳ�ø��´��ڿ���
//     �����flash�����Ĵ��ڷ�ת
void WindowAnimateBase::GetSrcBitmap_Layered()
{
    // TODO: δ���Ƕ��������д��ڱ������
    if (m_SrcImage.IsNull())
    {
        // +1 �����ڽ���ڶ������Բ�ֵ����������⣬���ӱ߽��ڱ�
        int nSrcImageW = m_nSrcWndWidth + 2;
        int nSrcImageH = m_nSrcWndHeight + 2;
        m_SrcImage.Create(nSrcImageW, -nSrcImageH, 32, Image::createAlphaChannel);

        m_pSrcBits  = (BYTE*)m_SrcImage.GetBits();
        m_nSrcPitch = m_SrcImage.GetPitch();

		m_rcWindowInBuffer.SetRect(0, 0, m_nSrcWndWidth, m_nSrcWndHeight);
		::OffsetRect(&m_rcWindowInBuffer, 1, 1);
    }
	
	HDC hMemDC = m_SrcImage.BeginDrawToMyself();
    RECT rc = {0,0, m_nSrcWndWidth, m_nSrcWndHeight};
	::OffsetRect(&rc, 1, 1);
    m_pWindow->DrawMemBitmap(hMemDC, &rc, false);
	m_SrcImage.EndDrawToMyself();
}
void WindowAnimateBase::GetSrcBitmap_Normal()
{
	if (NULL == m_pWindow)
		return;

	int nSrcImageW = m_nSrcWndWidth + 2;
	int nSrcImageH = m_nSrcWndHeight + 2;

    // TODO: δ���Ƕ��������д��ڱ������
    if (m_SrcImage.IsNull())
    {
	    // +1 �����ڽ���ڶ������Բ�ֵ�����������
	    m_SrcImage.Create(nSrcImageW, -nSrcImageH, 32, Image::createAlphaChannel);
    
        m_pSrcBits  = (BYTE*)m_SrcImage.GetBits();
        m_nSrcPitch = m_SrcImage.GetPitch();

		m_rcWindowInBuffer.SetRect(0, 0, nSrcImageW, nSrcImageH);
		::OffsetRect(&m_rcWindowInBuffer, 1, 1);
    }

	HDC hMemDC = m_SrcImage.BeginDrawToMyself();
#if 0 // (_WIN32_WINNT >= 0x0501)
	::PrintWindow(m_pWindow->GetHWND(), hMemDC, 0); // ������ͨ���ڣ������ᵼ��Ҫ������ʾ�����Ĵ�����ʾ����
#else
    RECT rc = {0,0, m_nSrcWndWidth, m_nSrcWndHeight};
    m_pWindow->DrawMemBitmap(hMemDC, &rc, false);
	//::BitBlt( hMemDC, 0,0, rc.right-rc.left,rc.bottom-rc.top, hWindowDC, 0,0, SRCCOPY ); // <-- �����ڱ��ڸ�ʱ�޷���ͼ
#endif
	m_SrcImage.EndDrawToMyself();

	// ��ȡ��ǰ���ڵ������������ڻ��Ʒֲ㴰��ʱ��͸��
	HRGN hRgnWindow = ::CreateRectRgn(0,0,0,0);
	HRGN hRgnTransparent = NULL;

	int nRet = GetWindowRgn(m_pWindow->GetHWND(), hRgnWindow);
	bool bFullWindow = false;
	if (NULLREGION == nRet || ERROR == nRet)
	{
		bFullWindow = true;
	}
	else
	{
		hRgnTransparent = ::CreateRectRgn(0,0, m_nSrcWndWidth, m_nSrcWndHeight);
		::CombineRgn(hRgnTransparent, hRgnTransparent, hRgnWindow,RGN_DIFF);
	}
	SAFE_DELETE_GDIOBJECT(hRgnWindow);

	// ��ȡλͼ����   
	LPBYTE lpbits = (LPBYTE)m_SrcImage.GetBits();
	int    nPitch = m_SrcImage.GetPitch();

	// ͨ����������ͼƬ���أ���λ�ڴ��������������alphaֵ��Ϊȫ͸��
	for (LONG y = 0; y < nSrcImageH ; y++)   
	{
		for (LONG x = 0; x < nSrcImageW; x++)      
		{
			if (bFullWindow || !::PtInRegion(hRgnTransparent, x, y))
			{
				((DWORD*)lpbits)[x] |= 0xFF000000;
			}
			else
			{
				((DWORD*)lpbits)[x] = 0;
			}
		}
		lpbits += nPitch;
	}

	SAFE_DELETE_GDIOBJECT(hRgnTransparent);
}

}
