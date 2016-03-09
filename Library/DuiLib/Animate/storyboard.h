#pragma once
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

namespace UI
{
interface ITimeline;
class  AnimateManager;

typedef vector<ITimeline*>  TimelineList;
typedef vector<ITimeline*>::iterator  TimelineIter;


// 用于管理一个动画中的几个timeline 
class Storyboard
{
public:
    Storyboard();
    ~Storyboard();

    bool  AddTimeline(ITimeline* p);
    bool  DestroyTimeline(int nTimelineId);
    ITimeline*  GetTimeline(unsigned int nIndex);
    ITimeline*  FindTimeline(int nTimelineId);
    IIntTimeline*  CreateIntTimeline(int nTimelineId);
	IFloatTimeline*  CreateFloatTimeline(int nTimelineId);
	INoneTimeline*   CreateNoneTimeline(int nTimelineId);

    IMessage*  GetNotifyObj();
    void  SetNotifyObj(IMessage* pNotify);
    void  SetId(int nID) { m_nID = nID; }
    int   GetId() { return m_nID; }
    void    SetWParam(WPARAM wParam) { m_pWParam = wParam; }
    WPARAM  GetWParam() { return m_pWParam; }
    void    SetLParam(LPARAM lParam) { m_pLParam = lParam; }
    LPARAM  GetLParam() { return m_pLParam; }

    void  Begin();
    void  BeginBlock();
    
    void  SetIStoryboard(IStoryboard* p) { m_pIStoryboard = p; };
    IStoryboard*  GetIStoryboard() { return m_pIStoryboard; }

public:
    void  SetAnimateMgr(AnimateManager*  p);
    void  OnAnimateStart();
    void  SetFinish();
    bool  IsFinish();
    bool  OnTick();

protected:
    void  DestroyAllTimeline();
    
protected:
    AnimateManager*    m_pAnimateMgr;
    IStoryboard*       m_pIStoryboard;
    IMessage*          m_pNotify;
    int                m_nID;
    WPARAM             m_pWParam;
    LPARAM             m_pLParam;

    TimelineList       m_listTimeline;  // 使用数组，加快外部直接定位，避免查找
};

}