#include "stdafx.h"
#include "Resource.h"
#include "UIAnimation.h"

#pragma comment(lib,"Msimg32.lib")

#define UI_FEATUREEFFECT_CONTROL		_T("VLayoutTotal")
#define WM_SHOW									WM_USER + 0x1000

CAnimationUI::CAnimationUI()
{
	m_pDraw = NULL;

	m_pEffect = NULL;
}

CAnimationUI::~CAnimationUI()
{
	if (m_pDraw)
		delete m_pDraw;
	if (m_pEffect)
		ReleaseAnimation(m_pEffect);
}

LPCTSTR CAnimationUI::GetWindowClassName() const
{
	return _T("Animation");
}

UILIB_RESOURCETYPE CAnimationUI::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CAnimationUI::GetZIPFileName() const
{
	return _T("Animaion.zip");
}

CDuiString CAnimationUI::GetSkinFile()
{
	return _T("QQLogin.xml");
}

CDuiString CAnimationUI::GetSkinFolder()
{
	return _T("Animation");
}

void CAnimationUI::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
// 	BOOL bHandle = FALSE;
// 	
// 	SetTimer(m_hWnd,1000,1,NULL);
	//OnSysCommand(WM_SYSCOMMAND,SC_MINIMIZE,0,bHandle);
	/*OnShow(NULL,NULL);*/
// 	SetWindowPos(m_hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
// 	AnimateWindow(m_hWnd,200,AW_VER_POSITIVE|AW_SLIDE);

	//AnimateWindow(m_hWnd,1000,AW_BLEND); 
}

void CAnimationUI::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	/*else if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		OnShow(NULL,NULL);*/
	else if (msg.sType == DUI_MSGTYPE_SHOWACTIVEX)
		OnShowActiveX(msg);
}

LRESULT CAnimationUI::OnShow(WPARAM wParam,LPARAM lParam)
{
	KillTimer(m_hWnd,1000);
	if (m_pEffect == NULL)
		m_pEffect = GetAnimation();

	CControlUI* pControl = m_PaintManager.FindControl(UI_FEATUREEFFECT_CONTROL);

	RECT rcWnd;
	GetWindowRect(m_hWnd,&rcWnd);
	CDrawingBoard DrawPaint;
	RECT rcItem = {0,0,rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};
	DrawPaint.Create(rcItem.right,rcItem.bottom,false,true);
	m_PaintManager.GetRoot()->DoPaint(DrawPaint.GetSafeHdc(),rcItem);

	RECT rcPos = pControl->GetPos();
	if (m_pDraw == NULL)
		m_pDraw = new CDrawingBoard;

	m_pDraw->Create(rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,false,true);

	BLENDFUNCTION BlendFunction = {AC_SRC_OVER,0,255,AC_SRC_ALPHA};

	AlphaBlend(m_pDraw->GetSafeHdc(),0,0,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
		DrawPaint.GetSafeHdc(),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
		BlendFunction);

	AnimationParam Param;
	ZeroMemory(&Param,sizeof(AnimationParam));
	Param.effectKey = (WPARAM)pControl;
	Param.bShow = TRUE;
	Param.animationEffect = 38;
	Param.animationFrequency = 35;
	Param.hdc = m_pDraw->GetSafeHdc();
	Param.bmpSize = m_pDraw->GetDcSize();
	Param.hBitmap = m_pDraw->GetBmpHandle();
	Param.pBmpData = m_pDraw->GetBits();

	m_pEffect->AppendAnimation(Param);

	m_pEffect->Animation(this,-1);

	return TRUE;
}

LRESULT CAnimationUI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bHandle = TRUE;
	switch (uMsg)
	{
	case WM_TIMER:
				bHandle = OnShow(wParam,lParam);
			break;
		default:
			bHandle = WindowImplBase::HandleMessage(uMsg,wParam,lParam);
	}
	
	return bHandle;
}

