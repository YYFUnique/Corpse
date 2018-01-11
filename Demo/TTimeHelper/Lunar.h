/****************************************************************************************
�� ��飺
	Lunar.dll����һ�����졢�ȶ����������������ר��ΪC++����ƣ��������ʽ���ṩ��һϵ������ת��������

	��ϸʹ�÷���˵���뿴���ġ�

	���ļ���Lunar.h����Ӧ�Ķ�̬��汾ΪV4.1�������桢רҵ�棩������ʱ��ע��DLLƥ��˰汾�ţ�������ִ���

�� ��������
==================================================================================================
����C++��
==================================================================================================
����һ����ʽ����
	1. ����ͷ�ļ���#include "lunar.h"������LoadLibrary(_T("Lunar.dll"));����dll��·�����Զ���
	2. ��ȡ�ӿڣ�pLunarIfMgr GetInterface = (pLunarIfMgr)GetProcAddress(DLLʵ�����,
	"GetLunarInterface");�����С�GetLunarInterface���ӿں���������Dependency Walker�ȹ�������ѯ�õ���
	3. ��ȡ�ӿڹ�������CLunarInterface * pLunarIf = GetInterface();
	4. ��ȡCLunar��CLunarEx�ӿڣ�ͨ��GetClassPtr�������õ��ӿڡ�
	5. ͨ��pLunar�������ָ�������ú������磺pLunar->GDateToLDate(,);
	6. ��ʹ�õ�ʱ�򣬿���ͨ��ReleaseClassPtr�������ͷŽӿڡ�
	7. �ȵ�������Ҫʹ�ñ���ʱ���ͷŽӿ� pLunarIf->ReleaseInterface(); pLunarIf = NULL;
	8. ������FreeLibrary���ͷ�dll��
==================================================================================================
����������ʽ����
	1. ����USING_LUNAR_STATIC��ע�⣬һ��Ҫ�ȶ��壬�ٰ���ͷ�ļ����������Ŀ���Ե�Ԥ�������ж���ʱ�����漰�����⣻
	2. �ٰ���Lunar.h��
	3. ������Ӧ��lib�ļ���
	4. CLunarInterface * pLunarIf = GetLunarInterface();
	5. ��ȡָ�����ࣨ��CLunar�������������еķ�����
==================================================================================================
�� ʾ�����룺
==================================================================================================
��ʽ���ã�

	#include "lunar.h"
	#include <iostream>

	using namespace std;
	using namespace LUNAR;

	static TCHAR* szDName[30]={_T("��һ"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("��ʮ"),_T("ʮһ"),_T("ʮ��"),_T("ʮ��"),_T("ʮ��"),_T("ʮ��"),_T("ʮ��"),_T("ʮ��"),_T("ʮ��"),_T("ʮ��"),_T("��ʮ"),_T("إһ"),_T("إ��"),_T("إ��"),_T("إ��"),_T("إ��"),_T("إ��"),_T("إ��"),_T("إ��"),_T("إ��"),_T("��ʮ")};
	static TCHAR* szMName[12]={_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("ʮ��"),_T("ʮһ��"),_T("ʮ����")};
	static TCHAR* szTrunk[10]={_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��")};
	static TCHAR* szBranch[12]={_T("��"),_T("��"),_T("��"),_T("î"),_T("��"),_T("��"),_T("��"),_T("δ"),_T("��"),_T("��"),_T("��"),_T("��")};
	static TCHAR* szShengxiao[12]={_T("��"),_T("ţ"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��")};
	static TCHAR* szWeek[7]={_T("������"),_T("����һ"),_T("���ڶ�"),_T("������"),_T("������"),_T("������"),_T("������")};
	static TCHAR* szXingzuo[12]={_T("Ħ����"),_T("ˮƿ��"),_T("˫����"),_T("������"),_T("��ţ��"),_T("˫����"),_T("��з��"),_T("ʨ����"),_T("��Ů��"),_T("�����"),_T("��Ы��"),_T("������")};
	static TCHAR* szSTerm[24]={_T("����"),_T("��ˮ"),_T("����"),_T("����"),_T("����"),_T("����"),_T("����"),_T("С��"),_T("â��"),_T("����"),_T("С��"),_T("����"),_T("����"),_T("����"),_T("��¶"),_T("���"),_T("��¶"),_T("˪��"),_T("����"),_T("Сѩ"),_T("��ѩ"),_T("����"),_T("С��"),_T("��")};
	static TCHAR* szShujiu[9]={_T("һ��"),_T("����"),_T("����"),_T("�ľ�"),_T("���"),_T("����"),_T("�߾�"),_T("�˾�"),_T("�ž�")};
	static TCHAR* szMeiyu[2]={_T("��÷"),_T("��÷")};
	static TCHAR* szFuri[3]={_T("����"),_T("�з�"),_T("ĩ��")};
	static TCHAR* szSheri[2]=(_T("������"),_T("������"));

	//�ں�����������´���Ƭ��
	CLunarInterface * pLunarIf = NULL;
	CLunar * pLunar = NULL;//��ָ�����
	HINSTANCE hLunarDll=LoadLibrary(_T("Lunar.dll"));//����DLL
	if (hLunarDll==INVALID_HANDLE_VALUE)
	{
		cout<<"����Lunar.dllʧ�ܣ������Ƿ��ж�ȡȨ�޻��ļ�·���Ƿ���ȷ��"<<endl;
		return;
	}
	pLunarIfMgr GetInterface=(pLunarIfMgr)GetProcAddress(hLunarDll,"GetLunarInterface");
	if (!LunarInterface)
	{
		cout<<"��ȡ�ӿ�ʧ�ܣ�����DLL�ļ��汾�������ԡ�"<<endl;
		return;
	}else
	{
		pLunarIf=GetInterface();//��ȡ�ӿ���
		if (pLunarIf)
		{
			pLunarIf->GetClassPtr(CT_Lunar,(void **)&pLunar);
		}else
		{
			cout<<"��ȡ�ӿ���ʧ��"<<endl;
			return;
		}
	}
	if (pLunar)
	{
		pLunar->GDateToLDate(,);//����
	}
//------------------------------------------------------------------------------------------------
	//ж��
	if (pLunarIf)
	{
		if (pLunar)
		{
			pLunarIf->ReleaseClassPtr((void **)&pLunar);//���д���ɼӿɲ��ӣ���Ϊ�����ͷŽӿ�ʱ���Զ��ͷ�ָ��
		}
		pLunarIf->ReleaseInterface();
		pLunarIf=NULL;
	}
	if (hLunarDll!=NULL)
	{
		FreeLibrary(hLunarDll);//�ͷ�DLL
		hLunarDll=NULL;
	}
=================================================================================================
��ʽ���ӣ�

	#define USING_LUNAR_STATIC
	#include "Lunar.h"

	#pragma comment(lib,"lunar.lib")	//����ֱ������Ŀ�����е�����������ӿ��ļ�

	using namespace LUNAR;

	CLunarInterface * pLunarIf = GetLunarInterface();
	Clunar * pLunar = NULL;
	if (pLunarIf)
	{
		pLunarIf->GetClassPtr(CT_Lunar, (void **)&pLunar);
		if (pLunar)
		{
			pLunar->GDateToLDate(,);//����
		}
	}
//------------------------------------------------------------------------------------------------
	//�ͷ�
	if (pLunarIf)
	{
		if (pLunar)
		{
			pLunarIf->ReleaseClassPtr((void **)&pLunar);//���д���ɼӿɲ��ӣ���Ϊ�����ͷŽӿ�ʱ���Զ��ͷ�ָ��
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
//	Co_InitValue = 0,//��ʼ��ֵ����ʵ�����壩����ע��������±�ƥ��
//	Co_Capricorn,//Ħ����
//	Co_Aquarius,//ˮƿ��
//	Co_Pisces,//˫����
//	Co_Aries,//������
//	Co_Taurus,//��ţ��
//	Co_Gemini,//˫����
//	Co_Cancer,//��з��
//	Co_Leo,//ʨ����
//	Co_Virgo,//��Ů��
//	Co_Libra,//�����
//	Co_Scorpio,//��Ы��
//	Co_Sagittarius,//������
//	Co_End,//�յ㣨��Ч���ͣ������ô���
//}CO_INDEX;//�������
//
//typedef enum
//{
//	St_InitValue = 0,//��ʼ��ֵ����ʵ�����壩����ע��������±�ƥ��
//	St_Lichun,//����
//	St_Yushui,//��ˮ
//	St_Jingzhe,//����
//	St_Chunfen,//����
//	St_Qingming,//����
//	St_Guyu,//����
//	St_Lixia,//����
//	St_Xiaoman,//С��
//	St_Mangzhong,//â��
//	St_Xiazhi,//����
//	St_Xiaoshu,//С��
//	St_Dashu,//����
//	St_Liqiu,//����
//	St_Chushu,//����
//	St_Bailu,//��¶
//	St_Qiufen,//���
//	St_Hanlu,//��¶
//	St_Shuangjiang,//˪��
//	St_Lidong,//����
//	St_Xiaoxue,//Сѩ
//	St_Daxue,//��ѩ
//	St_Dongzhi,//����
//	St_Xiaohan,//С��
//	St_Dahan,//��
//	St_End,//�յ㣨��Ч���ͣ������ô���
//}ST_INDEX;//�������
//
//typedef struct tagGDATE{
//	signed short sYear;//������
//	unsigned short wMonth;//������
//	unsigned short wDay;//������
//	unsigned short wDayOfWeek;//����
//	tagGDATE() : sYear(0), wMonth(0), wDay(0), wDayOfWeek(0){}
//}GDATE,*PGDATE,LPGDATE;//��������
//
//typedef struct tagTIME{
//	unsigned short wHour;//ʱ
//	unsigned short wMinute;//��
//	unsigned short wSecond;//��
//	unsigned short wMillisecond;//����
//	tagTIME() : wHour(0) ,wMinute(0), wSecond(0), wMillisecond(0){}
//}TIME, *PTIME, LPTIME;//ʱ��
//
//typedef struct tagDATETIME : GDATE , TIME{
//}DATETIME, *PDATETIME, LPDATETIME;//��������ʱ��
//
//typedef struct tagLUNARINFOBASE{
//	signed short sYear;//ũ�����ڵĹ������
//	unsigned short wLeapMonth:1;//�Ƿ�Ϊ����
//	unsigned short wMonth:15;//ũ���£�������
//	unsigned short wDaysOfMonth;//ũ���µ�������
//	tagLUNARINFOBASE() : sYear(0), wLeapMonth(0), wMonth(0), wDaysOfMonth(0){}
//}LUNARINFOBASE, *PLUNARINFOBASE, LPLUNARINFOBASE;
//
//typedef struct tagLUNARMONTH : LUNARINFOBASE{
//	signed short sShuoOrdinal;//ũ�����ף���һ����sYear�����������
//	tagLUNARMONTH() : sShuoOrdinal(0){}
//}LUNARMONTH, *PLUNARMONTH, LPLUNARMONTH;//ũ���·���Ϣ
//
//typedef struct tagTBElement{
//	unsigned short wTrunk:8;//���
//	unsigned short wBranch:8;//��֧
//	tagTBElement() : wTrunk(0), wBranch(0){}
//}TBElement;//��֧Ԫ��
//
//typedef struct tagLDATE : LUNARINFOBASE{
//	TBElement tbYear;//��ݵ���ɵ�֧����֧��ʾ��Ф�������³�һͬ�������Ǳ�׼ũ����
//	unsigned short wDay;//ũ����
//	unsigned short wDayOfWeek;//����
//	tagLDATE() : wDay(0), wDayOfWeek(0){}
//}LDATE, *PLDATE, LPLDATE;//ũ������
//
//typedef struct tagEXTREMESEASON{
//	signed short sYear;//�������
//	signed short sYijiuOrdinal;//�����еġ�һ�š���Ӧ��sYear�е������������������ž��ڴ˻���������ɵõ�����ʡ�ԣ�
//	signed short sChufuOrdinal;//��������������
//	signed short sMofuOrdinal;//ĩ���������������з��ڳ�����10�죬��ʡ�ԣ�
//	tagEXTREMESEASON() : sYear(0), sYijiuOrdinal(0), sChufuOrdinal(0), sMofuOrdinal(0){}
//}EXTREMESEASON, *PEXTREMESEASON, LPEXTREMESEASON;//���š����ա���÷����÷
//
//typedef struct tagMEIYU{
//	signed short sYear;//�������
//	signed short sRumeiOrdinal;//��÷����������
//	signed short sChumeiOrdinal;//��÷����������
//	tagMEIYU() : sYear(0), sRumeiOrdinal(0), sChumeiOrdinal(0){}
//}MEIYU, *PMEIYU, LPMEIYU;//÷��
//
//typedef struct tagSOLARTERM : TIME{
//	signed short sYear;//�������
//	ST_INDEX STIndex;//�������
//	signed short sSTOrdial;//��������������
//	signed short sCalibration;//��������
//	tagSOLARTERM() : sYear(0), STIndex(St_InitValue), sSTOrdial(0), sCalibration(0){}
//}SOLARTERM, *PSOLARTERM, LPSOLARTERM;//����
//
//typedef struct tagSHERI{
//	signed short sYear;//�������
//	signed short sChunSheOrdinal;//�����յ���������
//	signed short sQiuSheOrdinal;//�����յ���������
//	tagSHERI() : sYear(0), sChunSheOrdinal(0), sQiuSheOrdinal(0){}
//}SHERI, *PSHERI, LPSHERI;//����
//
//typedef struct tagSIZHU{
//	TBElement tbYear;//����
//	TBElement tbMonth;//����
//	TBElement tbDay;//����
//	TBElement tbHour;//ʱ��
//}SIZHU, *PSIZHU, LPSIZHU;//����

//ũ����
class CLunar
{
public:
	CLunar();
	~CLunar();
public:
	BOOL IsLeapYear(WORD wYear);

	/*************************************************************************/
	/* �������ƣ�GetDateFromOfIndex																*/
	/* �������ܣ���ȡָ��������ŵ�����															*/
	/* ���������wYear		�������																	*/
	/*					 wIndex	ָ���������															*/
	/*					 wMonth	���ڵ��·�																*/
	/*					 wDay		���ڶ�Ӧ�·ݺ���														*/
	/* �� ��  ֵ����ȡ�ɹ�����TRUE�����򷵻�FALSE										*/
	/*************************************************************************/
	BOOL GetDateFromIndex(WORD wYear, WORD wIndex, WORD &wMonth, WORD &wDay);

	/*************************************************************************/
	/* �������ƣ�GetDayIndex																			*/
	/* �������ܣ���ȡָ�������������																*/
	/* ���������wYear		�������																	*/
	/*					 wMonth	ָ�������·�															*/
	/*					 wDay		���ڵ��·ݺ���															*/
	/* �� ��  ֵ����ȡ�ɹ�����������ţ����򷵻�0											*/
	/*************************************************************************/
	WORD GetIndexOfDay(WORD wYear, WORD wMonth, WORD wDay);

	/*************************************************************************/
	/* �������ƣ�GetIndexOfNewYearl																*/
	/* �������ܣ���ȡũ������Ĺ�����������													*/
	/* ���������wLunarYear ũ�������															*/
	/* �� ��  ֵ��ũ������Ĺ�����������															*/
	/*************************************************************************/
	WORD GetIndexOfNewYear(WORD wLunarYear);

	/************************************************************************
	��������	GetConstellation
	�������ܣ�	��ȡ����
	������		DateG				��������
				CoIndex				������ţ����CO_INDEX
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetConstellation(_In_ GDATE DateG, _Out_ CO_INDEX & CoIndex) = 0;


	/************************************************************************
	��������	GetSizhu
	�������ܣ�	��ȡĳһʱ�̵����������֣�
	������		Datetime��			����ʱ��
				Sizhu:				������������������ʱ��				
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetSizhu(_In_ DATETIME Datetime, _Out_ SIZHU & Sizhu) = 0;


	/************************************************************************
	��������	LDateToGDate
	�������ܣ�	ũ��ת����
	������		DateL��				ũ������
				DateG��				��������
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool LDateToGDate(_In_ LDATE DateL, _Out_ GDATE & DateG) = 0;


	/************************************************************************
	��������	GDateToLDate
	�������ܣ�	����תũ������ȡũ�����ڣ���С�£����£�
	������		DateG��				��������
				DateL��				ũ������
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GDateToLDate(_In_ GDATE DateG, _Out_ LDATE & DateL) = 0;


	/************************************************************************
	��������	GetFirstMonthInfo
	�������ܣ�	��ȡũ��ĳ�������·ݣ�һ��Ϊ���£���Ϣ
	������		sYear��				�������
				MonthInfo��			ũ���·���Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetFirstMonthInfo(_In_ signed short sYear, _Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	��������	GetNextMonthInfo
	�������ܣ�	��ȡ�¸�ũ���·���Ϣ����GetFirstMonthInfo���ʹ�ã�
	������		MonthInfo��			ũ���·���Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetNextMonthInfo(_Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	��������	GetPreviousMonthInfo
	�������ܣ�	��ȡ�ϸ�ũ���·���Ϣ����GetFirstMonthInfo���ʹ�ã�
	������		MonthInfo��			ũ���·���Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetPreviousMonthInfo(_Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	��������	GetMonthInfoByIndex
	�������ܣ�	��ȡũ���굱��ָ���������·���Ϣ
	������		sYear��				�������
				wMIIndex��			������Ϊ0���·���������������������ũ�����·���
				MonthInfo��			ũ�����·���Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetMonthInfoByIndex(_In_ signed short sYear, _In_ unsigned short wMIIndex, _Out_ LUNARMONTH & MonthInfo) = 0;


	/************************************************************************
	��������	GetFirstSolarTerm
	�������ܣ�	��ȡ����ĳһ�굱���׸�������Ϣ
	������		sYear��				�������
				SolarTerm��			������Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetFirstSolarTerm(_In_ signed short sYear, _Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	��������	GetNextSolarTerm
	�������ܣ�	��ȡ��һ��������Ϣ
	������		SolarTerm��			������Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetNextSolarTerm(_Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	��������	GetPreviousSolarTerm
	�������ܣ�	��ȡ��һ��������Ϣ
	������		SolarTerm��			������Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetPreviousSolarTerm(_Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	��������	GetSolarTermByIndex
	�������ܣ�	��ȡ�����굱��ָ����������Ϣ������������Ϊһ��Ķ�ʮ�ĸ�����
	������		sYear��				�������
				StIndex��			������Ϊ��ʼ�Ľ�����ţ����ST_INDEX
				SolarTerm��			������Ϣ
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetSolarTermByIndex(_In_ signed short sYear, _In_ ST_INDEX StIndex, _Out_ SOLARTERM & SolarTerm) = 0;


	/************************************************************************
	��������	GetExtremeSeason
	�������ܣ�	��ȡ���ţ�����
	������		sYear��				�������
				ExtremeSeason��		���ţ�������Ϣ��ʱ�����sYear���Ԫ��Ϊ�ο�
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetExtremeSeason(_In_ signed short sYear, _Out_ EXTREMESEASON & ExtremeSeason) = 0;


	/************************************************************************
	��������	GetMeiyu
	�������ܣ�	��ȡ��÷����÷
	������		sYear��				�������
				Meiyu��				��÷����÷��Ϣ��ʱ�����sYear���Ԫ��Ϊ�ο�
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetMeiyu(_In_ signed short sYear, _Out_ MEIYU & Meiyu) = 0;


	/************************************************************************
	��������	GetSheri
	�������ܣ�	��ȡ�������յ���������
	������		sYear��				�������
				Sheri��				���������յ���Ϣ��ʱ�����sYear���Ԫ��Ϊ�ο�
	����ֵ��	falseΪʧ�ܣ�����Ƿ��򳬳���Χ����trueΪ�ɹ�
	************************************************************************/
	//virtual bool GetSheri(_In_ signed short sYear, _Out_ SHERI & Sheri) = 0;

	/*************************************************************************/
	/* �������ƣ�GetDayOfWeek																		*/
	/* �������ܣ���ȡĳһ�������																	*/
	/* ���������wYear		�������																	*/
	/*					 wMonth	�����·�																	*/
	/*					 wDay		��������																	*/
	/* �� ��  ֵ���ɹ�����������ţ����򷵻�-1												*/
	/*************************************************************************/
	WORD GetDayOfWeek(WORD wYear, WORD wMonth, WORD wDay);

	/*************************************************************************/
	/* �������ƣ�GetDaysOfMonth																	*/
	/* �������ܣ���ȡ�����·ݵ�������																*/
	/* ���������wYear		�������																	*/
	/*					 wMonth	�����·�																	*/
	/* �� ��  ֵ�����µ�������������0���ʾʧ��												*/
	/*************************************************************************/
	WORD GetDaysOfMonth(WORD wYear, WORD wMonth);

	/*************************************************************************/
	/* �������ƣ�GetDaysOfYear																		*/
	/* �������ܣ���ȡ������ݵ�������																*/
	/* ���������wYear �������																		*/
	/* �� ��  ֵ�������������������0���ʾʧ��												*/
	/*************************************************************************/
	WORD GetDaysOfYear(WORD wYear);

	/*************************************************************************/
	/* �������ƣ�GetLeapMonth																		*/
	/* �������ܣ���ȡũ�����£���Χ����wYear��ũ������ڣ�							*/
	/* ���������wYear ũ����������Ӧ�Ĺ������											*/
	/* �� ��  ֵ�����µ��·ݣ��磺����6�������������,����0�������������	*/
	/*************************************************************************/
	WORD GetLeapMonth(WORD wYear);

	/*************************************************************************/
	/* �������ƣ�GetDaysOfLunarMonth															*/
	/* �������ܣ���ȡũ���·ݵ�������																*/
	/* ���������wYear ũ����������Ӧ�Ĺ������											*/
	/*					wLunarMonth	ũ���·�															*/
	/*					bLeapMonth	��ǰ���Ƿ�������												*/
	/* ����ֵ��	��ũ���µ�������������0���ʾʧ��										*/
	/*************************************************************************/
	WORD GetDaysOfLunarMonth(WORD wYear, WORD wLunarMonth, BOOL bLeapMonth);

	/*************************************************************************/
	/* �������ƣ�GetDaysOfLunarYear																*/
	/* �������ܣ���ȡũ����ݵ�������																*/
	/* ���������wYear ��Ҫ��ѯ��	ũ����������Ӧ�Ĺ������						*/
	/* ����ֵ��	��ũ�����������������0���ʾʧ��										*/
	/*************************************************************************/
	WORD GetDaysOfLunarYear(WORD wYear);

	/*************************************************************************/
	/* �������ƣ�GetLunarDayInfo																	*/
	/* �������ܣ�������ת��Ϊũ������																*/
	/* ���������wYear ��Ҫת���Ĺ������														*/
	/*					 wMonth	��Ҫת���Ĺ����·�													*/
	/*					 wDay		��Ҫת���Ĺ�������													*/
	/*					 wLunarYear ת�����ũ�����													*/
	/*                  wLunarMonth ת�����ũ���·�                                            */
	/*					 wLunarDay      ת�����ũ������												*/
	/*					 bLeapMonth ת�����ũ���Ƿ�������									*/
	/* �� �� ֵ��ת���ɹ�����TRUE�����򷵻�FALSE											*/
	/*************************************************************************/
	BOOL GetLunarDayInfo(WORD wYear, WORD wMonth, WORD wDay, WORD &wLunarYear, WORD &wLunarMonth, WORD &wLunarDay, BOOL &bLeapMonth);

	/*************************************************************************/
	/* �������ƣ�FormatLunarDay																		*/
	/* �������ܣ���ʽ����ʾũ�����ڣ����һ��ʮһ��										*/
	/* ���������wDay				��Ҫת����ũ��ʱ��											*/
	/*					 lpszLunarDay ת�����ũ����ʽ��ʱ��									*/
	/* �� �� ֵ�� ��																							*/
	/*************************************************************************/
	static void FormatLunarDay(WORD wDay, LPTSTR lpszLunarDay);
