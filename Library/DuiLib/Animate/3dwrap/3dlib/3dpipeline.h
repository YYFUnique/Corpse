#pragma once
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"

//
// 3d��ˮ�ߴ�����
//
// Ŀǰֻ֧��World��ƽ��+��ת�任. ��֧��world scale����֧��View�任��
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