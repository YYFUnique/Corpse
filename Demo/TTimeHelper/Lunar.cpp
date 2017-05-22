#include "stdafx.h"
#include "Lunar.h"
#include "LunarData.h"

//ƽ���ÿ����1������Ӧ����������
int const DayOrdinal[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};
//�����ÿ����1�ն�Ӧ����������
int const DayOrdinalLeap[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};

CLunar::CLunar()
{

}

CLunar::~CLunar()
{

}

BOOL CLunar::IsLeapYear(WORD wYear)
{
	//ѹ���㷨�涨�˵��������
	if (wYear < START_YEAR || wYear >= END_YEAR)
		return FALSE;

	//�ж����������
	if (wYear%4 == 0 && wYear%100 !=0 || wYear%400 == 0)
		return TRUE;
	else
		return FALSE;
}

BOOL CLunar::GetDateFromIndex(WORD wYear, WORD wOrdinal, WORD &wMonth, WORD &wDay)
{
	//�Ӿ���Ԫ���������������
	if (wOrdinal < 0)
		return FALSE;

	BOOL bLeapYear = IsLeapYear(wYear);

	//�ж��Ƿ񳬳��˸����������
	if (bLeapYear==TRUE)
	{
		if (wOrdinal >= 366)
			return FALSE;
	}else
	{
		if (wOrdinal >= 365)
			return FALSE;
	}

	wMonth=0;

	for (int n = 0; n < 12; ++n)
	{
		if (bLeapYear != FALSE)
		{
			//�ҳ��·�
			if (wOrdinal >= DayOrdinalLeap[n] && wOrdinal < DayOrdinalLeap[n+1])
			{
				wMonth = n+1;
				//���������
				wDay = wOrdinal - DayOrdinalLeap[n]+1;
				break;
			}
		}else
		{
			if (wOrdinal >= DayOrdinal[n] && wOrdinal < DayOrdinal[n+1])
			{
				wMonth = n+1;
				wDay = wOrdinal-DayOrdinal[n]+1;
				break;
			}
		}
	}

	return TRUE;
}

WORD CLunar::GetIndexOfDay(WORD wYear, WORD wMonth, WORD wDay)
{
	WORD wDayIndex = -1;
	//�������������Ԫ��������
	if (DateCheck(wYear,wMonth,wDay)==0)//����������ڽ��м��
		return wDayIndex;

	if (IsLeapYear(wYear) != FALSE)
		wDayIndex=DayOrdinalLeap[wMonth-1]+wDay-1;//Ԫ��Ϊ����0����˼�1
	else
		wDayIndex=DayOrdinal[wMonth-1]+wDay-1;
	
	return wDayIndex;
}

WORD CLunar::GetIndexOfNewYear(WORD wLunarYear)
{
	UINT uData=0;
	if (GetMonthInfo(wLunarYear, uData)==FALSE)
		return 0;

	return (WORD)((uData>>17) & 0x3F);//ȡ��ũ���������������
}

WORD CLunar::GetDayOfWeek(WORD wYear, WORD wMonth, WORD wDay)
{
	WORD wDayIndex = GetIndexOfDay(wYear, wMonth, wDay);
	if (wDayIndex == 0)
		return -1;

	WORD wDayofWeek = 0;
	//һ���еĵڼ��죬��ΪGetDayOrdinal���õ��������������Ҫ��һ
	++wDayIndex;

	wYear--;
	//���ֻ�������ڵ�ͨ�ù�ʽ
	wDayofWeek=(wYear + wYear/4 - wYear/100 + wYear/400 + wDayIndex)%7;

	return wDayofWeek;
}

