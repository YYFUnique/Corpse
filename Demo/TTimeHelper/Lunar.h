/****************************************************************************************
● 简介：
	Lunar.dll，是一个成熟、稳定的日历解决方案，专门为C++而设计，以类的形式，提供的一系列日期转换函数。

	详细使用方法说明请看下文。

	本文件（Lunar.h）对应的动态库版本为V4.1（基础版、专业版），调用时请注意DLL匹配此版本号，以免出现错误。

● 调用流程
==================================================================================================
仅限C++：
==================================================================================================
方法一：显式调用
	1. 包含头文件：#include "lunar.h"，并用LoadLibrary(_T("Lunar.dll"));载入dll（路径可自定）
	2. 获取接口：pLunarIfMgr GetInterface = (pLunarIfMgr)GetProcAddress(DLL实例句柄,
	"GetLunarInterface");。其中“GetLunarInterface”接口函数可以用Dependency Walker等工具来查询得到。
	3. 获取接口管理器：CLunarInterface * pLunarIf = GetInterface();
	4. 获取CLunar与CLunarEx接口：通过GetClassPtr方法来得到接口。
	5. 通过pLunar这个对象指针来调用函数，如：pLunar->GDateToLDate(,);
	6. 不使用的时候，可以通过ReleaseClassPtr函数来释放接口。
	7. 等到不再需要使用本库时，释放接口 pLunarIf->ReleaseInterface(); pLunarIf = NULL;
	8. 最终用FreeLibrary来释放dll。
==================================================================================================
方法二：隐式链接
	1. 定义USING_LUNAR_STATIC，注意，一定要先定义，再包含头文件，如果在项目属性的预处理器中定义时，不涉及此问题；
	2. 再包含Lunar.h；
	3. 引入相应的lib文件；
	4. CLunarInterface * pLunarIf = GetLunarInterface();
	5. 获取指定的类（如CLunar），并调用其中的方法。
==================================================================================================
● 示例代码：
==================================================================================================
显式调用：

	#include "lunar.h"
	#include <iostream>

	using namespace std;
	using namespace LUNAR;

	static TCHAR* szDName[30]={_T("初一"),_T("初二"),_T("初三"),_T("初四"),_T("初五"),_T("初六"),_T("初七"),_T("初八"),_T("初九"),_T("初十"),_T("十一"),_T("十二"),_T("十三"),_T("十四"),_T("十五"),_T("十六"),_T("十七"),_T("十八"),_T("十九"),_T("二十"),_T("廿一"),_T("廿二"),_T("廿三"),_T("廿四"),_T("廿五"),_T("廿六"),_T("廿七"),_T("廿八"),_T("廿九"),_T("三十")};
	static TCHAR* szMName[12]={_T("正月"),_T("二月"),_T("三月"),_T("四月"),_T("五月"),_T("六月"),_T("七月"),_T("八月"),_T("九月"),_T("十月"),_T("十一月"),_T("十二月")};
	static TCHAR* szTrunk[10]={_T("甲"),_T("乙"),_T("丙"),_T("丁"),_T("戊"),_T("己"),_T("庚"),_T("辛"),_T("壬"),_T("癸")};
	static TCHAR* szBranch[12]={_T("子"),_T("丑"),_T("寅"),_T("卯"),_T("辰"),_T("巳"),_T("午"),_T("未"),_T("申"),_T("酉"),_T("戌"),_T("亥")};
	static TCHAR* szShengxiao[12]={_T("鼠"),_T("牛"),_T("虎"),_T("兔"),_T("龙"),_T("蛇"),_T("马"),_T("羊"),_T("猴"),_T("鸡"),_T("狗"),_T("猪")};
	static TCHAR* szWeek[7]={_T("星期日"),_T("星期一"),_T("星期二"),_T("星期三"),_T("星期四"),_T("星期五"),_T("星期六")};
	static TCHAR* szXingzuo[12]={_T("摩羯座"),_T("水瓶座"),_T("双鱼座"),_T("白羊座"),_T("金牛座"),_T("双子座"),_T("巨蟹座"),_T("狮子座"),_T("处女座"),_T("天秤座"),_T("天蝎座"),_T("射手座")};
	static TCHAR* szSTerm[24]={_T("立春"),_T("雨水"),_T("惊蛰"),_T("春分"),_T("清明"),_T("谷雨"),_T("立夏"),_T("小满"),_T("芒种"),_T("夏至"),_T("小暑"),_T("大暑"),_T("立秋"),_T("处暑"),_T("白露"),_T("秋分"),_T("寒露"),_T("霜降"),_T("立冬"),_T("小雪"),_T("大雪"),_T("冬至"),_T("小寒"),_T("大寒")};
	static TCHAR* szShujiu[9]={_T("一九"),_T("二九"),_T("三九"),_T("四九"),_T("五九"),_T("六九"),_T("七九"),_T("八九"),_T("九九")};
	static TCHAR* szMeiyu[2]={_T("入梅"),_T("出梅")};
	static TCHAR* szFuri[3]={_T("初伏"),_T("中伏"),_T("末伏")};
	static TCHAR* szSheri[2]=(_T("春社日"),_T("秋社日"));

	//在函数中添加以下代码片段
	CLunarInterface * pLunarIf = NULL;
	CLunar * pLunar = NULL;//类指针对象
	HINSTANCE hLunarDll=LoadLibrary(_T("Lunar.dll"));//加载DLL
	if (hLunarDll==INVALID_HANDLE_VALUE)
	{
		cout<<"加载Lunar.dll失败！请检查是否有读取权限或文件路径是否正确。"<<endl;
		return;
	}
	pLunarIfMgr GetInterface=(pLunarIfMgr)GetProcAddress(hLunarDll,"GetLunarInterface");
	if (!LunarInterface)
	{
		cout<<"获取接口失败，请检查DLL文件版本或完整性。"<<endl;
		return;
	}else
	{
		pLunarIf=GetInterface();//获取接口类
		if (pLunarIf)
		{
			pLunarIf->GetClassPtr(CT_Lunar,(void **)&pLunar);
		}else
		{
			cout<<"获取接口类失败"<<endl;
			return;
		}
	}
	if (pLunar)
	{
		pLunar->GDateToLDate(,);//调用
	}
//------------------------------------------------------------------------------------------------
	//卸载
	if (pLunarIf)
	{
		if (pLunar)
		{
			pLunarIf->ReleaseClassPtr((void **)&pLunar);//此行代码可加可不加，因为最终释放接口时会自动释放指针
		}
		pLunarIf->ReleaseInterface();
		pLunarIf=NULL;
	}
	if (hLunarDll!=NULL)
	{
		FreeLibrary(hLunarDll);//释放DLL
		hLunarDll=NULL;
	}
=================================================================================================
隐式链接：

	#define USING_LUNAR_STATIC
	#include "Lunar.h"

	#pragma comment(lib,"lunar.lib")	//或是直接在项目属性中的链接器中添加库文件

	using namespace LUNAR;

	CLunarInterface * pLunarIf = GetLunarInterface();
	Clunar * pLunar = NULL;
	if (pLunarIf)
	{
		pLunarIf->GetClassPtr(CT_Lunar, (void **)&pLunar);
		if (pLunar)
		{
			pLunar->GDateToLDate(,);//调用
		}
	}
//------------------------------------------------------------------------------------------------
	//释放
	if (pLunarIf)
	{
		if (pLunar)
		{
			pLunarIf->ReleaseClassPtr((void **)&pLunar);//此行代码可加可不加，因为最终释放接口时会自动释放指针
		}
		pLunarIf->ReleaseInterface();
		pLunarIf=NULL;
	}
*************************************************************************************************/

