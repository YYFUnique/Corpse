#pragma once
#include "DuiLib/Utils/observer_impl_base.h"

typedef enum tagCITYINFO_CHANGED
{
	CITYINFO_CHANGED_TIME_INVALID = 0,				//��������ʱ����Ч
	CITYINFO_CHANGED_LOCATION_INVALID = 2,	//����λ����Ч
	CITYINFO_CHANGED_WEATHER_INVALID = 3,		//������Ϣ��Ч
	CITYINFO_CHANGED_LOAD_SUCCESS = 4,			//���ݼ��سɹ�
	CITYINFO_CHANGED_LOCATION = 10,					//��ȡ����λ�óɹ�
	CITYINFO_CHANGED_WEATHERINFO  = 11,					//��ȡ���������ɹ�
	CITYINFO_CHANGED_TEMPERATURE = 12,			//��ȡ�����¶���Ϣ�ɹ�
}CITYINFO_CHANGED;

typedef struct CityInfoChangedParam
{
	LPCTSTR strJsonString;
	CITYINFO_CHANGED CityInfoChanged;
}CityInfoChangedParam;

typedef class ObserverImpl<BOOL, CityInfoChangedParam> CityInfoObserver;
typedef class ReceiverImpl<BOOL, CityInfoChangedParam> CityInfoReceiver;

