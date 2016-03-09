/************************************************************************/
/*   Desc：序列帧动画              
     Author ：Skilla（QQ：848861075）       */
/************************************************************************/
#ifndef SKILL_ANIMATION_SEQFRAMEANIMATION_H
#define SKILL_ANIMATION_SEQFRAMEANIMATION_H

namespace Skill
{

class ISeqFrameAnimation
{
public:
	virtual void SetAnimationSeqFrame(const int framePos) = 0;
};

class SKILL_API SeqFrameAnimation : public BaseAnimation
{
public:
	SeqFrameAnimation(String name);
	~SeqFrameAnimation();
	void SetFramePos(int pos);
    void SetFrameSize(int size);
    void SetIntervalTime(std::int64_t time);
	void BindObject(ISeqFrameAnimation* object);
	virtual bool FirstRun();
	virtual void Run();
	virtual bool LastRun();
protected:
	LEAKED_THE_CLASS(SeqFrameAnimation)
	ISeqFrameAnimation*     ownerObject;
	int frameSize;
	std::int64_t intervalTime;   //间隔时间

};


}
#endif