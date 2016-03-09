#include "StdAfx.h"

namespace Skill
{
	AnimationFactory* AnimationFactory::instance = nullptr;


	AnimationFactory::AnimationFactory()
		:createInterface(nullptr)
	{

	}

	void AnimationFactory::RegistCreateInterface(ICreateAnimation* icreate)
	{
		createInterface = icreate;
	}



	BaseAnimation* AnimationFactory::CreateAnimation(String animationName,LPCTSTR animationClass,ICreateAnimation* icreate)
	 {
		 if (_tcscmp(animationClass,POS_CHANGE_ANIMATION) == 0)
		 {
			 BaseAnimation* a =  new PosChangeAnimation(animationName);
			 animations.push_back(a);
			 return a;
		 }else if (_tcscmp(animationClass,ALPHA_CHANGE_ANIMATION) == 0)
		 { 
			 BaseAnimation* a =  new AlphaChangeAnimation(animationName);
		     animations.push_back(a);
		     return a;
		 }else if (_tcscmp(animationClass,SEQ_FRAME_ANIMATION) == 0)
		 {
			 BaseAnimation* a =  new SeqFrameAnimation(animationName);
			 animations.push_back(a);
			 return a;
		 }else
		 {
			 if (icreate)
			 {
				 BaseAnimation* a =  icreate->OnCreateAnimation();
				 animations.push_back(a);
				 return a;
			 }

			 if (createInterface)
			 {
				  BaseAnimation* a =   createInterface->OnCreateAnimation();
				  animations.push_back(a);
				  return a;
			 }
		 }

		 return NULL;
	 }            

	BaseAnimation* AnimationFactory::FindAnimation(String animationName)
	 {
		 for (unsigned int i=0;i<animations.size();++i)
		 {
			 if (animations[i]->GetAnimationName() == animationName)
			 {
				 return animations[i];
			 }
		 }
		 return NULL;
	 }


	bool AnimationFactory::DeleteAnimation(String animationName)
	 {
		 std::vector<SharedObjectPtr<BaseAnimation>>::iterator itr = animations.begin();

		 bool bRet = false;
		 while (itr!=animations.end())
		 {
			 if ((*itr)->GetAnimationName() == animationName)
			 {
				 itr=animations.erase(itr);
				 bRet = true;
			 }
			 else
				 itr++;
		 }

		 return bRet;
	 }

	void AnimationFactory::DeleteAll()
	 {
		 BaseAnimation::Flush();
		 animations.clear();
	 }

    AnimationFactory* AnimationFactory::GetInstance()
	{
		if (instance==nullptr)
		{
			instance = new AnimationFactory;
		}

		return instance;
	}

}