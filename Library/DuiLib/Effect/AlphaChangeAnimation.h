/************************************************************************/
/* Desc£º½¥Òþ½¥ÏÖ¶¯»­              
    Author £ºSkilla£¨QQ£º848861075£©       */
/************************************************************************/
#ifndef SKILL_ANIMATION_ALPHACHANGEANIMATION_H
#define SKILL_ANIMATION_ALPHACHANGEANIMATION_H


namespace Skill
{

class  IAlphaChangeAnimation
{
public:
	virtual void SetAnimationAlpha(const int Alpha) = 0;
};
class SKILL_API AlphaChangeAnimation : public BaseAnimation
{
public:
	AlphaChangeAnimation(String name);
	~AlphaChangeAnimation();
	void BindObject(IAlphaChangeAnimation* object);
	void SetKeyFrameRect(const  int start,const int  end);
	virtual bool FirstRun();
	virtual void Run();
	virtual bool LastRun();
protected:
	LEAKED_THE_CLASS(AlphaChangeAnimation)
	IAlphaChangeAnimation*     ownerObject;

	int startAlpha;
	int endAlpha;

};


}
#endif