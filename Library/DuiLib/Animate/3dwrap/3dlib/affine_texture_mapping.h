#pragma once
#include "texture_mapping_base.h"

//
// ��������ӳ����Ȼ�Ǵ�ģ���ӵ�������ŵ㣺
// 1. ��Զ���ۿ�ʱ����������ӳ���Ч���ܲ���
//

// 1. ����һ��ƽ�ס��������Ҳ�����y++����x += 1/k (kΪֱ��б��)��
//    �����Ͳ���ÿ��ȥ����y����xֵ�ˡ��Ƶ���������
//      y = kx + b������ʽ���߶���1
//      y + 1 = kx + b + 1����Ч��Ҫ���� y + 1 = k(x + x') + b��Ҫ���x'������
//      kx + b + 1 = k(x + x') + b������õ�
//      1 = kx'�����x' = 1/k
//
//
// 2. �����õ�������Ҳ�Ǹ����������ºܶ�߽����⴦�������൱���ˣ������ٸ��ˣ�
//
namespace UI
{


// ��������ӳ�� -- ����������
class AffineTextureMapping : public TextureMappingBase
{
public:
    AffineTextureMapping();
    ~AffineTextureMapping();

    struct MappingParam
    {
        int    yStart;
        int    yEnd;

        float  xStart;  // ����ֱ�ߵ�б��
        float  xEnd;
        float  kLeft;
        float  kRight;
        
        float  uStart; // ɨ��������Ӧ���������귶Χ
        float  vStart;
        float  uEnd;
        float  vEnd;

        float  kuLeft;  // ����������y�����ϵ�������
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