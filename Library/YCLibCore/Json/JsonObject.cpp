// JsonObject.cpp : implementation file
//

#include "stdafx.h"
#include "JsonObject.h"
#include "json/json.h"

#define MAX_REPORT_INFO_SIZE	20000

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif

LPWSTR AnsiToWideChar(LPCSTR lpAnsiStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, lpAnsiStr, -1, NULL, 0);
	if(!len)
		return NULL;
	
	LPWSTR lpWStr = new WCHAR[len];
	if(!lpWStr)
		return NULL;
	
	MultiByteToWideChar(CP_ACP, 0, lpAnsiStr, -1, lpWStr, len);
	
	return lpWStr;
}

/////////////////////////////////////////////////////////////////////////////
// CJsonObject

CJsonObject::CJsonObject()
{

}

CJsonObject::~CJsonObject()
{

}

void CJsonObject::SetValueItem()
{
	m_value=Json::nullValue;
}

void CJsonObject::SetValueItem(int value)
{
	m_value=value;
}

void CJsonObject::SetValueItem(unsigned int value)
{
	m_value=value;
}

void CJsonObject::SetValueItem(double value)
{
	m_value=value;
}

void CJsonObject::SetValueItem(LPCTSTR value)
{
	CStringA strValue(value);

	m_value=(LPCSTR)strValue;
}

void CJsonObject::SetValueItem(bool value)
{
	m_value=value;
}

void CJsonObject::SetValueItem(const CJsonObject *value)
{
	m_value=value->m_value;
}

int CJsonObject::GetValueItem(int * value)
{
	int nValueTemp=0;

	nValueTemp=m_value.asInt();
	if (NULL != value)
		*value=nValueTemp;
	
	return nValueTemp;
}

unsigned int CJsonObject::GetValueItem(unsigned int * value)
{
	unsigned int uValueTemp=0;

	uValueTemp=m_value.asUInt();
	if(NULL != value)
		*value=uValueTemp;

	return uValueTemp;
}

double CJsonObject::GetValueItem(double * value)
{
	double dValueTemp=0;

	dValueTemp=m_value.asDouble();
	if(NULL != dValueTemp)
		*value=dValueTemp;

	return dValueTemp;
}

CString CJsonObject::GetValueItem()//需要手动释放内存
{
	CString strReturn;//(m_value.asString());
	strReturn.Format(_T("%s"),m_value.asString());
	return strReturn;
}

bool CJsonObject::GetValueItem(bool * value)
{
	bool bValueTemp=false;

	bValueTemp=m_value.asBool();
	if(NULL != value)
		*value=bValueTemp;

	return bValueTemp;
}

CJsonObject CJsonObject::GetValueItem(CJsonObject *value)
{
	CJsonObject JsonObjectTemp;

	JsonObjectTemp.m_value=m_value;
	if (NULL != value)
		value->Duplicate(&JsonObjectTemp);

	return JsonObjectTemp;
}

void CJsonObject::AppendItem()
{
	m_value.append(Json::nullValue);
}

void CJsonObject::AppendItem(int value)
{
	m_value.append(value);
}

void CJsonObject::AppendItem(unsigned int value)
{
	m_value.append(value);
}

void CJsonObject::AppendItem(double value)
{
	m_value.append(value);
}

void CJsonObject::AppendItem(LPCTSTR value)
{
	ASSERT(NULL!=value);
	CStringA strValue(value);
	m_value.append((LPCSTR)strValue);
}

void CJsonObject::AppendItem(bool value)
{
	m_value.append(value);
}

void CJsonObject::AppendItem(const CJsonObject *value)
{
	ASSERT(NULL!=value);

	//解决调用:jsonObject.AppendItem(&jsonObject) 多出一个null的问题
	CJsonObject JsonObjectTemp;
	JsonObjectTemp.Duplicate(value);

	m_value.append(JsonObjectTemp.m_value);
}

CJsonObject CJsonObject::GetArrayItem(int nIndex)
{
	CJsonObject JsonObjectTemp;
	JsonObjectTemp.m_value=m_value[nIndex];

	return JsonObjectTemp;
}

