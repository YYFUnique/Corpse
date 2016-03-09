
// PEToolView.cpp : CPEToolView 类的实现
//

#include "stdafx.h"
#include "PETool.h"

#include "PEToolDoc.h"
#include "PEToolView.h"
#include "PELibrary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPEToolView

IMPLEMENT_DYNCREATE(CPEToolView, CListView)

BEGIN_MESSAGE_MAP(CPEToolView, CListView)
	ON_WM_STYLECHANGED()
END_MESSAGE_MAP()

// CPEToolView 构造/析构

CPEToolView::CPEToolView()
{
	// TODO: 在此处添加构造代码

}

CPEToolView::~CPEToolView()
{
}

BOOL CPEToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

void CPEToolView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
	CListCtrl& ListCtrl = GetListCtrl();

	SetWindowTheme(ListCtrl.GetSafeHwnd(),L"Explorer",NULL);

	ModifyStyle(LVS_TYPEMASK , LVS_REPORT , 0);
	ListCtrl.SetExtendedStyle(LVS_EX_SINGLEROW|LVS_EX_DOUBLEBUFFER|LVS_EX_LABELTIP|LVS_EX_FULLROWSELECT);
}


// CPEToolView 诊断

#ifdef _DEBUG
void CPEToolView::AssertValid() const
{
	CListView::AssertValid();
}

void CPEToolView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CPEToolDoc* CPEToolView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPEToolDoc)));
	return (CPEToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CPEToolView 消息处理程序
void CPEToolView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: 添加代码以响应用户对窗口视图样式的更改	
	CListView::OnStyleChanged(nStyleType,lpStyleStruct);	
}

void CPEToolView::OnShowItemData(ITEM_DATA_INFO ItemDataInfo,CPELibrary* pPELibrary)
{
	CListCtrl& ListCtrl = GetListCtrl();
	ListCtrl.SetRedraw(FALSE);

	UINT nCount = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for(UINT n=0;n<nCount;++n)
		ListCtrl.DeleteColumn(0);

	ListCtrl.DeleteAllItems();

	switch (ItemDataInfo)
	{
		case ITEM_DATA_INFO_DOS_HEADER:
				ShowItemDosHeader(pPELibrary);
			break;
		case ITEM_DATA_INFO_FILE_HEADER:
				ShowItemFileHeader(pPELibrary);
			break;
	}

	int nTotal = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i=0;i<nTotal;++i)
		ListCtrl.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);

	ListCtrl.SetRedraw(TRUE);
}

