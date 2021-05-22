#include "StdAfx.h"
#include "FontHelper.h"
#include "FontQuality.h"

extern CRITICAL_SECTION crPlatformLock;

FontCached *FontCached::m_pFirst = NULL;

FontParameters::FontParameters(LPCTSTR lpszFaceName, float fsize /* = 10 */, int nWeight /* = 400 */, BOOL bItalic /* = FALSE */, int nExtraFontFlag /* = 0 */, int nTechnology /* = 0 */, int nCharacterSet /* = 0 */)
	: m_strFaceName(lpszFaceName)
	, m_fSize(fsize)
	, m_nWeight(nWeight)
	, m_bItalic(bItalic)
	, m_nExtraFontFlag(nExtraFontFlag)
	, m_nTechnology(nTechnology)
	, m_nCharacterSet(nCharacterSet)
{

}

CString FontParameters::GetFaceName() const
{
	return m_strFaceName;
}

float FontParameters::GetFontSize() const
{
	return m_fSize;
}

int FontParameters::GetFontWeight() const
{
	return m_nWeight;
}

BOOL FontParameters::IsFontItalic() const
{
	return m_bItalic;
}

int FontParameters::GetExtraFontFlag() const
{
	return m_nExtraFontFlag;
}

int FontParameters::GetTechnology() const
{
	return m_nTechnology;
}

int FontParameters::GetCharacterSet() const
{
	return m_nCharacterSet;
}

/////////////////////////////////////////////////////////////////////
FormatAndMetrics::FormatAndMetrics(HFONT hFont, int nExtraFontFlag)
	: m_nTechnology(SCWIN_TECH_GDI)
	, m_hFont(hFont)
	, m_nExtraFontFlag(nExtraFontFlag)
	, m_yAscent(2)
	, m_yDescent(1)
	, m_yInternalleading(0)
{
	
}

FormatAndMetrics::~FormatAndMetrics()
{
	if (m_hFont)
		::DeleteObject(m_hFont);

	m_nExtraFontFlag = 0;
	m_yAscent = 2;
	m_yDescent = 1;
	m_yInternalleading = 0;
}

HFONT FormatAndMetrics::GetFont() const
{
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	if (GetObject(m_hFont, sizeof(lf), &lf))
		return 0;
	return CreateFontIndirect(&lf);
}

//////////////////////////////////////////////////////////////////
FontHelper::FontHelper()
{
	m_FontID = NULL;
}

FontHelper::FontHelper(const FontHelper &)
{

}

FontHelper& FontHelper::operator=(const FontHelper &)
{
	return *this;
}

FontHelper::~FontHelper()
{

}

void FontHelper::Create(const FontParameters& fp)
{
	Release();
	if (fp.GetFaceName().IsEmpty() == FALSE)
		m_FontID = FontCached::FindOrCreate(fp);
}

void FontHelper::Release()
{
	if (m_FontID)
		FontCached::ReleaseId(m_FontID);
}


///////////////////////////////////////////////////////////////
FontCached::FontCached(const FontParameters& fp)
	: m_pNext (NULL)
	, m_nUsage(0)
	, m_fSize(1.0)
	, m_nHash(0)
{
	SetLogFont(m_lf, fp);

	m_nTechnology = fp.GetTechnology();
	m_nHash = HashFont(fp);
	m_FontID = NULL;
	if (m_nTechnology == SCWIN_TECH_GDI) {
		HFONT hFont = ::CreateFontIndirect(&m_lf);
		m_FontID = reinterpret_cast<void*>(new FormatAndMetrics(hFont, fp.GetExtraFontFlag()));
	} else {
#if defined(USE_D2D)
		IDWriteTextFormat *pTextFormat;
		const int faceSize = 200;
		WCHAR wszFace[faceSize];
		UTF16FromUTF8(fp.GetFaceName(), static_cast<UINT>(fp.GetFaceName().GetLength())+1, wszFace, faceSize);
		FLOAT fHeight = fp.GetFontSize();
		DWRITE_FONT_STYLE style = fp.IsFontItalic() ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
		HRESULT hr = pIDWriteFactory->CreateTextFormat(wszFace, NULL,
																								static_cast<DWRITE_FONT_WEIGHT>(fp.GetFontWeight()),
																								style,
																								DWRITE_FONT_STRETCH_NORMAL, fHeight, L"en-us", &pTextFormat);
		if (SUCCEEDED(hr)) {
			pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

			const int maxLines = 2;
			DWRITE_LINE_METRICS lineMetrics[maxLines];
			UINT32 lineCount = 0;
			FLOAT yAscent = 1.0f;
			FLOAT yDescent = 1.0f;
			FLOAT yInternalLeading = 0.0f;
			IDWriteTextLayout *pTextLayout = 0;
			hr = pIDWriteFactory->CreateTextLayout(L"X", 1, pTextFormat, 100.0f, 100.0f, &pTextLayout);
			if (SUCCEEDED(hr)) {
				hr = pTextLayout->GetLineMetrics(lineMetrics, maxLines, &lineCount);
				if (SUCCEEDED(hr)) {
					yAscent = lineMetrics[0].baseline;
					yDescent = lineMetrics[0].height - lineMetrics[0].baseline;

					FLOAT emHeight;
					hr = pTextLayout->GetFontSize(0, &emHeight);
					if (SUCCEEDED(hr)) {
						yInternalLeading = lineMetrics[0].height - emHeight;
					}
				}
				pTextLayout->Release();
			}
			m_FontID = reinterpret_cast<void *>(new FormatAndMetrics(pTextFormat, fp.extraFontFlag, yAscent, yDescent, yInternalLeading));
		}
#endif
	}

	m_nUsage = 1;
}