#ifndef _LUNAR_H_
#define _LUNAR_H_

//#ifndef _In_
//#define _In_
//#endif
//
//#ifndef _Out_
//#define _Out_
//#endif
//
//#ifndef _Inout_
//#define _Inout_
//#endif
//
//namespace LUNAR
//{
//
//typedef enum
//{
//	Co_InitValue = 0,//初始化值（无实际意义），请注意和数组下标匹配
//	Co_Capricorn,//摩羯座
//	Co_Aquarius,//水瓶座
//	Co_Pisces,//双鱼座
//	Co_Aries,//白羊座
//	Co_Taurus,//金牛座
//	Co_Gemini,//双子座
//	Co_Cancer,//巨蟹座
//	Co_Leo,//狮子座
//	Co_Virgo,//处女座
//	Co_Libra,//天秤座
//	Co_Scorpio,//天蝎座
//	Co_Sagittarius,//射手座
//	Co_End,//终点（无效类型，特殊用处）
//}CO_INDEX;//星座编号
//
//typedef enum
//{
//	St_InitValue = 0,//初始化值（无实际意义），请注意和数组下标匹配
//	St_Lichun,//立春
//	St_Yushui,//雨水
//	St_Jingzhe,//惊蛰
//	St_Chunfen,//春分
//	St_Qingming,//清明
//	St_Guyu,//谷雨
//	St_Lixia,//立夏
//	St_Xiaoman,//小满
//	St_Mangzhong,//芒种
//	St_Xiazhi,//夏至
//	St_Xiaoshu,//小暑
//	St_Dashu,//大暑
//	St_Liqiu,//立秋
//	St_Chushu,//处暑
//	St_Bailu,//白露
//	St_Qiufen,//秋分
//	St_Hanlu,//寒露
//	St_Shuangjiang,//霜降
//	St_Lidong,//立冬
//	St_Xiaoxue,//小雪
//	St_Daxue,//大雪
//	St_Dongzhi,//冬至
//	St_Xiaohan,//小寒
//	St_Dahan,//大寒
//	St_End,//终点（无效类型，特殊用处）
//}ST_INDEX;//节气编号
//
//typedef struct tagGDATE{
//	signed short sYear;//公历年
//	unsigned short wMonth;//公历月
//	unsigned short wDay;//公历日
//	unsigned short wDayOfWeek;//星期
//	tagGDATE() : sYear(0), wMonth(0), wDay(0), wDayOfWeek(0){}
//}GDATE,*PGDATE,LPGDATE;//公历日期
//
//typedef struct tagTIME{
//	unsigned short wHour;//时
//	unsigned short wMinute;//分
//	unsigned short wSecond;//秒
//	unsigned short wMillisecond;//毫秒
//	tagTIME() : wHour(0) ,wMinute(0), wSecond(0), wMillisecond(0){}
//}TIME, *PTIME, LPTIME;//时间
//
//typedef struct tagDATETIME : GDATE , TIME{
//}DATETIME, *PDATETIME, LPDATETIME;//公历日期时间
//
//typedef struct tagLUNARINFOBASE{
//	signed short sYear;//农历所在的公历年份
//	unsigned short wLeapMonth:1;//是否为闰月
//	unsigned short wMonth:15;//农历月（月名）
//	unsigned short wDaysOfMonth;//农历月的总天数
//	tagLUNARINFOBASE() : sYear(0), wLeapMonth(0), wMonth(0), wDaysOfMonth(0){}
//}LUNARINFOBASE, *PLUNARINFOBASE, LPLUNARINFOBASE;
//
//typedef struct tagLUNARMONTH : LUNARINFOBASE{
//	signed short sShuoOrdinal;//农历月首（初一）在sYear年的年内序数
//	tagLUNARMONTH() : sShuoOrdinal(0){}
//}LUNARMONTH, *PLUNARMONTH, LPLUNARMONTH;//农历月份信息
//
//typedef struct tagTBElement{
//	unsigned short wTrunk:8;//天干
//	unsigned short wBranch:8;//地支
//	tagTBElement() : wTrunk(0), wBranch(0){}
//}TBElement;//干支元素
//
//typedef struct tagLDATE : LUNARINFOBASE{
//	TBElement tbYear;//年份的天干地支，地支表示生肖（与正月初一同步，这是标准农历）
//	unsigned short wDay;//农历日
//	unsigned short wDayOfWeek;//星期
//	tagLDATE() : wDay(0), wDayOfWeek(0){}
//}LDATE, *PLDATE, LPLDATE;//农历日期
//
//typedef struct tagEXTREMESEASON{
//	signed short sYear;//公历年份
//	signed short sYijiuOrdinal;//数九中的“一九”对应在sYear中的年内序数（二九至九九在此基础上推算可得到，故省略）
//	signed short sChufuOrdinal;//初伏的年内序数
//	signed short sMofuOrdinal;//末伏的年内序数（中伏在初伏后10天，故省略）
//	tagEXTREMESEASON() : sYear(0), sYijiuOrdinal(0), sChufuOrdinal(0), sMofuOrdinal(0){}
//}EXTREMESEASON, *PEXTREMESEASON, LPEXTREMESEASON;//数九、伏日、入梅、出梅
//
//typedef struct tagMEIYU{
//	signed short sYear;//公历年份
//	signed short sRumeiOrdinal;//入梅的年内序数
//	signed short sChumeiOrdinal;//出梅的年内序数
//	tagMEIYU() : sYear(0), sRumeiOrdinal(0), sChumeiOrdinal(0){}
//}MEIYU, *PMEIYU, LPMEIYU;//梅雨
//
//typedef struct tagSOLARTERM : TIME{
//	signed short sYear;//公历年份
//	ST_INDEX STIndex;//节气编号
//	signed short sSTOrdial;//节气的年内序数
//	signed short sCalibration;//修正天数
//	tagSOLARTERM() : sYear(0), STIndex(St_InitValue), sSTOrdial(0), sCalibration(0){}
//}SOLARTERM, *PSOLARTERM, LPSOLARTERM;//节气
//
//typedef struct tagSHERI{
//	signed short sYear;//公历年份
//	signed short sChunSheOrdinal;//春社日的年内序数
//	signed short sQiuSheOrdinal;//秋社日的年内序数
//	tagSHERI() : sYear(0), sChunSheOrdinal(0), sQiuSheOrdinal(0){}
//}SHERI, *PSHERI, LPSHERI;//社日
//
//typedef struct tagSIZHU{
//	TBElement tbYear;//年柱
//	TBElement tbMonth;//月柱
//	TBElement tbDay;//日柱
//	TBElement tbHour;//时柱
//}SIZHU, *PSIZHU, LPSIZHU;//四柱

