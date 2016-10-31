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

//[[ֵ����
	//����CJsonObject��ֵ
	void SetValueItem();
	void SetValueItem(IN int value);
	void SetValueItem(IN unsigned int value);
	void SetValueItem(IN double value);
	void SetValueItem(IN LPCTSTR value);
	void SetValueItem(IN bool value);
	void SetValueItem(IN const CJsonObject *value);

	//��ȡCJsonObject��ֵ
	int GetValueItem(int * value);
	unsigned int GetValueItem(unsigned int * value);
	double GetValueItem(double * value);
	CString GetValueItem();
	bool GetValueItem(bool * value);
	CJsonObject GetValueItem(CJsonObject *value);

	//����CJsonObjectֵ������
	void AppendItem();
	void AppendItem(int value);
	void AppendItem(unsigned int value);
	void AppendItem(double value);
	void AppendItem(LPCTSTR value);
	void AppendItem(bool value);
	void AppendItem(const CJsonObject *value);

	//��ȡCJsonObjectֵ�����������
	int GetArraySize();

	//��ȡCJsonObjectֵ��������ض����ֵ
	CJsonObject GetArrayItem(int nIndex);

	//����CJsonObject������ض����ֵ
	void SetArrayItem(const int &nIndex,const CJsonObject *value);

	//ͨ��ָ�� nIndex ɾ��  ��CJsonObject ֵ�ĵ��ض����ֵ
	//ע��ɾ���� GetArraySize()����Ӧ�ı�
	void DeleteArrayItem(int nIndex);

	//��json������ֵתΪutf8��ʽ
	void AllValueToUtf8();

	//��json������ֵתΪgb2312��ʽ
	void AllValueToGb23();

	//���ƴ�������,�ѱ�GetArrayItem����
	const CJsonObject operator[]( int index ) const;

//]]����/ֵ�Բ���
	//����CJsonObject"����/ֵ"��,key��Ӧ����
	void SetValue(LPCTSTR key);
	void SetValue(LPCTSTR key, int value);
	void SetValue(LPCTSTR key, unsigned int value);
	void SetValue(LPCTSTR key, double value);
	void SetValue(LPCTSTR key, LPCTSTR value);
	void SetValue(LPCTSTR key, bool value);
	void SetValue(LPCTSTR key, const CJsonObject *value);
//	void SetValue(LPCTSTR key, const CJsonObjectArray *value);

	//��ȡkey��Ӧ���Ƶ�"����/ֵ"�Ե�ֵ
	int GetValue(LPCTSTR key, int * value);
	unsigned int GetValue(LPCTSTR key, unsigned int * value);
	double GetValue(LPCTSTR key, double * value);
	CString GetValue();
	//const char * GetValue(const char *key, const char **value);//��Ҫ�ֶ��ͷ��ڴ�
	bool GetValue(LPCTSTR key, bool * value);
	CJsonObject * GetValue(LPCTSTR key, CJsonObject *value);
	CString GetValue(LPCTSTR lpszKeyName);
	////////////////////////////////////////////////////////
	// �������ƣ�GetUTF8Value
	// �������ã���ȡֵUTF8���������
	// ����������lpszKeyName����
	// ��������ֵ����ȡ����UTF8խ�ֽڱ���
	////////////////////////////////////////////////////////
	CStringA GetUTF8Value(LPCTSTR lpszKeyName);
	//���key��Ӧ"����/ֵ"����,ֵ�����������
	void Append(LPCTSTR key);
	void Append(LPCTSTR key, int value);
	void Append(LPCTSTR key, unsigned int value);
	void Append(LPCTSTR key, double value);
	void Append(LPCTSTR key, LPCTSTR value);
	void Append(LPCTSTR key, bool value);
	void Append(LPCTSTR key, const CJsonObject *value);

	//���"����/ֵ"��,��ֵΪ������ʽ������
	void ResetArray(LPCTSTR key);

	//��"����/ֵ"�����е�ֵ����ת��Ϊ�ַ�������
//	void GetSubValueToArray(LPCTSTR key,CStringArray&);
	
	//BOOL GetSubValueToArray(LPCTSTR key,CMap<DWORD,DWORD,DWORD,DWORD>& IpHostMap);

//	BOOL GetSubValueToArray(LPCTSTR key,CDWordArray& dwIpArray);

	//���CJsonObject����������
	void Clear();
	
	//����CJsonObject����
	void Duplicate(const CJsonObject *value);
	
	//��CJsonObject����ת��Ϊ�ַ�����ʽ,���ص��ַ�����ת����ΪUnicode	
	void ToString(CString&) const;

	CString ToString() const;

	// ��CJsonObject����ת��Ϊ�ַ�����ʽ,���ص��ַ��������κ�ת��
	CString ToRawString() const;

	//��szOptions�ַ�����ʽת��ΪCJsonObject����
	bool FromString(LPCTSTR szOptions);

	//�ͷ��ڴ�
	void Free(void * p) const;//�ͷ��ڴ�

public:
	typedef std::vector<std::string> Members;

	//�Ƚ�CJsonObject�Ƿ�һ��
	BOOL CompareJson(const CJsonObject *value);

	//�Ƚ�CJsonObject��ָ��"����/ֵ"���Ƿ�һ��
	BOOL CompareJsonbyKey(LPCTSTR key, const CJsonObject *value);
	
	//ö��Json���Ƽ�ֵ����
//	void EnumJsonMemberNames(CJsonMemberNameList& JsonMemberNameList);

	//�ж������Ƿ�ΪJson��Ա
	BOOL IsMember(LPCTSTR key);
	
	//��������ɾ��Json��Ա
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
	
	//��CJsonObject����������CJsonObject��
	int Add(const CJsonObject &json);
	//��ȡ��Ӧ���CJsonObeject
	CJsonObject& GetAt(int nIndex);
	
	BOOL FromString(CString& strJson);
	//���CJsonObject���������е���
	void Clear();
	//��CJsonObjectת��Ϊ�ַ�����ʽ
	CString ToString();
	CString ToString() const;
	//�������Ľ���ת��
	CString ToRawString() const;
	//��ȡ����ĸ���
	int GetArraySize();
	
private:
	CArray<CJsonObject, const CJsonObject&> m_arrJsons;
};
*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