FontCached::~FontCached()
{

}

BOOL FontCached::IsSameAs(const FontParameters& fp)
{
	return (m_fSize == fp.GetFontSize()) && 
			  (m_lf.lfWeight == fp.GetFontWeight()) &&
			  (m_lf.lfItalic == static_cast<BYTE>(fp.IsFontItalic() ? 1 : 0)) &&
			  (m_lf.lfCharSet == fp.GetCharacterSet()) &&
			  (m_lf.lfQuality == Win32MapFontQuality(fp.GetExtraFontFlag())) &&
			  (m_nTechnology == fp.GetTechnology()) && 
			  (fp.GetFaceName().CompareNoCase(m_lf.lfFaceName) == 0) ;
}

void FontCached::Release()
{
	delete reinterpret_cast<FormatAndMetrics *>(m_FontID);
	m_FontID = NULL;
}

FontID FontCached::FindOrCreate(const FontParameters& fp)
{
	FontID nRet = NULL;
	::EnterCriticalSection(&crPlatformLock);
	int nHashFind = HashFont(fp);
	for (FontCached* pCur = m_pFirst; pCur != NULL; pCur = pCur->m_pNext) {
		if ((pCur->m_nHash == nHashFind) && (pCur->IsSameAs(fp))) {
			++pCur->m_nUsage;
			nRet = pCur->m_FontID;
		}
	}

	if (nRet == 0) {
		FontCached* fc = new FontCached(fp);
		if (fc) {
			fc->m_pNext = m_pFirst;
			m_pFirst = fc;
			nRet = fc->m_FontID;
		}
	}

	::LeaveCriticalSection(&crPlatformLock);
	return nRet;
}

void FontCached::ReleaseId(FontID fid)
{
	::EnterCriticalSection(&crPlatformLock);
	FontCached* pfc = m_pFirst;
	for (FontCached* pCur = m_pFirst; pCur != NULL; pCur = pCur->m_pNext) {
		if (pCur->m_FontID == fid) {
			--pCur->m_nUsage;
			if (pCur->m_nUsage == 0) {
				pfc = pCur->m_pNext;
				pCur->Release();
				pCur->m_pNext = NULL;
				delete pCur;
			}
			break;
		}
		pfc = pCur->m_pNext;
	}

	::LeaveCriticalSection(&crPlatformLock);
}

int FontCached::HashFont(const FontParameters &fp) {
	return
		static_cast<int>(fp.GetFontSize()) ^
		(fp.GetCharacterSet() << 10) ^
		((fp.GetExtraFontFlag() & SC_EFF_QUALITY_MASK) << 9) ^
		((fp.GetFontWeight()/100) << 12) ^
		(fp.IsFontItalic() ? 0x20000000 : 0) ^
		(fp.GetTechnology() << 15) ^
		(fp.GetFaceName().IsEmpty() == FALSE);
}

#ifndef CLEARTYPE_QUALITY
#define CLEARTYPE_QUALITY 5
#endif

BYTE FontCached::Win32MapFontQuality(int extraFontFlag) 
{
	switch (extraFontFlag & SC_EFF_QUALITY_MASK)
	{
		case SC_EFF_QUALITY_NON_ANTIALIASED:
			return NONANTIALIASED_QUALITY;
		case SC_EFF_QUALITY_ANTIALIASED:
			return ANTIALIASED_QUALITY;
		case SC_EFF_QUALITY_LCD_OPTIMIZED:
			return CLEARTYPE_QUALITY;
		default:
			return SC_EFF_QUALITY_DEFAULT;
	}
}

void FontCached::SetLogFont(LOGFONT& lf, const FontParameters& fp)
{
	SetLogFont(lf, fp.GetFaceName(), fp.GetCharacterSet(), fp.GetFontSize(), fp.GetFontWeight(), fp.IsFontItalic(), fp.GetExtraFontFlag());
}

void FontCached::SetLogFont(LOGFONT &lf, LPCTSTR lpszFaceName, int nCharacterSet, float fSize, int nWeight, BOOL bItalic, int nExtraFontFlag) 
{
	memset(&lf, 0, sizeof(lf));
	// The negative is to allow for leading
	lf.lfHeight = -(abs(static_cast<int>(fSize + 0.5)));
	lf.lfWeight = nWeight;
	lf.lfItalic = static_cast<BYTE>(bItalic ? 1 : 0);
	lf.lfCharSet = static_cast<BYTE>(nCharacterSet);
	lf.lfQuality = Win32MapFontQuality(nExtraFontFlag);
	/*strncpy(lf.lfFaceName, faceName, sizeof(lf.lfFaceName));*/
	_tcsncpy(lf.lfFaceName, lpszFaceName, _countof(lf.lfFaceName));
}