//农历类
class CLunar
{
public:
	CLunar();
	~CLunar();
public:
	BOOL IsLeapYear(WORD wYear);

	/*************************************************************************/
	/* 函数名称：GetDateFromOfIndex																*/
	/* 函数功能：获取指定年内序号的日期															*/
	/* 输入参数：wYear		公历年份																	*/
	/*					 wIndex	指定年内序号															*/
	/*					 wMonth	年内的月份																*/
	/*					 wDay		年内对应月份号数														*/
	/* 返 回  值：获取成功返回TRUE，否则返回FALSE										*/
	/*************************************************************************/
	BOOL GetDateFromIndex(WORD wYear, WORD wIndex, WORD &wMonth, WORD &wDay);

	/*************************************************************************/
	/* 函数名称：GetDayIndex																			*/
	/* 函数功能：获取指定年内日期序号																*/
	/* 输入参数：wYear		公历年份																	*/
	/*					 wMonth	指定年内月份															*/
	/*					 wDay		年内的月份号数															*/
	/* 返 回  值：获取成功返回年内序号，否则返回0											*/
	/*************************************************************************/
	WORD GetIndexOfDay(WORD wYear, WORD wMonth, WORD wDay);

	/*************************************************************************/
	/* 函数名称：GetIndexOfNewYearl																*/
	/* 函数功能：获取农历新年的公历年内序数													*/
	/* 输入参数：wLunarYear 农历年年份															*/
	/* 返 回  值：农历新年的公历年内序数															*/
	/*************************************************************************/
	WORD GetIndexOfNewYear(WORD wLunarYear);

