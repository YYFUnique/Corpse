#include "stdafx.h"
#include "perspective_texture_mapping.h"
#include "fpu_rounding.h"

namespace UI
{

PerspectiveTextureMapping::PerspectiveTextureMapping()
{

}

PerspectiveTextureMapping::~PerspectiveTextureMapping()
{

}

//
//  A     B
//
//    C
//
void  PerspectiveTextureMapping::DoFlatTopTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    if (A->z <= 0 || B->z <= 0 || C->z <= 0) 
        return;

    MappingParam  param = {0};

    param.yStart = A->yDest;
    param.yEnd = C->yDest;

    param.xStart = A->xDest << FIXP16_SHIFT;
    param.xEnd = B->xDest << FIXP16_SHIFT;

    int  nAz = round(A->z);
    int  nBz = round(B->z);
    int  nCz = round(C->z);
    int  Az = (1<<FIXP28_SHIFT) / nAz;
    int  Bz = (1<<FIXP28_SHIFT) / nBz;
    int  Cz = (1<<FIXP28_SHIFT) / nCz;

    int   Au = (A->u << FIXP20_SHIFT) / nAz; 
    int   Av = (A->v << FIXP20_SHIFT) / nAz; 
    int   Bu = (B->u << FIXP20_SHIFT) / nBz; 
    int   Bv = (B->v << FIXP20_SHIFT) / nBz; 
    int   Cu = (C->u << FIXP20_SHIFT) / nCz; 
    int   Cv = (C->v << FIXP20_SHIFT) / nCz; 

    // 避免图片太大导致溢出。FIXP20_SHIFT只能保证2048的大小
    assert(Au >= 0 && Av >= 0 && Bu >= 0 && Bv >= 0 && Cu >= 0 && Cv >= 0);  

    int  nHeight = (C->yDest - A->yDest);  // A-B等高

    param.kxLeft  = ((C->xDest - A->xDest) << FIXP16_SHIFT) / nHeight; 
    param.kxRight = ((C->xDest - B->xDest) << FIXP16_SHIFT) / nHeight; 

    param.uStart = Au;
    param.vStart = Av;
    param.uEnd = Bu;
    param.vEnd = Bv;

    param.zStart = Az;  
    param.zEnd = Bz;

    param.kuLeft  = (Cu - Au) / nHeight;
    param.kvLeft  = (Cv - Av) / nHeight;
    param.kuRight = (Cu - Bu) / nHeight;
    param.kvRight = (Cv - Bv) / nHeight;

    param.kzLeft  = (Cz - Az) / nHeight;
    param.kzRight = (Cz - Bz) / nHeight;

    _do_triangle(&param);
}

//
//   A
//
// B    C
//
void  PerspectiveTextureMapping::DoFlatBottomTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    if (A->z <= 0 || B->z <= 0 || C->z <= 0)
        return;

    MappingParam  param = {0};

    param.yStart = A->yDest;
    param.yEnd = C->yDest;

    param.xStart = A->xDest << FIXP16_SHIFT;
    param.xEnd = param.xStart;

    int  nAz = round(A->z);
    int  nBz = round(B->z);
    int  nCz = round(C->z);
    int  Az = (1<<FIXP28_SHIFT) / nAz;
    int  Bz = (1<<FIXP28_SHIFT) / nBz;
    int  Cz = (1<<FIXP28_SHIFT) / nCz;

    int  Au = (A->u << FIXP20_SHIFT) / nAz; 
    int  Av = (A->v << FIXP20_SHIFT) / nAz; 
    int  Bu = (B->u << FIXP20_SHIFT) / nBz; 
    int  Bv = (B->v << FIXP20_SHIFT) / nBz; 
    int  Cu = (C->u << FIXP20_SHIFT) / nCz; 
    int  Cv = (C->v << FIXP20_SHIFT) / nCz; 

    // 避免图片太大导致溢出。FIXP20_SHIFT只能保证2048的大小
    assert(Au >= 0 && Av >= 0 && Bu >= 0 && Bv >= 0 && Cu >= 0 && Cv >= 0);  

    int  nHeight = (C->yDest - A->yDest);  // B-C等高

    param.kxLeft = ((B->xDest - A->xDest) << FIXP16_SHIFT) / nHeight;
    param.kxRight = ((C->xDest - A->xDest) << FIXP16_SHIFT) / nHeight;

    param.uStart = Au;
    param.vStart = Av;
    param.uEnd = Au;
    param.vEnd = Av;

    param.zStart = Az;
    param.zEnd = param.zStart;

    param.kuLeft  = (Bu - Au) / nHeight;
    param.kvLeft  = (Bv - Av) / nHeight;
    param.kuRight = (Cu - Au) / nHeight;
    param.kvRight = (Cv - Av) / nHeight;

    param.kzLeft  = (Bz - Az) / nHeight;
    param.kzRight = (Cz - Az) / nHeight;

    _do_triangle(&param);
}

