#pragma  once

#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_transform.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowanimatebase.h"

namespace UI
{
class WindowAlphaShowAnimate : public WindowAnimateBase
{
public:
    WindowAlphaShowAnimate();
    virtual ~WindowAlphaShowAnimate();

public:
    virtual void  Initialize();
    virtual void  OnTick(int nCount, IStoryboard** ppTimerArray);

public:
    void  SetIWindowAlphaShowAnimate(IWindowAlphaShowAnimate* p) { m_pIWindowAlphaShowAnimate = p; }
    IWindowAlphaShowAnimate*  GetIWindowAlphaShowAnimate() { return m_pIWindowAlphaShowAnimate; }

    bool  Show(int nId, int nDuration);
    bool  Hide(int nId, int nDuration);

protected:
    bool  DoAction(int nId, int nDuration, bool bShow);

protected:
    IWindowAlphaShowAnimate*  m_pIWindowAlphaShowAnimate;
    bool  m_bShow;
};

}