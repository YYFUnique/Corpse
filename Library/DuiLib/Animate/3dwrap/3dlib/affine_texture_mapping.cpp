#include "stdafx.h"
#include "affine_texture_mapping.h"
#include "fpu_rounding.h"

namespace UI
{

AffineTextureMapping::AffineTextureMapping()
{
   
}

AffineTextureMapping::~AffineTextureMapping()
{

}

//
//  A     B
//
//    C
//
void  AffineTextureMapping::DoFlatTopTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    MappingParam  param = {0};

    param.yStart = A->yDest;
    param.yEnd = C->yDest;

    param.xStart = (float)A->xDest;
    param.xEnd = (float)B->xDest;

    float fHeight = (float)(C->yDest - A->yDest);  // A-B等高
    float _fHeight = 1/fHeight;

    param.kLeft = (float)(C->xDest - A->xDest) * _fHeight; 
    param.kRight = (float)(C->xDest - B->xDest) * _fHeight;  

    param.uStart = (float)A->u;
    param.vStart = (float)A->v;
    param.uEnd = (float)B->u;
    param.vEnd = (float)B->v;

    param.kuLeft  = float(C->u - A->u) * _fHeight;
    param.kvLeft  = float(C->v - A->v) * _fHeight;
    param.kuRight = float(C->u - B->u) * _fHeight;
    param.kvRight = float(C->v - B->v) * _fHeight;

    _do_triangle(&param);
}

//
//   A
//
// B    C
//
void  AffineTextureMapping::DoFlatBottomTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    MappingParam  param = {0};

    param.yStart = A->yDest;
    param.yEnd = C->yDest;

    param.xStart = (float)A->xDest;
    param.xEnd = (float)A->xDest;

    float fHeight = (float)(C->yDest - A->yDest);  // B-C等高
    float _fHeight = 1/fHeight;

    param.kLeft = (float)(B->xDest - A->xDest) * _fHeight; 
    param.kRight = (float)(C->xDest - A->xDest) * _fHeight;  

    param.uStart = (float)A->u;
    param.vStart = (float)A->v;
    param.uEnd = (float)A->u;
    param.vEnd = (float)A->v;

    param.kuLeft  = float(B->u - A->u) * _fHeight;
    param.kvLeft  = float(B->v - A->v) * _fHeight;
    param.kuRight = float(C->u - A->u) * _fHeight;
    param.kvRight = float(C->v - A->v) * _fHeight;

    _do_triangle(&param);
}

//
//   A                   A                     A             A
//  
// B-----     或者是    -----B   或者是   B--       或者是      B---
//
//       C             C                   C                            C
//     
void  AffineTextureMapping::DoGeneralTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    MappingParam  param = {0};

    param.yStart = 0;
    param.yEnd = 0;

    param.kLeft = 0.0f;  // 两条直线的斜率的倒数
    param.kRight = 0.0f;   

    param.xStart = (float)A->xDest;  // 扫描线x坐标范围
    param.xEnd = param.xStart;

    param.uStart = (float)A->u;   // 扫描线所对应的纹理坐标范围
    param.vStart = (float)A->v;
    param.uEnd = (float)A->u;
    param.vEnd = (float)A->v;

    float fHeightAB = (float)(B->yDest - A->yDest);
    float fHeightAC = (float)(C->yDest - A->yDest);
    float fHeightBC = (float)(C->yDest - B->yDest);
    float _fHeightAB = 1/fHeightAB;
    float _fHeightAC = 1/fHeightAC;
    float _fHeightBC = 1/fHeightBC;

    float kAB = (float)(B->xDest - A->xDest) * _fHeightAB; 
    float kAC = (float)(C->xDest - A->xDest) * _fHeightAC;  // 2. 相对于A,值越小越靠左。相对于C，值越大越靠左
    float kBC = (float)(C->xDest - B->xDest) * _fHeightBC;  

    float kuAB = float(B->u - A->u) * _fHeightAB;
    float kvAB = float(B->v - A->v) * _fHeightAB;
    float kuAC = float(C->u - A->u) * _fHeightAC;
    float kvAC = float(C->v - A->v) * _fHeightAC;
    float kuBC = float(C->u - B->u) * _fHeightBC;
    float kvBC = float(C->v - B->v) * _fHeightBC;

    // 分为两个三角形
    {
        param.yStart = A->yDest;
        param.yEnd = B->yDest;

        if (kAC > kAB)
        {
            param.kLeft = kAB;
            param.kRight = kAC;

            param.kuLeft  = kuAB;
            param.kvLeft  = kvAB;
            param.kuRight = kuAC;
            param.kvRight = kvAC;
        }
        else
        {
            param.kLeft = kAC;
            param.kRight = kAB;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuAB;
            param.kvRight = kvAB;
        }

        _do_triangle(&param);
    }
    {
        param.yStart = param.yEnd;
        param.yEnd = C->yDest;

        if (kAC > kBC)
        {
            param.kLeft = kAC;
            param.kRight = kBC;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuBC;
            param.kvRight = kvBC;
        }
        else
        {
            param.kLeft = kBC;
            param.kRight = kAC;

            param.kuLeft  = kuBC;
            param.kvLeft  = kvBC;
            param.kuRight = kuAC;
            param.kvRight = kvAC;
        }
        
        _do_triangle(&param);
    }
}