	/************************************************************************
	函数名：	GetConstellation
	函数功能：	获取星座
	参数：		DateG				公历日期
				CoIndex				星座编号，详见CO_INDEX
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetConstellation(_In_ GDATE DateG, _Out_ CO_INDEX & CoIndex) = 0;


	/************************************************************************
	函数名：	GetSizhu
	函数功能：	获取某一时刻的四柱（八字）
	参数：		Datetime：			公历时间
				Sizhu:				年柱、月柱、日柱、时柱				
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetSizhu(_In_ DATETIME Datetime, _Out_ SIZHU & Sizhu) = 0;


	/************************************************************************
	函数名：	LDateToGDate
	函数功能：	农历转公历
	参数：		DateL：				农历日期
				DateG：				公历日期
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool LDateToGDate(_In_ LDATE DateL, _Out_ GDATE & DateG) = 0;


	/************************************************************************
	函数名：	GDateToLDate
	函数功能：	公历转农历（获取农历日期，大小月，闰月）
	参数：		DateG：				公历日期
				DateL：				农历日期
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GDateToLDate(_In_ GDATE DateG, _Out_ LDATE & DateL) = 0;


	/************************************************************************
	函数名：	GetFirstMonthInfo
	函数功能：	获取农历某年年首月份（一般为正月）信息
	参数：		sYear：				公历年份
				MonthInfo：			农历月份信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetFirstMonthInfo(_In_ signed short sYear, _Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	函数名：	GetNextMonthInfo
	函数功能：	获取下个农历月份信息（与GetFirstMonthInfo配合使用）
	参数：		MonthInfo：			农历月份信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetNextMonthInfo(_Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	函数名：	GetPreviousMonthInfo
	函数功能：	获取上个农历月份信息（与GetFirstMonthInfo配合使用）
	参数：		MonthInfo：			农历月份信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetPreviousMonthInfo(_Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	函数名：	GetMonthInfoByIndex
	函数功能：	获取农历年当中指定索引的月份信息
	参数：		sYear：				公历年份
				wMIIndex：			以正月为0的月份索引，索引不超过该年农历总月份数
				MonthInfo：			农历的月份信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetMonthInfoByIndex(_In_ signed short sYear, _In_ unsigned short wMIIndex, _Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	函数名：	GetFirstSolarTerm
	函数功能：	获取公历某一年当中首个节气信息
	参数：		sYear：				公历年份
				SolarTerm：			节气信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetFirstSolarTerm(_In_ signed short sYear, _Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	函数名：	GetNextSolarTerm
	函数功能：	获取下一个节气信息
	参数：		SolarTerm：			节气信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetNextSolarTerm(_Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	函数名：	GetPreviousSolarTerm
	函数功能：	获取上一个节气信息
	参数：		SolarTerm：			节气信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetPreviousSolarTerm(_Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	函数名：	GetSolarTermByIndex
	函数功能：	获取公历年当中指定节气的信息，从立春到大寒为一年的二十四个节气
	参数：		sYear：				公历年份
				StIndex：			以立春为起始的节气编号，详见ST_INDEX
				SolarTerm：			节气信息
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetSolarTermByIndex(_In_ signed short sYear, _In_ ST_INDEX StIndex, _Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	函数名：	GetExtremeSeason
	函数功能：	获取数九，伏日
	参数：		sYear：				公历年份
				ExtremeSeason：		数九，伏日信息，时间均以sYear年份元旦为参考
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetExtremeSeason(_In_ signed short sYear, _Out_ EXTREMESEASON & ExtremeSeason) = 0;


	/************************************************************************
	函数名：	GetMeiyu
	函数功能：	获取入梅、出梅
	参数：		sYear：				公历年份
				Meiyu：				入梅、出梅信息，时间均以sYear年份元旦为参考
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetMeiyu(_In_ signed short sYear, _Out_ MEIYU & Meiyu) = 0;


	/************************************************************************
	函数名：	GetSheri
	函数功能：	获取春秋社日的年内序数
	参数：		sYear：				公历年份
				Sheri：				春、秋社日的信息，时间均以sYear年份元旦为参考
	返回值：	false为失败（输入非法或超出范围），true为成功
	************************************************************************/
	//virtual bool GetSheri(_In_ signed short sYear, _Out_ SHERI & Sheri) = 0;

