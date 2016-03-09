/************************************************************************/
/* Desc��λ�øı䶯��     
    Author ��Skilla��QQ��848861075��       */
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



enum PosChangeFactor        //����Ӱ������             
{
	Uniform_V_T,             //����-�ٶȡ�ʱ��
	Uniform_V_S,             //����-�ٶȡ�λ��
	Uniform_T_S,              //����-ʱ�䡢λ��


	Accelerated_V0_T_S,  //�ȼ���-��ʼ�ٶȡ�ʱ�䡢λ��
	Accelerated_A_T_S,    //�ȼ���-���ٶȡ�ʱ�䡢λ��
	Accelerated_V0_A_S,  //�ȼ���-��ʼ�ٶȡ����ٶȡ�λ��
	Accelerated_V0_A_T   //�ȼ���-��ʼ�ٶȡ����ٶȡ�ʱ��
};





class SKILL_API PosChangeAnimation : public BaseAnimation
{
public:
	PosChangeAnimation(String name);
	~PosChangeAnimation();
	void BindObject(IPosChangeAnimation* object);                       //�󶨶�����������
	void SetKeyFrameRect(const  RECT& start,const RECT&  end);   //������ʼλ�úͽ���λ��
	virtual bool FirstRun();
	virtual void Run();
	virtual bool LastRun();


	void SetFactor(PosChangeFactor factor)                                 //λ�øı�����
	{
		if (factor==Uniform_T_S)   //ʱ��λ�ƣ�Ĭ�ϣ�
		{
			
		}else if (factor == Uniform_V_S) //�ٶ�λ��
		{
			SKILL_ASSERT(vLeft==vTop==vRight==vBottom);
			SKILL_ASSERT(endRect.left - startRect.left==endRect.top - startRect.top==endRect.right - startRect.right==endRect.bottom - startRect.bottom);
			SetTotalTime((endRect.left - startRect.left)/vLeft);
		}else if (factor == Uniform_V_T)  //�ٶȣ�ʱ��
		{
			endRect.left = startRect.left + vLeft*GetTotalTime();
			endRect.top = startRect.top + vTop*GetTotalTime();
			endRect.right = startRect.right + vRight*GetTotalTime();
			endRect.bottom = startRect.bottom + vBottom*GetTotalTime();
		}else if (factor == Accelerated_A_T_S)  //���ٶȣ�ʱ�䣬λ��
		{
			v0Left =     InitialSpeed(endRect.left-startRect.left,aLeft,GetTotalTime());
			v0Top =     InitialSpeed(endRect.top-startRect.top,aTop,GetTotalTime());
			v0Right =    InitialSpeed(endRect.right-startRect.right,aRight,GetTotalTime());
			v0Bottom =  InitialSpeed(endRect.bottom-startRect.bottom,aBottom,GetTotalTime());
		}else if (factor == Accelerated_V0_A_S) //��ʼ�ٶȣ����ٶȣ�λ��
		{
			SKILL_ASSERT(v0Left==v0Top==v0Right==v0Bottom);
			SKILL_ASSERT(aLeft==aTop==aRight==aBottom);
			SKILL_ASSERT(endRect.left - startRect.left==endRect.top - startRect.top==endRect.right - startRect.right==endRect.bottom - startRect.bottom);
			SetTotalTime(UniformTime(v0Left,aLeft,endRect.left-startRect.left));

		}else if (factor == Accelerated_V0_A_T)  //��ʼ�ٶȣ����ٶȣ�ʱ��
		{
			endRect.left = startRect.left + UniformDis(v0Left,aLeft,GetTotalTime());
			endRect.top = startRect.top + UniformDis(v0Top,aTop,GetTotalTime());
			endRect.right = startRect.right + UniformDis(v0Right,aRight,GetTotalTime());
			endRect.bottom= startRect.bottom + UniformDis(v0Bottom,aBottom,GetTotalTime());
		}else if (factor == Accelerated_V0_T_S) //��ʼ�ٶȣ�ʱ�䣬λ��
		{
		    aLeft = AcceleratedSpeed(endRect.left-startRect.left,v0Left,GetTotalTime());
			aTop = AcceleratedSpeed(endRect.top-startRect.top,v0Top,GetTotalTime());
		    aRight = AcceleratedSpeed(endRect.right-startRect.right,v0Right,GetTotalTime());
		    aBottom = AcceleratedSpeed(endRect.bottom-startRect.bottom,v0Bottom,GetTotalTime());
		}



	    posChangeFactor = factor;
	}

	//����ֱ���˶�
	void SetSpeed(double l,double t,double r,double b)                     //�����ٶ� ����/����              
	{
		vLeft = l;
		vTop = t;
		vRight = r;
		vBottom = b;
	}
	

	void  SetAcceleratedSpeed(double l,double t,double r,double b)     //���ü��ٶ�
	{
		aLeft = l;
		aTop = t;
		aRight = r;
		aBottom = b;
	}

	void  SetInitialSpeed(double l,double t,double r,double b)            //���ó�ʼ�ٶ�
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


	double aLeft;                  //����ٶ�  px/s^2     ����/ƽ������
	double aTop;                  //�ϼ��ٶ�
	double aRight;                //�Ҽ��ٶ�
	double aBottom;             //�¼��ٶ�

	double v0Left;               //���ʼ�ٶ�
	double v0Top;               //�ϳ�ʼ�ٶ�
	double v0Right;              //�ҳ�ʼ�ٶ�
	double v0Bottom;           //�³�ʼ�ٶ�


	double vLeft;                //���ٶ�
	double vTop;                //���ٶ�
	double vRight;               //���ٶ�
	double vBottom;            //���ٶ�




	PosChangeFactor   posChangeFactor;

};

}
#endif