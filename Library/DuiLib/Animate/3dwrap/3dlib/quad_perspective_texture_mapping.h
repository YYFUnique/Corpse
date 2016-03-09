#pragma once
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_transform.h"
#include "texture_mapping_base.h"

// �����ı���͸�ӱ任��ʽ���е�����ӳ��
namespace UI
{

class QuadPerspectiveTextureMapping : public TextureMappingBase
{
public:
    virtual void  SetQuad(Quad* pQuad, RECT* prc);
    virtual void  Do(POINT*  ptOffset, RECT* prcClip);
    virtual void  MousePoint2TexturePoint(POINT pt, float* pfx, float* pfy);

protected:
	void  do_direct(POINT*  ptOffset, RECT* prcClip);
	void  do_bilinear(POINT*  ptOffset, RECT* prcClip);
	void  do_bilinear_alpha(POINT*  ptOffset, RECT* prcClip);
    
    
public:
    PerspectiveTransform  m_perspective;  // ���ڼ���ͼƬ����ӳ��


};

}