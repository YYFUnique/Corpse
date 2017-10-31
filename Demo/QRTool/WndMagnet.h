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
  @brief 磁性窗口工具类
  * 实现依靠用鼠标移动操作来组合或分离多个窗口,当窗口组合在一起时可以一起移动
  * 使用时,多个相互磁性的窗口要共享同一个CWndMagnet对象
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
    static const int      s_c_iThreshold = 15; /**////< 偏移阀值
    HWND                  m_hLead;        ///< 老板窗口
    std::map<HWND,POINT>      m_map_leadWnd;  ///< 粘合窗口列表
    std::map<HWND,int>				m_map_magWnd;   ///< 需要组合分离的窗口列表
    std::vector<vector<bool> >	m_vec_2DMatrix; ///< 表示任意两个窗口间相交或相切的邻接矩阵
};

#endif
