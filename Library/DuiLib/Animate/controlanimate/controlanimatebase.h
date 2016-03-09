#ifndef CONTROLANIMATEBASE_H_8B91B989_1615_4122_8AE0_CDEBD5F5CB4D
#define CONTROLANIMATEBASE_H_8B91B989_1615_4122_8AE0_CDEBD5F5CB4D
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\Animate\storyboard.h"

namespace UI
{

class ControlAnimateBase : public Storyboard
{

public:
    ControlAnimateBase();
    ~ControlAnimateBase();

    void SetIControlAnimateBase(IControlAnimateBase* p)
    {
        m_pIControlAnimateBase = p;
    }
    IControlAnimateBase*  GetIControlAnimateBase()
    {
        return m_pIControlAnimateBase;
    }

protected:
    IControlAnimateBase*  m_pIControlAnimateBase;

};

}

#endif // CONTROLANIMATEBASE_H_8B91B989_1615_4122_8AE0_CDEBD5F5CB4D