#include "stdafx.h"
#include "3dpipeline.h"

namespace UI
{

_3dPipeline::_3dPipeline()
{
    Init_Sin_Cos_Tables();
}

void  _3dPipeline::calc_locale_pos(RECT* prc, POINT2D ptOrigin, __out POINT3D* ptLocale)
{
    // 注：这里的坐标轴，y轴是从下到上，与屏幕的相反
    // A
    ptLocale[0].x = (float)prc->left - ptOrigin.x;
    ptLocale[0].y = (float)ptOrigin.y - prc->top;
    ptLocale[0].z = 0.0f;
    // B
    ptLocale[1].x = (float)(prc->right - ptOrigin.x);
    ptLocale[1].y = ptLocale[0].y;
    ptLocale[1].z = 0.0f;
    // C
    ptLocale[2].x = ptLocale[1].x;
    ptLocale[2].y = (float)-(prc->bottom - ptOrigin.y);
    ptLocale[2].z = 0.0f;
    // D
    ptLocale[3].x = ptLocale[0].x;
    ptLocale[3].y = ptLocale[2].y;
    ptLocale[3].z = 0.0f;
}

//
// prc
//      [in] 需要进行变换的矩形区域， 不包括最右边和最下边（索引范围），否则计算出来的quad值多了一个边界
//
// ptOrgin
//      [in] 坐标原点，相对于位图的坐标（左上角0，0）
//
//  返回3D变化之后的坐标pOutQuad
//
void _3dPipeline::Do(RECT* prc, POINT2D ptOrigin, MATRIX_4_4_PTR pMatWorld, __out Quad* pOutQuad)
{
    POINT3D pt3DWorld[4] = {0};
    POINT3D pt3DCamera[4] = {0};

	// 局部坐标
	POINT3D  m_ptLocale[4];  
	calc_locale_pos(prc, ptOrigin, m_ptLocale);

#pragma region // .局部坐标->世界坐标
    {
        // .以当前值作为角度进行旋转

        // 旋转
        if (pMatWorld)
        {
            for (int i = 0; i < 4; i++)
                Mat_Mul_VECTOR3D_4X4(&m_ptLocale[i], pMatWorld, &pt3DWorld[i]);
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                pt3DWorld[i].x = m_ptLocale[i].x;
                pt3DWorld[i].y = m_ptLocale[i].y;
            }
        }
    }
#pragma endregion

    // 相机位置(这里默认将视平面放在与矩形所在面的同一位置，这样透视出来的坐标直接就可以
    // 当成屏幕坐标来用了，省了一步操作。
    // 但是需要注意的是，如果nCameraPos过小的话看到的图像就会缩小，
    CAMERA camerpos = {0};
    float  fCameraPos = 2000.0f;  // 相机位置
    float  d = fCameraPos;      // 相机与视平面的距离。将两值设成一样，避免了一次到屏幕坐标的转换

    VECTOR4D_INITXYZ(&camerpos.WorldPos, 0,0, -fCameraPos);

#pragma region // 世界坐标转换为相机坐标
    {
        // 平移矩阵
        MATRIX_4_4  matCameraTrans = {0};
        MAT_IDENTITY_4_4(&matCameraTrans);
        matCameraTrans.M30 = -camerpos.WorldPos.x;
        matCameraTrans.M31 = -camerpos.WorldPos.y;
        matCameraTrans.M32 = -camerpos.WorldPos.z;

        // 相机角度为0，不旋转

        for (int i = 0; i < 4; i++)
        {
            Mat_Mul_VECTOR3D_4X4(&pt3DWorld[i], &matCameraTrans, &pt3DCamera[i]);
        }
    }
#pragma endregion

#pragma region // 相机坐标转换为透视坐标
    POINT3D pt3DPerspectivePos[4];
    for (int i = 0; i < 4; i++)
    {
        float z = pt3DCamera[i].z;  // 这里的z是用于和d相比的距离，不是坐标. 当d值取的比较小时，会导致z为负
        float i_z = 0;
        if (0 != z)
            i_z = 1/z;

        if (pt3DCamera[i].z > 0)    
        {
            pt3DPerspectivePos[i].x = d * pt3DCamera[i].x * i_z;  
            pt3DPerspectivePos[i].y = d * pt3DCamera[i].y * i_z; 
        }
    }
#pragma endregion

    // 转换到屏幕坐标系上（屏幕坐标的y轴是从上自下，与世界坐标相反）
    for (int i = 0; i < 4; ++i)
    {
        pt3DPerspectivePos[i].x += ptOrigin.x;
        pt3DPerspectivePos[i].y = -pt3DPerspectivePos[i].y;
        pt3DPerspectivePos[i].y += ptOrigin.y;
    }

    // 赋值给返回值
    for (int i = 0; i < 4; i++)
    {
        pOutQuad->pos[2*i]   = round(pt3DPerspectivePos[i].x);  // 注：不能直接用+0.5，有可能是负数
        pOutQuad->pos[2*i+1] = round(pt3DPerspectivePos[i].y);
    }
    pOutQuad->Az = pt3DCamera[0].z;
    pOutQuad->Bz = pt3DCamera[1].z;
    pOutQuad->Cz = pt3DCamera[2].z;
    pOutQuad->Dz = pt3DCamera[3].z;
}


}