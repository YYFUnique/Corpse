#include "StdAfx.h"

namespace Skill
{
	AlphaChangeAnimation::AlphaChangeAnimation(String name)
		:BaseAnimation(name)
	{

	}
	AlphaChangeAnimation::~AlphaChangeAnimation()
	{

	}
	void AlphaChangeAnimation::BindObject(IAlphaChangeAnimation* object)
	{
		SKILL_ASSERT(object);
		ownerObject = object;
	}

	void AlphaChangeAnimation::SetKeyFrameRect(const  int start,const int  end)
	{
		startAlpha = start;
		endAlpha = end;
	}


    bool AlphaChangeAnimation::FirstRun()
	{
		ownerObject->SetAnimationAlpha(startAlpha);
		return true;
	}

	void AlphaChangeAnimation::Run()
	{
		int alpha;
		alpha =static_cast<int>(startAlpha+(endAlpha-startAlpha)*runningTime/totalTime);
		ownerObject->SetAnimationAlpha(alpha);
	}

	bool AlphaChangeAnimation::LastRun()
	{
		ownerObject->SetAnimationAlpha(endAlpha);
		return true;
	}
}