#include "stdafx.h"
#include "animatemgr.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\window3danimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowupdownalphashowanimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowalphashowanimate.h"
#include "UISDK\Kernel\Src\Animate\controlanimate\slideanimate.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"

AnimateManager::AnimateManager()
{
    m_pIAnimateManager = NULL;
	m_hTimer = NULL;

	m_bHandlingTimerCallback = false;
	m_nFps = 60;
	m_pUIApplication = NULL;

	::QueryPerformanceFrequency(&m_liPerFreq);
}

AnimateManager::~AnimateManager()
{
	UIASSERT(NULL == m_hTimer && 0 == m_listObjStoryboard.size());

	// 注：close timer handle不能在UnCoInitialize后调用
    Destroy();
    SAFE_DELETE(m_pIAnimateManager);
}

IAnimateManager*  AnimateManager::GetIAnimateManager()
{
    if (NULL == m_pIAnimateManager)
        m_pIAnimateManager = new IAnimateManager(this);

    return m_pIAnimateManager;
}
int  AnimateManager::SetFps(int n) 
{ 
    int nOld = m_nFps;
    m_nFps = n; 
    if (m_nFps > 1000)
        m_nFps = 1000;
    return nOld;
}
void AnimateManager::Destroy()
{
	EndAnimate();

	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();
	for (; iter != iterEnd; iter++)
	{
		ObjectStoryboard* p = *iter;
		delete p;
	}
	m_listObjStoryboard.clear();
}

IStoryboard*  AnimateManager::CreateStoryboard(IMessage* pNotify, int nId, WPARAM wParam, LPARAM lParam)
{
    IStoryboard* p = new IStoryboard;
    p->CreateImpl();

	Storyboard* pStoryboard = p->GetImpl();
    pStoryboard->SetAnimateMgr(this);
	pStoryboard->SetNotifyObj(pNotify);
	pStoryboard->SetId(nId);
	pStoryboard->SetWParam(wParam);
	pStoryboard->SetLParam(lParam);

    return p;
}

IWindowAnimateBase* AnimateManager::CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow)
{
	if (E_WINDOW_ANIMATE_3D_ROTATE == eType)
	{
		IWindow3DAnimate* p = new IWindow3DAnimate;
        p->CreateImpl();

        Window3DAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
		pImpl->SetAnimateWindow(pWindow);
        pImpl->Initialize();
		return p;
	}
    else if (E_WINDOW_ANIMATE_UPDOWNALPHASHOW == eType)
    {
        IWindowUpDownAlphaShowAnimate* p = new IWindowUpDownAlphaShowAnimate;
        p->CreateImpl();

        WindowUpDownAlphaShowAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
        pImpl->SetAnimateWindow(pWindow);
        pImpl->Initialize();
        return p;
    }
    else if (E_WINDOW_ANIMATE_ALPHASHOW == eType)
    {
        IWindowAlphaShowAnimate* p = new IWindowAlphaShowAnimate;
        p->CreateImpl();

        WindowAlphaShowAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
        pImpl->SetAnimateWindow(pWindow);
        pImpl->Initialize();
        return p;
    }

	return NULL;
}

IControlAnimateBase*  AnimateManager::CreateControlAnimateInstance(E_CONTROL_ANIMATE_TYPE eType)
{
    if (E_CONTROL_ANIMATE_SLIDE == eType)
    {
        ISlideAnimate* p = new ISlideAnimate;
        p->CreateImpl();

        SlideAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
        return p;
    }

    return NULL;
}

ObjectStoryboard::ObjectStoryboard(IMessage* p)
{
	m_pNotifyObject = p;
	m_pStoryboardList = NULL; 
	m_nCount = 0;
    m_bNeedCheckFinish = false;
}
ObjectStoryboard::~ObjectStoryboard()
{
	if (m_pStoryboardList)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			SAFE_DELETE_Ixxx(m_pStoryboardList[i]);
		}
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_nCount = 0;
	}
}