	/*************************************************************************/
	/* 函数名称：GetDayOfWeek																		*/
	/* 函数功能：获取某一天的星期																	*/
	/* 输入参数：wYear		公历年份																	*/
	/*					 wMonth	公历月份																	*/
	/*					 wDay		公历日期																	*/
	/* 返 回  值：成功返回星期序号，否则返回-1												*/
	/*************************************************************************/
	WORD GetDayOfWeek(WORD wYear, WORD wMonth, WORD wDay);

	/*************************************************************************/
	/* 函数名称：GetDaysOfMonth																	*/
	/* 函数功能：获取公历月份的总天数																*/
	/* 输入参数：wYear		公历年份																	*/
	/*					 wMonth	公历月份																	*/
	/* 返 回  值：该月的总天数，返回0则表示失败												*/
	/*************************************************************************/
	WORD GetDaysOfMonth(WORD wYear, WORD wMonth);

	/*************************************************************************/
	/* 函数名称：GetDaysOfYear																		*/
	/* 函数功能：获取公历年份的总天数																*/
	/* 输入参数：wYear 公历年份																		*/
	/* 返 回  值：该年的总天数，返回0则表示失败												*/
	/*************************************************************************/
	WORD GetDaysOfYear(WORD wYear);

	/*************************************************************************/
	/* 函数名称：GetLeapMonth																		*/
	/* 函数功能：获取农历闰月（范围是在wYear的农历年份内）							*/
	/* 输入参数：wYear 农历纪年所对应的公历年份											*/
	/* 返 回  值：闰月的月份，如：返回6，则代表闰六月,返回0代表该年无闰月	*/
	/*************************************************************************/
	WORD GetLeapMonth(WORD wYear);

	/*************************************************************************/
	/* 函数名称：GetDaysOfLunarMonth															*/
	/* 函数功能：获取农历月份的总天数																*/
	/* 输入参数：wYear 农历纪年所对应的公历年份											*/
	/*					wLunarMonth	农历月份															*/
	/*					bLeapMonth	当前月是否是闰月												*/
	/* 返回值：	该农历月的总天数，返回0则表示失败										*/
	/*************************************************************************/
	WORD GetDaysOfLunarMonth(WORD wYear, WORD wLunarMonth, BOOL bLeapMonth);

