#include "stdafx.h"
#include "quad_perspective_texture_mapping.h"
#include "UISDK\Kernel\Src\UIEffect\CacheBitmap\cachebitmap.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\fpu_rounding.h"

namespace UI
{

void QuadPerspectiveTextureMapping::SetQuad(Quad* pQuad, RECT* prc) 
{
    if (!pQuad || !prc)
        return;
	__super::SetQuad(pQuad, prc);

    m_perspective.SetQuad(pQuad, prc);
    //m_perspective.ChangeToFixedPoint();
}


void  QuadPerspectiveTextureMapping::MousePoint2TexturePoint(POINT pt, float* pfx, float* pfy)
{
    perspective_transform/*_fp*/(m_perspective, pt.x, pt.y, pfx, pfy); 
}
// void  QuadPerspectiveTextureMapping::SetSrcBuffer(ImageData* pBuffer)
// {
//     UIASSERT(pBuffer->m_nbpp == 32);
// 
// 	if (m_lTextMappingMode & TextMapping_Bilinear)
// 	{
// 		// �½�һ��ͼƬ�������߽��ڱ�
// 
// 		int nWidth = pBuffer->m_nWidth+2;
// 		int nHeight = pBuffer->m_nHeight+2;
// 		HBITMAP hBitmap = CacheBitmap::GetInstance()->Create(nWidth, nHeight);
// 		BITMAP bm;
// 		::GetObject(hBitmap, sizeof(bm), &bm);
// 
// 		BYTE*  pSrc = pBuffer->m_pScan0;
// 		BYTE*  pDst = (BYTE*)bm.bmBits + bm.bmWidthBytes + 4;
// 		for (int i = 0; i < pBuffer->m_nHeight; i++)
// 		{
// 			memcpy(pDst, pSrc, pBuffer->m_nStride);
// 			pSrc += pBuffer->m_nStride;
// 			pDst += bm.bmWidthBytes;
// 		}
// 
// 		m_texture.m_bNeedDeletePtr = false;
// 		m_texture.m_nbpp = 32;
// 		m_texture.m_nWidth = nWidth;
// 		m_texture.m_nHeight = nHeight;
// 		m_texture.m_nStride = bm.bmWidthBytes;
// 		m_texture.m_pScan0 = m_texture.m_ptr = (BYTE*)bm.bmBits;
// 	}
// 	else
// 	{
// 		m_texture = *pBuffer;
// 	}
// }

#define GetPixelValue(pBits, nPitch, x, y) \
	(((DWORD*)(pBits + nPitch*(y)))[(x)])

// ������ֵ��ֱ��ӳ��
void  QuadPerspectiveTextureMapping::do_direct(POINT*  ptOffset, RECT* prcClip)
{
    BYTE* pbSrcBits = NULL;
    BYTE* pDstBits = (BYTE*)m_destBuffer.m_pScan0;

    // ���Դͼ��400�Ŀ�ȣ����ڼ����ֵʱ��������ķ�Χ�� (-0.9999 ~ 399.99999)
    CRect rcSrcIndexRange;  // �±귶Χ
    rcSrcIndexRange.SetRect(0, 0, m_texture.m_nWidth-1, m_texture.m_nHeight-1);

    int   nSrcPitch = m_texture.m_nStride;
    BYTE* pSrcBits = (BYTE*)m_texture.m_pScan0;

    float fxSrc = 0.0f;
    float fySrc = 0.0f;
    float *pfxSrc = &fxSrc;  // Ϊ����perspective_transform��ʹ��ָ��
    float *pfySrc = &fySrc;
    int   nxSrc = 0;         // ע: ֱ�Ӹ�ֵ int i = floatvalue; �����һ�� _ftol()����
    int   nySrc = 0;         //     ����Ч�ʽ��͡�����ʹ����Ƕ���ָ�(3D��Ϸ��̴�ʦ���� P918)

    // ת��Ŀ������
    Quad  quadDest = m_quad;
    quadDest.Offset(ptOffset->x, ptOffset->y);

    // �ڴ�ѭ��֮ǰ�޳���һЩ�հ�����
    int nMinX = max(prcClip->left,     min(min(min(quadDest.Ax,quadDest.Bx),quadDest.Cx),quadDest.Dx));
    int nMinY = max(prcClip->top,      min(min(min(quadDest.Ay,quadDest.By),quadDest.Cy),quadDest.Dy));
    int nMaxX = min(prcClip->right-1,  max(max(max(quadDest.Ax,quadDest.Bx),quadDest.Cx),quadDest.Dx));  // -1, ʹ���±�������Χ
    int nMaxY = min(prcClip->bottom-1, max(max(max(quadDest.Ay,quadDest.By),quadDest.Cy),quadDest.Dy));

    pDstBits = (BYTE*)m_destBuffer.m_pScan0 + (nMinY*m_destBuffer.m_nStride);
    for (int X = 0, Y = nMinY; Y <= nMaxY; Y++)  // quad�ı߽�ֵҲҪ���㣬�ڼ���ʱ�Ѿ���������Χ
    {
        for (X = nMinX; X <= nMaxX; X++)
        {
            perspective_transform/*_fp*/(m_perspective, X-ptOffset->x, Y-ptOffset->y, pfxSrc, pfySrc); 

            FLOAT_TO_INT(fxSrc, nxSrc);
            FLOAT_TO_INT(fySrc, nySrc);

            // �����������귶Χ...(���if���� for.for ���кܲ���.)
            if (nxSrc < rcSrcIndexRange.top   ||    
                nxSrc > rcSrcIndexRange.right || 
                nySrc < rcSrcIndexRange.top   || 
                nySrc > rcSrcIndexRange.bottom)
            {
                continue;
            }

            //////////////////////////////////////////////////////////////////////////
            // �ڽ�ȡ��, �ٶȿ죬���о��

            ((DWORD*)pDstBits)[X] = ((DWORD*)(pSrcBits + nSrcPitch*nySrc))[nxSrc];


            //////////////////////////////////////////////////////////////////////////
        }
        pDstBits += m_destBuffer.m_nStride;
    }
}

void  QuadPerspectiveTextureMapping::do_bilinear(POINT*  ptOffset, RECT* prcClip)
{
	set_round_down();  //����FPU������ģʽ����Bilinear��������Ҫʹ�û��ӿ�float->int��

    BYTE* pbSrcBits = NULL;
    BYTE* pDstBits = (BYTE*)m_destBuffer.m_pScan0;

    // ���Դͼ��400�Ŀ�ȣ����ڼ����ֵʱ��������ķ�Χ�� (-0.9999 ~ 399.99999)
    CRect rcSrcIndexRange;  // �±귶Χ
    rcSrcIndexRange.SetRect(0, 0, m_texture.m_nWidth-1, m_texture.m_nHeight-1);    // m_texture�����ڱ��߽�

    int   nSrcPitch = m_texture.m_nStride;
    BYTE* pSrcBits = (BYTE*)m_texture.m_pScan0;

    float fxSrc = 0.0f;
    float fySrc = 0.0f;
    float *pfxSrc = &fxSrc;  // Ϊ����perspective_transform��ʹ��ָ��
    float *pfySrc = &fySrc;
    int   nxSrc = 0;         // ע: ֱ�Ӹ�ֵ int i = floatvalue; �����һ�� _ftol()����
    int   nySrc = 0;         //     ����Ч�ʽ��͡�����ʹ����Ƕ���ָ�(3D��Ϸ��̴�ʦ���� P918)

    Color Color0, Color1, Color2, Color3, ColorResult;  // �������Բ�ֵ����

    // ת��Ŀ������
    Quad  quadDest = m_quad;
    quadDest.Offset(ptOffset->x, ptOffset->y);

    // �ڴ�ѭ��֮ǰ�޳���һЩ�հ�����
    int nMinX = max(prcClip->left,     min(min(min(quadDest.Ax,quadDest.Bx),quadDest.Cx),quadDest.Dx));
    int nMinY = max(prcClip->top,      min(min(min(quadDest.Ay,quadDest.By),quadDest.Cy),quadDest.Dy));
    int nMaxX = min(prcClip->right-1,  max(max(max(quadDest.Ax,quadDest.Bx),quadDest.Cx),quadDest.Dx));  // -1, ʹ���±�������Χ
    int nMaxY = min(prcClip->bottom-1, max(max(max(quadDest.Ay,quadDest.By),quadDest.Cy),quadDest.Dy));

    pDstBits = (BYTE*)m_destBuffer.m_pScan0 + (nMinY*m_destBuffer.m_nStride);
    for (int X = 0, Y = nMinY; Y <= nMaxY; Y++)  // quad�ı߽�ֵҲҪ���㣬�ڼ���ʱ�Ѿ���������Χ
    {
        for (X = nMinX; X <= nMaxX; X++)
        {
            perspective_transform/*_fp*/(m_perspective, X-ptOffset->x, Y-ptOffset->y, pfxSrc, pfySrc); 

            FLOAT_TO_INT(fxSrc, nxSrc);
            FLOAT_TO_INT(fySrc, nySrc);


            // �����������귶Χ...(���if���� for.for ���кܲ���.)
            if (nxSrc < rcSrcIndexRange.left   ||    
                nxSrc >= rcSrcIndexRange.right ||  // ��ΪҪȡ��һ�����������������Բ�ֵ����˲��������
                nySrc < rcSrcIndexRange.top    || 
                nySrc >= rcSrcIndexRange.bottom)
            {
                continue;
            }

			int u = 0, v = 0;
			float fu = (fxSrc - nxSrc)*256;
			float fv = (fySrc - nySrc)*256;
            FLOAT_TO_INT(fu, u);  
            FLOAT_TO_INT(fv, v);

            //////////////////////////////////////////////////////////////////////////
            // �������Բ�ֵ������Чȥ����ݣ����ٶȷǳ���
            //((DWORD*)pDstBits)[X] = Bilinear(m_pSrcBits, m_nSrcPitch, nxSrc, nySrc, (float)fxSrc, (float)fySrc);

            //
            // �������Բ�ֵ��ʽ�Ƶ�
            // http://blog.csdn.net/dakistudio/article/details/1767100 �������Բ�ֵ��ʽ�Ƶ�
            // http://blog.csdn.net/housisong/article/details/1452249  �Ż�����
            //
            // xΪfx������ȡ����yΪfy������ȡ��
            //
            //inline DWORD& Bilinear(BYTE* pBits, const int& nPitch, const int& x, const int& y, const float& fx, const float& fy)
            {
                // �ѽ�ԭʼͼƬ��right/bottom����1px�������ڻ�ȡ x+1, y+1ʱ�ﵽ�����ԵҲ�������
                // ȡ�������ĸ����ص���ɫֵ(x,y) (x+1, y) (x, y+1) (x+1, y+1)
                // PS: ���ĸ�����Ҳ�ǳ���CPU�����ǲ�֪����ô�Ż���Խ�Ż�CPUԽ����

                //  Color0.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc,  nySrc);
                //  Color2.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc+1,nySrc);
                //  Color1.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc,  nySrc+1);
                //  Color3.m_col = GetPixelValue(pSrcBits, nSrcPitch, nxSrc+1,nySrc+1);

				pbSrcBits = pSrcBits + (nySrc*nSrcPitch + (nxSrc<<2));
				Color0.m_col = *((DWORD*)(pbSrcBits));  // ע�������˳����bgra
				Color2.m_col = (((DWORD*)(pbSrcBits))[1]);
				pbSrcBits += nSrcPitch;
				Color1.m_col = *((DWORD*)(pbSrcBits));
				Color3.m_col = (((DWORD*)(pbSrcBits))[1]);

                // ���������˷�תΪ�������˷���ͬʱ�Ż�������ת��������
                // 	float u = (float)fxSrc - nxSrc;
                // 	float v = (float)fySrc - nySrc;
                // 	float pm3 = u*v;
                // 	float pm2 = u*(1.0f-v);
                // 	float pm1 = v*(1.0f-u);
                // 	float pm0 = (1.0f-u)*(1.0f-v);
                // 
                // 	ColorResult.a = (byte)(pm0*Color0.a + pm1*Color1.a + pm2*Color2.a + pm3*Color3.a);
                // 	ColorResult.r = (byte)(pm0*Color0.r + pm1*Color1.r + pm2*Color2.r + pm3*Color3.r);
                // 	ColorResult.g = (byte)(pm0*Color0.g + pm1*Color1.g + pm2*Color2.g + pm3*Color3.g);
                // 	ColorResult.b = (byte)(pm0*Color0.b + pm1*Color1.b + pm2*Color2.b + pm3*Color3.b);

                // 	((DWORD*)pDstBits)[X] = ColorResult.m_col;
                int pm3_16  = u*v; 
                int pm2_16  = u*(256-v);
                int pm1_16  = v*(256-u);
                int pm0_16  = (256-u)*(256-v);

                ColorResult.a = (byte)((pm0_16*Color0.a + pm1_16*Color1.a + pm2_16*Color2.a + pm3_16*Color3.a) >> FIXP16_SHIFT);
                ColorResult.r = (byte)((pm0_16*Color0.r + pm1_16*Color1.r + pm2_16*Color2.r + pm3_16*Color3.r) >> FIXP16_SHIFT);
                ColorResult.g = (byte)((pm0_16*Color0.g + pm1_16*Color1.g + pm2_16*Color2.g + pm3_16*Color3.g) >> FIXP16_SHIFT);
                ColorResult.b = (byte)((pm0_16*Color0.b + pm1_16*Color1.b + pm2_16*Color2.b + pm3_16*Color3.b) >> FIXP16_SHIFT);

#if 1 // ALPHABELND
                // Dst.Red = Src.Red  + (1 - Src.Alpha) * Dst.Red 
                // Dst.Green = Src.Green + (1 - Src.Alpha) * Dst.Green 
                // Dst.Blue = Src.Blue + (1 - Src.Alpha) * Dst.Blue 
                // Dst.Alpha = Src.Alpha + (1 - Src.Alpha) * Dst.Alpha 

                // ��Ŀ��ֵ����alpha��ϣ�����Ԥ��?
                //                 ColorResult.r = ColorResult.r * ColorResult.a >> 8;
                //                 ColorResult.g = ColorResult.g * ColorResult.a >> 8;
                //                 ColorResult.b = ColorResult.b * ColorResult.a >> 8;

                int nPos = X << 2;
                int nAlphaSub = 255 - ColorResult.a;
                pDstBits[nPos  ] = ColorResult.r + (nAlphaSub * pDstBits[nPos  ] >> 8);  // ע:������Ȼ��r������b����Ϊ��ȡ��ʱ����Ƿ�������
                pDstBits[nPos+1] = ColorResult.g + (nAlphaSub * pDstBits[nPos+1] >> 8);
                pDstBits[nPos+2] = ColorResult.b + (nAlphaSub * pDstBits[nPos+2] >> 8);
                pDstBits[nPos+3] = ColorResult.a + (nAlphaSub * pDstBits[nPos+3] >> 8);
#else
                ((DWORD*)pDstBits)[X] = ColorResult.m_col;
#endif
            }
        }
        pDstBits += m_destBuffer.m_nStride;
    }

	restore_cw();
}

void  QuadPerspectiveTextureMapping::Do(POINT*  ptOffset, RECT* prcClip)
{
	if (m_lTextMappingMode & TextMapping_Bilinear)
	{
		do_bilinear(ptOffset, prcClip);
	}
	else
	{
		do_direct(ptOffset, prcClip);
	}
}

}