void AnimateManager::RemoveStoryboard(IStoryboard* p)
{
	if (NULL == p)
		return;

    if (m_bHandlingTimerCallback)  // 如果正在对m_listObjTimeline进行遍历中，则不能在这里对list做erase操作
    {
        p->GetImpl()->SetFinish();
    }
    else
    {
        ObjectStoryboardIter iter = this->FindObjectStoryboardIter(p->GetNotifyObj());
        if (iter == m_listObjStoryboard.end())
            return;

        ObjectStoryboard* pObjTimeline = *iter;
        pObjTimeline->RemoveStoryboard(p);

        if (0 == pObjTimeline->m_nCount)
        {
            m_listObjStoryboard.erase(iter);
            SAFE_DELETE(pObjTimeline);

            if (0 == m_listObjStoryboard.size())
                KillTimer();
        }
    }
}
void AnimateManager::ClearStoryboardOfNotify(IMessage* pMsg)
{
	if (NULL == pMsg)
		return;

	ObjectStoryboardIter iter = this->FindObjectStoryboardIter(pMsg);
	if (iter == m_listObjStoryboard.end())
		return;

	if (m_bHandlingTimerCallback)  // 如果正在对m_listObjTimeline进行遍历中，则不能在这里对list做erase操作
	{
		ObjectStoryboard* pObjTimeline = *iter;
		for (int i = 0; i < pObjTimeline->m_nCount; i++)
		{
			pObjTimeline->m_pStoryboardList[i]->GetImpl()->SetFinish();;
		}
	}
	else
	{
		SAFE_DELETE(*iter);
		m_listObjStoryboard.erase(iter);

		if (0 == m_listObjStoryboard.size())
		{
			KillTimer();
		}
	}
}

void AnimateManager::AddStoryboard(IStoryboard* p)
{
	if (NULL == p)
		return;

	ObjectStoryboard* pObjStoryboard = this->FindObjectStoryboard(p->GetNotifyObj());
	if (NULL == pObjStoryboard)
	{
		pObjStoryboard = new ObjectStoryboard(p->GetNotifyObj());
		m_listObjStoryboard.push_back(pObjStoryboard);
	}
	pObjStoryboard->AddStoryboard(p);

	if (NULL == m_hTimer)
		StartAnimate();

    p->GetImpl()->OnAnimateStart();
}

// 阻塞型动画
void  AnimateManager::AddStoryboardBlock(IStoryboard* p)
{
    if (!p)
        return;

    Storyboard*  pStoryboard = p->GetImpl();
    IMessage*    pNotify = pStoryboard->GetNotifyObj();

    if (NULL == m_hTimer)
        StartAnimate();

    p->GetImpl()->OnAnimateStart();

    bool bFinish = false;
    while (1)
    {
        bFinish = pStoryboard->OnTick();
        if (pNotify)
        {
            UISendMessage(pNotify, UI_WM_ANIMATE_TICK, (WPARAM)1, (LPARAM)&p);
        }

        if (bFinish)
        {
            if (pNotify)
            {
                UISendMessage(pNotify, UI_WM_ANIMATE_OVER);  // TODO: 有可能该对象还有其它非阻塞型动画
            }
            SAFE_DELETE_Ixxx(p);

            if (0 == m_listObjStoryboard.size())
                KillTimer();

            return;
        }
        else
        {
            // sleep效率太低，换成使用Waitable Timer
            WaitForSingleObject(m_hTimer,INFINITE); 
        }
    }
}

// 查找相同ID的time line，要添加时，如果发现该ID已经存在，则先取消前一个time line
int ObjectStoryboard::FindStoryboard(int nID)
{
	for (int i = 0; i < m_nCount; i++)
	{
		if (m_pStoryboardList[i]->GetId() == nID)
			return i;
	}
	return -1;
}

