#pragma once
#include "DuiLib/Utils/observer_impl_base.h"

typedef enum tagCITYINFO_CHANGED
{
	CITYINFO_CHANGED_TIME_INVALID = 0,				//保存数据时间无效
	CITYINFO_CHANGED_LOCATION_INVALID = 2,	//城市位置无效
	CITYINFO_CHANGED_WEATHER_INVALID = 3,		//天气信息无效
	CITYINFO_CHANGED_LOAD_SUCCESS = 4,			//数据加载成功
	CITYINFO_CHANGED_LOCATION = 10,					//获取城市位置成功
	CITYINFO_CHANGED_WEATHERINFO  = 11,					//获取城市天气成功
	CITYINFO_CHANGED_TEMPERATURE = 12,			//获取城市温度信息成功
}CITYINFO_CHANGED;

typedef struct CityInfoChangedParam
{
	LPCTSTR strJsonString;
	CITYINFO_CHANGED CityInfoChanged;
}CityInfoChangedParam;

typedef class ObserverImpl<BOOL, CityInfoChangedParam> CityInfoObserver;
typedef class ReceiverImpl<BOOL, CityInfoChangedParam> CityInfoReceiver;

