 /**************************************************************************
   @file  wnd_magnet.h
   * Author: qinqing
   * Date:   2009-12-10
 ***************************************************************************/
 
 #ifndef _WND_MAGNET_H
 #define _WND_MAGNET_H
 
#include <map>
#include <vector>
#include <set>

/** 
  @brief ���Դ��ڹ�����
  * ʵ������������ƶ���������ϻ����������,�����������һ��ʱ����һ���ƶ�
  * ʹ��ʱ,����໥���ԵĴ���Ҫ����ͬһ��CWndMagnet����
*/
class CWndMagnet
{
public:
    CWndMagnet();
	virtual ~CWndMagnet();

public:
    void SetLeadWindow(HWND hWnd) { m_hLead = hWnd; }
    HWND GetLeadWindow() const { return m_hLead; }
    
    void AddMagnetWnd(HWND hWnd);
    void RemoveMagnetWnd(HWND hWnd);
    void OnLButtonDown(HWND hWnd);
    void OnNcLButtonDown(HWND hWnd);
    void OnMoving(HWND hWnd, LPRECT lpRect);
    void OnSize(HWND hWnd, UINT uType);

protected:
    void MoveLeadWndSet(HWND hWnd, LPCRECT lpRect);
    void UpdateLeadWndSet(HWND hWnd, LPCRECT lpRect = 0);
    void DeleteMagWnd(HWND hWnd);
    void Add2DMatrix();
    void Delete2DMatrix(HWND hWnd);
    void Update2DMatrix(HWND hWnd, LPRECT lpRect = 0);

private:
    int GetFirstNeighbor(int v);
    int GetNextNeighbor(int v, int w);
    void DFS(int v, std::vector<bool>& vecVisited, std::vector<int>& vecNeighbor);

private:
    static const int      s_c_iThreshold = 15; /**////< ƫ�Ʒ�ֵ
    HWND                  m_hLead;        ///< �ϰ崰��
    std::map<HWND,POINT>      m_map_leadWnd;  ///< ճ�ϴ����б�
    std::map<HWND,int>				m_map_magWnd;   ///< ��Ҫ��Ϸ���Ĵ����б�
    std::vector<vector<bool> >	m_vec_2DMatrix; ///< ��ʾ�����������ڼ��ཻ�����е��ڽӾ���
};

#endif
