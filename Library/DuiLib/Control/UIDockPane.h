#pragma once

namespace DuiLib
{
	class CDockItemUI : public CButtonUI
	{
	public:
		CDockItemUI();
		~CDockItemUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		/*
		 *	@brief ��дDoEvent���޸Ķ�MOUSEMOVE��MOUSELEAVE��Ϣ����Ӧ
		 */
		void DoEvent(TEventUI& event);

		void SetPos(RECT rc);

	private:
	};

	typedef enum tagDOCKPANE_POSITION
	{
		DOCKPANE_POSITION_LEFT			= 0,				//λ����
		DOCKPANE_POSITION_TOP			= 1,				//λ����
		DOCKPANE_POSITION_RIGHT		= 2,				//λ����
		DOCKPANE_POSITION_BOTTOM   = 3,				//λ����
	}DOCKPANE_POSITION;

	class CDockPaneUI : public CHorizontalLayoutUI
	{
	public:
		CDockPaneUI(void);
		~CDockPaneUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		/*
		 *	@brief ֻ���CDockItem�ؼ�
		 */
		bool Add(CControlUI* pControl) ;

		/*
		 *	@brief ��дSetPos���������ڲ��ӿؼ�Ĭ�ϵ׶����ж����Ƕ������У�����ȥ������Ҫ�Ĵ���
		 */
		void SetPos(RECT rc) ;

		/*
		 *	@brief ΪSetItemSize��SetItemMaxRate��SetChangeRange���Ӷ�Ӧ������
		 */
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) ;
		void DoEvent(TEventUI& event) ;
		
		/*
		 *	@param sz:����/��ȡÿһ���ӿؼ��Ĵ�С��Ĭ�� 60x60
		 */
		void SetItemSize(SIZE sz);
		SIZE GetItemSize() const;

		/*
		 *	@param dRate:����/��ȡ�ӿؼ����仯������Ĭ�� 2.0
		 */
		void SetItemMaxRate(double dRate);
		double GetItemMaxRate() const;

		/*
		 *	@param nNum:����/��ȡ�ӿؼ�����Ӱ��ķ�Χ����λ�ǿؼ��ĸ�����Ĭ��3����Ӱ������ӿؼ�������3���ؼ���С
		 */
		void SetChangeRange(int nNum);
		int	 GetChangeRange() const;


		/*
		 *	@param szPositon:����/��ȡ�ӿؼ���λ�ã�Ĭ��Ϊbottom
		 */
		void SetPositon(DOCKPANE_POSITION DockPanePosition);
		DOCKPANE_POSITION GetPositon() const;

		/*
		 *  @brief �ӿؼ�����֪ͨDockPane�Ľӿڣ�֪ͨ��������������뿪ĳ���ӿؼ�
		 *	@param pItem:��ǰ������ڵ��ӿؼ���ָ��
		 *	@param dRate:��ǰ�ӿؼ�������xλ��ռ�����ؼ��ĵĿ�ȵı���
		 */
		void NotifyEnterItem(CDockItemUI* pItem, double dXRate, double dYRate);
		void NotifyLeaveItem(CDockItemUI* pItem);

	private:
		SIZE	m_szItemSize ;
		double	m_dbMaxRate;
		int		m_nChangeRange;
		DOCKPANE_POSITION m_DockPane;
	};
}
