#pragma once
#include "texture_mapping_base.h"

// ͸������ӳ�� -- ����������

// �ο����ϣ�
// http://www.chrishecker.com/Miscellaneous_Technical_Articles
// http://blog.csdn.net/popy007/article/details/5570803

//
// 3D��Ϸ���ʦ���� 12.2.1
//
// �ڴ��������£��������ĳ˷��ȶ������죬����դ���ӱ�����˵��һ�֡�λ�����㣬
// �������RGBֵ������������ڴ档���ⷽ�棬�������ǳ���⣬�ڹ�դ����������
// ѭ������ѭ����ɨ���߽��й�դ��ʱ����Ҫ�ڸ�����������֮�����ת�����俪����
// �޷��е��ġ�������������£���������������ת����Ҫ70��ʱ�����ڣ�������ȫ
// ���ܽ��ܵġ����⣬ʹ��32λ����ʱ�����Խ��и��ָ������޷����е��Ż���
// 
// ��ˣ�����դ���⣬����������¶�ʹ�ø�������������õ�ѡ���ڱ任�͹�����
// ʹ�ø��������ٶȸ��죬���ȸ��ߣ����ڹ�դ���ڼ䣬ʹ�ø������Ŀ������޷��е�
// �ġ�
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

        int    xStart;  // [������] FIXP16_SHIFT
        int    xEnd;    // [������]
        int    kxLeft;  // [������] ����ֱ�ߵ�б��
        int    kxRight; // [������]

        int    zStart;  // [������] FIXP28_SHIFT
        int    zEnd;    // [������]
        int    kzLeft;  // [������]
        int    kzRight; // [������]
        
        int    uStart;  // [������] ɨ��������Ӧ���������귶Χ, FIXP20_SHIFT
        int    vStart;  // [������]
        int    uEnd;    // [������]
        int    vEnd;    // [������]

        int    kuLeft;  // [������] ����������y�����ϵ�������, FIXP20_SHIFT
        int    kvLeft;  // [������]
        int    kuRight; // [������]
        int    kvRight; // [������]
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