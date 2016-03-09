#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowanimatebase.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\window3danimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowupdownalphashowanimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowalphashowanimate.h"
#include "UISDK\Kernel\Src\Animate\controlanimate\controlanimatebase.h"
#include "UISDK\Kernel\Src\Animate\controlanimate\slideanimate.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IStoryboard, Storyboard)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl3(IStoryboard, Storyboard)

UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindowAnimateBase, WindowAnimateBase)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindowAnimateBase, WindowAnimateBase, Storyboard)

UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindow3DAnimate, Window3DAnimate)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindow3DAnimate, Window3DAnimate, WindowAnimateBase)

UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate, WindowAnimateBase)

UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IWindowAlphaShowAnimate, WindowAlphaShowAnimate)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IWindowAlphaShowAnimate, WindowAlphaShowAnimate, WindowAnimateBase)

UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(IControlAnimateBase, ControlAnimateBase)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IControlAnimateBase, ControlAnimateBase, Storyboard)

UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(ISlideAnimate, SlideAnimate)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(ISlideAnimate, SlideAnimate, ControlAnimateBase)

ITimeline*  IStoryboard::FindTimeline(int nTimelineId)   { return m_pStoryboardImpl->FindTimeline(nTimelineId); }
ITimeline*  IStoryboard::GetTimeline(unsigned int nIndex){ return m_pStoryboardImpl->GetTimeline(nIndex); }
IIntTimeline*    IStoryboard::CreateIntTimeline(int nTimelineId)   { return m_pStoryboardImpl->CreateIntTimeline(nTimelineId); }
IFloatTimeline*  IStoryboard::CreateFloatTimeline(int nTimelineId) { return m_pStoryboardImpl->CreateFloatTimeline(nTimelineId); }
INoneTimeline*   IStoryboard::CreateNoneTimeline(int nTimelineId)  { return m_pStoryboardImpl->CreateNoneTimeline(nTimelineId); }
void  IStoryboard::Begin()                               { m_pStoryboardImpl->Begin(); }
void  IStoryboard::BeginBlock()                          { m_pStoryboardImpl->BeginBlock(); }
IMessage*  IStoryboard::GetNotifyObj()                   { return m_pStoryboardImpl->GetNotifyObj(); }
void  IStoryboard::SetNotifyObj(IMessage* pNotify)       { m_pStoryboardImpl->SetNotifyObj(pNotify); }
void  IStoryboard::SetId(int nID)                        { m_pStoryboardImpl->SetId(nID) ;}
int   IStoryboard::GetId()                               { return m_pStoryboardImpl->GetId(); }
bool  IStoryboard::IsFinish()                            { return m_pStoryboardImpl->IsFinish(); }
void    IStoryboard::SetWParam(WPARAM wParam)            { m_pStoryboardImpl->SetWParam(wParam); }
WPARAM  IStoryboard::GetWParam()                         { return m_pStoryboardImpl->GetWParam(); }
void    IStoryboard::SetLParam(LPARAM lParam)            { m_pStoryboardImpl->SetLParam(lParam); }
LPARAM  IStoryboard::GetLParam()                         { return m_pStoryboardImpl->GetLParam(); }

void  IWindowAnimateBase::UpdateWindowSourceImage()
{
    m_pWindowAnimateBaseImpl->UpdateWindowSourceImage();
}

//////////////////////////////////////////////////////////////////////////

int*  IWindow3DAnimate::GetRotateXPtr() { return m_pWindow3DAnimateImpl->GetRotateXPtr(); }
int*  IWindow3DAnimate::GetRotateYPtr() { return m_pWindow3DAnimateImpl->GetRotateYPtr(); }
int*  IWindow3DAnimate::GetRotateZPtr() { return m_pWindow3DAnimateImpl->GetRotateZPtr(); }

int*  IWindow3DAnimate::GetOffsetXPtr() { return m_pWindow3DAnimateImpl->GetOffsetXPtr(); }
int*  IWindow3DAnimate::GetOffsetYPtr() { return m_pWindow3DAnimateImpl->GetOffsetYPtr(); }
int*  IWindow3DAnimate::GetOffsetZPtr() { return m_pWindow3DAnimateImpl->GetOffsetZPtr(); }

//////////////////////////////////////////////////////////////////////////

bool  IWindowUpDownAlphaShowAnimate::Show(int nId, int nDuration)
{
    return m_pWindowUpDownAlphaShowAnimateImpl->Show(nId, nDuration);
}
bool  IWindowUpDownAlphaShowAnimate::Hide(int nId, int nDuration)
{
    return m_pWindowUpDownAlphaShowAnimateImpl->Hide(nId, nDuration);
}

//////////////////////////////////////////////////////////////////////////

bool  IWindowAlphaShowAnimate::Show(int nId, int nDuration)
{
    return m_pWindowAlphaShowAnimateImpl->Show(nId, nDuration);
}
bool  IWindowAlphaShowAnimate::Hide(int nId, int nDuration)
{
    return m_pWindowAlphaShowAnimateImpl->Hide(nId, nDuration);
}


IAnimateManager::IAnimateManager(AnimateManager* p)
{
    m_pImpl = p;
}
AnimateManager*  IAnimateManager::GetImpl()
{
    return m_pImpl;
}

int   IAnimateManager::SetFps(int n) { return m_pImpl->SetFps(n); }
// void  IAnimateManager::AddStoryboard(IStoryboard* p)   { return m_pImpl->AddStoryboard(p); }
void  IAnimateManager::ClearStoryboardOfNotify(IMessage* pMsg) { return m_pImpl->ClearStoryboardOfNotify(pMsg); }
void  IAnimateManager::RemoveStoryboard(IStoryboard* p){ return m_pImpl->RemoveStoryboard(p); }

IStoryboard*  IAnimateManager::CreateStoryboard(IMessage* pNotify, int nId, WPARAM wParam, LPARAM lParam) { return m_pImpl->CreateStoryboard(pNotify, nId, wParam, lParam); }
IWindowAnimateBase*  IAnimateManager::CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow)
{ return m_pImpl->CreateWindowAnimateInstance(eType, pWindow) ; }
IControlAnimateBase*  IAnimateManager::CreateControlAnimateInstance(E_CONTROL_ANIMATE_TYPE eType)
{ return m_pImpl->CreateControlAnimateInstance(eType); }

bool  ISlideAnimate::Slide(IObject* pObj1, IObject* pObj2, CRect* prcCommitInWindow, int nFlags)
{ return m_pSlideAnimateImpl->Slide(pObj1, pObj2, prcCommitInWindow, nFlags); }
}
