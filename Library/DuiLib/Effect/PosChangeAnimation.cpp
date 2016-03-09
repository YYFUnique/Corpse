#include "StdAfx.h"

namespace Skill
{
	PosChangeAnimation::PosChangeAnimation(String name)
		:BaseAnimation(name)
		,posChangeFactor(Uniform_T_S)
	{

	}

	PosChangeAnimation::~PosChangeAnimation()
	{
	}

	void PosChangeAnimation::BindObject(IPosChangeAnimation* object)
	{
		SKILL_ASSERT(object);
		ownerObject = object;
	}

	void PosChangeAnimation::SetKeyFrameRect(const  RECT& start,const RECT&  end)
	{
		startRect = start;
		endRect = end;
	}

    bool PosChangeAnimation::FirstRun()
	{
		ownerObject->SetAnimationPos(startRect);
		return true;
	}


	void PosChangeAnimation::Run()
	{
		RECT rect;
		if (posChangeFactor == Uniform_T_S||posChangeFactor == Uniform_V_T||posChangeFactor == Uniform_V_S)       //‘»ÀŸ
		{
			rect.left = static_cast<int>(startRect.left+(endRect.left-startRect.left)*GetRunningTime()/GetTotalTime());
			rect.top = static_cast<int>(startRect.top +(endRect.top-startRect.top)*GetRunningTime()/GetTotalTime());
			rect.right = static_cast<int>(startRect.right + (endRect.right-startRect.right)*GetRunningTime()/GetTotalTime());
			rect.bottom = static_cast<int>(startRect.bottom + (endRect.bottom-startRect.bottom)*GetRunningTime()/GetTotalTime());
		}else if (posChangeFactor==Accelerated_A_T_S||posChangeFactor==Accelerated_V0_A_S||posChangeFactor==Accelerated_V0_A_T||posChangeFactor == Accelerated_V0_T_S)
		{
			rect.left = startRect.left + UniformDis(v0Left,aLeft,GetRunningTime());
			rect.top = startRect.top + UniformDis(v0Top,aTop,GetRunningTime());
			rect.right = startRect.right + UniformDis(v0Right,aRight,GetRunningTime());
			rect.bottom = startRect.bottom + UniformDis(v0Bottom,aBottom,GetRunningTime());
		}


		SKILL_ASSERT(ownerObject);
		ownerObject->SetAnimationPos(rect);
	}

   bool PosChangeAnimation::LastRun()
	{
		ownerObject->SetAnimationPos(endRect);
		return true;
	}
}