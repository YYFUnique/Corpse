#pragma once
#include "animatemgr.h"

// 2014.2.20  TODO: 增加repeat delay功能。每一次循环时可以延时一段时间，但要加这个的话，还得加一个动画状态功能，以后再加

namespace UI
{
#define TIMELINE_FLAG_FINISH       0x0004   // 指示一个timeline是否已经到期结束了
#define TIMELINE_FLAG_NEED_RESET   0x0008   // 表示一个timeline一次循环结束，下次开始前需要重置
#define TIMELIEN_FLAG_REVERSING    0x0010   // 表示正在反向进行动画

    template <class T>
    class ITimelineImpl : public T //public ITimeline
    {
    protected:
        ITimelineImpl()
        {
			m_pMoveAlgorithm = NULL;
            m_nBeginTime = 0;
            m_nFrameElapse = 0;
            m_eTimeType = TT_BY_MS;  // 默认以毫秒为单位

            m_nRepeatTimes = 1;  // 默认只播放一次
            m_bAutoReverse = false;

            m_nFlags = 0;
            m_nID = 0;
            m_pParam = NULL;
        }

    public:
        virtual ~ITimelineImpl()
		{
			SAFE_DELETE(m_pMoveAlgorithm);
		};
        virtual void  Destroy() 
        { 
            delete this; 
        }
        virtual void  SetRepeateCount(int n) 
        { 
            m_nRepeatTimes = n; 
        }
        virtual bool  IsFinish() 
        { 
            return m_nFlags & TIMELINE_FLAG_FINISH ? true:false; 
        }
        virtual void SetFinish() 
        { 
            m_nFlags |= TIMELINE_FLAG_FINISH;
        }
        virtual void  SetUserData(LPARAM lParam)
        { 
            m_pParam = lParam;
        }
        virtual LPARAM  GetUserData() 
        { 
            return m_pParam; 
        }
        virtual void  SetId(int nID) 
        { 
            m_nID = nID; 
        }
        virtual int  GetId() 
        { 
            return m_nID; 
        }
        virtual void  SetTimeType(TIMELINE_TIME_TYPE eType) 
        {
            m_eTimeType = eType; 
        }
        virtual void  SetAutoReverse(bool b)
        {
            m_bAutoReverse = b;
        }

        // virtual void GetCurrentValue(void** ppOut) = 0;  // 由各个子类去实现

        // TODO: 该函数本不应该作为接口暴露给外部的。但是无奈在AnimateManager中也只能拿到ITimeline指针。
        // 后面再想想办法能否不用模块类来实现，同时还能保存IIntTimeline继承于ITimeline
        virtual void x_OnAnimateStart()
        {
            this->Init();
            m_nBeginTime = GetTickCount();
        }

        // 注：GetTickCount的精度很低，只有15/16ms
        // 返回是否结束
        virtual bool x_OnTick()
        {
            if (m_nFlags & TIMELINE_FLAG_NEED_RESET)
            {
                m_nFlags &= ~TIMELINE_FLAG_NEED_RESET;

                m_nFrameElapse = 0;
                m_nBeginTime = GetTickCount();
            }
            else if (IsFinish())
            {
                return true;
            }

			if (!m_pMoveAlgorithm)
			{
				return true;
			}

            m_nFrameElapse ++;

            int nTimeElapse = 0;
            switch (m_eTimeType)
            {
            case TT_BY_FRAME:
                nTimeElapse = m_nFrameElapse;
                break;

            case TT_BY_MS:
                nTimeElapse = GetTickCount() - m_nBeginTime;
                break;

            case TT_BY_SECOND:
                nTimeElapse = (GetTickCount() - m_nBeginTime)/1000;
            }

//             TCHAR szText[8];
//             _stprintf(szText, _T("%d\n"), nTimeElapse);
//             OutputDebugString(szText);

			float fValue = 0;
			bool  bTimelineFinish = m_pMoveAlgorithm->OnTick(nTimeElapse, &fValue);
			this->OnTick(fValue);  // 通知子对象当前的新值

            if (bTimelineFinish)
            {
                if (m_bAutoReverse)
                {
                    m_nFlags |= TIMELINE_FLAG_NEED_RESET;  // 下次开始前先重置动画参数

                    // 将数据进行反向
                    OnReverse();

                    // 反向的结束，代表一次播放结束
                    if (m_nFlags&TIMELIEN_FLAG_REVERSING)
                    {
                        if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                        {
                            SetFinish();
                        }
                        m_nFlags &= ~TIMELIEN_FLAG_REVERSING;
                    }
                    // 正向的结束，进入反向
                    else
                    {
                        m_nFlags |= TIMELIEN_FLAG_REVERSING;
                    }
                }
                else
                {
                    // 一次播放结束
                    if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                    {
                        SetFinish();
                    }
                    else
                    {
                        m_nFlags |= TIMELINE_FLAG_NEED_RESET;  // 下次开始前先重置动画参数
                    }
				}
			}

			return IsFinish();
		}

