/************************************************************************/
/* Desc  �����������࣬�������ж�������������    
    Author ��Skilla��QQ��848861075��                                                      */
/************************************************************************/
#ifndef SKILL_ANIMATION_ANIMATIONFATORY
#define SKILL_ANIMATION_ANIMATIONFATORY

namespace Skill
{
#define POS_CHANGE_ANIMATION  _T("PosChangeAnimation")
#define ALPHA_CHANGE_ANIMATION _T("AlphaChangeAnimation")
#define SEQ_FRAME_ANIMATION _T("SeqFrameAnimation")


class ICreateAnimation   //�Զ��嶯�������ӿ�
{
public:
   virtual  BaseAnimation* OnCreateAnimation() = 0;
};

class SKILL_API AnimationFactory                     //��������
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