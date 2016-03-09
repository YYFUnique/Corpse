#include "stdafx.h"
#include "window3danimate.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\layeredanimatewindow.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dpipeline.h"


Window3DAnimate::Window3DAnimate()
{
	m_nRotateX = 0;
	m_nRotateY = 0;
	m_nRotateZ = 0;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	m_nOffsetZ = 0;

    m_pIWindow3DAnimate = NULL;
}
Window3DAnimate::~Window3DAnimate()
{
}


void  Window3DAnimate::Initialize()
{
    __super::Initialize();

    Init_Sin_Cos_Tables();

	CRect  rc(&m_rcWindowInBuffer);
	rc.right--;  //不包含右下侧
	rc.bottom--;

	textmapping.SetTexturePointA(rc.left, rc.top);
	textmapping.SetTexturePointB(rc.right, rc.top);
	textmapping.SetTexturePointC(rc.right, rc.bottom);
	textmapping.SetTexturePointD(rc.left, rc.bottom);

	// 设置纹理(src)和目标缓存(dest)
	ImageData  srcBuffer;
	srcBuffer.m_nbpp = 32;
	srcBuffer.m_ptr = m_pSrcBits;
	srcBuffer.m_pScan0 = m_pSrcBits;
	srcBuffer.m_nStride = m_nSrcPitch;
	srcBuffer.m_nWidth = m_nSrcWndWidth+2;
	srcBuffer.m_nHeight = m_nSrcWndHeight+2;
	textmapping.SetSrcBuffer(&srcBuffer);

	ImageData  dstBuffer;
	dstBuffer.m_nbpp = 32;
	dstBuffer.m_ptr = m_pLayeredWindow->m_pBits;
	dstBuffer.m_pScan0 = m_pLayeredWindow->m_pBits;
	dstBuffer.m_nStride = m_pLayeredWindow->m_nPitch;
	dstBuffer.m_nWidth = m_pLayeredWindow->m_nWidth;
	dstBuffer.m_nHeight = m_pLayeredWindow->m_nHeight;
	textmapping.SetDstBuffer(&dstBuffer);
}

void Window3DAnimate::OnTick(int nCount, IStoryboard** ppTimerArray)
{
	MATRIX_4_4  matWorld;
	MAT_IDENTITY_4_4(&matWorld);
	Mat_Rotate_4X4(&matWorld, m_nRotateX, m_nRotateY, m_nRotateZ);
	Mat_Move_4X4(&matWorld, m_nOffsetX, m_nOffsetY, m_nOffsetZ);

	CRect  rc(&m_rcWindowInBuffer);
	rc.right--;  //不包含右下侧
	rc.bottom--;
	POINT2D  ptOrigin = {1+(float)m_nSrcWndWidth/2,1+(float)m_nSrcWndHeight/2};// 坐标原点

	Quad  quad;
	_3dPipeline  _3dpipeline;
	_3dpipeline.Do(&rc, ptOrigin, &matWorld, __out &quad);
	textmapping.SetQuad(&quad, &rc);

	m_pLayeredWindow->Clear();
	POINT ptOffset = {0, 0};
	textmapping.Do(&ptOffset, &rc);
}