WORD CLunar::GetDaysOfMonth(WORD wYear, WORD wMonth)
{
	BOOL bLeapYear = IsLeapYear(wYear);
	if (wMonth == 1 || wMonth == 3 || wMonth == 5 || wMonth == 7 || wMonth == 8 || wMonth == 10 || wMonth == 12)
		return 31;
	else if (wMonth == 4 || wMonth == 6 || wMonth == 9 || wMonth == 11)
		return 30;
	else 
	{
		if (bLeapYear)
			return 28;
		else
			return 29;
	}
}

WORD CLunar::GetDaysOfYear(WORD wYear)
{
	BOOL bLeapYear = IsLeapYear(wYear);
	if (bLeapYear)
		return 365;
	else
		return 366;
}

WORD CLunar::GetDaysOfLunarYear(WORD wYear)
{
	return 0;
}

WORD CLunar::GetLeapMonth(WORD wYear)
{
	UINT uData=0;
	if (GetMonthInfo(wYear, uData)==FALSE)
		return 0;

	WORD wLeapMonth=(WORD)((uData>>13)&0x0F);
	return wLeapMonth;
}

WORD CLunar::GetDaysOfLunarMonth(WORD wLunarYear, WORD wLunarMonth, BOOL bLeapMonth)
{
	int i=0;//ѭ������
	WORD wMonthInfo = 0; //�����С�����
	WORD wLeapMonth = 0;

	//ũ��������ʼ��ݵ�ǰһ��
	if (wLunarYear == START_YEAR - 1)
	{
		if (cPreLeapIndex==-1)//������
		{
			if (bLeapMonth==TRUE)
				return 0;

			return cPreMonth[wLunarMonth-9] - cPreMonth[wLunarMonth-10];
		}
		else
		{
			wLeapMonth = cPreLeapIndex + 9;
			if (wLeapMonth > wLunarMonth)
			{
				if (bLeapMonth==TRUE)
					return 0;

				return cPreMonth[wLunarMonth-9] - cPreMonth[wLunarMonth-10];
			}
			else
			{
				if ((bLeapMonth == 1 && wLunarMonth == wLeapMonth) || wLunarMonth > wLeapMonth)
				{
					return cPreMonth[wLunarMonth-8] - cPreMonth[wLunarMonth-9];
				}else
				{
					return cPreMonth[wLunarMonth-9] - cPreMonth[wLunarMonth-10];
				}
			}
		}
	}

	UINT uDate=0;
	if (GetMonthInfo(wLunarYear, uDate)==FALSE)
		return 0;

	wMonthInfo=(WORD)(uDate & 0x1FFF);//�����С�����
	//��ȡ��ʵ�����·�
	wLeapMonth = GetLeapMonth(wLunarYear);
	//ָ����ѯ�ĵ�ǰ��������
	if (bLeapMonth)
	{
		if (wLeapMonth != wLunarMonth)
			return 0;//�����ڵ�����

		for (i = 0; i < wLunarMonth; ++i)
		{
			wMonthInfo >>= 1;//����һλ������ĩβ��ʼ�Ҹ������·����ڵ�λ
		}
	}else
	{
		if (wLeapMonth > 0)//��������
		{
			if (wLunarMonth<=wLeapMonth)//�·�������֮ǰ��������Ҫ����һλ
			{
				for (i = 0; i < wLunarMonth-1; ++i)
				{
					wMonthInfo >>= 1;
				}
			}else
			{
				for (i = 0; i < wLunarMonth; ++i)//�·�������֮��
				{
					wMonthInfo >>= 1;
				}
			}
		}else
		{
			for (i = 0; i < wLunarMonth - 1; ++i)//������
			{
				wMonthInfo >>= 1;
			}
		}
	}

	if (wMonthInfo & 0x01)
		return 30;//����
	else
		return 29;//С��
}

