#pragma  once
#include "UISDK\Kernel\Inc\Util\layeredwindow.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"

namespace UI
{
interface  ILayeredAnimateWindow;
class  WindowAnimateBase;
class  WindowBase;

class LayeredAnimateWindow : public CLayeredWindow, public Message
{
public:
    LayeredAnimateWindow();
    ~LayeredAnimateWindow();

    void  SetILayeredAnimateWindow(ILayeredAnimateWindow* p) { m_pILayeredAnimateWindow = p; }
    ILayeredAnimateWindow*  GetILayeredAnimateWindow() { return m_pILayeredAnimateWindow;}

    void  SetWindowAnimate(WindowAnimateBase* p) { m_pWindowAnimate = p; }

    void  Soul_Attach(WindowBase* pWnd);
    BOOL  nvProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook);


private:
    ILayeredAnimateWindow*  m_pILayeredAnimateWindow;
    WindowBase*             m_pWindow;
    WindowAnimateBase*      m_pWindowAnimate;
    LONG                    m_lOldExStyle;
};


interface  ILayeredAnimateWindow : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(ILayeredAnimateWindow, LayeredAnimateWindow);
};

}