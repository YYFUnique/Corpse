#include "stdafx.h"
#include "WndMagnet.h"
#include <cassert>
using namespace std;

/***************************************************************************************
  �ڽӾ���----������ʾ���д���֮����ڽӹ�ϵ,ֵ1��ʾ�������ڼ��ཻ������,ֵ0��ʾ
              �������ڼ�����,ʵ�ʴ洢�������ɴ�����ɵķ���ͨ����ͼ�ṹ
  ճ�ϴ���----���ڽӾ������ɵİ����ϰ崰�ڵļ���,����갴���ϰ崰�����ƶ�ʱ,����
              ճ�ϴ��ڸ����ƶ�,ʵ�ʴ洢���Ǻ����ϰ崰�ڵ�һ����ͨ����
  �ϰ崰��----ֻ�е����������ڴ˴���ʱ,�����ƶ�����ճ�ϴ���,��������һ���ϰ崰��

  2009-12-11  ���½��Ϳ�ʱ,����ȷ�����ڽӾ����ճ�ϴ����б�; ���رմ���ʱ,����ȷ�����ڽӾ���
                          ��ճ�ϴ����б���С�ı�ʱ(����󻯣���С��ʱ),�ܸ����ڽӾ����ճ�ϴ����б�
  2009-12-30  �Ľ��˾������������ľ�ȷ�ж�
  2009-12-31  ֧���ϰ崰�������������ƶ�
****************************************************************************************/

CWndMagnet::CWndMagnet()
:m_hLead(0)
{
}

CWndMagnet::~CWndMagnet()
{
}

/** 
  @brief ���Ӵ��Դ���
  @param hWnd  ��Ҫ��Ϸ���Ĵ���
  * һ���ڴ��ڳ�ʼ���򴴽�ʱ���ô˷���,�������Ϸ���Ĵ���,ע�ⴰ�ھ����Ч��
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
  @brief �Ƴ����Դ���
  @param hWnd ��Ҫ�Ƴ��Ĵ��ھ��
  * һ���ڴ��ڹرջ�����ʱ���ô˷���,�Ƴ�����Ϸ���Ĵ���,ע�ⴰ�ھ����Ч��
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
 ����Ϊ��Ϣӳ�䴦����,�ڴ��ڶ�Ӧ��Ϣ�����е���
(1) ��OnLButtonDown����::SendMessage(m_hWnd,WM_SYSCOMMAND,0xF012,0)��ʵ�ְ��ͻ����ƶ�ʱ
    �����Է���: �����������ڱ������ƶ�����ʱ,�����ͷ�������,ȴ�ղ���WM_NCLBUTTONUP��Ϣ,���յ���
    WM_NCLBUTTONDOWN��Ϣ.ͬ�����������ڿͻ����ƶ�����ʱ,,�����ͷ�������,ȴ�ղ���WM_LBUTTONUP
    ��Ϣ,���յ���WM_LBUTTONDOWN��Ϣ.�����ƶ�����ʱ���ֵĹ���,�������ǵ��������ͷ�ʱ������ճ�ϴ����б�,
    �����ڴ˹���,���÷��ϵ���������ճ�ϴ����б�
(2) ���Ӧ�ó��������OnLButtonDown��OnNcLButtonDown���ڵ������ʱ�ͻ�ճ��һ��,����ֻ���ƶ�ʱ
    ��ճ��һ��
****************************************************************************************/

/** 
*  @brief ���������ϰ崰�ڿͻ��������¼�������
*  @param hWnd  ���ھ��
*  ����Ϊ��ʹ�ϰ崰���л����ܶ��������ƶ����õķ���
*/
void CWndMagnet::OnLButtonDown(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))  return;
    if (hWnd != m_hLead)           return;
 //   Update2DMatrix(hWnd);
    UpdateLeadWndSet(hWnd);
}

/** 
  @brief ���������ϰ崰�ڷǿͻ��������¼�������
  @param hWnd  ���ھ��
  * ����Ϊ��ʹ�ϰ崰���л����ܶ��������ƶ����õķ���
*/
void CWndMagnet::OnNcLButtonDown(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))  return;
    if (hWnd != m_hLead)           return;
    UpdateLeadWndSet(hWnd);
}

/** 
  @brief ����ƶ��¼�������
  @param hWnd  ���ھ��
  @param lpRect  ���ھ�������
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
        //�ƶ��ϰ崰��ʱ,ʹ���л����ܶ��������ƶ�
        if (hWnd!=m_hLead) 
        {
            UpdateLeadWndSet(hWnd, lpRect);
        }
    }
}

/** 
  @brief ���ڴ�С�ı���¼������� 
  @param hWnd  ���ھ��
  @param uType ��С�仯����
  * ����С�ı�ʱ�����ڽӾ���
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
   ����Ϊʵ�ַ���,�����ڲ�������ʵ�ֹ��з����Ĺ���
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

//���½�����ʱҪ���ô˺��������Ӷ�Ӧ���ڽӾ���ֵ
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
//����������ʱҪ���ô˺�����ɾ����Ӧ���ڽӾ���ֵ
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
//�������ƶ����С�ı�ʱҪ���ô˺����������ڽӾ����ֵ
//����С�ı�ʱlpRectΪ0
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
                  nOffsetָʾ��ϻ����ʱ�ľ���ƫ��ֵ,Ĭ��Ϊ0   
                  PtInRect�ж�һ�����Ƿ���ָ��������,��ζ�Ų������ױߺ��ұ�
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
//���ƶ�����㴰��ʱ���ô˺�������ճ�ϴ����б�
//���������ʱ,lpRectΪ0
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
//���ƶ��ϰ崰��ʱ���ô˺������ƶ�����ճ�ϴ���
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
               ������������ش���,����С������󻯴���,��������´���
            if ((IsWindowVisible(hTemp))&&!IsIconic(hTemp) && !IsZoomed(hTemp))
         *********************************************************************************/
            SetWindowPos(hTemp, 0, lNewLeft, lNewTop, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
        }
    }
}

/***************************************************************************************
 ����Ϊʵ��ͼ������������㷨�ĺ���
 DFS:���Ӷ�ΪO(N*N),NΪ����ͼ�Ķ������,�ɿ������ڽӱ�洢ͼ��ʵ��,
 �临�Ӷ�ΪN+E,NΪ�������,EΪ����
****************************************************************************************/
//�ҵ�����v�ĵ�һ���ڽӵ�
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
//�ҵ�����V���ڽӵ�W����һ���ڽӵ�
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