BOOL CLunar::GetLunarDayInfo(WORD wYear, WORD wMonth, WORD wDay, WORD &wLunarYear, WORD &wLunarMonth, WORD &wLunarDay, BOOL &bLeapMonth)
{
	WORD wIndexDays = GetIndexOfDay(wYear, wMonth, wDay);
	
	//�жϵ�ǰ����������Ϣ�Ƿ���ȷ
	if (wIndexDays == -1)
		return FALSE;

	wLunarYear = wYear;

	//ũ�������ڹ�������������ʣ������
	WORD wIndexOfLunarNewYear = 0, wRemainDays = 0;
	//ũ����һ�����£���������
	WORD wPreLeapMonth = 0, wLeapMonth = 0;
	wIndexOfLunarNewYear = GetIndexOfNewYear(wLunarYear);

	//���ũ���·�����
	WORD DaysOfLunarMonth[15]={0};
	//��ȡ�·��������������һ��ʮһ�¿�ʼ�����꣨��ʮ���£���������
	LunarExpandDX(wLunarYear, DaysOfLunarMonth);

	wPreLeapMonth = GetLeapMonth(wLunarYear - 1);
	if (wPreLeapMonth == 0)
		wLeapMonth = GetLeapMonth(wLunarYear);//��һ��û�����£����ѯ��������

	bLeapMonth = FALSE;
	wRemainDays = wIndexDays - wIndexOfLunarNewYear;//����ũ����������

	int i=0;
	if (wPreLeapMonth > 10)
		i = 3;//���������ڡ�DaysOfLunarMonth���е���������һ��ʮһ�»�ʮ����������
	else
		i = 2;//��һ��ʮһ�º�ʮ����������ʱ���������µ�����

	if (wIndexOfLunarNewYear > wIndexDays)//����ũ������
	{
		wLunarYear -= 1;//ũ�����1
		while(wRemainDays < 0)
		{
			--i;//��һ���ȼ�ȥ����Ϊ��ǰi�����£���1��ʾ��һ��ʮ���£�����ʮ���£�
			wRemainDays += DaysOfLunarMonth[i];//������һ��ʮ���¡�ʮһ�µ��������������£�ֱ����������0
		}

		//�����һ��ʮһ�»�ʮ���´�������
		if (wPreLeapMonth > 10)
		{
			if (wPreLeapMonth == 11)//��ʮһ��
			{
				if (i == 0)//ʮһ�£���������֮ǰ��
				{
					wLunarMonth = 11+i;//ת�����·�
				}else
				{
					wLunarMonth = 10 + i;
					if (wLunarMonth == wPreLeapMonth)
					{
						bLeapMonth = TRUE;
					}
				}
			}else if (wPreLeapMonth == 12)//��ʮ����
			{
				if (i < 2)//������֮ǰ
				{
					wLunarMonth = 11 + i;
				}else
				{
					wLunarMonth = 10 + i;
					if (wLunarMonth == wPreLeapMonth)
					{
						bLeapMonth = TRUE;
					}
				}
			}
		}else
		{
			wLunarMonth = 11 + i;
		}
		wLunarDay = wRemainDays;
	}else
	{
		while (wRemainDays >= DaysOfLunarMonth[i])
		{
			wRemainDays -= DaysOfLunarMonth[i];//Ѱ��ũ����
			i++;//�����¸���
		}
		if (wPreLeapMonth > 10)
		{
			wLunarMonth = i - 2;
		}else 
		{
			if (wLeapMonth > 0)
			{
				if (i - 2 < wLeapMonth)
				{
					wLunarMonth = i - 1;
				}else
				{
					wLunarMonth = i - 2;
					if (wLunarMonth == wLunarMonth)
					{
						bLeapMonth = TRUE;
					}
				}
			}else
			{
				wLunarMonth = i - 1;
			}
		}
		wLunarDay = wRemainDays;
	}

	wLunarDay += 1;//����ת��������
	return TRUE;
}

