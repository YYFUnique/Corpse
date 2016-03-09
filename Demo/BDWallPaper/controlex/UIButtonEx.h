#ifndef __BUTTONEX_H__
#define __BUTTONEX_H__

namespace DuiLib
{
	#define DUI_MSGTYPE_CLICK_WPFAV            (_T("click_wpfav"))
	#define DUI_MSGTYPE_CLICK_WPSET            (_T("click_wpset"))

	class CWPButton : public CButtonUI
	{
	public:
		CWPButton()
		{
		}
		~CWPButton(void)
		{
		}

	public:
		virtual void PaintStatusImage(HDC hDC)
		{
			// 控件位置
			RECT rcItem = m_rcItem;
			// 绘制壁纸图片
			CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, PrepareImage(), NULL);
			// 绘制状态
			CButtonUI::PaintStatusImage(hDC);
			// 工具栏位置
			RECT rcToolBar = rcItem;
			RECT rcFav = rcItem;
			RECT rcSet = rcItem;
			rcToolBar.top = rcToolBar.bottom - 25;
			rcFav.top = rcFav.bottom - 25;
			rcFav.right = rcFav.left + 86;
			rcSet.top = rcSet.bottom - 25;
			rcSet.left = rcSet.right - 86;
			// 工具栏绘制
			CRenderEngine::DrawImageString(hDC, m_pManager, rcToolBar, m_rcPaint, m_sWPToolbarImage, NULL);
			CRenderEngine::DrawImageString(hDC, m_pManager, rcFav, m_rcPaint, m_sWPFavImage, NULL);
			CRenderEngine::DrawImageString(hDC, m_pManager, rcSet, m_rcPaint, m_sWPSetImage, NULL);
		}
		CString PrepareImage()
		{
			CString sAppPath = CPaintManagerUI::GetInstancePath().GetData();
			CString sImageName = CrackUrl(m_sWPImage.GetData());
			sImageName.Replace(_T("/"), _T("\\"));
			TCHAR szFile[MAX_PATH] = {0};
			lstrcpy(szFile, sAppPath);
			::PathAppend(szFile, sImageName);
			if(!::PathFileExists(szFile))
			{
				CreateDirectory(szFile, NULL);
				::URLDownloadToFile(NULL, m_sWPImage, szFile, 0, NULL);
			}
			return szFile;

		}
		BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
		{
			TCHAR cPath[MAX_PATH] = {0};
			TCHAR cTmpPath[MAX_PATH] = {0};
			TCHAR * lpPos = NULL;
			TCHAR cTmp = _T('\0');

			if (NULL == lpPathName || NULL == *lpPathName)
				return FALSE;

			_tcsncpy(cPath, lpPathName, MAX_PATH);

			for (int i = 0; i < (int)_tcslen(cPath); i++)
			{
				if (_T('\\') == cPath[i])
					cPath[i] = _T('/');
			}

			lpPos = _tcschr(cPath, _T('/'));
			while (lpPos != NULL)
			{
				if (lpPos == cPath)
				{
					lpPos++;
				}
				else
				{
					cTmp = *lpPos;
					*lpPos = _T('\0');
					_tcsncpy(cTmpPath, cPath, MAX_PATH);
					::CreateDirectory(cTmpPath, lpSecurityAttributes);
					*lpPos = cTmp;
					lpPos++;
				}
				lpPos = _tcschr(lpPos, _T('/'));
			}

			return TRUE;
		}

		CString CrackUrl(CString sUrl)
		{
			URL_COMPONENTS uc;
			TCHAR Scheme[1000];
			TCHAR HostName[1000];
			TCHAR UserName[1000];
			TCHAR Password[1000];
			TCHAR UrlPath[1000];
			TCHAR ExtraInfo[1000];

			uc.dwStructSize = sizeof(uc);
			uc.lpszScheme = Scheme;
			uc.lpszHostName = HostName;
			uc.lpszUserName = UserName;
			uc.lpszPassword = Password;
			uc.lpszUrlPath = UrlPath;
			uc.lpszExtraInfo = ExtraInfo;

			uc.dwSchemeLength = 1000;
			uc.dwHostNameLength = 1000;
			uc.dwUserNameLength = 1000;
			uc.dwPasswordLength = 1000;
			uc.dwUrlPathLength = 1000;
			uc.dwExtraInfoLength = 1000;

			InternetCrackUrl(sUrl, 0, 0, &uc);
			return UrlPath;
		}

	public:
		void SetWPImage(LPCTSTR pstrImage)
		{
			m_sWPImage = pstrImage;
			Invalidate();
		}
		void SetWPToolbarImage(LPCTSTR pstrImage)
		{
			m_sWPToolbarImage = pstrImage;
		}
		void SetWPFavImage(LPCTSTR pstrImage)
		{
			m_sWPFavImage = pstrImage;
		}
		void SetWPSetImage(LPCTSTR pstrImage)
		{
			m_sWPSetImage = pstrImage;
		}
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
		{
			if( _tcsicmp(pstrName, _T("wpimage")) == 0 ) SetWPImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("wptoolbarimage")) == 0 ) SetWPToolbarImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("wpfavimage")) == 0 ) SetWPFavImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("wpsetimage")) == 0 ) SetWPSetImage(pstrValue);
			else CButtonUI::SetAttribute(pstrName, pstrValue);
		}
	private:
		CDuiString m_sWPImage;
		CDuiString m_sWPToolbarImage;
		CDuiString m_sWPFavImage;
		CDuiString m_sWPSetImage;
	};
}

#endif __BUTTONEX_H__
