#include "stdafx.h"
#include "storyboard.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "timeline.h"

Storyboard::Storyboard()
{
    m_pAnimateMgr = NULL;
    m_pIStoryboard = NULL;
    m_pNotify = NULL;
    m_nID = 0;
    m_pWParam = m_pLParam = 0;
}
Storyboard::~Storyboard()
{
    DestroyAllTimeline();
}

void  Storyboard::SetAnimateMgr(AnimateManager*  p)
{
    m_pAnimateMgr = p;
}

bool  Storyboard::AddTimeline(ITimeline* p)
{
    if (NULL == p)
        return false;

    if (NULL != FindTimeline(p->GetId()))
    {
        UIASSERT(0);
        return false;
    }

    m_listTimeline.push_back(p);
    return true;
}

bool  Storyboard::DestroyTimeline(int nTimelineId)
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        ITimeline*  p = *iter;
        if (p->GetId() == nTimelineId)
        {
            m_listTimeline.erase(iter);
            SAFE_DELETE(p);
            return true;
        }
    }
    return false;
}

IIntTimeline*  Storyboard::CreateIntTimeline(int nTimelineId)
{
	IntTimeline* p = new IntTimeline();
	p->SetAnimateMgrPtr(m_pAnimateMgr);
	
	p->SetId(nTimelineId);
	if (false == AddTimeline(p))
	{
		SAFE_DELETE(p);
		return NULL;
	}

	return p;
}
IFloatTimeline*  Storyboard::CreateFloatTimeline(int nTimelineId)
{
	FloatTimeline* p = new FloatTimeline();
	p->SetAnimateMgrPtr(m_pAnimateMgr);

	p->SetId(nTimelineId);
	if (false == AddTimeline(p))
	{
		SAFE_DELETE(p);
		return NULL;
	}

	return p;
}

INoneTimeline*  Storyboard::CreateNoneTimeline(int nTimelineId)
{
	NoneAnimateTimeline* p = new NoneAnimateTimeline();
	p->SetAnimateMgrPtr(m_pAnimateMgr);

	p->SetId(nTimelineId);
	if (false == AddTimeline(p))
	{
		SAFE_DELETE(p);
		return NULL;
	}

	return static_cast<INoneTimeline*>(p);
}
void  Storyboard::DestroyAllTimeline()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        ITimeline*  p = *iter;
        SAFE_DELETE(p);
    }
    m_listTimeline.clear();
}

ITimeline*  Storyboard::GetTimeline(unsigned int nIndex)
{
    if (nIndex >= m_listTimeline.size())
        return NULL;

    return m_listTimeline[nIndex];
}
ITimeline*  Storyboard::FindTimeline(int nTimelineId)
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        if ((*iter)->GetId() == nTimelineId)
            return *iter;
    }

    return NULL;
}

void  Storyboard::Begin()
{
    m_pAnimateMgr->AddStoryboard(m_pIStoryboard);
}
void  Storyboard::BeginBlock()
{
    m_pAnimateMgr->AddStoryboardBlock(m_pIStoryboard);
}

IMessage*  Storyboard::GetNotifyObj()
{
    return m_pNotify;
}
void  Storyboard::SetNotifyObj(IMessage* pNotify)
{
    m_pNotify = pNotify;
}


// TODO: 优化
void  Storyboard::OnAnimateStart()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        (*iter)->x_OnAnimateStart();
    }
}

void  Storyboard::SetFinish()
{
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        (*iter)->SetFinish();
    }

    // 在AnimateMgr::OnTick中将释放自己
    ObjectStoryboard* p = m_pAnimateMgr->FindObjectStoryboard(m_pNotify);
    if (p)
    {
        p->SetNeedCheckFinishFlag(true);
    }
}

bool  Storyboard::IsFinish()
{   
    TimelineIter iter = m_listTimeline.begin();
    for (; iter != m_listTimeline.end(); iter++)
    {
        if (false == (*iter)->IsFinish())
            return false;
    }
    return true;
}

bool  Storyboard::OnTick()
{
    bool bAllFinish = true;
    unsigned int nSize = m_listTimeline.size();
    for (unsigned int i = 0; i < nSize; i++)
    {
        if (!m_listTimeline[i]->x_OnTick())
        {
            bAllFinish = false;
        }
    }

    if (bAllFinish)
    {
        ObjectStoryboard* p = m_pAnimateMgr->FindObjectStoryboard(m_pNotify);
        if (p)
        {
            p->SetNeedCheckFinishFlag(true);
        }
    }

    return bAllFinish;
}