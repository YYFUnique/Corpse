#include "stdafx.h"
#include "WndMagnet.h"
#include <cassert>
using namespace std;

/***************************************************************************************
  邻接矩阵----用来表示所有窗口之间的邻接关系,值1表示两个窗口间相交或相切,值0表示
              两个窗口间相离,实际存储的是若干窗口组成的非连通无向图结构
  粘合窗口----由邻接矩阵生成的包含老板窗口的集合,当鼠标按在老板窗口上移动时,整个
              粘合窗口跟随移动,实际存储的是含有老板窗口的一个连通分量
  老板窗口----只有当鼠标左键按在此窗口时,才能移动整个粘合窗口,仅允许有一个老板窗口

  2009-12-11  当新建客口时,能正确构建邻接矩阵和粘合窗口列表; 当关闭窗口时,能正确重组邻接矩阵
                          和粘合窗口列表当大小改变时(如最大化，最小化时),能更新邻接矩阵和粘合窗口列表
  2009-12-30  改进了矩形相切条件的精确判断
  2009-12-31  支持老板窗口能连续独立移动
****************************************************************************************/

CWndMagnet::CWndMagnet()
:m_hLead(0)
{
}

CWndMagnet::~CWndMagnet()
{
}

/** 
  @brief 增加磁性窗口
  @param hWnd  需要组合分离的窗口
  * 一般在窗口初始化或创建时调用此方法,加入待组合分离的窗口,注意窗口句柄有效性
*/
void CWndMagnet::AddMagnetWnd(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd)) return;
    int uIndex = m_map_magWnd.size();
    pair<map<HWND,int>::iterator,bool> pr = m_map_magWnd.insert(make_pair(hWnd, uIndex));
    if (!pr.second) return; 
    Add2DMatrix();
}

/** 
  @brief 移除磁性窗口
  @param hWnd 需要移除的窗口句柄
  * 一般在窗口关闭或销毁时调用此方法,移除不组合分离的窗口,注意窗口句柄有效性
*/
void CWndMagnet::RemoveMagnetWnd(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd)) return;
    m_map_leadWnd.erase(hWnd);
    Delete2DMatrix(hWnd);
    DeleteMagWnd(hWnd);
    UpdateLeadWndSet(hWnd);
}

/***************************************************************************************
 以下为消息映射处理方法,在窗口对应消息处理中调用
(1) 在OnLButtonDown调用::SendMessage(m_hWnd,WM_SYSCOMMAND,0xF012,0)来实现按客户区移动时
    经测试发现: 当鼠标左键按在标题栏移动窗口时,而后释放鼠标左键,却收不到WM_NCLBUTTONUP消息,但收到了
    WM_NCLBUTTONDOWN消息.同理当鼠标左键按在客户区移动窗口时,,而后释放鼠标左键,却收不了WM_LBUTTONUP
    消息,但收到了WM_LBUTTONDOWN消息.这是移动窗口时发现的规律,本质上是当鼠标左键释放时来更新粘合窗口列表,
    但鉴于此规律,正好符合调用来更新粘合窗口列表
(2) 如果应用程序调用了OnLButtonDown或OnNcLButtonDown则在点击窗体时就会粘在一起,否则只有移动时
    才粘在一起
****************************************************************************************/

/** 
*  @brief 鼠标左键在老板窗口客户区按下事件处理方法
*  @param hWnd  窗口句柄
*  这是为了使老板窗口有机会能独立连续移动调用的方法
*/
void CWndMagnet::OnLButtonDown(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))  return;
    if (hWnd != m_hLead)           return;
 //   Update2DMatrix(hWnd);
    UpdateLeadWndSet(hWnd);
}

/** 
  @brief 鼠标左键在老板窗口非客户区按下事件处理方法
  @param hWnd  窗口句柄
  * 这是为了使老板窗口有机会能独立连续移动调用的方法
*/
void CWndMagnet::OnNcLButtonDown(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))  return;
    if (hWnd != m_hLead)           return;
    UpdateLeadWndSet(hWnd);
}

/** 
  @brief 鼠标移动事件处理方法
  @param hWnd  窗口句柄
  @param lpRect  窗口矩形区域
*/
void CWndMagnet::OnMoving(HWND hWnd, LPRECT lpRect)
{
    if ( !m_map_leadWnd.empty() && hWnd== m_hLead) // return;
    {
        MoveLeadWndSet(hWnd, lpRect);
    }
    else
    {
		Update2DMatrix(hWnd, lpRect);
        //移动老板窗口时,使其有机会能独立连续移动
        if (hWnd!=m_hLead) 
        {
            UpdateLeadWndSet(hWnd, lpRect);
        }
    }
}