#define GetPixelValue(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y)))[(x)])
#define GetPixelAddr(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y))) + x)

void  AffineTextureMapping::_do_triangle(MappingParam* pParam)
{
    set_round_down();  //设置FPU的舍入模式，在Bilinear函数中需要使用汇编加快float->int。

    byte* pSrcBit = (byte*)m_texture.m_pScan0;
    byte* pDstBit = (byte*)m_destBuffer.m_pScan0;

    int nSrcPitch = m_texture.m_nStride;
    int nDstPitch = m_destBuffer.m_nStride;

    Color Color0, Color1, Color2, Color3, ColorResult;  // 二次线性插值数据

    for (int y = pParam->yStart; y <= pParam->yEnd; y++)
    {
        float fWidth = pParam->xEnd - pParam->xStart;
        if (fWidth > EPSILON_E6 || fWidth < -EPSILON_E6)  // != 0
        {
            float _fWidth = 1/fWidth;

            // u, v 与 x 的线性关系
            float kuScanline = (pParam->uEnd - pParam->uStart) * _fWidth;
            float kvScanline = (pParam->vEnd -pParam->vStart) * _fWidth; 

            float fu = pParam->uStart;
            float fv = pParam->vStart; 

            int nStart = round(pParam->xStart);
            int nEnd = round(pParam->xEnd);
            byte*  pDestBuf = (byte*)GetPixelAddr(pDstBit, nDstPitch, nStart, y);

            for (int x = nStart ; x <= nEnd; x++)
            {
                int  nuSrc = 0, nvSrc = 0;
                FLOAT_TO_INT(fu, nuSrc);
                FLOAT_TO_INT(fv, nvSrc);
#if 1
                int u = 0, v = 0;
                float fuTemp = (fu - nuSrc)*256;
                float fvTemp = (fv - nvSrc)*256;
                FLOAT_TO_INT(fuTemp, u);  
                FLOAT_TO_INT(fvTemp, v);

                byte* pbSrcBits = (byte*)GetPixelAddr(pSrcBit, nSrcPitch, nuSrc, nvSrc);
                Color0.m_col = *((DWORD*)(pbSrcBits));  // 注：这里的顺序是bgra
                Color2.m_col = (((DWORD*)(pbSrcBits))[1]);
                pbSrcBits += nSrcPitch;
                Color1.m_col = *((DWORD*)(pbSrcBits));
                Color3.m_col = (((DWORD*)(pbSrcBits))[1]);

                int pm3_16  = u*v; 
                int pm2_16  = u*(256-v);
                int pm1_16  = v*(256-u);
                int pm0_16  = (256-u)*(256-v);

                ColorResult.a = (byte)((pm0_16*Color0.a + pm1_16*Color1.a + pm2_16*Color2.a + pm3_16*Color3.a) >> FIXP16_SHIFT);
                ColorResult.r = (byte)((pm0_16*Color0.r + pm1_16*Color1.r + pm2_16*Color2.r + pm3_16*Color3.r) >> FIXP16_SHIFT);
                ColorResult.g = (byte)((pm0_16*Color0.g + pm1_16*Color1.g + pm2_16*Color2.g + pm3_16*Color3.g) >> FIXP16_SHIFT);
                ColorResult.b = (byte)((pm0_16*Color0.b + pm1_16*Color1.b + pm2_16*Color2.b + pm3_16*Color3.b) >> FIXP16_SHIFT);

#if 1
                // AlphaBlend
                int nAlphaSub = 255 - ColorResult.a;
                pDestBuf[0] = ColorResult.r + (nAlphaSub * pDestBuf[0] >> 8);  // 注:这里仍然用r而不是b，因为获取的时候就是反过来的
                pDestBuf[1] = ColorResult.g + (nAlphaSub * pDestBuf[1] >> 8);
                pDestBuf[2] = ColorResult.b + (nAlphaSub * pDestBuf[2] >> 8);
                pDestBuf[3] = ColorResult.a + (nAlphaSub * pDestBuf[3] >> 8);
#else
                
                *(DWORD*)pDestBuf = ColorResult.m_col;
#endif
#else
                // 直接映射
                *(DWORD*)pDestBuf = GetPixelValue(pSrcBit, nSrcPitch, (int)fu, (int)fv);
#endif

                fu += kuScanline;
                fv += kvScanline;
                pDestBuf += 4;
            }
        }
        pParam->xStart += pParam->kLeft;
        pParam->xEnd += pParam->kRight;
        pParam->uStart += pParam->kuLeft;
        pParam->vStart += pParam->kvLeft;
        pParam->uEnd += pParam->kuRight;
        pParam->vEnd += pParam->kvRight;
    }

    restore_cw();
}

}