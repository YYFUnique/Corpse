#pragma once

namespace DuiLib
{
	class UILIB_API CImageAttribute
	{
	public:
		CImageAttribute();
		CImageAttribute(const CImageAttribute&);
		const CImageAttribute& operator=(const CImageAttribute&);
		virtual ~CImageAttribute();
	public:
		CDuiString GetAttributeString() const;
		void SetAttributeString(LPCTSTR pStrImageAttr);

		void ModifyAttribute(LPCTSTR pStrModify);
		bool LoadImage(CPaintManagerUI* pManager);
		bool IsLoadSuccess();
		operator LPCTSTR() const;

		bool operator == (LPCTSTR pStrImage) const;
		const CDuiString& operator=(const CDuiString& src);
		const CDuiString& operator=(const TCHAR ch);
		const CDuiString& operator=(LPCTSTR pstr);
	public:
		LPCTSTR GetImage() const;
		LPCTSTR GetResType() const;
		TImageInfo* GetImageInfo() const;
		RECT GetDestRect() const;
		RECT GetCornerRect() const;
		RECT GetSourceRect() const;
		BYTE GetFade() const;
		DWORD GetMask() const;
		bool IsHole() const;
		bool IsTiledX() const;
		bool IsTiledY() const;
	private:
		void Clone(const CImageAttribute&);
		void Clear();
		void ParseAttribute(LPCTSTR pStrImageAttr);
	protected:
		CDuiString m_sImageAttribute;

		CDuiString	m_sImage;
		CDuiString	m_sResType;
		TImageInfo*	m_pImageInfo;
		bool			m_bLoadSuccess;

		RECT			m_rcDest;
		RECT			m_rcSource;
		RECT			m_rcCorner;
		BYTE			m_bFade;
		DWORD		m_dwMask;
		bool				m_bHole;
		bool				m_bTiledX;
		bool				m_bTiledY;
	};
}