void CJsonObject::SetArrayItem(const int &nIndex,const CJsonObject *value)
{
	if(nIndex>=(int)m_value.size())//超界
		return;
	m_value[nIndex]=value->m_value;
}

void CJsonObject::DeleteArrayItem(int nIndex)
{
	if(nIndex>=(int)m_value.size())//超界
		return;
	Json::Value  jValueTemp=Json::nullValue;
	for(int i=0,j=0;i<(int)m_value.size();++i)
	{
		if(i!=nIndex)
		{
			jValueTemp[j++]=m_value[i];
		}
	}

	m_value=jValueTemp;
	
}

CString CJsonObject::Gb32ToUtf8(LPCTSTR lpszGb32Text)
{
	CStringA strGB32Text(lpszGb32Text);

	int nUnicodeBufLen=MultiByteToWideChar(CP_ACP, 0, strGB32Text, -1, 0, 0);
	if (nUnicodeBufLen==0)
		return _T("");

	WCHAR* pUnicodeBuf=new WCHAR[nUnicodeBufLen];
	if (pUnicodeBuf==0)
		return _T("");

	MultiByteToWideChar(CP_ACP, 0, strGB32Text, -1, pUnicodeBuf, nUnicodeBufLen);

	int nUtf8BufLen=WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuf, -1, 0, 0, NULL, NULL);
	if (nUtf8BufLen==0)
	{
		delete[] pUnicodeBuf;
		return _T("");
	}

	char* pUft8Buf=new char[nUtf8BufLen];
	if (pUft8Buf==0)
	{
		delete[] pUnicodeBuf;
		return _T("");
	}

	WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuf, -1, pUft8Buf, nUtf8BufLen, NULL, NULL);

	CString strUtf8(pUft8Buf);

	delete[] pUnicodeBuf;
	delete[] pUft8Buf;

	return strUtf8;
}

CString CJsonObject::Utf8ToGb32(LPCTSTR lpszUft8Text)
{
	CStringA strUTF8Text(lpszUft8Text);
	int nUnicodeBufLen=MultiByteToWideChar(CP_UTF8, 0, strUTF8Text, -1, 0, 0);
	if (nUnicodeBufLen==0)
		return _T("");

	WCHAR* pUnicodeBuf=new WCHAR[nUnicodeBufLen];
	if (pUnicodeBuf==0)
		return _T("");

	MultiByteToWideChar(CP_UTF8, 0, strUTF8Text, -1, pUnicodeBuf, nUnicodeBufLen);

	int nGb32BufLen=WideCharToMultiByte(CP_ACP, 0, pUnicodeBuf, -1, 0, 0, NULL, NULL);
	if (nGb32BufLen==0)
	{
		delete[] pUnicodeBuf;
		return _T("");
	}

	char* pGb32Buf=new char[nGb32BufLen];
	if (pGb32Buf==0)
	{
		delete[] pUnicodeBuf;
		return _T("");
	}

	WideCharToMultiByte(CP_ACP, 0, pUnicodeBuf, -1, pGb32Buf, nGb32BufLen, NULL, NULL);

	CString strGb32(pGb32Buf);

	delete[] pUnicodeBuf;
	delete[] pGb32Buf;

	return strGb32;
}

const CJsonObject CJsonObject::operator[]( int index ) const
{
	CJsonObject JsonObject;
	JsonObject.m_value=m_value[index];
	return JsonObject; 
}


void CJsonObject::SetValue(LPCTSTR key)
{
	CStringA strKey(key);
	m_value[strKey] = Json::Value();
}

void CJsonObject::SetValue(LPCTSTR key, int value)
{
	CStringA strkey(key);
	m_value[strkey] = value;
}

void CJsonObject::SetValue(LPCTSTR key, unsigned int value)
{
	CStringA strKey(key);
	m_value[strKey] = value;
}

void CJsonObject::SetValue(LPCTSTR key, double value)
{
	CStringA strKey(key);
	m_value[strKey] = value;
}

