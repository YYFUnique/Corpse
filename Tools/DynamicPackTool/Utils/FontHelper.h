#pragma once
/**
 * Font management.
 */

typedef void* FontID;

class FontParameters
{
public:
	FontParameters(LPCTSTR lpszFaceName, float fsize = 10, int nWeight = 400, BOOL bItalic = FALSE,
					int nExtraFontFlag = 0, int nTechnology = 0, int nCharacterSet = 0);
public:
	CString GetFaceName() const;
	float GetFontSize() const;
	int GetFontWeight() const;
	BOOL IsFontItalic() const;
	int	GetExtraFontFlag() const;
	int GetTechnology() const;
	int GetCharacterSet() const;
private:
	CString m_strFaceName;
	float		 m_fSize;
	int		 m_nWeight;
	BOOL	 m_bItalic;
	int		 m_nExtraFontFlag;
	int		 m_nTechnology;
	int		 m_nCharacterSet;
};

class FormatAndMetrics
{
public:
	FormatAndMetrics(HFONT hFont, int nExtraFontFlag);
#ifdef USE_D2D
	//FormatAndMetrics(IDWriteTextFormat* pTextFormat, int nExtraFontFlag, FLOAT)
#endif
	~FormatAndMetrics();
public:
	HFONT GetFont() const;
protected:
	int			m_nTechnology;
	HFONT	m_hFont;
	int			m_nExtraFontFlag;
	FLOAT		m_yAscent;
	FLOAT		m_yDescent;
	FLOAT		m_yInternalleading;
};

class FontHelper
{
public:
	FontHelper();
	virtual ~FontHelper();
protected:
	FontHelper(const FontHelper &);
	FontHelper& operator=(const FontHelper&);
public:
	virtual void Create(const FontParameters& fp);
	virtual void Release();

	FontID GetID() const;
	void SetID(FontID fid);
protected:
	FontID m_FontID;
};

class FontCached : public FontHelper
{
public:
	static FontID FindOrCreate(const FontParameters& fp);
	static void ReleaseId(FontID fid);
protected:
	FontCached(const FontParameters& fp);
	~FontCached();
protected:
	BOOL IsSameAs(const FontParameters& fp);
	virtual void Release();
public:
	static int HashFont(const FontParameters &fp);
	static BYTE FontCached::Win32MapFontQuality(int extraFontFlag) ;
	static void SetLogFont(LOGFONT& lf, const FontParameters& fp);
	static void SetLogFont(LOGFONT &lf, LPCTSTR lpszFaceName, int nCharacterSet, float fSize, int nWeight, BOOL bItalic, int nExtraFontFlag) ;
protected:
	FontCached*	m_pNext;
	int					m_nUsage;
	float					m_fSize;
	LOGFONT		m_lf;
	int					m_nTechnology;
	int					m_nHash;

	static FontCached* m_pFirst;
};