		void  OnReverse()
		{
			// 将数据进行反向
			m_pMoveAlgorithm->Reverse();
		}

	protected:
		void  _SetEaseParam(float from, float to, float t, EaseType eType)
		{
			SAFE_DELETE(m_pMoveAlgorithm);
			EasingMove* p = new EasingMove;
			p->SetParam(from, to, t, eType);

			m_pMoveAlgorithm = static_cast<IMoveAlgorithm*>(p);
		}

		void  _SetLinerParam1(float from, float to, float t)
		{
			SAFE_DELETE(m_pMoveAlgorithm);
			LinearMove* p = new LinearMove;
			p->SetParam1(from, to, t);

			m_pMoveAlgorithm = static_cast<IMoveAlgorithm*>(p);
		}

		void  _SetAccelerateParam1(float from, float to, float t, float Vo)
		{
			SAFE_DELETE(m_pMoveAlgorithm);
			AccelerateMove* p = new AccelerateMove;
			p->SetParam1(from, to, t, Vo);

			m_pMoveAlgorithm = static_cast<IMoveAlgorithm*>(p);
		}

	public:
		// 给子类的虚函数
        virtual void  Init() = 0;
        virtual void  OnTick(float fNewValue) = 0; // 子对象将float值，转换成自己的类型

        void  SetAnimateMgrPtr(AnimateManager* p)
        { m_pAnimateMgr = p; }

    protected:
        AnimateManager*  m_pAnimateMgr;
		IMoveAlgorithm*  m_pMoveAlgorithm;

        int      m_nBeginTime;   // 记录动画开始时的time tick，当时间到达m_nBeginTime+m_nDuretion时，动画结束。
        int      m_nFrameElapse; // 经历的帧数
        int      m_nRepeatTimes; // 1表示播放一次，小于等于0表示一直播放，当--m_nRepeatTimes==0时停止. 
        bool     m_bAutoReverse;

        int      m_nFlags;
        TIMELINE_TIME_TYPE  m_eTimeType;

        int      m_nID;
        LPARAM   m_pParam;
    };


	class IntTimeline : public ITimelineImpl<IIntTimeline>
	{
	public:
		IntTimeline();
		~IntTimeline();

        virtual void  GetCurrentValue(void* pOut);
        virtual void  SetOutRef(int* pRef);
		virtual void  SetEaseParam(int from, int to, int t, EaseType eType);
		virtual void  SetLinerParam1(int from, int to, int t);
		virtual void  SetAccelerateParam1(int from, int to, int t, float Vo);

	public:
		virtual void  Init();
        virtual void  OnTick(float fNewValue);

	protected:
		int     m_nCurValue;
		int*    m_pOutRef;
	};

    class FloatTimeline : public ITimelineImpl<IFloatTimeline>
    {
    public:
        FloatTimeline();
        ~FloatTimeline();

        virtual void  GetCurrentValue(void* pOut);
        virtual void  SetOutRef(float* pRef);
		virtual void  SetEaseParam(float from, float to, float t, EaseType eType);
		virtual void  SetLinerParam1(float from, float to, float t);
		virtual void  SetAccelerateParam1(float from, float to, float t, float Vo);

    public:
        virtual void  Init();
        virtual void  OnTick(float fNewValue);

    protected:
        float     m_fCurValue;
        float*    m_pOutRef;
    };

	// 什么也不做，只是为了维持动画计时器
	class NoneAnimateTimeline : public ITimelineImpl<INoneTimeline>
	{
	public:
		virtual void  GetCurrentValue(void* ppOut){}
		virtual void  Init();
		virtual void  OnTick(float f){};
		virtual void  OnReverse() {}
	};
}