BOOL CLunar::GetMonthInfo(WORD wYear,UINT& uData)
{
	if (wYear < START_YEAR || wYear >= END_YEAR)
		return FALSE;
	
	int iStartPos=(wYear - START_YEAR)*3;
	uData=byMonthInfo[iStartPos];
	uData<<=8;
	uData|=byMonthInfo[iStartPos+1];
	uData<<=8;
	uData|=byMonthInfo[iStartPos+2];
	return TRUE;
}

BOOL CLunar::DateCheck(WORD wYear, WORD wMonth, WORD wDay)
{
	BOOL bSuccess = FALSE;
	do 
	{
		if (wYear < START_YEAR || wYear >= END_YEAR || wMonth > 12 || wMonth < 1 || wDay < 1 || wDay > 31)
			break;

		if (wMonth == 4 || wMonth == 6 || wMonth == 9 || wMonth == 11)
		{
			if (wDay == 31)
				break;
		} 
		else if (wMonth==2)
		{
			BOOL bLeapYear = IsLeapYear(wYear);
			if (bLeapYear == FALSE)
			{
				if (wDay>28)
					break;
			}else
			{
				if (wDay>29)
					break;
			}
		}

		bSuccess = TRUE;
	} while (FALSE);

	return bSuccess;
}

BOOL CLunar::LunarExpandDX(WORD wLunarYear, WORD wDayOfMonth[15])
{
	if (wLunarYear<START_YEAR||wLunarYear>=END_YEAR)
	{
		return FALSE;
	}
	int i=0;//ѭ������
	WORD pos=0,iLeapMonth=0;//����д��λ�ã�����
	for (i=0;i<15;i++)
	{
		wDayOfMonth[i]=0;//����������
	}
	if (wLunarYear==START_YEAR)
	{
		if (cPreLeapIndex==-1)//������ʼ���֮ǰһ������
		{
			wDayOfMonth[pos]=cPreMonth[2]-cPreMonth[1];//ũ����һ��ʮһ��������
			pos++;
		}else
		{
			if (cPreLeapIndex==1)//��ʮ��
			{
				wDayOfMonth[pos]=cPreMonth[3]-cPreMonth[2];
				pos++;
			}else
			{
				//��ʮһ�»���ʮ����
				wDayOfMonth[pos]=cPreMonth[2]-cPreMonth[1];
				pos++;
				wDayOfMonth[pos]=cPreMonth[3]-cPreMonth[2];
				pos++;
			}
		}
		wDayOfMonth[pos]=GetIndexOfNewYear(wLunarYear)-cPreMonth[2];//����ʮ����
		pos++;
	}else
	{
		iLeapMonth=GetLeapMonth(wLunarYear-1);//ȡ��ǰһũ������������
		if (iLeapMonth<11)//һ����ʮ�µ�����
		{
			wDayOfMonth[pos]= GetDaysOfLunarMonth(wLunarYear-1,11,0);//ȡ��һ��ʮһ������
			pos++;
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,0);//ȡ��һ��ʮ��������
			pos++;
		}else
		{
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,11,0);//ȡ��һ��ʮһ�µ�����
			pos++;
			if (iLeapMonth==11)//��ʮһ��
			{
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,11,1);//ȡ��һ����ʮһ�µ�����
				pos++;
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,0);//ȡ��һ��ʮ��������
				pos++;
			}else if (iLeapMonth==12)
			{
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,0);//ȡ��һ��ʮ��������
				pos++;
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,1);//ȡ��һ����ʮ��������
				pos++;
			}
		}
	}
	iLeapMonth= GetLeapMonth(wLunarYear);//��ȡ��ǰũ������������
	if (iLeapMonth==0)//������
	{
		for (i=0;i<12;i++)
		{
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear,i+1,0);//ȡÿ��ũ��������
			pos++;
		}
	}else
	{
		for (i=0;i<12;i++)
		{
			if (i==iLeapMonth)
			{
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear,i,1);//ȡ���µ�����
				pos++;
			}
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear,i+1,0);//ȡ�����µ�����
			pos++;
		}
	}
	return TRUE;
}