void CPEToolView::ShowItemDosHeader(CPELibrary* pPELibrary)
{
	CListCtrl& ListCtrl = GetListCtrl();

	int nIndex=0;
	ListCtrl.InsertColumn(nIndex++,_T("项目名称"),LVCFMT_LEFT,100);
	ListCtrl.InsertColumn(nIndex++,_T("项目值"),LVCFMT_LEFT,100);
	ListCtrl.InsertColumn(nIndex++,_T("描述"),LVCFMT_LEFT,100);

	const IMAGE_DOS_HEADER& DosHeader = pPELibrary->GetImageDosHeader();

	LPCTSTR lpszDosHeader[][2] = 
	{
		{_T("IMAGE_DOS_HEADER::e_magic"),_T("Magic number:MZ")},
		{_T("IMAGE_DOS_HEADER::e_cblp"),_T("Bytes on last page of file")},
		{_T("IMAGE_DOS_HEADER::e_cp"),_T("Pages in file")},
		{_T("IMAGE_DOS_HEADER::e_crlc"),_T("Relocations")},
		{_T("IMAGE_DOS_HEADER::e_cparhdr"),_T("Size of header in paragraphs")},
		{_T("IMAGE_DOS_HEADER::e_minalloc"),_T("Minimum extra paragraphs needed")},
		{_T("IMAGE_DOS_HEADER::e_maxalloc"),_T("Maximum extra paragraphs needed")},
		{_T("IMAGE_DOS_HEADER::e_ss"),_T("Initial (relative) SS value")},
		{_T("IMAGE_DOS_HEADER::e_sp"),_T("Initial SP value")},
		{_T("IMAGE_DOS_HEADER::e_csum"),_T("Checksum")},
		{_T("IMAGE_DOS_HEADER::e_ip"),_T("Initial IP value")},
		{_T("IMAGE_DOS_HEADER::e_cs"),_T("Initial (relative) CS value")},
		{_T("IMAGE_DOS_HEADER::e_lfarlc"),_T("File address of relocation table")},
		{_T("IMAGE_DOS_HEADER::e_ovno"),_T("Overlay number")},
		{_T("IMAGE_DOS_HEADER::e_lfanew"),_T("File address of new exe header")},
	};
	UINT nDosHeader[] = {DosHeader.e_magic,DosHeader.e_cblp,DosHeader.e_cp,
		DosHeader.e_crlc,DosHeader.e_cparhdr,DosHeader.e_minalloc,DosHeader.e_maxalloc,
		DosHeader.e_ss,DosHeader.e_sp,DosHeader.e_csum,DosHeader.e_ip,
		DosHeader.e_cs,DosHeader.e_lfarlc,DosHeader.e_ovno,DosHeader.e_lfanew};

	for (UINT n=0;n<_countof(lpszDosHeader);++n)
	{
		int nItem = ListCtrl.InsertItem(0x7FFFFFFF,lpszDosHeader[n][0]);
		TCHAR szItemText[20];
		_stprintf_s(szItemText,_countof(szItemText),_T("0x%04X(%d)"),nDosHeader[n],nDosHeader[n]);
		ListCtrl.SetItemText(nItem,1,szItemText);
		ListCtrl.SetItemText(nItem,2,lpszDosHeader[n][1]);
	}
}

void CPEToolView::ShowItemFileHeader(CPELibrary* pPELibrary)
{
	CListCtrl& ListCtrl = GetListCtrl();

	int nIndex=0;
	ListCtrl.InsertColumn(nIndex++,_T("项目名称"),LVCFMT_LEFT,100);
	ListCtrl.InsertColumn(nIndex++,_T("项目值"),LVCFMT_LEFT,100);
	ListCtrl.InsertColumn(nIndex++,_T("描述"),LVCFMT_LEFT,100);

	const IMAGE_NT_HEADERS& DosHeader = pPELibrary->GetImageNtHeader();
	LPCTSTR lpszDosHeader[][2] = 
	{
		{_T("PIMAGE_NT_HEADERS::Signature"),_T("Magic number:MZ")},
		{_T("PIMAGE_NT_HEADERS::Machine"),_T("Magic number:MZ")},
		{_T("PIMAGE_NT_HEADERS::NumberOfSections"),_T("Magic number:MZ")},
		{_T("PIMAGE_NT_HEADERS::TimeDateStamp"),_T("Magic number:MZ")},
		{_T("PIMAGE_NT_HEADERS::PointerToSymbolTable"),_T("Magic number:MZ")},
		{_T("PIMAGE_NT_HEADERS::NumberOfSymbols"),_T("Magic number:MZ")},
		{_T("PIMAGE_NT_HEADERS::SizeOfOptionalHeader"),_T("Magic number:MZ")},
		{_T("PIMAGE_NT_HEADERS::Characteristics"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::Magic"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MajorLinkerVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MinorLinkerVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::SizeOfCode"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::SizeOfInitializedData"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::SizeOfUninitializedData"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::AddressOfEntryPoint"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::BaseOfCode"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::BaseOfData"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::ImageBase"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::SectionAlignment"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::FileAlignment"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MajorOperatingSystemVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MinorOperatingSystemVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MajorImageVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MinorImageVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MajorSubsystemVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::MinorSubsystemVersion"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::Win32VersionValue"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::SizeOfImage"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::SizeOfHeaders"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::CheckSum"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::Subsystem"),_T("Magic number:MZ")},
		{_T("IMAGE_OPTIONAL_HEADER::DllCharacteristics"),_T("Magic number:MZ")},
	};
}