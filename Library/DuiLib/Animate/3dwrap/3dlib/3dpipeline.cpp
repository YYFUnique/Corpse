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
    // ע������������ᣬy���Ǵ��µ��ϣ�����Ļ���෴
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
//      [in] ��Ҫ���б任�ľ������� ���������ұߺ����±ߣ�������Χ����������������quadֵ����һ���߽�
//
// ptOrgin
//      [in] ����ԭ�㣬�����λͼ�����꣨���Ͻ�0��0��
//
//  ����3D�仯֮�������pOutQuad
//
void _3dPipeline::Do(RECT* prc, POINT2D ptOrigin, MATRIX_4_4_PTR pMatWorld, __out Quad* pOutQuad)
{
    POINT3D pt3DWorld[4] = {0};
    POINT3D pt3DCamera[4] = {0};

	// �ֲ�����
	POINT3D  m_ptLocale[4];  
	calc_locale_pos(prc, ptOrigin, m_ptLocale);

#pragma region // .�ֲ�����->��������
    {
        // .�Ե�ǰֵ��Ϊ�ǶȽ�����ת

        // ��ת
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

    // ���λ��(����Ĭ�Ͻ���ƽ�����������������ͬһλ�ã�����͸�ӳ���������ֱ�ӾͿ���
    // ������Ļ���������ˣ�ʡ��һ��������
    // ������Ҫע����ǣ����nCameraPos��С�Ļ�������ͼ��ͻ���С��
    CAMERA camerpos = {0};
    float  fCameraPos = 2000.0f;  // ���λ��
    float  d = fCameraPos;      // �������ƽ��ľ��롣����ֵ���һ����������һ�ε���Ļ�����ת��

    VECTOR4D_INITXYZ(&camerpos.WorldPos, 0,0, -fCameraPos);

#pragma region // ��������ת��Ϊ�������
    {
        // ƽ�ƾ���
        MATRIX_4_4  matCameraTrans = {0};
        MAT_IDENTITY_4_4(&matCameraTrans);
        matCameraTrans.M30 = -camerpos.WorldPos.x;
        matCameraTrans.M31 = -camerpos.WorldPos.y;
        matCameraTrans.M32 = -camerpos.WorldPos.z;

        // ����Ƕ�Ϊ0������ת

        for (int i = 0; i < 4; i++)
        {
            Mat_Mul_VECTOR3D_4X4(&pt3DWorld[i], &matCameraTrans, &pt3DCamera[i]);
        }
    }
#pragma endregion

#pragma region // �������ת��Ϊ͸������
    POINT3D pt3DPerspectivePos[4];
    for (int i = 0; i < 4; i++)
    {
        float z = pt3DCamera[i].z;  // �����z�����ں�d��ȵľ��룬��������. ��dֵȡ�ıȽ�Сʱ���ᵼ��zΪ��
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

    // ת������Ļ����ϵ�ϣ���Ļ�����y���Ǵ������£������������෴��
    for (int i = 0; i < 4; ++i)
    {
        pt3DPerspectivePos[i].x += ptOrigin.x;
        pt3DPerspectivePos[i].y = -pt3DPerspectivePos[i].y;
        pt3DPerspectivePos[i].y += ptOrigin.y;
    }

    // ��ֵ������ֵ
    for (int i = 0; i < 4; i++)
    {
        pOutQuad->pos[2*i]   = round(pt3DPerspectivePos[i].x);  // ע������ֱ����+0.5���п����Ǹ���
        pOutQuad->pos[2*i+1] = round(pt3DPerspectivePos[i].y);
    }
    pOutQuad->Az = pt3DCamera[0].z;
    pOutQuad->Bz = pt3DCamera[1].z;
    pOutQuad->Cz = pt3DCamera[2].z;
    pOutQuad->Dz = pt3DCamera[3].z;
}


}