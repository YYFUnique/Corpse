#include "stdafx.h"
#include "texture_mapping_base.h"

namespace UI
{

TextureMappingBase::TextureMappingBase()
{
    memset(&m_quad, 0, sizeof(Quad));
    memset(m_texturePoint, 0, sizeof(TexturePoint)*4);
    m_lTextMappingMode = TextMapping_Bilinear;
}

void  TextureMappingBase::GetTexturePadding(RECT* prc)
{
    if (m_lTextMappingMode & TextMapping_Bilinear)
    {
        SetRect(prc, 1,1,1,1);  // 需要一px边界哨兵
    }
    else
    {
        SetRect(prc, 0, 0, 0, 0);
    }
}

void  TextureMappingBase::Do(POINT*  ptOffset, RECT* prcClip)
{
	m_ptOffset = *ptOffset;
	m_rcClip = *prcClip;

	// 计算在目标缓存中的位置
	for (int i = 0; i < 4; i++)
	{
		m_texturePoint[i].xDest = m_texturePoint[i].xTransform + ptOffset->x;
		m_texturePoint[i].yDest = m_texturePoint[i].yTransform + ptOffset->y;
	}

	// 分解为两个三角形: ∥ABCD --> △ABD + △BCD
	DoTriangle(&m_texturePoint[0], &m_texturePoint[1], &m_texturePoint[3]);
	DoTriangle(&m_texturePoint[1], &m_texturePoint[2], &m_texturePoint[3]);
}

void  TextureMappingBase::DoTriangle(TexturePoint* V0, TexturePoint* V1, TexturePoint* V2)
{
	// 检查三角形合法性
	if (V0->xDest == V1->xDest && V1->xDest == V2->xDest)
		return;
	if (V0->yDest == V1->yDest && V1->yDest == V2->yDest)
		return;

	// 三角形顶点索引
	TexturePoint* A = V0;
	TexturePoint* B = V1;
	TexturePoint* C = V2; 

#pragma region // 判断三角形类型，将y值小的排在前面

	TexturePoint* temp = NULL;

	if (B->yDest < A->yDest)
	{
		temp = B; B = A; A = temp;
	}
	if (C->yDest < A->yDest)
	{
		temp = C; C = A; A = temp;
	}
	if (C->yDest < B->yDest)
	{
		temp = C; C = B; B = temp;
	}


	TRIANGLE_TYPE  eTriangleType = TRIANGLE_GENERAL;
	if (A->yDest == B->yDest)
	{
		eTriangleType = TRIANGLE_FLAT_TOP;

		if (B->xDest < A->xDest)
		{
			temp = B; B = A; A = temp;
		}
	}
	else if (B->yDest == C->yDest)
	{
		eTriangleType = TRIANGLE_FLAT_BOTTOM;

		if (C->xDest < B->xDest)
		{
			temp = C; C = B; B = temp;
		}
	}

#pragma endregion
	switch (eTriangleType)
	{
	case TRIANGLE_FLAT_TOP:
		{
			DoFlatTopTriangle(A, B, C);
		}
		break;

	case TRIANGLE_FLAT_BOTTOM:
		{
			DoFlatBottomTriangle(A, B, C);
		}
		break;

	case TRIANGLE_GENERAL:
		{
			DoGeneralTriangle(A, B, C);
		}
		break;
	}
}

void TextureMappingBase::SetQuad(Quad* pQuad, RECT* prc) 
{
	if (pQuad)
	{
		memcpy(&m_quad, pQuad, sizeof(Quad));
		m_texturePoint[0].xTransform = pQuad->Ax;
		m_texturePoint[0].yTransform = pQuad->Ay;
        m_texturePoint[0].z  = pQuad->Az;

		m_texturePoint[1].xTransform = pQuad->Bx;
		m_texturePoint[1].yTransform = pQuad->By;
        m_texturePoint[1].z  = pQuad->Bz;

		m_texturePoint[2].xTransform = pQuad->Cx;
		m_texturePoint[2].yTransform = pQuad->Cy;
        m_texturePoint[2].z  = pQuad->Cz;

		m_texturePoint[3].xTransform = pQuad->Dx;
		m_texturePoint[3].yTransform = pQuad->Dy;
        m_texturePoint[3].z  = pQuad->Dz;
	}
	if (prc)
		m_rect.CopyRect(prc);
}

void TextureMappingBase::SetSrcBuffer(ImageData* pBuffer)
{
    m_texture = *pBuffer;
}
void TextureMappingBase::SetDstBuffer(ImageData* pBuffer)
{
    m_destBuffer = *pBuffer;
}
void TextureMappingBase::SetTexturePointA(int u, int v)
{
    SetTexturePoint(0, u, v);
}
void TextureMappingBase::SetTexturePointB(int u, int v)
{
    SetTexturePoint(1, u, v);
}
void TextureMappingBase::SetTexturePointC(int u, int v)
{
    SetTexturePoint(2, u, v);
}
void TextureMappingBase::SetTexturePointD(int u, int v)
{
    SetTexturePoint(3, u, v);
}

void TextureMappingBase::SetTexturePoint(int nIndex, int u, int v)
{
    if (nIndex < 0 || nIndex > 3)
        return;

    m_texturePoint[nIndex].u = u;
    m_texturePoint[nIndex].v = v;
}
}