void ObjectStoryboard::AddStoryboard(IStoryboard* p)
{
	if (NULL == p)
		return;

	int nIndex = -1;
	if (0 != p->GetId())
	{
		nIndex = this->FindStoryboard(p->GetId());
	}
	if (-1 == nIndex)
	{
		IStoryboard** ppArray = new IStoryboard*[m_nCount+1];
		memcpy(ppArray, m_pStoryboardList, sizeof(ITimeline*)*m_nCount);
		ppArray[m_nCount] = p;

		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_pStoryboardList = ppArray;
		m_nCount++;
	}
	else
	{
		// 替换
		SAFE_DELETE_Ixxx(m_pStoryboardList[nIndex]);
		m_pStoryboardList[nIndex] = p;
	}
}

void ObjectStoryboard::RemoveStoryboard(IStoryboard* p)
{
	UIASSERT(m_nCount > 0);
	if (NULL == p)
		return;

	if (m_nCount > 1)
	{
		IStoryboard** ppArray = new IStoryboard*[m_nCount-1];

		int nIndex = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			if (m_pStoryboardList[i] == p)
			{
				SAFE_DELETE_Ixxx(m_pStoryboardList[i]);
			}
			else
			{
				ppArray[nIndex++] = m_pStoryboardList[i];
			}
		}
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_pStoryboardList = ppArray;

		m_nCount--;
	}
	else if (m_nCount == 1)
	{
		SAFE_DELETE_Ixxx(m_pStoryboardList[0]);
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_nCount--;
	}
}

void ObjectStoryboard::CheckFinishFlag()
{
    if (!m_bNeedCheckFinish)
        return;

    m_bNeedCheckFinish = false;

	int nAliveCount = 0;
	for (int i = 0; i < m_nCount; i++)
	{
		if (!m_pStoryboardList[i]->IsFinish())
			nAliveCount++;
	}
    if (nAliveCount == m_nCount)
        return;

	if (0 == nAliveCount)
	{
		if (m_pStoryboardList)
		{
			for (int i = 0; i < m_nCount; i++)
			{
				SAFE_DELETE_Ixxx(m_pStoryboardList[i]);
			}
			SAFE_ARRAY_DELETE(m_pStoryboardList);
		}
	}
	else
	{
		IStoryboard** ppArray = new IStoryboard*[nAliveCount];

		int nIndex = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			if (m_pStoryboardList[i]->IsFinish())
			{
				SAFE_DELETE_Ixxx(m_pStoryboardList[i]);				
			}
			else
			{
				ppArray[nIndex++] = m_pStoryboardList[i];
			}
		}
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_pStoryboardList = ppArray;
	}

	m_nCount = nAliveCount;
}

ObjectStoryboard* AnimateManager::FindObjectStoryboard(IMessage* pNotify)
{
	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter)->m_pNotifyObject == pNotify)
			return (*iter);
	}
	return NULL;
}
ObjectStoryboardIter AnimateManager::FindObjectStoryboardIter(IMessage* pNotify)
{
	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter)->m_pNotifyObject == pNotify)
			return iter;
	}
	return m_listObjStoryboard.end();
}

