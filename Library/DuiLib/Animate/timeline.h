#pragma once
#include "animatemgr.h"

// 2014.2.20  TODO: ����repeat delay���ܡ�ÿһ��ѭ��ʱ������ʱһ��ʱ�䣬��Ҫ������Ļ������ü�һ������״̬���ܣ��Ժ��ټ�

namespace UI
{
#define TIMELINE_FLAG_FINISH       0x0004   // ָʾһ��timeline�Ƿ��Ѿ����ڽ�����
#define TIMELINE_FLAG_NEED_RESET   0x0008   // ��ʾһ��timelineһ��ѭ���������´ο�ʼǰ��Ҫ����
#define TIMELIEN_FLAG_REVERSING    0x0010   // ��ʾ���ڷ�����ж���

    template <class T>
    class ITimelineImpl : public T //public ITimeline
    {
    protected:
        ITimelineImpl()
        {
			m_pMoveAlgorithm = NULL;
            m_nBeginTime = 0;
            m_nFrameElapse = 0;
            m_eTimeType = TT_BY_MS;  // Ĭ���Ժ���Ϊ��λ

            m_nRepeatTimes = 1;  // Ĭ��ֻ����һ��
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

        // virtual void GetCurrentValue(void** ppOut) = 0;  // �ɸ�������ȥʵ��

        // TODO: �ú�������Ӧ����Ϊ�ӿڱ�¶���ⲿ�ġ�����������AnimateManager��Ҳֻ���õ�ITimelineָ�롣
        // ����������취�ܷ���ģ������ʵ�֣�ͬʱ���ܱ���IIntTimeline�̳���ITimeline
        virtual void x_OnAnimateStart()
        {
            this->Init();
            m_nBeginTime = GetTickCount();
        }

        // ע��GetTickCount�ľ��Ⱥܵͣ�ֻ��15/16ms
        // �����Ƿ����
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
			this->OnTick(fValue);  // ֪ͨ�Ӷ���ǰ����ֵ

            if (bTimelineFinish)
            {
                if (m_bAutoReverse)
                {
                    m_nFlags |= TIMELINE_FLAG_NEED_RESET;  // �´ο�ʼǰ�����ö�������

                    // �����ݽ��з���
                    OnReverse();

                    // ����Ľ���������һ�β��Ž���
                    if (m_nFlags&TIMELIEN_FLAG_REVERSING)
                    {
                        if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                        {
                            SetFinish();
                        }
                        m_nFlags &= ~TIMELIEN_FLAG_REVERSING;
                    }
                    // ����Ľ��������뷴��
                    else
                    {
                        m_nFlags |= TIMELIEN_FLAG_REVERSING;
                    }
                }
                else
                {
                    // һ�β��Ž���
                    if (m_nRepeatTimes > 0 && 0 == --m_nRepeatTimes)
                    {
                        SetFinish();
                    }
                    else
                    {
                        m_nFlags |= TIMELINE_FLAG_NEED_RESET;  // �´ο�ʼǰ�����ö�������
                    }
				}
			}

			return IsFinish();
		}

		void  OnReverse()
		{
			// �����ݽ��з���
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
		// ��������麯��
        virtual void  Init() = 0;
        virtual void  OnTick(float fNewValue) = 0; // �Ӷ���floatֵ��ת�����Լ�������

        void  SetAnimateMgrPtr(AnimateManager* p)
        { m_pAnimateMgr = p; }

    protected:
        AnimateManager*  m_pAnimateMgr;
		IMoveAlgorithm*  m_pMoveAlgorithm;

        int      m_nBeginTime;   // ��¼������ʼʱ��time tick����ʱ�䵽��m_nBeginTime+m_nDuretionʱ������������
        int      m_nFrameElapse; // ������֡��
        int      m_nRepeatTimes; // 1��ʾ����һ�Σ�С�ڵ���0��ʾһֱ���ţ���--m_nRepeatTimes==0ʱֹͣ. 
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

	// ʲôҲ������ֻ��Ϊ��ά�ֶ�����ʱ��
	class NoneAnimateTimeline : public ITimelineImpl<INoneTimeline>
	{
	public:
		virtual void  GetCurrentValue(void* ppOut){}
		virtual void  Init();
		virtual void  OnTick(float f){};
		virtual void  OnReverse() {}
	};
}
