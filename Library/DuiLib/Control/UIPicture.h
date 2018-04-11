/************************************************************************/
//ͼƬ������ʾ���ͼ��ʵ��ͼƬ�ĵ��� ������ֻ�������� ����ͼ��ǰ��ͼ���� ������Ŀ����Լ����
/************************************************************************/
#pragma once
namespace DuiLib
{
	class UILIB_API CPictureUI : public CControlUI
	{
	public:
		CPictureUI(void);
		~CPictureUI(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetIcon(HICON hIcon);

		void SetForeImage( LPCTSTR pStrImage );
		void PaintStatusImage(HDC hDC);

	protected:
		HICON			m_hIcon;
		CDuiString m_strResourcePath;
	};
}