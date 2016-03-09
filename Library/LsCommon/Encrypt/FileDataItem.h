#pragma once

#include <afxtempl.h>
/////////////////////////////////////////////////////////////////////////////
// CFileDataItem window

#ifdef LS_STATIC_LIB_CALL
#define LS_COMMON_API 
#elif defined  LS_EXPORTS
#define LS_COMMON_API __declspec(dllexport)
#else
#define LS_COMMON_API __declspec(dllimport)
#endif

class LS_COMMON_API CFileDataItem
{
// Construction
public:
	CFileDataItem();

// Attributes
public:

//==================================================================
//函 数 名: GetFileItemCount
//功    能: 获取数据项数
//输入参数: 无输入参数
//返 回 值: 返回数据项数
//==================================================================
	DWORD GetFileItemCount();

//==================================================================
//函 数 名: SetDataItem
//功    能: 根据数据项类型,由小到大依次在数据项链表中添加数据项.
//			如果已存在具有相同数据项类型的数据项则设置该数据项的值
//输入参数: [in] 数据项类型
//			[in] 数据项长度
//			[in] 数据项数据首地址
//返 回 值: 添加或设置数据项链表成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL SetDataItem(DWORD dwItemType,DWORD dwDataLen,const void* pDataBuffer);

//==================================================================
//函 数 名: SetDataItem
//功    能: 根据数据项类型,由小到大依次在数据项链表中添加数据项.
//			如果已存在具有相同数据项类型的数据项则设置该数据项的值
//输入参数: [in] 数据项类型
//			[in] 数据项数据
//返 回 值: 添加或设置数据项链表成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL SetDataItem(DWORD dwItemType,DWORD dwValue);

//==================================================================
//函 数 名: GetDataItem
//功    能: 根据数据项类型,获取FILE_DATA_ITEM中相应的数据长度和数据缓存首地址
//输入参数: [in]  dwItemType,数据项类型
//			[out] dwDataLen, 数据长度
//			[out] pDataBuffer, 数据缓存首地址
//返 回 值: 获取指定数据项成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetDataItem(DWORD dwItemType,DWORD& dwDataLen,const void*& pDataBuffer);

//==================================================================
//函 数 名: GetAllDataBuffer
//功    能: 读取数据项链表中所有信息,并依次考入缓存中.
//			传当数据缓存为NULL时,为获取所有数据信息的内存大小
//输入参数: [out] pDataBuffer, 数据缓存保存数据链表中所有数据信息
//			[in,out]  dwDataLen,数据缓存大小
//返 回 值: 读取数据链表中所有数据信息成功返回TRUE,否则返回FALSE
//==================================================================
	BOOL GetAllDataBuffer(void* pDataBuffer,DWORD& dwDataLen);

//==================================================================
//函 数 名: EnumDataItem
//功    能: 获取数据链表中指定数据项类型的下一数据项信息.
//			如指定数据项类型为-1,则为获取数据链表中第一项数据的信息.
//输入参数: [in,out] dwItemType, 传入指定数据项类型,传出获取到的数据项类型
//			[out]    dwDataLen, 获取到的数据长度
//			[out]    pDataBuffer, 获取到的数据缓存首地址
//返 回 值: 获取成功返回TURE,失败返回FALSE
//==================================================================
	BOOL EnumDataItem(DWORD& dwItemType,DWORD& dwDataLen,const void*& pDataBuffer);

//==================================================================
//函 数 名: WriteDataToFile
//功    能: 将数据链表中内容写入指定文件
//输入参数: [in] lpcszFileName,写入文件全路径名
//返 回 值: 写入成功返回TURE,失败返回FALSE
//==================================================================
	BOOL WriteDataToFile(LPCTSTR lpcszFileName);

//==================================================================
//函 数 名: ClearAllDataItem
//功    能: 删除数据项链表中所有内容
//输入参数: 无输入参数
//返 回 值: 无返回值
//==================================================================
	void ClearAllDataItem();

//==================================================================
//函 数 名: RemoveDataItem
//功    能: 删除具有指定数据项类型的数据项
//输入参数: [in] dwItemType,数据项类型
//返 回 值: 无返回值
//==================================================================
	void RemoveDataItem(DWORD dwItemType);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileDataItem)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileDataItem();

	// Generated message map functions
protected:
	typedef struct _FILE_DATA_ITEM
	{
		DWORD dwItemType;
		DWORD dwDataLen;
		void* pDataBuffer;
	}FILE_DATA_ITEM,*PFILE_DATA_ITEM;
	typedef CList<FILE_DATA_ITEM,FILE_DATA_ITEM&> CFileDataItemList;
	CFileDataItemList m_FileDataItemList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
