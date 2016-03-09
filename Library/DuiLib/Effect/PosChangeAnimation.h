/************************************************************************/
/* Desc：位置改变动画     
    Author ：Skilla（QQ：848861075）       */
/************************************************************************/
#ifndef SKILL_ANIMATION_POSCHANGANIMATION_H
#define SKILL_ANIMATION_POSCHANGANIMATION_H

namespace Skill
{

class IPosChangeAnimation
{
public:
	virtual void SetAnimationPos(const RECT& rect) = 0;
};



enum PosChangeFactor        //动画影响因素             
{
	Uniform_V_T,             //匀速-速度、时间
	Uniform_V_S,             //匀速-速度、位移
	Uniform_T_S,              //匀速-时间、位移


	Accelerated_V0_T_S,  //匀加速-初始速度、时间、位移
	Accelerated_A_T_S,    //匀加速-加速度、时间、位移
	Accelerated_V0_A_S,  //匀加速-初始速度、加速度、位移
	Accelerated_V0_A_T   //匀加速-初始速度、加速度、时间
};





class SKILL_API PosChangeAnimation : public BaseAnimation
{
public:
	PosChangeAnimation(String name);
	~PosChangeAnimation();
	void BindObject(IPosChangeAnimation* object);                       //绑定动画到对象上
	void SetKeyFrameRect(const  RECT& start,const RECT&  end);   //设置起始位置和结束位置
	virtual bool FirstRun();
	virtual void Run();
	virtual bool LastRun();


	void SetFactor(PosChangeFactor factor)                                 //位置改变因素
	{
		if (factor==Uniform_T_S)   //时间位移（默认）
		{
			
		}else if (factor == Uniform_V_S) //速度位移
		{
			SKILL_ASSERT(vLeft==vTop==vRight==vBottom);
			SKILL_ASSERT(endRect.left - startRect.left==endRect.top - startRect.top==endRect.right - startRect.right==endRect.bottom - startRect.bottom);
			SetTotalTime((endRect.left - startRect.left)/vLeft);
		}else if (factor == Uniform_V_T)  //速度，时间
		{
			endRect.left = startRect.left + vLeft*GetTotalTime();
			endRect.top = startRect.top + vTop*GetTotalTime();
			endRect.right = startRect.right + vRight*GetTotalTime();
			endRect.bottom = startRect.bottom + vBottom*GetTotalTime();
		}else if (factor == Accelerated_A_T_S)  //加速度，时间，位移
		{
			v0Left =     InitialSpeed(endRect.left-startRect.left,aLeft,GetTotalTime());
			v0Top =     InitialSpeed(endRect.top-startRect.top,aTop,GetTotalTime());
			v0Right =    InitialSpeed(endRect.right-startRect.right,aRight,GetTotalTime());
			v0Bottom =  InitialSpeed(endRect.bottom-startRect.bottom,aBottom,GetTotalTime());
		}else if (factor == Accelerated_V0_A_S) //初始速度，加速度，位移
		{
			SKILL_ASSERT(v0Left==v0Top==v0Right==v0Bottom);
			SKILL_ASSERT(aLeft==aTop==aRight==aBottom);
			SKILL_ASSERT(endRect.left - startRect.left==endRect.top - startRect.top==endRect.right - startRect.right==endRect.bottom - startRect.bottom);
			SetTotalTime(UniformTime(v0Left,aLeft,endRect.left-startRect.left));

		}else if (factor == Accelerated_V0_A_T)  //初始速度，加速度，时间
		{
			endRect.left = startRect.left + UniformDis(v0Left,aLeft,GetTotalTime());
			endRect.top = startRect.top + UniformDis(v0Top,aTop,GetTotalTime());
			endRect.right = startRect.right + UniformDis(v0Right,aRight,GetTotalTime());
			endRect.bottom= startRect.bottom + UniformDis(v0Bottom,aBottom,GetTotalTime());
		}else if (factor == Accelerated_V0_T_S) //初始速度，时间，位移
		{
		    aLeft = AcceleratedSpeed(endRect.left-startRect.left,v0Left,GetTotalTime());
			aTop = AcceleratedSpeed(endRect.top-startRect.top,v0Top,GetTotalTime());
		    aRight = AcceleratedSpeed(endRect.right-startRect.right,v0Right,GetTotalTime());
		    aBottom = AcceleratedSpeed(endRect.bottom-startRect.bottom,v0Bottom,GetTotalTime());
		}



	    posChangeFactor = factor;
	}

	//匀速直线运动
	void SetSpeed(double l,double t,double r,double b)                     //设置速度 像素/毫秒              
	{
		vLeft = l;
		vTop = t;
		vRight = r;
		vBottom = b;
	}
	

	void  SetAcceleratedSpeed(double l,double t,double r,double b)     //设置加速度
	{
		aLeft = l;
		aTop = t;
		aRight = r;
		aBottom = b;
	}

	void  SetInitialSpeed(double l,double t,double r,double b)            //设置初始速度
	{
		v0Left = l;
		v0Top = t;
		v0Right = r;
		v0Bottom = b;
	}

protected:
	LEAKED_THE_CLASS(PosChangeAnimation)
	IPosChangeAnimation*     ownerObject;
	RECT  startRect;
	RECT  endRect;


	double aLeft;                  //左加速度  px/s^2     像素/平方毫秒
	double aTop;                  //上加速度
	double aRight;                //右加速度
	double aBottom;             //下加速度

	double v0Left;               //左初始速度
	double v0Top;               //上初始速度
	double v0Right;              //右初始速度
	double v0Bottom;           //下初始速度


	double vLeft;                //左速度
	double vTop;                //上速度
	double vRight;               //右速度
	double vBottom;            //下速度




	PosChangeFactor   posChangeFactor;

};

}
#endif