	/*************************************************************************/
	/* 函数名称：GetDaysOfLunarYear																*/
	/* 函数功能：获取农历年份的总天数																*/
	/* 输入参数：wYear 需要查询的	农历纪年所对应的公历年份						*/
	/* 返回值：	该农历年的总天数，返回0则表示失败										*/
	/*************************************************************************/
	WORD GetDaysOfLunarYear(WORD wYear);

	/*************************************************************************/
	/* 函数名称：GetLunarDayInfo																	*/
	/* 函数功能：将阳历转换为农历日期																*/
	/* 输入参数：wYear 需要转换的公历年份														*/
	/*					 wMonth	需要转换的公历月份													*/
	/*					 wDay		需要转换的公历日期													*/
	/*					 wLunarYear 转换后的农历年份													*/
	/*                  wLunarMonth 转换后的农历月份                                            */
	/*					 wLunarDay      转换后的农历日期												*/
	/*					 bLeapMonth 转换后的农历是否是闰月									*/
	/* 返 回 值：转换成功返回TRUE，否则返回FALSE											*/
	/*************************************************************************/
	BOOL GetLunarDayInfo(WORD wYear, WORD wMonth, WORD wDay, WORD &wLunarYear, WORD &wLunarMonth, WORD &wLunarDay, BOOL &bLeapMonth);

	/*************************************************************************/
	/* 函数名称：FormatLunarDay																		*/
	/* 函数功能：格式化显示农历日期，如初一，十一等										*/
	/* 输入参数：wDay				需要转换的农历时间											*/
	/*					 lpszLunarDay 转换后的农历格式化时间									*/
	/* 返 回 值： 无																							*/
	/*************************************************************************/
	static void FormatLunarDay(WORD wDay, LPTSTR lpszLunarDay);
protected:

	BOOL DateCheck(WORD wYear, WORD wMonth, WORD wDay);

	/*************************************************************************/
	/* 函数名称：GetMonthInfo																		*/
	/* 函数功能：获取农历月份信息																	*/
	/* 输入参数：wYear 需要查询的	农历纪年所对应的公历年份						*/
	/* 返回值：	该农历年的总天数，返回0则表示失败										*/
	/*************************************************************************/
	BOOL GetMonthInfo(WORD wYear,UINT& uData);

	BOOL LunarExpandDX(WORD wLunarYear, WORD wDayOfMonth[15]);
};

//typedef enum
//{
//	Ct_InitValue = 0,//初始化值
//	Ct_Lunar,//农历类
//	Ct_LunarEx,//扩展类
//	Ct_End,//最后一个，特殊用处，无实际意义
//}ClassType;//类的类型
//
////接口类
//class CLunarInterface
//{
//public:
//	/************************************************************************
//	函数名：	GetClassPtr
//	函数功能：	获取类的指针
//	参数：		ppClass：			获取到的指针（指向类的二级指针）
//	返回值：	true为获取成功，false为获取失败
//	************************************************************************/
//	virtual bool GetClassPtr(_In_ ClassType ctClassType, _Inout_ void ** ppClass) = 0;
//
//
//	/************************************************************************
//	函数名：	ReleaseClassPtr
//	函数功能：	释放类的指针
//	参数：		ppClass：			要释放的指针（指向类的二级指针）
//	返回值：	接口类中，类的剩余引用计数
//	************************************************************************/
//	virtual long ReleaseClassPtr(_Inout_ void ** ppClass) = 0;
//
//
//	/************************************************************************
//	函数名：	ReleaseInterface
//	函数功能：	释放接口类
//	参数：		无
//	返回值：	无
//	************************************************************************/
//	virtual void ReleaseInterface(void) = 0;
//protected:
//	CLunarInterface(void);
//	virtual ~CLunarInterface(void);
//};
//
//}
//
//#ifdef USING_LUNAR_STATIC //使用隐式链接或静态库编译项目时，请在代码或编译器预处理器中定义USING_LUNAR_STATIC
//extern "C" LUNAR::CLunarInterface * GetLunarInterface(void);//接口函数
//#else
//typedef LUNAR::CLunarInterface * (__stdcall * pLunarIfMgr)(void);//接口函数（DLL导出函数）原型
//#endif

#endif  //_LUNAR_H_
