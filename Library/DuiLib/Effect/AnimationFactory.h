/************************************************************************/
/* Desc  ：动画工厂类，管理所有动画的生命周期    
    Author ：Skilla（QQ：848861075）                                                      */
/************************************************************************/
#ifndef SKILL_ANIMATION_ANIMATIONFATORY
#define SKILL_ANIMATION_ANIMATIONFATORY

namespace Skill
{
#define POS_CHANGE_ANIMATION  _T("PosChangeAnimation")
#define ALPHA_CHANGE_ANIMATION _T("AlphaChangeAnimation")
#define SEQ_FRAME_ANIMATION _T("SeqFrameAnimation")


class ICreateAnimation   //自定义动画创建接口
{
public:
   virtual  BaseAnimation* OnCreateAnimation() = 0;
};

class SKILL_API AnimationFactory                     //动画工厂
{
public:	 
	 void RegistCreateInterface(ICreateAnimation* icreate);
     BaseAnimation* CreateAnimation(String animationName,LPCTSTR animationClass,ICreateAnimation* icreate = NULL);
	 BaseAnimation* FindAnimation(String animationName);
	 bool DeleteAnimation(String animationName);
	 void DeleteAll();
	 std::vector<SharedObjectPtr<BaseAnimation>>  animations;
    static AnimationFactory* GetInstance();
protected:
	AnimationFactory();
    ICreateAnimation* createInterface;
     static AnimationFactory* instance;
};


}

#endif