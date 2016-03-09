#include "StdAfx.h"


namespace Skill
{


	std::vector<WeakPtr<BaseAnimation>>  BaseAnimation::s_animations;

	BaseAnimation::BaseAnimation(String name)
		:Thread(_T("animation"))
		,animationName(name)
		,runningTime(0)
		,seekTime(0)
		,isPlay(false)
		,isResume(false)
		,isLoop(false)
		,isReverse(false)
		,isReverseStateChanged(false)
		,blockThreadEvent(WaitableEvent(true))
		,listener(nullptr)
	{
		m_Shared.reset(this,&UnDelete);
		s_animations.push_back(this->GetWatch());
		Start();
	}


	BaseAnimation::~BaseAnimation()
	{
	    SignalExit();
		blockThreadEvent.signal();
		Thread::Stop(4000);   

		std::vector<WeakPtr<BaseAnimation>>::iterator itr = s_animations.begin();
		while (itr!=s_animations.end())
		{
			if (!itr->Expired()&&itr->Lock().get() == this)
			{
				itr=s_animations.erase(itr);
			}
			else
				itr++;
		}
		
	}

	void BaseAnimation::RegistListener(AnimationListener* l)
	{
		listener = l;
	}


	std::int64_t BaseAnimation::GetRunningTime()
	{
		SpinLock::ScopedLockType l(m_lock);
		return runningTime;
	}

	void BaseAnimation::SetRunningTime(std::int64_t _time)
	{
		SpinLock::ScopedLockType l(m_lock);
		runningTime = _time;
	}

	std::int64_t BaseAnimation::GetTotalTime()
	{
		SpinLock::ScopedLockType l(m_lock);
		return totalTime;
	}

	void BaseAnimation::SetTotalTime(std::int64_t _time)
	{
		SpinLock::ScopedLockType l(m_lock);
		totalTime = _time;

	}

	void BaseAnimation::SetSeekTime(std::int64_t _time)
	{
		SpinLock::ScopedLockType l(m_lock);
		seekTime = _time;
	}

	std::int64_t BaseAnimation::GetSeekTime()
	{
		SpinLock::ScopedLockType l(m_lock);
		return seekTime;
	}

	void BaseAnimation::SetSeek(bool b)
	{
		SpinLock::ScopedLockType l(m_lock);
		isSeek = b;
	}
	bool BaseAnimation::GetSeek()
	{
		SpinLock::ScopedLockType l(m_lock);
		return isSeek;
	}


	void BaseAnimation::SetResume(bool b)
	{
		SpinLock::ScopedLockType l(m_lock);
		isResume = b;
	}
	bool BaseAnimation::GetResume()
	{
		SpinLock::ScopedLockType l(m_lock);
		return isResume;
	}

	void BaseAnimation::SetPlay(bool b)
	{
		SpinLock::ScopedLockType l(m_lock);
		isPlay = b;
	}


	bool BaseAnimation::GetPlay()
	{
		SpinLock::ScopedLockType l(m_lock);
		return isPlay;
	}
	void BaseAnimation::SetReverseStateChanged(bool b)
	{
		SpinLock::ScopedLockType l(m_lock);
		isReverseStateChanged = b;
	}
	bool BaseAnimation::GetReverseStateChanged()
	{
		SpinLock::ScopedLockType l(m_lock);
		return isReverseStateChanged;
	}

	void BaseAnimation::PlayAnimation()
	{
		if (GetReverse())
		{
			SetSeekTime(GetTotalTime());
		}else
		{
			SetSeekTime(0);
		}

		SetPlay(true);
		blockThreadEvent.signal();
	}

	void BaseAnimation::PauseAnimation()
	{
		blockThreadEvent.reset();
		if (listener) listener->OnPause(GetRunningTime(),GetTotalTime(),GetReverse(),isLoop);
	}

	void BaseAnimation::ResumeAnimation()
	{
		SetSeekTime(GetRunningTime());
		SetResume(true);
		blockThreadEvent.signal();
		if (listener) listener->OnResume(GetRunningTime(),GetTotalTime(),GetReverse(),isLoop);
	}


	void BaseAnimation::StopAnimation()
	{
		SetSeekTime(0);
		blockThreadEvent.reset();
		if (listener) listener->OnStop(GetRunningTime(),GetTotalTime(),GetReverse(),isLoop);
	}


	void BaseAnimation::SeekAnimation(std::int64_t _time)
	{
		_time = jlimit((std::int64_t)0,totalTime,_time);
		SetSeekTime(_time);
		SetSeek(true);
	}


	String BaseAnimation::GetAnimationName()
	{
		return animationName;
	}

	void BaseAnimation::__Stop()
	{
		SignalExit();
		blockThreadEvent.signal();
		Thread::Stop(4000);   
	}


	void BaseAnimation::SetLoop(bool bLoop)
	{
		isLoop = bLoop;
	}

