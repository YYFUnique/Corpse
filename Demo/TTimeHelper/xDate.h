#pragma once

const  WORD START_YEAR =1901;  
const  WORD END_YEAR   =2050;  

void FormatLunarYear(WORD iYear, LPTSTR pLunarYear);
void GetSymbolicAnimal(WORD wYear, LPTSTR pSymbolicAnimal);
void FormatMonth(WORD wMonth, LPTSTR pMonth, BOOL bLunar);
void FormatLunarDay(DWORD wDay, LPTSTR pLunarDay);

//计算iYear,iMonth,iDay对应是星期几 1年1月1日 --- 65535年12月31日    
WORD WeekDay(WORD iYear, WORD iMonth, WORD iDay);   
//返回iYear年iMonth月的天数 1年1月 --- 65535年12月    
WORD MonthDays(WORD iYear, WORD iMonth);    
//返回阴历iLunarYer年阴历iLunarMonth月的天数，如果iLunarMonth为闰月，   
//高字为第二个iLunarMonth月的天数，否则高字为0    
// 1901年1月---2050年12月    
LONG LunarMonthDays(WORD iLunarYear, WORD iLunarMonth);    
//返回阴历iLunarYear年的总天数   
// 1901年1月---2050年12月    
WORD LunarYearDays(WORD iLunarYear);    
//返回阴历iLunarYear年的闰月月份，如没有返回0    
// 1901年1月---2050年12月   
WORD GetLeapMonth(WORD iLunarYear);    
//把iYear年格式化成天干记年法表示的字符串    
void FormatLunarYear(WORD  iYear, char *pBuffer);    
//把iMonth格式化成中文字符串    
void FormatMonth(WORD iMonth, char *pBuffer, BOOL bLunar = TRUE);    
//把iDay格式化成中文字符串    
void FormatLunarDay(WORD  iDay, char *pBuffer);   
//计算公历两个日期间相差的天数  1年1月1日 --- 65535年12月31日    
LONG CalcDateDiff(WORD iEndYear, WORD iEndMonth, WORD iEndDay,WORD iStartYear = START_YEAR,WORD iStartMonth =1, WORD iStartDay =1);    
//计算公历iYear年iMonth月iDay日对应的阴历日期,返回对应的阴历节气 0-24   
//1901年1月1日---2050年12月31日    
WORD GetLunarDate(WORD iYear, WORD iMonth, WORD iDay,WORD &iLunarYear, WORD &iLunarMonth, WORD &iLunarDay);  