void CJsonObject::SetValue(LPCTSTR key, LPCTSTR value)
{
	CStringA strKey(key);
	CStringA strValue(value);
	m_value[strKey] = (LPCSTR)strValue;
}

void CJsonObject::SetValue(LPCTSTR key, bool value)
{
	CStringA strKey(key);
	m_value[strKey] = value;
}

void CJsonObject::SetValue(LPCTSTR key, const CJsonObject *value)
{
	if(value)
	{
		//解决JsonObject.SetValue("Key", &JsonObject);出现给"key"赋null
		CStringA strKey(key);
		CJsonObject JsonObjectTemp;
		JsonObjectTemp.Duplicate(value);
		m_value[strKey] =JsonObjectTemp.m_value;
	}
}

int CJsonObject::GetValue(LPCTSTR key, int * value)
{
	int nRet = 0;
	CStringA strKey(key);
	if (NULL != value)
	{
		*value = m_value[strKey].asInt();
		nRet = *value;
	}
	return nRet;
}

unsigned int CJsonObject::GetValue(LPCTSTR key, unsigned int * value)
{
	unsigned int nRet = 0;
	CStringA strKey(key);
	if (NULL != value)
	{
		*value = m_value[strKey].asUInt();
		nRet = *value;
	}
	return nRet;
}

double CJsonObject::GetValue(LPCTSTR key, double * value)
{
	double ret = 0;
	CStringA strKey(key);
	if (NULL != value)
	{
		*value = m_value[strKey].asDouble();
		ret = *value;
	}
	return ret;
}

CString CJsonObject::GetValue()
{
	CString strReturn(m_value.asString().c_str());
	return strReturn;
}

CString CJsonObject::GetValue(LPCTSTR lpszKeyName)
{
	CStringA strKey(lpszKeyName);
	
	CString strReturn(m_value[strKey].asString().c_str());
	return strReturn;
}

CStringA CJsonObject::GetUTF8Value(LPCTSTR lpszKeyName)
{
	CStringA strKey(lpszKeyName);

	CStringA strReturn(m_value[strKey].asString().c_str());
	return strReturn;
}

bool CJsonObject::GetValue(LPCTSTR key, bool * value)
{
	bool bRet = false;
	
	if (NULL != value)
	{
		CStringA strKey(key);
		*value = m_value[strKey].asBool();
		bRet = *value;
	}
	return bRet;
}

CJsonObject * CJsonObject::GetValue(LPCTSTR key, CJsonObject *value)
{
	if (NULL != value)
	{
		CStringA strkey(key);
//		((CJsonObject*)value)->Duplicate(this);
		(value->m_value)=m_value[strkey];
	}
	return value;
}

void CJsonObject::Free(void * p) const
{
	if (NULL != p)
	{
		free(p);
		p = NULL;
	}
}

void CJsonObject::Append(LPCTSTR key)
{
	CStringA strKey(key);
	m_value[strKey].append(Json::Value());
}

void CJsonObject::Append(LPCTSTR key, int value)
{
	CStringA strKey(key);
	m_value[strKey].append(value);
}

void CJsonObject::Append(LPCTSTR key, unsigned int value)
{
	CStringA strKey(key);
	m_value[strKey].append(value);
}

void CJsonObject::Append(LPCTSTR key, double value)
{
	CStringA strKey(key);
	m_value[strKey].append(value);
}

void CJsonObject::Append(LPCTSTR key,LPCTSTR value)
{
	CStringA strKey(key);
	CStringA strValue(value);
	m_value[strKey].append((LPCSTR)strValue);
}

void CJsonObject::Append(LPCTSTR key, bool value)
{
	CStringA strKey(key);
	m_value[strKey].append(value);
}

void CJsonObject::Append(LPCTSTR key, const CJsonObject *value)
{
	if(value)
	{
		CStringA strKey(key);
		CJsonObject JsonObjectTemp;
		JsonObjectTemp.Duplicate(value);

		m_value[strKey].append(JsonObjectTemp.m_value);
	}
}