protected:

	BOOL DateCheck(WORD wYear, WORD wMonth, WORD wDay);

	/*************************************************************************/
	/* �������ƣ�GetMonthInfo																		*/
	/* �������ܣ���ȡũ���·���Ϣ																	*/
	/* ���������wYear ��Ҫ��ѯ��	ũ����������Ӧ�Ĺ������						*/
	/* ����ֵ��	��ũ�����������������0���ʾʧ��										*/
	/*************************************************************************/
	BOOL GetMonthInfo(WORD wYear,UINT& uData);

	BOOL LunarExpandDX(WORD wLunarYear, WORD wDayOfMonth[15]);
};

//typedef enum
//{
//	Ct_InitValue = 0,//��ʼ��ֵ
//	Ct_Lunar,//ũ����
//	Ct_LunarEx,//��չ��
//	Ct_End,//���һ���������ô�����ʵ������
//}ClassType;//�������
//
////�ӿ���
//class CLunarInterface
//{
//public:
//	/************************************************************************
//	��������	GetClassPtr
//	�������ܣ�	��ȡ���ָ��
//	������		ppClass��			��ȡ����ָ�루ָ����Ķ���ָ�룩
//	����ֵ��	trueΪ��ȡ�ɹ���falseΪ��ȡʧ��
//	************************************************************************/
//	virtual bool GetClassPtr(_In_ ClassType ctClassType, _Inout_ void ** ppClass) = 0;
//
//
//	/************************************************************************
//	��������	ReleaseClassPtr
//	�������ܣ�	�ͷ����ָ��
//	������		ppClass��			Ҫ�ͷŵ�ָ�루ָ����Ķ���ָ�룩
//	����ֵ��	�ӿ����У����ʣ�����ü���
//	************************************************************************/
//	virtual long ReleaseClassPtr(_Inout_ void ** ppClass) = 0;
//
//
//	/************************************************************************
//	��������	ReleaseInterface
//	�������ܣ�	�ͷŽӿ���
//	������		��
//	����ֵ��	��
//	************************************************************************/
//	virtual void ReleaseInterface(void) = 0;
//protected:
//	CLunarInterface(void);
//	virtual ~CLunarInterface(void);
//};
//
//}
//
//#ifdef USING_LUNAR_STATIC //ʹ����ʽ���ӻ�̬�������Ŀʱ�����ڴ���������Ԥ�������ж���USING_LUNAR_STATIC
//extern "C" LUNAR::CLunarInterface * GetLunarInterface(void);//�ӿں���
//#else
//typedef LUNAR::CLunarInterface * (__stdcall * pLunarIfMgr)(void);//�ӿں�����DLL����������ԭ��
//#endif

#endif  //_LUNAR_H_
