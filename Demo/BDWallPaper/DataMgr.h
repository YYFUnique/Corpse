#include "StdAfx.h"
#include <atlstr.h>
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")
#include <string>
using namespace std;
#include "helper/jsoncpp/json.h"
#include <Urlmon.h>
#pragma comment(lib, "Urlmon.lib")

#define WM_GETWPPAGE_OK	WM_USER + 100
#define WM_GETWPINFO_OK	WM_USER + 101

struct WPInfoKey
{
	CString sType;
	int nPage;
};

static bool operator == (const WPInfoKey& key1, const WPInfoKey& key2)
{
	return (key1.sType.CompareNoCase(key2.sType) == 0) && (key1.nPage == key2.nPage);
}

static bool operator < (const WPInfoKey& key1, const WPInfoKey& key2)
{
	if(key1.sType.CompareNoCase(key2.sType) == 0)
	{
		return key1.nPage < key2.nPage;
	}
	else
	{
		return key1.sType < key2.sType;
	}
}

struct WPPage
{
	CString sType;
	int nTotalPage;
};

struct WPInfo
{
	string flag;
	string flag_desc;
	string id;
	string thumb_mid;
	string thumb_nail;
};

class CDataMgr
{
public:
	CDataMgr()
	{
		m_hWnd = NULL;
	}

public:
	void Init(HWND hWnd)
	{
		m_hWnd = hWnd;
	}

	int LoadBizhi(LPCTSTR lpszType, int nPage, int nCount = 10)
	{
		WPInfoKey* pKey = new WPInfoKey;
		pKey->sType = lpszType;
		pKey->nPage = nPage;
		if(m_mapWPInfos.find(*pKey) != m_mapWPInfos.end())
		{
			return 1;
		}

		CDuiString sUrl;
		sUrl.Format(_T("http://bizhi.baidu.com/wallpaper/getList?type=%s&page=%d&count=%d&g=C_0-D_100825PBN40317ERB89E-M_206A8A1263C4-V_DE5571AC-T_20140714225619485&tn=bdwp&version=2.0.0.1160&_t=%d"), lpszType, nPage, nCount, GetTickCount());
		std::string data = WebReadFile(sUrl);

		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(data.c_str(), root, false))
		{
			return -1;
		}

		string errormsg = root["errormsg"].asString();
		int errorno = root["errorno"].asInt();
		if(errorno != 0) return -1;
		int ntotal = 0;
		if(root["totalPage"].type() == Json::intValue)
		{
			ntotal = root["totalPage"].asInt();
		}
		else
		{
			string totalpage = root["totalPage"].asString();
			ntotal = atoi(totalpage.c_str());
		}
		// ����ҳ��
		WPPage *pPage = new WPPage;
		pPage->sType = lpszType;
		pPage->nTotalPage = ntotal;
		m_mapWPPages[lpszType] = *pPage;
		::PostMessage(m_hWnd, WM_GETWPPAGE_OK, (WPARAM)pPage, 0);

		int page = root["page"].asInt();
		Json::Value urls = root["list"];
		for(UINT i = 0; i < urls.size(); ++i)
		{
			Json::Value url = urls[i];
			WPInfo Info;
			Info.flag = url["flag"].asString();
			Info.flag_desc = url["flag_desc"].asString();
			Info.id = url["id"].asString();
			Info.thumb_mid = url["thumb_mid"].asString();
			Info.thumb_nail = url["thumbnail"].asString();
			m_mapWPInfos[*pKey].push_back(Info);
		}

		::PostMessage(m_hWnd, WM_GETWPINFO_OK, (WPARAM)pKey, 0);
		return 0;
	}

	bool GetBizhi(CString sType, int nPage, std::vector<WPInfo>& vInfos)
	{
		WPInfoKey Key;
		Key.sType = sType;
		Key.nPage = nPage;
		std::map<WPInfoKey, std::vector<WPInfo>>::iterator it = m_mapWPInfos.find(Key);
		if(it != m_mapWPInfos.end())
		{
			vInfos = it->second;
			return true;
		}

		return false;
	}
private:
	static string WebReadFile(LPCTSTR lpszUrl)
	{
		string data;
		// ��http����
		HINTERNET hConnect = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0); 

		if (hConnect)
		{
			DWORD dwTimeOut = 1;
			InternetSetOption(hConnect, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut));

			HINTERNET hSession = InternetOpenUrl(hConnect, lpszUrl, NULL, 0, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_PRAGMA_NOCACHE, 0);
			if (hSession)
			{
				// �������ݻ�����
				DWORD dwRead = 0;
				DWORD dwBuffer = 1024 * 1024;
				char *szBuffer = new char[dwBuffer];
				memset(szBuffer, 0, dwBuffer);

				if(InternetReadFile(hSession, szBuffer, dwBuffer, &dwRead) && (dwRead > 0))
				{
					int nLen = dwRead;
					char* pBuffer = new char[nLen + 1];
					memset(pBuffer, 0, nLen + 1);
					memcpy(pBuffer, szBuffer, nLen);
					if(pBuffer != NULL)
					{
						data = pBuffer;
					}
				}

				// �������ݻ�����
				delete []szBuffer;
				szBuffer = NULL;

				InternetCloseHandle(hSession);
			}
			InternetCloseHandle(hConnect);
		}

		return data;
	}


private:
	HWND m_hWnd;
	std::map<CString, WPPage> m_mapWPPages;
	std::map<WPInfoKey, std::vector<WPInfo>> m_mapWPInfos;
};