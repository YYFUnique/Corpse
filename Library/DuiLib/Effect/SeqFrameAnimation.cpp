#include "StdAfx.h"

namespace Skill
{

SeqFrameAnimation::SeqFrameAnimation(String name)
	:BaseAnimation(name)
	,frameSize(10)
	,intervalTime(30)
{
	SetTotalTime(intervalTime*frameSize);
}

SeqFrameAnimation::~SeqFrameAnimation()
{

}

void SeqFrameAnimation::SetFramePos(int pos)
{
	SeekAnimation(pos*intervalTime);
}

void SeqFrameAnimation::SetFrameSize(int size)
{
	frameSize = size;
	SetTotalTime(intervalTime*size);
}

void SeqFrameAnimation::SetIntervalTime(std::int64_t time)
{
	SKILL_ASSERT(time%30==0);
	intervalTime = time;
}

void SeqFrameAnimation::BindObject(ISeqFrameAnimation* object)
{
	SKILL_ASSERT(object);
	ownerObject = object;
}


bool SeqFrameAnimation::FirstRun()   //��ʼ״̬
{
	ownerObject->SetAnimationSeqFrame(0);
	return true;
}

void SeqFrameAnimation::Run()        //����ʱ״̬
{
	int curFramePos = 0;
	curFramePos = static_cast<int>(frameSize*runningTime/totalTime);
	ownerObject->SetAnimationSeqFrame(curFramePos);
}

bool SeqFrameAnimation::LastRun()   //����ʱ״̬
{
	ownerObject->SetAnimationSeqFrame(frameSize);
	return true;
}


}