	void BaseAnimation::SetReverse(bool bReverse)
	{
		if (GetReverse()==bReverse) return;
		SetReverseStateChanged(true);
		SetSeekTime(GetRunningTime());
		SpinLock::ScopedLockType l(m_lock);
		isReverse = bReverse;
	}

	bool BaseAnimation::GetReverse()
	{
		SpinLock::ScopedLockType l(m_lock);
		return isReverse;
	}

	 bool BaseAnimation::SFirstRun(BaseAnimation* sa)
	{
		sa->FirstRun();
		if (sa->GetReverse())
		{
			if (sa->isLoop)
			{
				sa->PlayAnimation();
			}else
			{
				sa->StopAnimation();
			}
			
		   			if (sa->listener) 	sa->listener->OnPlayEnd(sa->GetRunningTime(),sa->GetTotalTime(),sa->GetReverse(),sa->isLoop);
		}else
		{
			if (sa->listener) 	sa->listener->OnPlayStart(sa->GetRunningTime(),sa->GetTotalTime(),sa->GetReverse(),sa->isLoop);
		
		}
		
		
		sa->DecRefCount();
		return true;
	}

	 bool BaseAnimation::SLastRun(BaseAnimation* sa)
	{
		sa->LastRun();
		if (!sa->GetReverse())
		{
			if (sa->isLoop)
			{
				sa->PlayAnimation();
			}else
			{
				sa->StopAnimation();
			}
			if (sa->listener) 	sa->listener->OnPlayEnd(sa->GetRunningTime(),sa->GetTotalTime(),sa->GetReverse(),sa->isLoop);
		}else
		{
			if (sa->listener) 	sa->listener->OnPlayStart(sa->GetRunningTime(),sa->GetTotalTime(),sa->GetReverse(),sa->isLoop);
	
		}
		
	
		sa->DecRefCount();
		return true;
	}


		bool BaseAnimation::SUpdateRunningTime(BaseAnimation* sa,std::int64_t _time)
	{

		sa->Run();
		sa->SetRunningTime(_time);
		if (sa->listener) 	sa->listener->OnPlaying(sa->GetRunningTime(),sa->GetTotalTime(),sa->GetReverse(),sa->isLoop);
		sa->DecRefCount();
		return true;
	}

	void BaseAnimation::run()
	{
		B_Function<bool(BaseAnimation*,std::int64_t)> updateRunningTime =B_Bind(&SUpdateRunningTime,_1,_2);
		B_Function<bool(BaseAnimation*)> firstRun =B_Bind(&SFirstRun,_1);
		B_Function<bool(BaseAnimation*)> lastRun =B_Bind(&SLastRun,_1);

		std::int64_t _runningTime = 0;
		std::int64_t prevTime = 0;

		while(true)
		{
			blockThreadEvent.wait();
			if (IsShouldExit())
			{
				
				break;
			}
		
			if (GetPlay())
			{
				SetPlay(false);
				prevTime = Time::currentTimeMillis();
				if (GetReverse())  //ÅÐ¶ÏÊÇ·ñÄæÐò
				{
					IncRefCount();
					MsgPump::GetInstance()->CallFunUnRet(lastRun,this);
				}else
				{
					IncRefCount();
					MsgPump::GetInstance()->CallFunUnRet(firstRun,this);
				}
			}else if (GetResume()||GetSeek()||GetReverseStateChanged())
			{
				SetResume(false);
				SetSeek(false);
				SetReverseStateChanged(false);
				prevTime = Time::currentTimeMillis();
			}


			if (!GetReverse())   //ÊÇ·ñÄæÐò²¥·Å
			{
				_runningTime = Time::currentTimeMillis()-prevTime+GetSeekTime();
				if (_runningTime>=GetTotalTime())
				{
					IncRefCount();
					MsgPump::GetInstance()->CallFunUnRet(lastRun,this);
					blockThreadEvent.reset();
				}else
				{
					IncRefCount();
					MsgPump::GetInstance()->CallFunUnRet(updateRunningTime,this,_runningTime);
				}
			}else
			{
				_runningTime = GetSeekTime() - (Time::currentTimeMillis()-prevTime);
				if (_runningTime<=0)
				{
					IncRefCount();
					MsgPump::GetInstance()->CallFunUnRet(firstRun,this);
					blockThreadEvent.reset();
				}else
				{
					IncRefCount();
					MsgPump::GetInstance()->CallFunUnRet(updateRunningTime,this,_runningTime);
				}
			}


		
			Thread::Sleep(30);
		}

	}

	WeakPtr<BaseAnimation> BaseAnimation::GetWatch()
	{
		return m_Shared;     
	}


	 void BaseAnimation::Flush()
	{
		for (unsigned int i = 0;i<s_animations.size();++i)
		{
			if (!s_animations[i].Expired())
			{

				s_animations[i].Lock()->__Stop();
			}
		}
	}







}