#include "stdafx.h"
#include "Lunar.h"
#include "LunarData.h"

//平年的每个月1日所对应的年内序数
int const DayOrdinal[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};
//闰年的每个月1日对应的年内序数
int const DayOrdinalLeap[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};

CLunar::CLunar()
{

}

CLunar::~CLunar()
{

}

BOOL CLunar::IsLeapYear(WORD wYear)
{
	//压缩算法规定了的年份区间
	if (wYear < START_YEAR || wYear >= END_YEAR)
		return FALSE;

	//判断闰年的条件
	if (wYear%4 == 0 && wYear%100 !=0 || wYear%400 == 0)
		return TRUE;
	else
		return FALSE;
}

BOOL CLunar::GetDateFromIndex(WORD wYear, WORD wOrdinal, WORD &wMonth, WORD &wDay)
{
	//从距离元旦的天数算出日期
	if (wOrdinal < 0)
		return FALSE;

	BOOL bLeapYear = IsLeapYear(wYear);

	//判断是否超出了该年的总天数
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
			//找出月份
			if (wOrdinal >= DayOrdinalLeap[n] && wOrdinal < DayOrdinalLeap[n+1])
			{
				wMonth = n+1;
				//计算出日期
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
	//从日期算出距离元旦的天数
	if (DateCheck(wYear,wMonth,wDay)==0)//对输入的日期进行检查
		return wDayIndex;

	if (IsLeapYear(wYear) != FALSE)
		wDayIndex=DayOrdinalLeap[wMonth-1]+wDay-1;//元旦为序数0，因此减1
	else
		wDayIndex=DayOrdinal[wMonth-1]+wDay-1;
	
	return wDayIndex;
}

WORD CLunar::GetIndexOfNewYear(WORD wLunarYear)
{
	UINT uData=0;
	if (GetMonthInfo(wLunarYear, uData)==FALSE)
		return 0;

	return (WORD)((uData>>17) & 0x3F);//取出农历新年的年内序数
}

WORD CLunar::GetDayOfWeek(WORD wYear, WORD wMonth, WORD wDay)
{
	WORD wDayIndex = GetIndexOfDay(wYear, wMonth, wDay);
	if (wDayIndex == 0)
		return -1;

	WORD wDayofWeek = 0;
	//一年中的第几天，因为GetDayOrdinal所得到的是索引，因此要加一
	++wDayIndex;

	wYear--;
	//这个只是算星期的通用公式
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
	int i=0;//循环变量
	WORD wMonthInfo = 0; //该年大小月情况
	WORD wLeapMonth = 0;

	//农历还在起始年份的前一年
	if (wLunarYear == START_YEAR - 1)
	{
		if (cPreLeapIndex==-1)//无闰月
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

	wMonthInfo=(WORD)(uDate & 0x1FFF);//整年大小月情况
	//获取真实闰月月份
	wLeapMonth = GetLeapMonth(wLunarYear);
	//指定查询的当前月是闰月
	if (bLeapMonth)
	{
		if (wLeapMonth != wLunarMonth)
			return 0;//不存在的闰月

		for (i = 0; i < wLunarMonth; ++i)
		{
			wMonthInfo >>= 1;//右移一位，即从末尾开始找该闰月月份所在的位
		}
	}else
	{
		if (wLeapMonth > 0)//存在闰月
		{
			if (wLunarMonth<=wLeapMonth)//月份在闰月之前，倒找需要多找一位
			{
				for (i = 0; i < wLunarMonth-1; ++i)
				{
					wMonthInfo >>= 1;
				}
			}else
			{
				for (i = 0; i < wLunarMonth; ++i)//月份在闰月之后
				{
					wMonthInfo >>= 1;
				}
			}
		}else
		{
			for (i = 0; i < wLunarMonth - 1; ++i)//无闰月
			{
				wMonthInfo >>= 1;
			}
		}
	}

	if (wMonthInfo & 0x01)
		return 30;//大月
	else
		return 29;//小月
}

BOOL CLunar::GetLunarDayInfo(WORD wYear, WORD wMonth, WORD wDay, WORD &wLunarYear, WORD &wLunarMonth, WORD &wLunarDay, BOOL &bLeapMonth)
{
	WORD wIndexDays = GetIndexOfDay(wYear, wMonth, wDay);
	
	//判断当前传入日期信息是否正确
	if (wIndexDays == -1)
		return FALSE;

	wLunarYear = wYear;

	//农历新年在公历年内序数，剩余天数
	WORD wIndexOfLunarNewYear = 0, wRemainDays = 0;
	//农历上一年闰月，今年闰月
	WORD wPreLeapMonth = 0, wLeapMonth = 0;
	wIndexOfLunarNewYear = GetIndexOfNewYear(wLunarYear);

	//存放农历月份天数
	WORD DaysOfLunarMonth[15]={0};
	//获取月份天数，数组从上一年十一月开始到今年（闰）十二月，包含闰月
	LunarExpandDX(wLunarYear, DaysOfLunarMonth);

	wPreLeapMonth = GetLeapMonth(wLunarYear - 1);
	if (wPreLeapMonth == 0)
		wLeapMonth = GetLeapMonth(wLunarYear);//上一年没有闰月，则查询今年闰月

	bLeapMonth = FALSE;
	wRemainDays = wIndexDays - wIndexOfLunarNewYear;//距离农历新年天数

	int i=0;
	if (wPreLeapMonth > 10)
		i = 3;//今年正月在“DaysOfLunarMonth”中的索引，上一年十一月或十二月有闰月
	else
		i = 2;//上一年十一月和十二月无闰月时，今年正月的索引

	if (wIndexOfLunarNewYear > wIndexDays)//早于农历新年
	{
		wLunarYear -= 1;//农历年减1
		while(wRemainDays < 0)
		{
			--i;//第一次先减去是因为当前i是正月，减1表示上一年十二月（或闰十二月）
			wRemainDays += DaysOfLunarMonth[i];//加上上一年十二月、十一月的总天数（含闰月）直到日数大于0
		}

		//如果上一年十一月或十二月存在闰月
		if (wPreLeapMonth > 10)
		{
			if (wPreLeapMonth == 11)//闰十一月
			{
				if (i == 0)//十一月（即在闰月之前）
				{
					wLunarMonth = 11+i;//转换到月份
				}else
				{
					wLunarMonth = 10 + i;
					if (wLunarMonth == wPreLeapMonth)
					{
						bLeapMonth = TRUE;
					}
				}
			}else if (wPreLeapMonth == 12)//闰十二月
			{
				if (i < 2)//在闰月之前
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
			wRemainDays -= DaysOfLunarMonth[i];//寻找农历月
			i++;//移至下个月
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

	wLunarDay += 1;//索引转换到数量
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
	int i=0;//循环变量
	WORD pos=0,iLeapMonth=0;//数组写入位置，闰月
	for (i=0;i<15;i++)
	{
		wDayOfMonth[i]=0;//对数组清零
	}
	if (wLunarYear==START_YEAR)
	{
		if (cPreLeapIndex==-1)//处理起始年份之前一年数据
		{
			wDayOfMonth[pos]=cPreMonth[2]-cPreMonth[1];//农历上一年十一月总天数
			pos++;
		}else
		{
			if (cPreLeapIndex==1)//闰十月
			{
				wDayOfMonth[pos]=cPreMonth[3]-cPreMonth[2];
				pos++;
			}else
			{
				//闰十一月或闰十二月
				wDayOfMonth[pos]=cPreMonth[2]-cPreMonth[1];
				pos++;
				wDayOfMonth[pos]=cPreMonth[3]-cPreMonth[2];
				pos++;
			}
		}
		wDayOfMonth[pos]=GetIndexOfNewYear(wLunarYear)-cPreMonth[2];//（闰）十二月
		pos++;
	}else
	{
		iLeapMonth=GetLeapMonth(wLunarYear-1);//取得前一农历年的闰月情况
		if (iLeapMonth<11)//一月至十月的闰月
		{
			wDayOfMonth[pos]= GetDaysOfLunarMonth(wLunarYear-1,11,0);//取上一年十一月天数
			pos++;
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,0);//取上一年十二月天数
			pos++;
		}else
		{
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,11,0);//取上一年十一月的天数
			pos++;
			if (iLeapMonth==11)//闰十一月
			{
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,11,1);//取上一年闰十一月的天数
				pos++;
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,0);//取上一年十二月天数
				pos++;
			}else if (iLeapMonth==12)
			{
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,0);//取上一年十二月天数
				pos++;
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear-1,12,1);//取上一年闰十二月天数
				pos++;
			}
		}
	}
	iLeapMonth= GetLeapMonth(wLunarYear);//获取当前农历年的闰月情况
	if (iLeapMonth==0)//无闰月
	{
		for (i=0;i<12;i++)
		{
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear,i+1,0);//取每个农历月天数
			pos++;
		}
	}else
	{
		for (i=0;i<12;i++)
		{
			if (i==iLeapMonth)
			{
				wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear,i,1);//取闰月的天数
				pos++;
			}
			wDayOfMonth[pos]=GetDaysOfLunarMonth(wLunarYear,i+1,0);//取非闰月的天数
			pos++;
		}
	}
	return TRUE;
}