//
//   A                   A                     A             A
//  
// B-----     或者是    -----B   或者是   B--       或者是      B---
//
//       C             C                   C                            C
//     
void  PerspectiveTextureMapping::DoGeneralTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C)
{
    if (A->z <= 0 || B->z <= 0 || C->z <= 0)  // 增加这个限制，防止运算过程中出现z=0，导致除0错误
        return;

    MappingParam  param = {0};

    param.yStart = 0;
    param.yEnd = 0;

    param.kxLeft = 0;  // 两条直线的斜率的倒数
    param.kxRight = 0;   

    param.xStart = A->xDest << FIXP16_SHIFT;  // 扫描线x坐标范围
    param.xEnd = param.xStart;

    // a. 1/z 和 x、y 是成线性关系的
    // b. u/z 和 x、y 是成线性关系的

    int  nAz = round(A->z);
    int  nBz = round(B->z);
    int  nCz = round(C->z);
    int  Az = (1<<FIXP28_SHIFT) / nAz;
    int  Bz = (1<<FIXP28_SHIFT) / nBz;
    int  Cz = (1<<FIXP28_SHIFT) / nCz;

    int   Au = (A->u << FIXP20_SHIFT) / nAz;   // 将u,v转化为u/z,v/z后，才能采用线性插值的方法
    int   Av = (A->v << FIXP20_SHIFT) / nAz;
    int   Bu = (B->u << FIXP20_SHIFT) / nBz;
    int   Bv = (B->v << FIXP20_SHIFT) / nBz;
    int   Cu = (C->u << FIXP20_SHIFT) / nCz;
    int   Cv = (C->v << FIXP20_SHIFT) / nCz;

    // 避免图片太大导致溢出。FIXP20_SHIFT只能保证2048的大小
    assert(Au > 0 && Av > 0 && Bu > 0 && Bv > 0 && Cu > 0 && Cv > 0);  

    param.uStart = Au;   // 扫描线所对应的纹理坐标范围
    param.vStart = Av;
    param.uEnd = param.uStart;
    param.vEnd = param.vStart;

    param.zStart = Az;  
    param.zEnd = param.zStart;

    int  nHeightAB = (B->yDest - A->yDest);
    int  nHeightAC = (C->yDest - A->yDest);
    int  nHeightBC = (C->yDest - B->yDest);

    int  kxAB = ((B->xDest - A->xDest) << FIXP16_SHIFT) / nHeightAB; 
    int  kxAC = ((C->xDest - A->xDest) << FIXP16_SHIFT) / nHeightAC;  // 2. 相对于A,值越小越靠左。相对于C，值越大越靠左
    int  kxBC = ((C->xDest - B->xDest) << FIXP16_SHIFT) / nHeightBC;  

    int   kuAB = (Bu - Au) / nHeightAB;   // 使用u/z，而不是u
    int   kvAB = (Bv - Av) / nHeightAB;
    int   kuAC = (Cu - Au) / nHeightAC;
    int   kvAC = (Cv - Av) / nHeightAC;
    int   kuBC = (Cu - Bu) / nHeightBC;
    int   kvBC = (Cv - Bv) / nHeightBC;

    int   kzAB = (Bz - Az) / nHeightAB;
    int   kzAC = (Cz - Az) / nHeightAC;
    int   kzBC = (Cz - Bz) / nHeightBC;

    // 分为两个三角形
    {
        param.yStart = A->yDest;
        param.yEnd = B->yDest;

        if (kxAC > kxAB)
        {
            param.kxLeft = kxAB;
            param.kxRight = kxAC;

            param.kuLeft  = kuAB;
            param.kvLeft  = kvAB;
            param.kuRight = kuAC;
            param.kvRight = kvAC;

            param.kzLeft = kzAB;
            param.kzRight = kzAC;
        }
        else
        {
            param.kxLeft = kxAC;
            param.kxRight = kxAB;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuAB;
            param.kvRight = kvAB;

            param.kzLeft = kzAC;
            param.kzRight = kzAB;
        }

        _do_triangle(&param);
    }
    //---------------------------------------------\\ 
    {
        param.yStart = param.yEnd;
        param.yEnd = C->yDest;

        if (kxAC > kxBC)
        {
            param.kxLeft = kxAC;
            param.kxRight = kxBC;

            param.kuLeft  = kuAC;
            param.kvLeft  = kvAC;
            param.kuRight = kuBC;
            param.kvRight = kvBC;

            param.kzLeft = kzAC;
            param.kzRight = kzBC;
        }
        else
        {
            param.kxLeft = kxBC;
            param.kxRight = kxAC;

            param.kuLeft  = kuBC;
            param.kvLeft  = kvBC;
            param.kuRight = kuAC;
            param.kvRight = kvAC;

            param.kzLeft = kzBC;
            param.kzRight = kzAC;
        }

        _do_triangle(&param);
    }
}

