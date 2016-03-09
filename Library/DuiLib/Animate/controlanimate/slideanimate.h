#ifndef SLIDEANIMATE_H_DAC6CF26_3D53_4c98_BC9E_2743F53EAF59
#define SLIDEANIMATE_H_DAC6CF26_3D53_4c98_BC9E_2743F53EAF59
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "controlanimatebase.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK/Kernel/Src/Base/Message/message.h"
#include "UISDK/Kernel/Src/UIObject/Window/window.h"

namespace UI
{

class SlideAnimateMessageHandler : public Message
{
    UI_BEGIN_MSG_MAP
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_ANIMATE_OVER(OnAnimateOver)
    UI_END_MSG_MAP

    void  OnAnimateTick(int nCount, IStoryboard** ppArray);
    void  OnAnimateOver(){};
};

class SlideAnimate : public ControlAnimateBase
{
public:
    SlideAnimate();
    ~SlideAnimate();

    void  SetISlideAnimate(ISlideAnimate* p)
    {
        m_pISlideAnimate = p;
    }
    bool  Slide(IObject* pObj1, IObject* pObj2, CRect* prcCommitInWindow, int nFlags);

    void  OnAnimateTick(int nValue);
    void  OnAnimateOver();

protected:
    ISlideAnimate*  m_pISlideAnimate;
    WindowBase*  m_pWindow;
    CRect  m_rcCommit;

    IObject*  m_pObjLeft;
    IObject*  m_pObjRight;
    Image  m_imageObjLeft;
    Image  m_imageObjRight;
    Image  m_imageObjBkgnd;
    int  m_nFlags;
};

}

#endif // SLIDEANIMATE_H_DAC6CF26_3D53_4c98_BC9E_2743F53EAF59