void CAnimationUI::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnClose")))
	{
		//PostQuitMessage(0);
		if (m_pEffect == NULL)
			m_pEffect = GetAnimation();

		CControlUI* pControl = m_PaintManager.FindControl(UI_FEATUREEFFECT_CONTROL);

		RECT rcWnd;
		GetWindowRect(m_hWnd,&rcWnd);
		CDrawingBoard DrawPaint;
		RECT rcItem = {0,0,rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};
		DrawPaint.Create(rcItem.right,rcItem.bottom,false,true);
		m_PaintManager.GetRoot()->DoPaint(DrawPaint.GetSafeHdc(),rcItem);

		RECT rcPos = pControl->GetPos();
		if (m_pDraw == NULL)
			m_pDraw = new CDrawingBoard;

		m_pDraw->Create(rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,false,true);

		BLENDFUNCTION BlendFunction = {AC_SRC_OVER,0,255,AC_SRC_ALPHA};

		AlphaBlend(m_pDraw->GetSafeHdc(),0,0,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
			DrawPaint.GetSafeHdc(),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
			BlendFunction);


		AnimationParam Param;
		ZeroMemory(&Param,sizeof(AnimationParam));
		Param.effectKey = (WPARAM)pControl;
		Param.bShow = FALSE;
		Param.animationEffect = 37;
		Param.animationFrequency = 35;
		Param.hdc = m_pDraw->GetSafeHdc();
		Param.bmpSize = m_pDraw->GetDcSize();
		Param.hBitmap = m_pDraw->GetBmpHandle();
		Param.pBmpData = m_pDraw->GetBits();

		m_pEffect->AppendAnimation(Param);

		m_pEffect->Animation(this,-1);
	}
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnMin")))
		SendMessage(WM_SYSCOMMAND,SC_MINIMIZE, 0);
	else if (msg.pSender == (CButtonUI*)m_PaintManager.FindControl(_T("BtnSet")))
	{
		if (m_pEffect == NULL)
			m_pEffect = GetAnimation();

		CControlUI* pControl = m_PaintManager.FindControl(UI_FEATUREEFFECT_CONTROL);

		RECT rcWnd;
		GetWindowRect(m_hWnd,&rcWnd);
		CDrawingBoard DrawPaint;
		RECT rcItem = {0,0,rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};
		DrawPaint.Create(rcItem.right,rcItem.bottom,false,true);
		m_PaintManager.GetRoot()->DoPaint(DrawPaint.GetSafeHdc(),rcItem);

		RECT rcPos = pControl->GetPos();
		if (m_pDraw == NULL)
			m_pDraw = new CDrawingBoard;

		m_pDraw->Create(rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,false,true);
		
		BLENDFUNCTION BlendFunction = {AC_SRC_OVER,0,255,AC_SRC_ALPHA};

		AlphaBlend(m_pDraw->GetSafeHdc(),0,0,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
							DrawPaint.GetSafeHdc(),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
							BlendFunction);


		AnimationParam Param;
		ZeroMemory(&Param,sizeof(AnimationParam));
		Param.effectKey = (WPARAM)pControl;
		Param.bShow = FALSE;
		Param.animationEffect = 2;
		Param.animationFrequency = 20;
		Param.hdc = m_pDraw->GetSafeHdc();
		Param.bmpSize = m_pDraw->GetDcSize();
		Param.hBitmap = m_pDraw->GetBmpHandle();
		Param.pBmpData = m_pDraw->GetBits();
		
		m_pEffect->AppendAnimation(Param);

		m_pEffect->Animation(this,-1);
	}
}

void CAnimationUI::OnShowActiveX(TNotifyUI& msg)
{
	if( msg.pSender->GetName() == _T("flashActiveX") )
	{
		IShockwaveFlash* pFlash = NULL;
		CActiveXUI* pActiveX = static_cast<CActiveXUI*>(msg.pSender);

		pActiveX->GetControl(__uuidof(IShockwaveFlash), (void**)&pFlash);

		if( pFlash != NULL )
		{
			pFlash->put_WMode( _bstr_t(_T("Transparent") ) );
			pFlash->put_Movie( _bstr_t(CPaintManagerUI::GetInstancePath() + _T("\\Animation\\Flash\\noon.swf")) );
			pFlash->DisableLocalSecurity();
			pFlash->put_AllowScriptAccess(L"always");

			BSTR request,response;
			request = SysAllocString(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>");
			response = SysAllocString(L"");
			pFlash->CallFunction(request, &response);
			SysFreeString(request);
			SysFreeString(response);
		}
	}
}

void CAnimationUI::OnUiEffectBegin(WPARAM effectKey, DWORD animaType)
{
	m_nCount = 0;
	CControlUI* pControl = (CControlUI*)effectKey;
	if (pControl)
		pControl->SetVisible(false);
}

void CAnimationUI::OnUiEffectEnd(WPARAM effectKey, DWORD animaType)
{
	m_nCount = 0;

	if (animaType == 37)
		PostQuitMessage(0);
	else
	{
		CControlUI* pControl = (CControlUI*)effectKey;
		if (pControl)
			pControl->SetVisible(true);
	}
}

void CAnimationUI::OnUiEffectDraw()
{
	HDC hDc = m_PaintManager.GetPaintDC();
	CControlUI* pControl = m_PaintManager.FindControl(UI_FEATUREEFFECT_CONTROL);
	RECT rcPos = pControl->GetPos();
	BLENDFUNCTION bf={AC_SRC_OVER,0,255,AC_SRC_ALPHA};

	RECT rcWnd;
	GetWindowRect(m_hWnd,&rcWnd);

	CDrawingBoard WndMemDc;
	WndMemDc.Create(rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top,false,true);

	RECT rcItem = {0,0,rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};
	if (pControl != m_PaintManager.GetRoot())
		m_PaintManager.GetRoot()->DoPaint(WndMemDc.GetSafeHdc(),rcItem);

	CRenderEngine::SaveBitmapFile(m_pDraw->GetBmpHandle(),_T("C:\\Show.bmp"));

	AlphaBlend(WndMemDc.GetSafeHdc(),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,
		m_pDraw->GetSafeHdc(),0,0,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,bf);

	CRenderEngine::SaveBitmapFile(WndMemDc.GetBmpHandle(),_T("C:\\WndMemDc.bmp"));

	POINT ptWinPos = {rcWnd.left, rcWnd.top};
	POINT ptSrc = {0, 0};
	SIZE sizeWindow = {rcWnd.right-rcWnd.left,rcWnd.bottom-rcWnd.top};

	::UpdateLayeredWindow(m_hWnd, hDc, &ptWinPos, &sizeWindow, WndMemDc.GetSafeHdc(), &ptSrc, 0, &bf, ULW_ALPHA);

	WndMemDc.Delete();
}