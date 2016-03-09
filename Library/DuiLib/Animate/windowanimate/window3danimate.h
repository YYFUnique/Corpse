#ifndef WINDOWANIMATE_H_CBBEE65C_14A5_4811_9AED_65F25D658C01
#define WINDOWANIMATE_H_CBBEE65C_14A5_4811_9AED_65F25D658C01

#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_transform.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowanimatebase.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_texture_mapping.h"

namespace UI
{
class Window3DAnimate : public WindowAnimateBase
{
public:
	Window3DAnimate();
	virtual ~Window3DAnimate();

public:
    virtual void  Initialize();
    virtual void  OnTick(int nCount, IStoryboard** ppTimerArray);

public:
    void  SetIWindow3DAnimate(IWindow3DAnimate* p) { m_pIWindow3DAnimate = p; }
    IWindow3DAnimate*  GetIWindow3DAnimate() { return m_pIWindow3DAnimate; }

    int*  GetRotateXPtr() { return &m_nRotateX; }
    int*  GetRotateYPtr() { return &m_nRotateY; } 
    int*  GetRotateZPtr() { return &m_nRotateZ; } 

    int*  GetOffsetXPtr() { return &m_nOffsetX; } 
    int*  GetOffsetYPtr() { return &m_nOffsetY; } 
    int*  GetOffsetZPtr() { return &m_nOffsetZ; } 
protected:
	void    GetQuadByAnimateValue(int nDegreeX, int nDegreeY, int nDegreeZ, int nZOffset, Quad* pOut);

protected:
    IWindow3DAnimate*  m_pIWindow3DAnimate;

	int     m_nRotateX;
	int     m_nRotateY;
	int     m_nRotateZ;
	int     m_nOffsetX;
	int     m_nOffsetY;
	int     m_nOffsetZ;

	QuadPerspectiveTextureMapping  textmapping;
};

}


#endif // WINDOWANIMATE_H_CBBEE65C_14A5_4811_9AED_65F25D658C01