int CJsonObject::GetArraySize()
{
	return m_value.size();
}

void CJsonObject::ResetArray(LPCTSTR key)
{
	CStringA strKey(key);
	m_value[strKey].append(1);
	m_value[strKey].clear();
}

void CJsonObject::Clear()
{
	m_value.clear();
}

void CJsonObject::Duplicate(const CJsonObject *value)
{
	if(value)
	{
		m_value = ((CJsonObject*)value)->m_value;
	}
}

char* CJsonObject::_ToString() const
{
	LPWSTR lpWStr = AnsiToWideChar(Json::FastWriter().write(m_value).c_str());
	if(!lpWStr)
		return NULL;

	std::string str;

	int len = lstrlenW(lpWStr);
	for(int i = 0; i < len; i++)
	{
		if(lpWStr[i] >= 0x20 && lpWStr[i] <= 0x7e)
		{
			str += (char)lpWStr[i];
		}
		else
		{
			char fmt[] = "\\u0000";	// format buffer
			wsprintfA(fmt, "\\u%04x", lpWStr[i]);
			str += fmt;
		}
	}

	delete[] lpWStr;

	int nLen = str.length() + 1;
	char *pStr = new char[nLen];
	if(pStr)
		strcpy_s(pStr, nLen,str.c_str());

	return pStr;
}

CString CJsonObject::ToRawString() const 
{
	CString strText(Json::FastWriter().write(m_value).c_str());
	return strText;
}

void CJsonObject::ToString(CString& strText) const
{
	char* lpszText=_ToString();
	strText=lpszText;
	if (lpszText != NULL)
	{
		delete [] lpszText;
		lpszText = NULL;
	}
}

CString CJsonObject::ToString() const
{
	char* lpszText=_ToString();
	CString strText(lpszText);
	if (lpszText != NULL)
	{
		delete [] lpszText;
		lpszText = NULL;
	}
	return strText;
}

bool CJsonObject::FromString(LPCTSTR szOptions)
{
	CStringA strJsonString(szOptions);
	Json::Reader reader;
	return reader.parse((LPCSTR)strJsonString, m_value);
}

BOOL CJsonObject::CompareJson(const CJsonObject *value)
{
	return m_value==((CJsonObject*)value)->m_value;
}

BOOL CJsonObject::CompareJsonbyKey(LPCTSTR key, const CJsonObject *value)
{
	if (!key)
		return FALSE;

	CStringA strkey(key);

	if (!m_value.isMember(strkey) || !((CJsonObject*)value)->m_value.isMember(strkey))
		return FALSE;

	if (m_value[strkey].type() != ((CJsonObject*)value)->m_value[strkey].type())
		return FALSE;
	
	if (m_value[strkey] != ((CJsonObject*)value)->m_value[strkey])
		return FALSE;

	return TRUE;
}
//
//void CJsonObject::EnumJsonMemberNames(CJsonMemberNameList& JsonMemberNameList)
//{
//	JsonMemberNameList.RemoveAll();	
//	Members member = m_value.getMemberNames();
//	for (int i=0;i<member.end()-member.begin();i++)
//	{
//		ENUM_JSON_MEMBER_NAME EnumJsonMemberName;
//		EnumJsonMemberName.strMemberName.Format(_T("%s"), member.at(i).c_str());
//		CStringA strMem(EnumJsonMemberName.strMemberName);
//		EnumJsonMemberName.enumType = m_value[(LPCSTR)strMem].type();
//		JsonMemberNameList.AddTail(EnumJsonMemberName);
//	}
//}

BOOL CJsonObject::IsMember(LPCTSTR key)
{
	if (key == NULL)
		return FALSE;

	CStringA strKey(key);
	return m_value.isMember(strKey);
}

void CJsonObject::RemoveMember(LPCTSTR key)
{
	CStringA strKey(key);
	m_value.removeMember(strKey);
}


//////////////////////////////////////////////////////////////////////
// CJsonObjectArray
