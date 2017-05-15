#pragma once

const  WORD START_YEAR =1901;  
const  WORD END_YEAR   =2050;  

void FormatLunarYear(WORD iYear, LPTSTR pLunarYear);
void GetSymbolicAnimal(WORD wYear, LPTSTR pSymbolicAnimal);
void FormatMonth(WORD wMonth, LPTSTR pMonth, BOOL bLunar);
void FormatLunarDay(DWORD wDay, LPTSTR pLunarDay);

//����iYear,iMonth,iDay��Ӧ�����ڼ� 1��1��1�� --- 65535��12��31��    
WORD WeekDay(WORD iYear, WORD iMonth, WORD iDay);   
//����iYear��iMonth�µ����� 1��1�� --- 65535��12��    
WORD MonthDays(WORD iYear, WORD iMonth);    
//��������iLunarYer������iLunarMonth�µ����������iLunarMonthΪ���£�   
//����Ϊ�ڶ���iLunarMonth�µ��������������Ϊ0    
// 1901��1��---2050��12��    
LONG LunarMonthDays(WORD iLunarYear, WORD iLunarMonth);    
//��������iLunarYear���������   
// 1901��1��---2050��12��    
WORD LunarYearDays(WORD iLunarYear);    
//��������iLunarYear��������·ݣ���û�з���0    
// 1901��1��---2050��12��   
WORD GetLeapMonth(WORD iLunarYear);    
//��iYear���ʽ������ɼ��귨��ʾ���ַ���    
void FormatLunarYear(WORD  iYear, char *pBuffer);    
//��iMonth��ʽ���������ַ���    
void FormatMonth(WORD iMonth, char *pBuffer, BOOL bLunar = TRUE);    
//��iDay��ʽ���������ַ���    
void FormatLunarDay(WORD  iDay, char *pBuffer);   
//���㹫���������ڼ���������  1��1��1�� --- 65535��12��31��    
LONG CalcDateDiff(WORD iEndYear, WORD iEndMonth, WORD iEndDay,WORD iStartYear = START_YEAR,WORD iStartMonth =1, WORD iStartDay =1);    
//���㹫��iYear��iMonth��iDay�ն�Ӧ����������,���ض�Ӧ���������� 0-24   
//1901��1��1��---2050��12��31��    
WORD GetLunarDate(WORD iYear, WORD iMonth, WORD iDay,WORD &iLunarYear, WORD &iLunarMonth, WORD &iLunarDay);  