/** 
  @brief 窗口大小改变后事件处理方法 
  @param hWnd  窗口句柄
  @param uType 大小变化类型
  * 当大小改变时更新邻接矩阵
*/
void CWndMagnet::OnSize(HWND hWnd, UINT uType)
{
    if (!hWnd || !IsWindow(hWnd))  return;
   switch (uType)
    {
    case SIZE_RESTORED:
    case SIZE_MINIMIZED:
    case SIZE_MAXIMIZED:
        {
            Update2DMatrix(hWnd);
            UpdateLeadWndSet(hWnd);
        }
        break;

    case SIZE_MAXSHOW:
        {
        }
        break;
    }
}

/**************************************************************************
   以下为实现方法,在类内部调用以实现公有方法的功能
  **************************************************************************/
void CWndMagnet::DeleteMagWnd(HWND hWnd)
{
    map<HWND,int>::iterator iter = m_map_magWnd.find(hWnd);
    if (iter==m_map_magWnd.end())  return;
 
    int index = (*iter).second;
    m_map_magWnd.erase(iter);
    for (iter = m_map_magWnd.begin(); iter != m_map_magWnd.end(); ++iter)
    {
        int& indexTemp = (*iter).second;
        if ( indexTemp > index) --indexTemp;
    }
}

//当新建窗口时要调用此函数来增加对应的邻接矩阵值
void CWndMagnet::Add2DMatrix()
{
    int uColCount = m_map_magWnd.size();
    int uRowCount = m_vec_2DMatrix.size();
    for (int row = 0; row < uRowCount; ++row)
    {
        for (int col = uRowCount; col < uColCount; ++col)
        {
            m_vec_2DMatrix[row].push_back(false);
        }
    }
    vector<bool> vec_bool; 
    vec_bool.resize(uColCount);
    m_vec_2DMatrix.push_back(vec_bool);
}
//当窗口销毁时要调用此函数来删除对应的邻接矩阵值
void CWndMagnet::Delete2DMatrix(HWND hWnd)
{
    map<HWND,int>::iterator mapIter = m_map_magWnd.find(hWnd);
    if (mapIter==m_map_magWnd.end())  return;

    int index = (*mapIter).second, row;
    vector<vector<bool> >::iterator iter;
    for (vector<vector<bool> >::iterator iter1 = m_vec_2DMatrix.begin(); iter1 != m_vec_2DMatrix.end(); ++iter1)
    {
        row = distance(m_vec_2DMatrix.begin(), iter1);
        if (row == index) 
        {
            iter = iter1;
       }
        for (vector<bool>::iterator iter2 = m_vec_2DMatrix[row].begin(); iter2 != m_vec_2DMatrix[row].end(); ++iter2)
        {
            if (distance(m_vec_2DMatrix[row].begin(),iter2)==index) 
            {
               m_vec_2DMatrix[row].erase(iter2); break;
            }
        }
    }
    m_vec_2DMatrix.erase(iter);
}
//当窗口移动或大小改变时要调用此函数来更新邻接矩阵的值
//当大小改变时lpRect为0
void CWndMagnet::Update2DMatrix(HWND hWnd, LPRECT lpRect)
{
    map<HWND,int>::iterator iter = m_map_magWnd.find(hWnd);
    if (iter == m_map_magWnd.end())  return;

    UINT  uRow = (*iter).second, uCol;
    HWND  hWndTemp;
    RECT  rcWnd,rcTemp;   GetWindowRect(hWnd,&rcWnd);
    for (iter = m_map_magWnd.begin(); iter != m_map_magWnd.end(); ++iter)
    {
        hWndTemp = (*iter).first;  uCol = (*iter).second;
        if (hWnd != hWndTemp)
        {
            GetWindowRect(hWndTemp, &rcTemp);
            if (0 == lpRect)
            {
                RECT rcInter;
                if (IntersectRect(&rcInter,&rcWnd,&rcTemp)
                    ||rcWnd.left==rcTemp.right||rcWnd.right==rcTemp.left
                    ||rcWnd.top==rcTemp.bottom||rcWnd.bottom==rcTemp.top
                    )
                    m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = true;
                else
                    m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = false;
            }
            else
            {
                long  lWidth = lpRect->right-lpRect->left, lHeight = lpRect->bottom-lpRect->top;
                long  lOffX  = lWidth/2, lOffY = lHeight/2;
                POINT ptCenter = { lpRect->left + lOffX, lpRect->top + lOffY };
                RECT  rcLeft, rcTop, rcRight, rcBottom, rcCenter;

                SetRect(&rcLeft, rcTemp.left-s_c_iThreshold-lOffX, rcTemp.top-lOffY, 
                    rcTemp.left-lOffX, rcTemp.bottom+lOffY);
                SetRect(&rcTop, rcTemp.left-lOffX, rcTemp.top-s_c_iThreshold-lOffY, 
                    rcTemp.right+lOffX, rcTemp.top-lOffY);
                SetRect(&rcRight, rcTemp.right+lOffX, rcTemp.top-lOffY, 
                    rcTemp.right+s_c_iThreshold+lOffX, rcTemp.bottom+lOffY);
                SetRect(&rcBottom, rcTemp.left-lOffX, rcTemp.bottom+lOffY, 
                    rcTemp.right+lOffX, rcTemp.bottom+s_c_iThreshold+lOffY);
                SetRect(&rcCenter, rcTemp.left-lOffX, rcTemp.top-lOffY, 
                    rcTemp.right+lOffX, rcTemp.bottom+lOffY);

              /***********************************************************************
                  nOffset指示组合或分离时的矩形偏移值,默认为0   
                  PtInRect判断一个点是否在指定矩形内,意味着不包含底边和右边
              ***********************************************************************/
                int nOffset = 0;  
                if(!m_vec_2DMatrix[uRow][uCol])
                {
                    if (PtInRect(&rcLeft, ptCenter))
                    {
                        m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = true;
                        lpRect->right = rcTemp.left - nOffset;
                        lpRect->left = lpRect->right - lWidth;
                    }
                    else if (PtInRect(&rcTop, ptCenter))
                    {
                        m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = true;
                        lpRect->bottom = rcTemp.top - nOffset;
                        lpRect->top = lpRect->bottom - lHeight;
                    }
                    else if (PtInRect(&rcRight, ptCenter))
                    {
                        m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = true;
                        lpRect->left = rcTemp.right + nOffset;
                        lpRect->right = lpRect->left + lWidth;
                    }
                    else if (PtInRect(&rcBottom, ptCenter))
                    {
                        m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = true;
                        lpRect->top = rcTemp.bottom + nOffset;
                        lpRect->bottom = lpRect->top + lHeight;
                    }
                    else if (PtInRect(&rcCenter, ptCenter))
                    {
                        m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = true;
                    }
                    
                }
                else
                {
                    if (!PtInRect(&rcLeft, ptCenter)&&!PtInRect(&rcTop, ptCenter)&&!PtInRect(&rcRight, ptCenter)
                        &&!PtInRect(&rcBottom, ptCenter)&&!PtInRect(&rcCenter, ptCenter))
                    {
                        m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = false;
                    }
                    else
                    {
                        ++nOffset;
                        OffsetRect(&rcLeft, -nOffset, 0);  OffsetRect(&rcRight, nOffset, 0);
                        OffsetRect(&rcTop, 0, -nOffset);   OffsetRect(&rcBottom, 0, nOffset); 
                        if (PtInRect(&rcLeft, ptCenter))
                        {
                            lpRect->right = rcTemp.left - s_c_iThreshold;
                            lpRect->left = lpRect->right - lWidth;
                            m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = false;
                        }
                        else if (PtInRect(&rcTop, ptCenter))
                        {
                            lpRect->bottom = rcTemp.top - s_c_iThreshold;
                            lpRect->top = lpRect->bottom - lHeight;
                            m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = false;
                        }
                        else if (PtInRect(&rcRight, ptCenter))
                        {
                            lpRect->left  = rcTemp.right + s_c_iThreshold;
                            lpRect->right = lpRect->left + lWidth;
                            m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = false;
                        }
                        else if (PtInRect(&rcBottom, ptCenter))
                        {
                            lpRect->top    = rcTemp.bottom + s_c_iThreshold;
                            lpRect->bottom = lpRect->top + lHeight;
                            m_vec_2DMatrix[uRow][uCol] = m_vec_2DMatrix[uCol][uRow] = false;
                        }
                    }
                }
            }
        }
    }
}
//当移动或击点窗口时调用此函数更新粘合窗口列表
//当点击窗口时,lpRect为0
void CWndMagnet::UpdateLeadWndSet(HWND hWnd, LPCRECT lpRect /**//*=0*/)
{
    if (m_vec_2DMatrix.empty())  return;

    map<HWND,int>::iterator iter = m_map_magWnd.find(m_hLead);
    if (iter == m_map_magWnd.end())  return;
        
    m_map_leadWnd.clear();
    int lead_wnd_index = (*iter).second;
    int rows = m_vec_2DMatrix.size(), cols = m_vec_2DMatrix[0].size();
    assert(rows == cols);
    
    vector<int> vecCol;
    vector<bool> vecVisited(rows);
    DFS(lead_wnd_index, vecVisited, vecCol);

   RECT rcLead; GetWindowRect(m_hLead, &rcLead);
    for (vector<int>::iterator vecIter = vecCol.begin()+1; vecIter!=vecCol.end();++vecIter)
    {
        int index = *vecIter;
       for (iter = m_map_magWnd.begin(); iter != m_map_magWnd.end(); ++iter)
        {
            if ((*iter).second==index) 
            {
                HWND hTemp = (*iter).first; RECT rcTemp; 
                GetWindowRect(hTemp, &rcTemp);
                LPCRECT pRect = 0;
                if (0==lpRect||hWnd!=m_hLead) 
                    pRect = &rcLead;
                else 
                    pRect = lpRect;
                POINT pt = {rcTemp.left-pRect->left, rcTemp.top-pRect->top};
                m_map_leadWnd[hTemp] = pt; 
                break;
            }
        }
    }
}
//当移动老板窗口时调用此函数来移动整个粘合窗口
void CWndMagnet::MoveLeadWndSet(HWND hWnd, LPCRECT lpRect)
{
    if (hWnd != m_hLead) return;

    HWND hTemp;    RECT rcTemp; 
    long lNewLeft, lNewTop;
    for (map<HWND,POINT>::iterator Iter = m_map_leadWnd.begin(); Iter != m_map_leadWnd.end();++Iter)
    {
        lNewLeft = lpRect->left + (*Iter).second.x; 
        lNewTop  = lpRect->top  + (*Iter).second.y;
        hTemp = (*Iter).first; GetWindowRect(hTemp,&rcTemp);
        if (rcTemp.left != lNewLeft || rcTemp.top != lNewTop)
       {
         /*********************************************************************************
               如果不考虑隐藏窗口,已最小化和最大化窗口,则加上以下代码
            if ((IsWindowVisible(hTemp))&&!IsIconic(hTemp) && !IsZoomed(hTemp))
         *********************************************************************************/
            SetWindowPos(hTemp, 0, lNewLeft, lNewTop, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
        }
    }
}

/***************************************************************************************
 下面为实现图深度搜索遍历算法的函数
 DFS:复杂度为O(N*N),N为无向图的顶点个数,可考虑用邻接表存储图来实现,
 其复杂度为N+E,N为顶点个数,E为边数
****************************************************************************************/
//找到顶点v的第一个邻接点
int CWndMagnet::GetFirstNeighbor(int v)
{
    if (-1==v)  return -1;
    int  num = m_vec_2DMatrix.size();
    assert(v < num);
    for (int col = 0; col < num; ++col)
    {
        if (m_vec_2DMatrix[v][col]) return col;
    }
    return -1;
}
//找到顶点V的邻接点W的下一个邻接点
int CWndMagnet::GetNextNeighbor(int v, int w)
{
    if (-1==v || -1==w)  return -1;
    int num = m_vec_2DMatrix.size();
    for (int col = w + 1; col < num; ++col)
    {
        if (m_vec_2DMatrix[v][col])  return col;
    }
    return -1;
}

void CWndMagnet::DFS(int v, vector<bool>& vecVisited, vector<int>& vecNeighbor)
{
   vecVisited[v] = true;
   vecNeighbor.push_back(v);
   int w = GetFirstNeighbor(v); 
   for (; w !=-1 ;)
   {
       if (!vecVisited[w]) DFS(w, vecVisited, vecNeighbor);
       w = GetNextNeighbor(v, w);
   }
}
