#pragma once
#include "texture_mapping_base.h"

//
// 仿射纹理映射虽然是错的，但拥有以下优点：
// 1. 从远处观看时，仿射纹理映射的效果很不错；
//

// 1. 对于一个平底△的左侧或右侧连，y++，则x += 1/k (k为直线斜率)。
//    这样就不用每次去根据y计算x值了。推导过程如下
//      y = kx + b，将等式两边都加1
//      y + 1 = kx + b + 1，等效于要计算 y + 1 = k(x + x') + b，要求出x'，则有
//      kx + b + 1 = k(x + x') + b，化简得到
//      1 = kx'，因此x' = 1/k
//
//
// 2. 由于拿到的坐标也是浮点数，导致很多边界问题处理起来相当烦人，不想再搞了！
//
namespace UI
{


// 仿射纹理映射 -- 基于三角形
class AffineTextureMapping : public TextureMappingBase
{
public:
    AffineTextureMapping();
    ~AffineTextureMapping();

    struct MappingParam
    {
        int    yStart;
        int    yEnd;

        float  xStart;  // 两条直线的斜率
        float  xEnd;
        float  kLeft;
        float  kRight;
        
        float  uStart; // 扫描线所对应的纹理坐标范围
        float  vStart;
        float  uEnd;
        float  vEnd;

        float  kuLeft;  // 纹理坐标在y方向上的增加率
        float  kvLeft;
        float  kuRight;
        float  kvRight; 
    };
private:
    virtual void  DoFlatTopTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C);
	virtual void  DoFlatBottomTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C);
    virtual void  DoGeneralTriangle(TexturePoint* A, TexturePoint* B, TexturePoint* C);
    void  _do_triangle(MappingParam* pParam);
};

}