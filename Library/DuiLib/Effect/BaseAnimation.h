/************************************************************************/
/* Desc  ���������࣬�������Զ��嶯��        
    Author ��Skilla��QQ��848861075��                                                      */
/************************************************************************/
#ifndef _BASEANIMETION_H_
#define _BASEANIMETION_H_

#include <vector>
namespace Skill
{
	class BaseAnimation;
	class AnimationListener
	{
	public:  
		virtual LRESULT OnPlayStart(std::int64_t startTime,std::int64_t totalTime,bool bReverse,bool bLoop)
		{
			return 0;
		}
		virtual LRESULT OnPause(std::int64_t runningTime,std::int64_t totalTime,bool bReverse,bool bLoop)
		{
			return 0;
		}
		virtual LRESULT OnResume(std::int64_t runningTime,std::int64_t totalTime,bool bReverse,bool bLoop)
		{
			return 0;
		}
		virtual LRESULT OnPlaying(std::int64_t runningTime,std::int64_t totalTime,bool bReverse,bool bLoop)
		{
			return 0;
		}
		virtual LRESULT OnPlayEnd(std::int64_t runningTime,std::int64_t totalTime,bool bReverse,bool bLoop)
		{
			return 0;
		}
		virtual LRESULT OnStop(std::int64_t runningTime,std::int64_t totalTime,bool bReverse,bool bLoop)
		{
			return 0;
		}

		BaseAnimation* m_pOwner;
	};

	class SKILL_API BaseAnimation : public Thread ,public RefCntObject
	{
	public:
        BaseAnimation(String name);
		virtual ~BaseAnimation();
		void RegistListener(AnimationListener* l);   //ע�����
		void PlayAnimation();                              //��ʼ���Ŷ���
		void PauseAnimation();                            //��ͣ����
		void ResumeAnimation();                         //��������
		void StopAnimation();                             //ֹͣ����
		void SeekAnimation(std::int64_t _time);    //ָ������λ��
		void SetLoop(bool bLoop);                       //�Ƿ�ѭ��
		void SetReverse(bool bReverse);              //�Ƿ����򲥷�
		bool GetReverse();
		virtual bool FirstRun() = 0;
		virtual void Run() = 0;
		virtual bool LastRun() = 0;
	    void  run();
        static void Flush();
        std::int64_t GetTotalTime();
		void SetTotalTime(std::int64_t _time);
        WeakPtr<BaseAnimation> GetWatch();  
        String GetAnimationName();
	protected:
		std::int64_t GetRunningTime();
		void SetRunningTime(std::int64_t _time);

		void SetSeekTime(std::int64_t _time);
		std::int64_t GetSeekTime();
		void SetSeek(bool b);
		bool GetSeek();
		void SetResume(bool b);
		bool GetResume();
		void SetPlay(bool b);
		bool GetPlay();

		void SetReverseStateChanged(bool b);
		bool GetReverseStateChanged();

	    void __Stop();
        SharedPtr<BaseAnimation> m_Shared;                                               //��ɾ����ǿ����
		static bool SFirstRun(BaseAnimation* sa);                                          //��ʼʱ
		static bool SLastRun(BaseAnimation* sa);                                          //���ʱ
		static	bool SUpdateRunningTime(BaseAnimation* sa,std::int64_t _time);  //����ʱ
		SpinLock m_lock;
		std::int64_t  runningTime;
		std::int64_t  totalTime;
		std::int64_t  seekTime;
        static std::vector<WeakPtr<BaseAnimation>>  s_animations;
		bool   isLoop;
		bool   isPlay;
		bool   isSeek;
		bool   isResume;
		WaitableEvent  blockThreadEvent;
        String animationName;
		bool   isReverse;                                                                            //�Ƿ����򲥷�
		bool   isReverseStateChanged; 
	    AnimationListener* listener;
	};













}



#endif