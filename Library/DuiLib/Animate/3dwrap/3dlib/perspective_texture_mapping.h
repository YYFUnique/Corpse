#pragma once
#include "texture_mapping_base.h"

// 透视纹理映射 -- 基于三角形

// 参考资料：
// http://www.chrishecker.com/Miscellaneous_Technical_Articles
// http://blog.csdn.net/popy007/article/details/5570803

//
// 3D游戏编程师技巧 12.2.1
//
// 在大多数情况下，浮点数的乘法比定点数快，但光栅化从本质上说是一种“位”运算，
// 处理的是RGB值、纹理坐标和内存。在这方面，浮点数非常糟糕，在光栅化函数的外
// 循环和内循环对扫描线进行光栅化时，需要在浮点数和整数之间进行转换，其开销是
// 无法承担的。在最糟糕的情况下，浮点数到整数的转换需要70个时钟周期，这是完全
// 不能接受的。另外，使用32位整数时，可以进行各种浮点数无法进行的优化。
// 
// 因此，除光栅化外，在其他情况下都使用浮点数，这是最好的选择：在变换和光照中
// 使用浮点数，速度更快，精度更高；但在光栅化期间，使用浮点数的开销是无法承担
// 的。
//

namespace UI
{

class PerspectiveTextureMapping : public TextureMappingBase
{
public:
	PerspectiveTextureMapping();
    ~PerspectiveTextureMapping();

    struct MappingParam
    {
        int    yStart;
        int    yEnd;

        int    xStart;  // [定点数] FIXP16_SHIFT
        int    xEnd;    // [定点数]
        int    kxLeft;  // [定点数] 两条直线的斜率
        int    kxRight; // [定点数]

        int    zStart;  // [定点数] FIXP28_SHIFT
        int    zEnd;    // [定点数]
        int    kzLeft;  // [定点数]
        int    kzRight; // [定点数]
        
        int    uStart;  // [定点数] 扫描线所对应的纹理坐标范围, FIXP20_SHIFT
        int    vStart;  // [定点数]
        int    uEnd;    // [定点数]
        int    vEnd;    // [定点数]

        int    kuLeft;  // [定点数] 纹理坐标在y方向上的增加率, FIXP20_SHIFT
        int    kvLeft;  // [定点数]
        int    kuRight; // [定点数]
        int    kvRight; // [定点数]
    };
private:
    void  DoTriangle(TexturePoint* V0, TexturePoint* V1, TexturePoint* V2);
    void  DoFlatTopTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C);
	void  DoFlatBottomTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C);
    void  DoGeneralTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C);
    void  _do_triangle(MappingParam* pParam);
	void  _do_triangle_subdivided_affine(MappingParam* pParam);
};


}