#include "StdAfx.h"
#include "3DView.h"

C3DViewUI::C3DViewUI()
{
	m_hBmpOrig = NULL;
	m_hBmpTrans = NULL;
	m_bPlay = FALSE;
	memset(&m_3dParam,0,sizeof(m_3dParam));
}

LPCTSTR C3DViewUI::GetClass() const
{
	return _T("3DViewUI");
}

LPVOID C3DViewUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcsicmp(pstrName, _T("3DView")) == 0)	 return static_cast<C3DViewUI*>(this);
	else
		return CVerticalLayoutUI::GetInterface(pstrName);
}

PARAM3DTRANSFORM& C3DViewUI::Get3dParam()
{
	return m_3dParam;
}

void C3DViewUI::StartEffect(BOOL bStart)
{
	m_bPlay = bStart;
	if (m_bPlay != FALSE)
	{
		m_hMemDC = ::CreateCompatibleDC(m_pManager->GetPaintDC());
		m_hBmpTrans = CRenderEngine::GenerateBitmap(m_pManager, this, m_rcItem); 
		SelectObject(m_hMemDC, m_hBmpTrans);
	}
}

void C3DViewUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrName, _T("rotatex")) == 0)				SetRotateX(_ttoi(pstrValue));
	else if (_tcsicmp(pstrName, _T("rotatey")) == 0)		SetRotateY(_ttoi(pstrValue));
	else if (_tcsicmp(pstrName, _T("rotatez")) == 0)		SetRotateZ(_ttoi(pstrValue));
	else if (_tcsicmp(pstrName, _T("offsetz")) == 0)		SetOffsetZ(_ttoi(pstrValue));
	else
		CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
}

void C3DViewUI::DoPaint(HDC hDC,const RECT& rcPaint)
{
	if (m_bPlay == FALSE)
	{
		CVerticalLayoutUI::DoPaint(hDC,rcPaint);
		return;
	}
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
	//CMemDC memdc(dc,m_hBmpTrans);
	
	CDuiRect rcClient = GetPos();
	//GetClient(&rcClient);
	BLENDFUNCTION bf={AC_SRC_OVER,0,255,AC_SRC_ALPHA};
	AlphaBlend(hDC, rcClient.left, rcClient.top, rcClient.GetWidth(), rcClient.GetHeight(), m_hMemDC, 0, 0, rcClient.GetWidth(), rcClient.GetHeight(),bf);
}

void C3DViewUI::SetBmpOrig(HBITMAP hBmpOrig) 
{
	m_hBmpOrig = hBmpOrig; 
}

void C3DViewUI::Update()
{
	if (m_hBmpOrig == NULL ||
		m_hBmpTrans == NULL)
		return;

	C3DTransform Image3d;
	BITMAP bmSour,bmDest;
	GetObject(m_hBmpOrig, sizeof(BITMAP), &bmSour);
	GetObject(m_hBmpTrans, sizeof(BITMAP), &bmDest);
	Image3d.SetImage((LPBYTE)bmSour.bmBits,(LPBYTE)bmDest.bmBits,bmSour.bmWidth,bmSour.bmHeight,bmSour.bmBitsPixel);
	Image3d.Render(m_3dParam);

	Invalidate();
}

void C3DViewUI::SetRotateX(int nRotateX)
{
	m_3dParam.nRotateX = nRotateX;
}

void C3DViewUI::SetRotateY(int nRotateY)
{
	m_3dParam.nRotateY = nRotateY;
}

void C3DViewUI::SetRotateZ(int nRotateZ)
{
	m_3dParam.nRotateZ = nRotateZ;
}

void C3DViewUI::SetOffsetZ(int nOffsetZ)
{
	m_3dParam.nOffsetZ = nOffsetZ;
}