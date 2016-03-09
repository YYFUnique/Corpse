#pragma once
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"

//
// 3d流水线处理类
//
// 目前只支持World的平移+旋转变换. 不支持world scale，不支持View变换。
//
namespace UI
{

class _3dPipeline
{
public:
    _3dPipeline();
    void  Do(RECT* prc, POINT2D ptOrigin, MATRIX_4_4_PTR pMatWorld, __out Quad* pOutQuad);

protected:
	void  calc_locale_pos(RECT* prc, POINT2D ptOrigin, __out POINT3D* ptLocale);
};

}