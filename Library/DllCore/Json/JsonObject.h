#pragma once

#include <atlstr.h>
#include <tchar.h>
#include <crtdbg.h>
#include "json/value.h"
using namespace Json;

#ifdef LS_STATIC_LIB_CALL
#define DLL_API 
#elif defined  DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

//typedef struct _ENUM_JSON_MEMBER_NAME 
//{
//	CString strMemberName;
//	ValueType enumType;
//}ENUM_JSON_MEMBER_NAME,*PENUM_JSON_MEMBER_NAME;

//typedef CList <ENUM_JSON_MEMBER_NAME,ENUM_JSON_MEMBER_NAME&> CJsonMemberNameList;

/*class  CJsonObjectArray;*/

class DLL_API CJsonObject
{
public:
	CJsonObject();
	~CJsonObject();

//[[值操作
	//设置CJsonObject的值
	void SetValueItem();
	void SetValueItem(IN int value);
	void SetValueItem(IN unsigned int value);
	void SetValueItem(IN double value);
	void SetValueItem(IN LPCTSTR value);
	void SetValueItem(IN bool value);
	void SetValueItem(IN const CJsonObject *value);

	//获取CJsonObject的值
	int GetValueItem(int * value);
	unsigned int GetValueItem(unsigned int * value);
	double GetValueItem(double * value);
	CString GetValueItem();
	bool GetValueItem(bool * value);
	CJsonObject GetValueItem(CJsonObject *value);

	//设置CJsonObject值的数组
	void AppendItem();
	void AppendItem(int value);
	void AppendItem(unsigned int value);
	void AppendItem(double value);
	void AppendItem(LPCTSTR value);
	void AppendItem(bool value);
	void AppendItem(const CJsonObject *value);

	//获取CJsonObject值的数组的项数
	int GetArraySize();

	//获取CJsonObject值的数组的特定项的值
	CJsonObject GetArrayItem(int nIndex);

	//设置CJsonObject数组的特定项的值
	void SetArrayItem(const int &nIndex,const CJsonObject *value);

	//通过指定 nIndex 删除  ，CJsonObject 值的的特定项的值
	//注：删除后 GetArraySize()会相应改变
	void DeleteArrayItem(int nIndex);

	//把json里所有值转为utf8格式
	void AllValueToUtf8();

	//把json里所有值转为gb2312格式
	void AllValueToGb23();

	//名称存在歧义,已被GetArrayItem代替
	const CJsonObject operator[]( int index ) const;

//]]名称/值对操作
	//设置CJsonObject"名称/值"对,key对应名称
	void SetValue(LPCTSTR key);
	void SetValue(LPCTSTR key, int value);
	void SetValue(LPCTSTR key, unsigned int value);
	void SetValue(LPCTSTR key, double value);
	void SetValue(LPCTSTR key, LPCTSTR value);
	void SetValue(LPCTSTR key, bool value);
	void SetValue(LPCTSTR key, const CJsonObject *value);
//	void SetValue(LPCTSTR key, const CJsonObjectArray *value);

	//获取key对应名称的"名称/值"对的值
	int GetValue(LPCTSTR key, int * value);
	unsigned int GetValue(LPCTSTR key, unsigned int * value);
	double GetValue(LPCTSTR key, double * value);
	CString GetValue();
	//const char * GetValue(const char *key, const char **value);//需要手动释放内存
	bool GetValue(LPCTSTR key, bool * value);
	CJsonObject * GetValue(LPCTSTR key, CJsonObject *value);
	CString GetValue(LPCTSTR lpszKeyName);
	////////////////////////////////////////////////////////
	// 函数名称：GetUTF8Value
	// 函数作用：获取值UTF8编码的内容
	// 函数参数：lpszKeyName键名
	// 函数返回值：获取到的UTF8窄字节编码
	////////////////////////////////////////////////////////
	CStringA GetUTF8Value(LPCTSTR lpszKeyName);
	//添加key对应"名称/值"对中,值的数组的项数
	void Append(LPCTSTR key);
	void Append(LPCTSTR key, int value);
	void Append(LPCTSTR key, unsigned int value);
	void Append(LPCTSTR key, double value);
	void Append(LPCTSTR key, LPCTSTR value);
	void Append(LPCTSTR key, bool value);
	void Append(LPCTSTR key, const CJsonObject *value);

	//清空"名称/值"对,中值为数组形式的数据
	void ResetArray(LPCTSTR key);

	//将"名称/值"数组中的值数组转化为字符串数组
//	void GetSubValueToArray(LPCTSTR key,CStringArray&);
	
	//BOOL GetSubValueToArray(LPCTSTR key,CMap<DWORD,DWORD,DWORD,DWORD>& IpHostMap);

//	BOOL GetSubValueToArray(LPCTSTR key,CDWordArray& dwIpArray);

	//情况CJsonObject中所有数据
	void Clear();
	
	//复制CJsonObject对象
	void Duplicate(const CJsonObject *value);
	
	//将CJsonObject对象转换为字符串格式,返回的字符串会转换成为Unicode	
	void ToString(CString&) const;

	CString ToString() const;

	// 将CJsonObject对象转换为字符串格式,返回的字符串不作任何转换
	CString ToRawString() const;

	//将szOptions字符串格式转换为CJsonObject对象
	bool FromString(LPCTSTR szOptions);

	//释放内存
	void Free(void * p) const;//释放内存

public:
	typedef std::vector<std::string> Members;

	//比较CJsonObject是否一致
	BOOL CompareJson(const CJsonObject *value);

	//比较CJsonObject中指定"名称/值"对是否一致
	BOOL CompareJsonbyKey(LPCTSTR key, const CJsonObject *value);
	
	//枚举Json名称及值类型
//	void EnumJsonMemberNames(CJsonMemberNameList& JsonMemberNameList);

	//判断名称是否为Json成员
	BOOL IsMember(LPCTSTR key);
	
	//按照名称删除Json成员
	void RemoveMember(LPCTSTR key);

private:

	char* _ToString() const;

	void  EnumJsonMemberSetValueEncoding(CJsonObject * lpJson,UCHAR chEncodingType);

	static CString Utf8ToGb32(LPCTSTR lpszUft8Text);

	static CString Gb32ToUtf8(LPCTSTR lpszGb32Text);

	Json::Value m_value;

	enum enumEncodingType
	{
		JSON_UNKNOW=0,
		JSON_UTF8,
		JSON_GB2312
	};

};
/*
class LS_COMMON_API CJsonObjectArray
{
public:
	CJsonObjectArray();
	virtual ~CJsonObjectArray();
	
	//向CJsonObject数组中增加CJsonObject项
	int Add(const CJsonObject &json);
	//获取对应项的CJsonObeject
	CJsonObject& GetAt(int nIndex);
	
	BOOL FromString(CString& strJson);
	//清空CJsonObject数组中所有的项
	void Clear();
	//将CJsonObject转换为字符串格式
	CString ToString();
	CString ToString() const;
	//不对中文进行转义
	CString ToRawString() const;
	//获取数组的个数
	int GetArraySize();
	
private:
	CArray<CJsonObject, const CJsonObject&> m_arrJsons;
};
*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