void AnimateManager::OnWaitForHandleObjectCallback(HANDLE h, LPARAM l)
//void  AnimateManager::OnTime()
{ 
// 	static long lPrev = 0;
// 	static LARGE_INTEGER liPrev = {0};
// 	static LARGE_INTEGER liNow = {0};
// 	::QueryPerformanceCounter(&liNow);
// 	 int time = (int)((liNow.QuadPart - liPrev.QuadPart)*1000/m_liPerFreq.QuadPart);
// 
// 	char  szText[64] = {0};
// 	sprintf(szText, "%d\n", time);
// 	liPrev = liNow;
// 	::OutputDebugStringA(szText);
	
	m_bHandlingTimerCallback = true;

	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();

	for (; iter != iterEnd;)
	{
		ObjectStoryboard* pObjStoryboard = *iter;
		for (int i = 0; i < pObjStoryboard->m_nCount; i++)
		{
			pObjStoryboard->m_pStoryboardList[i]->GetImpl()->OnTick();
		}

		// 统一发送消息。有可能一个pNotify添加了多个属性变化的动画，但只能发送一个通知，让它知道所有值的最终结果即可
        // 这样也只用刷新一次
		if (pObjStoryboard->m_pNotifyObject)
		{
			UISendMessage(pObjStoryboard->m_pNotifyObject, UI_WM_ANIMATE_TICK, (WPARAM)pObjStoryboard->m_nCount, (LPARAM)pObjStoryboard->m_pStoryboardList);
		}
        // 	注：有可能在UISendMessage中调用了RemoveStoryboard

        pObjStoryboard->CheckFinishFlag();
        if (0 == pObjStoryboard->m_nCount)
        {
            // 通知对象它的动画结束了
            if (pObjStoryboard->m_pNotifyObject)
            {
                UISendMessage(pObjStoryboard->m_pNotifyObject, UI_WM_ANIMATE_OVER);
            }

            iter = m_listObjStoryboard.erase(iter);
            SAFE_DELETE(pObjStoryboard);

            if (0 == m_listObjStoryboard.size())
                KillTimer();
        }
        else
        {
            iter++;
        }
	}

	m_bHandlingTimerCallback = false;
}

VOID CALLBACK TimerAPCProc(
						   LPVOID lpArgToCompletionRoutine,
						   DWORD dwTimerLowValue,
						   DWORD dwTimerHighValue
						   )
{
	AnimateManager*  pThis = (AnimateManager*)lpArgToCompletionRoutine;
	//pThis->OnTime();
}

void AnimateManager::SetTimer()
{
	if (m_hTimer)
		return;

	if (0 == m_nFps)
		m_nFps = 10;

	int nPeriod = 1000/m_nFps;  // 计时器周期

	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	IWaitForHandlesMgr* pMgr = NULL;
	m_pUIApplication->GetWaitForHandlesMgr(&pMgr);
	if (pMgr)
	{
		pMgr->AddHandle(m_hTimer, static_cast<IWaitForHandleCallback*>(this), 0);
	}

    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -1000*10*nPeriod;   // 第一次响应延迟时间。负值表示一个相对的时间，代表以100纳秒为单位的相对时间，（如从现在起的5ms，则设置为-50000）

    // 注： 不要使用TimerAPCProc,该方式需要让线程处理alertable状态，
    //      即调用SleepEx(x, TRUE)让线程进入等待状态
    // LONG lPeriod：设置定时器周期性的自我激发，该参数的单位为毫秒。
    // 如果为0，则表示定时器只发出一次信号，大于0时，定时器没隔一段时
    // 间自动重新激活一个计时器，直至取消计时器使用
    if (!SetWaitableTimer(m_hTimer, &liDueTime, nPeriod, NULL, NULL, 0))  
    {
        UIASSERT(0);
    }

// 	SleepEx
// 	if (!SetWaitableTimer(m_hTimer, &liDueTime, nPeriod, TimerAPCProc, (LPVOID)this, 0))
// 	{
// 		UIASSERT(0);
// 	}
}
void AnimateManager::KillTimer()
{
	if (m_hTimer)
	{
		IWaitForHandlesMgr* pMgr = NULL;
		m_pUIApplication->GetWaitForHandlesMgr(&pMgr);
		if (pMgr)
			pMgr->RemoveHandle(m_hTimer);

		// CancelWaitableTimer 
		CloseHandle(m_hTimer);
		m_hTimer = NULL;
	}
}
void AnimateManager::StartAnimate()
{
	KillTimer();
	SetTimer();
}
void AnimateManager::EndAnimate()
{
	KillTimer();
}




