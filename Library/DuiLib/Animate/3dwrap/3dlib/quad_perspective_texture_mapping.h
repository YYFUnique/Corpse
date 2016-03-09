#pragma once
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_transform.h"
#include "texture_mapping_base.h"

// 采用四边形透视变换公式进行的纹理映射
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
    PerspectiveTransform  m_perspective;  // 用于计算图片变形映射


};

}