#define GetPixelValue(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y)))[(x)])
#define GetPixelValue2(pBits, nPitchOffset, x, y) \
    (((DWORD*)(pBits + (y<<nPitchOffset)))[(x)])
#define GetPixelAddr(pBits, nPitch, x, y) \
    (((DWORD*)(pBits + nPitch*(y))) + x)

void  PerspectiveTextureMapping::_do_triangle(MappingParam* pParam)
{
	_do_triangle_subdivided_affine(pParam);
	return;

    byte* pSrcBit = (byte*)m_texture.m_pScan0;
    byte* pDstBit = (byte*)m_destBuffer.m_pScan0;
    
    int nSrcPitch = m_texture.m_nStride;
    int nDstPitch = m_destBuffer.m_nStride;

    int  nDstWidth = (m_destBuffer.m_nWidth - 1);
    int  nDstHeight = (m_destBuffer.m_nHeight - 1);


	Color Color0, Color1, Color2, Color3, ColorResult;  // 二次线性插值数据

	// 临时变量
	int nx = 0;
	int ny = 0;

    int yStart = max(0, pParam->yStart);
    int yEnd = min(nDstHeight, pParam->yEnd);
    int xStart = 0;
    int xEnd = 0;
    int xWidth = 0;
    for (int y = yStart; y < yEnd; y++)
    {
        xStart = (pParam->xStart + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
        xEnd = (pParam->xEnd + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
        xWidth = xEnd - xStart;

        int  kuScanline = 0;
        int  kvScanline = 0;
        int  kzScanline = 0;

        if (xWidth > 0)
        {
            // u,v,z相对于x的线性关系，其实是(u/z,v/z,1/z)
            kuScanline = (pParam->uEnd - pParam->uStart) / xWidth;
            kvScanline = (pParam->vEnd - pParam->vStart) / xWidth; 
            kzScanline = (pParam->zEnd - pParam->zStart) / xWidth;
        }
        else
        {
            kuScanline = (pParam->uEnd - pParam->uStart);
            kvScanline = (pParam->vEnd - pParam->vStart);
            kzScanline = (pParam->zEnd - pParam->zStart);
        }

        int   nu = pParam->uStart;  
        int   nv = pParam->vStart;
        int   nz = pParam->zStart;

        DWORD* pDestBuf = GetPixelAddr(pDstBit, nDstPitch, xStart, y);

        if (xStart < 0)
        {
            int d = -xStart;

            nu += d* kuScanline;  
            nv += d* kvScanline;
            nz += d* kzScanline;

            pDestBuf += d;
            xStart = 0;
        }

        if (xEnd > nDstWidth)
        {
            xEnd = nDstWidth;
        }

        for (int x = xStart; x <= xEnd; x++)
        {
			// [注]：两次除法导致效率太低了

			// v/z要还原为v，需要再乘回z. 在这里 nz = 1/z，因此 ny = nv/nz

			nx = (nu << FIXP8_SHIFT) / nz;
			ny = (nv << FIXP8_SHIFT) / nz;


			Color  colorSrc = GetPixelValue(pSrcBit, nSrcPitch, nx, ny);

			Color  colorDst;
			colorDst.m_col = *pDestBuf;
#if 1 // ALPHABELND
			int nAlphaSub = 255 - colorSrc.a;
			colorDst.r = colorSrc.r + (nAlphaSub * colorDst.r >> 8);  // 注:这里仍然用r而不是b，因为获取的时候就是反过来的
			colorDst.g = colorSrc.g + (nAlphaSub * colorDst.g >> 8);
			colorDst.b = colorSrc.b + (nAlphaSub * colorDst.b >> 8);
			colorDst.a = colorSrc.a + (nAlphaSub * colorDst.a >> 8);
			*pDestBuf = colorDst.m_col;
#else
			*pDestBuf = GetPixelValue2(pSrcBit, nPitchOffset, nx, ny);
#endif

            nu += kuScanline;
            nv += kvScanline;
            nz += kzScanline;

            pDestBuf++;
        }

        pParam->xStart += pParam->kxLeft;
        pParam->xEnd += pParam->kxRight;
        pParam->uStart += pParam->kuLeft;
        pParam->vStart += pParam->kvLeft;
        pParam->uEnd += pParam->kuRight;
        pParam->vEnd += pParam->kvRight;
        pParam->zStart += pParam->kzLeft;
        pParam->zEnd += pParam->kzRight;
    }
}

// 直接使用除法效率太低，这里采用线性分段方式。
// 线性分段纹理映射每隔一定间隔（如16-32像素）计算像素的准确
// 纹理坐标，然后在这些像素之间进行仿射线性纹理映射。
// 通过这种方式，可避免每个像素都执行除法运算，而是每隔n个像素
// 执行这些除尘运行，然后通过线性插值计算这些像素之间的纹理坐标。
void  PerspectiveTextureMapping::_do_triangle_subdivided_affine(MappingParam* pParam)
{
	byte* pSrcBit = (byte*)m_texture.m_pScan0;
	byte* pDstBit = (byte*)m_destBuffer.m_pScan0;

	int nSrcPitch = m_texture.m_nStride;
	int nDstPitch = m_destBuffer.m_nStride;

	int  nDstWidth = (m_destBuffer.m_nWidth - 1);
	int  nDstHeight = (m_destBuffer.m_nHeight - 1);

	// 临时变量
	int nx = 0;
	int ny = 0;

	int yStart = max(0, pParam->yStart);
	int yEnd = min(nDstHeight, pParam->yEnd);
	int xStart = 0;
	int xEnd = 0;
	int xWidth = 0;
	int uStart = 0;
	int uEnd = 0;
	int vStart = 0;
	int vEnd = 0;

	Color Color0, Color1, Color2, Color3, ColorResult;  // 二次线性插值数据

	for (int y = yStart; y < yEnd; y++)
	{
		xStart = (pParam->xStart + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
		xEnd = (pParam->xEnd + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
		xWidth = xEnd - xStart;

		uStart = ((pParam->uStart << FIXP8_SHIFT) / (pParam->zStart >> FIXP8_SHIFT)) << 12;  // uStart仍是FIXP20_SHIFT，因此还需要<<12: (20+8 - (28-8))+12 = 20
		vStart = ((pParam->vStart << FIXP8_SHIFT) / (pParam->zStart >> FIXP8_SHIFT)) << 12;
		uEnd   = ((pParam->uEnd << FIXP8_SHIFT) / (pParam->zEnd >> FIXP8_SHIFT)) << 12;
		vEnd   = ((pParam->vEnd << FIXP8_SHIFT) / (pParam->zEnd >> FIXP8_SHIFT)) << 12;
		
		int  kuScanline = 0;
		int  kvScanline = 0;

		if (xWidth > 0)
		{
			// u,v,z相对于x的线性关系，其实是(u/z,v/z,1/z)
			kuScanline = (uEnd - uStart) / xWidth;
			kvScanline = (vEnd - vStart) / xWidth; 
		}
		else
		{
			kuScanline = (uEnd - uStart);
			kvScanline = (vEnd - vStart);
		}

		int  nu = uStart;  
		int  nv = vStart;

		DWORD* pDestBuf = GetPixelAddr(pDstBit, nDstPitch, xStart, y);

		if (xStart < 0)
		{
			int d = -xStart;

			nu += d* kuScanline;  
			nv += d* kvScanline;

			pDestBuf += d;
			xStart = 0;
		}

		if (xEnd > nDstWidth)
		{
			xEnd = nDstWidth;
		}

		for (int x = xStart; x <= xEnd; x++)
		{
			// 双线性插值
#if 1
			int u = nu >> FIXP20_SHIFT;
			int v = nv >> FIXP20_SHIFT;

			int nuInteger = nu>>12;//FIXP20_SHIFT;
			int nvInteger = nv>>12;//FIXP20_SHIFT;
			int nuDecimal = (nu&0xfff)>>4;
			int nvDecimal = (nv&0xfff)>>4;  // 都作成8的 
	
			byte*  pbSrcBits = (byte*)GetPixelAddr(pSrcBit, nSrcPitch, u, v);
			Color0.m_col = *((DWORD*)(pbSrcBits));  // 注：这里的顺序是bgra
			Color2.m_col = (((DWORD*)(pbSrcBits))[1]);
			pbSrcBits += nSrcPitch;
			Color1.m_col = *((DWORD*)(pbSrcBits));
			Color3.m_col = (((DWORD*)(pbSrcBits))[1]);

			int pm3 = nuDecimal*nvDecimal;
			int pm2 = nuDecimal*((1<<8) - nvDecimal);
			int pm1 = nvDecimal*((1<<8) - nuDecimal);
			int pm0 = ((1<<8) - nuDecimal)*((1<<8) - nvDecimal);

			ColorResult.a = (byte)((pm0*Color0.a + pm1*Color1.a + pm2*Color2.a + pm3*Color3.a) >> FIXP16_SHIFT);
			ColorResult.r = (byte)((pm0*Color0.r + pm1*Color1.r + pm2*Color2.r + pm3*Color3.r) >> FIXP16_SHIFT);
			ColorResult.g = (byte)((pm0*Color0.g + pm1*Color1.g + pm2*Color2.g + pm3*Color3.g) >> FIXP16_SHIFT);
			ColorResult.b = (byte)((pm0*Color0.b + pm1*Color1.b + pm2*Color2.b + pm3*Color3.b) >> FIXP16_SHIFT);

			*pDestBuf = ColorResult.m_col;
#else
    	    *pDestBuf = GetPixelValue(pSrcBit, nSrcPitch, (nu>>FIXP20_SHIFT), (nv>>FIXP20_SHIFT));
#endif
			nu += kuScanline;
			nv += kvScanline;
			pDestBuf++;
		}

		pParam->xStart += pParam->kxLeft;
		pParam->xEnd += pParam->kxRight;
		pParam->uStart += pParam->kuLeft;
		pParam->vStart += pParam->kvLeft;
		pParam->uEnd += pParam->kuRight;
		pParam->vEnd += pParam->kvRight;
		pParam->zStart += pParam->kzLeft;
		pParam->zEnd += pParam->kzRight;
	}
}
}