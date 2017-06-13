#include "StdAfx.h"
#include "MakeSfxFile.h"
#include "Lzma.h"
#include <atlstr.h>

#define  WRITE_DATA_LEN		4*1024

CMakeSfxFile::CMakeSfxFile()
{
	m_pDosHeader = NULL;
	memset(&m_NtHeader,0,sizeof(IMAGE_NT_HEADERS));
	m_dwMiniPointer = 0;
	m_dwEmptyDataLen = 0;
	m_dwSectionDataLen = 0;
	m_pSectionData = NULL;
}

CMakeSfxFile::~CMakeSfxFile()
{
	if (m_pDosHeader != NULL)
	{
		delete[] m_pDosHeader;
		m_pDosHeader = NULL;
	}

	if (m_pSectionData != NULL)
	{
		delete[] m_pSectionData;
		m_pSectionData = NULL;
	}
}

BOOL CMakeSfxFile::OpenTargetFile(LPCTSTR lpszTargetFileName)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hMapFile = INVALID_HANDLE_VALUE;
	LPVOID  lpImageBase = NULL;
	do 
	{
		hFile = CreateFile(lpszTargetFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			break;

		DWORD dwFileSize = GetFileSize(hFile, NULL);

		hMapFile	=	CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hMapFile == NULL)
			break;

		lpImageBase	=	MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
		if (NULL == lpImageBase)
			break;

		PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)lpImageBase;
		m_pDosHeader = new BYTE[pImageDosHeader->e_lfanew];
		DWORD dwNtHeaderOffset = pImageDosHeader->e_lfanew;
		memcpy_s(m_pDosHeader,dwNtHeaderOffset,lpImageBase,dwNtHeaderOffset);
		memcpy_s(&m_NtHeader,sizeof(IMAGE_NT_HEADERS),(LPBYTE)lpImageBase + dwNtHeaderOffset,sizeof(IMAGE_NT_HEADERS));
		//检测文件有效性
		
		LPBYTE lpSectionHeaderStartPos = (LPBYTE)pImageDosHeader+dwNtHeaderOffset+sizeof(IMAGE_NT_HEADERS);
		for (UINT n=0;n<m_NtHeader.FileHeader.NumberOfSections;++n)
		{
			PIMAGE_SECTION_HEADER pHeader = NULL;
			pHeader = (PIMAGE_SECTION_HEADER)(lpSectionHeaderStartPos + sizeof(IMAGE_SECTION_HEADER)*n);
			
			PIMAGE_SECTION_HEADER pImageSectionHeader = new IMAGE_SECTION_HEADER;

			memcpy_s(pImageSectionHeader,sizeof(IMAGE_SECTION_HEADER),pHeader,sizeof(IMAGE_SECTION_HEADER));

			if (m_dwMiniPointer == 0)
				m_dwMiniPointer = pImageSectionHeader->PointerToRawData;
			else if (pImageSectionHeader->PointerToRawData < m_dwMiniPointer)
				m_dwMiniPointer = pImageSectionHeader->PointerToRawData;

			m_dwSectionDataLen += pImageSectionHeader->SizeOfRawData;

			m_ImageSectionHeader.push_back(pImageSectionHeader);
		}
	
		//原始区段数据
		m_pSectionData = new BYTE[m_dwSectionDataLen];
		memcpy_s(m_pSectionData,m_dwSectionDataLen,(LPBYTE)lpImageBase+m_dwMiniPointer,m_dwSectionDataLen);

		bSuccess = TRUE;
	} while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (hMapFile != INVALID_HANDLE_VALUE)
		CloseHandle(hMapFile);
	if (lpImageBase != NULL)
		UnmapViewOfFile(lpImageBase);

	return bSuccess;
}

void CMakeSfxFile::ChangeSectionOffset(DWORD dwNumberOfSections)
{
	//填充空白区域
	PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)m_pDosHeader;
	DWORD dwTotalHeader = pImageDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS) + 
												m_NtHeader.FileHeader.NumberOfSections*sizeof(IMAGE_SECTION_HEADER);
	BOOL bModify = FALSE;

	if(m_dwMiniPointer >= dwTotalHeader)
		m_dwEmptyDataLen = m_dwMiniPointer - dwTotalHeader;
	else
	{
		m_dwEmptyDataLen = dwTotalHeader % (m_NtHeader.OptionalHeader.FileAlignment);

		if(m_dwEmptyDataLen != 0)
			m_dwEmptyDataLen = m_NtHeader.OptionalHeader.FileAlignment - m_dwEmptyDataLen;
		else 
			m_dwEmptyDataLen = 0;

		bModify = TRUE;
	}

	m_dwMiniPointer = dwTotalHeader + m_dwEmptyDataLen;

	m_NtHeader.OptionalHeader.SizeOfHeaders = m_dwMiniPointer;

	if (bModify)
		ChangeSectionsPointer(dwNumberOfSections);	
}

void CMakeSfxFile::ChangeSectionsPointer(DWORD dwNumberOfSections)
{
	if (dwNumberOfSections > m_ImageSectionHeader.size() )
		return;

	for (UINT n=0;n<dwNumberOfSections;++n)
	{
		if (n == 0)
			m_ImageSectionHeader[n]->PointerToRawData = m_dwMiniPointer;
		else
			m_ImageSectionHeader[n]->PointerToRawData = m_ImageSectionHeader[n-1]->PointerToRawData + m_ImageSectionHeader[n-1]->SizeOfRawData;
	}
}

BOOL CMakeSfxFile::FillSections(PIMAGE_SECTION_HEADER pImageSectionHeader,UINT nLen,LPCSTR lpszSectionName)
{
	ZeroMemory(pImageSectionHeader,sizeof(IMAGE_SECTION_HEADER));
	pImageSectionHeader->Characteristics = m_ImageSectionHeader[0]->Characteristics;
	memcpy_s(pImageSectionHeader->Name,_countof(pImageSectionHeader->Name),lpszSectionName,strlen(lpszSectionName));

	//文件对齐
	DWORD dwFileAlign = m_NtHeader.OptionalHeader.FileAlignment;
	DWORD dwFileMode = nLen % dwFileAlign;
	DWORD dwTotalAdd = 0;
	if (dwFileMode != 0)
		dwTotalAdd = nLen + dwFileAlign - dwFileMode;
	else 
		dwTotalAdd = nLen;

	pImageSectionHeader->SizeOfRawData = dwTotalAdd;
	

	//区段对齐
	DWORD dwSectionAlign = m_NtHeader.OptionalHeader.SectionAlignment;
	DWORD dwSectionMode = nLen % dwSectionAlign;
	DWORD dwSectionTotal = 0;
	if (dwSectionMode !=0 )
		dwSectionTotal = nLen + dwSectionAlign - dwSectionMode;
	else
		dwSectionTotal = nLen;

	pImageSectionHeader->Misc.VirtualSize = dwSectionTotal;
	//修改文件总大小
	m_NtHeader.OptionalHeader.SizeOfImage += dwSectionTotal;

	DWORD dwSectionIndex = m_NtHeader.FileHeader.NumberOfSections - 1;
	pImageSectionHeader->PointerToRawData = m_ImageSectionHeader[dwSectionIndex - 1]->PointerToRawData + m_ImageSectionHeader[dwSectionIndex - 1]->SizeOfRawData;

	//计算当前添加到区段表中的区段数据是区段对齐的几倍
	DWORD dwCount = m_ImageSectionHeader[dwSectionIndex - 1]->SizeOfRawData/m_NtHeader.OptionalHeader.SectionAlignment + 1;
	pImageSectionHeader->VirtualAddress = m_ImageSectionHeader[dwSectionIndex - 1]->VirtualAddress + dwCount * m_NtHeader.OptionalHeader.SectionAlignment;

	return TRUE;
}

BOOL CMakeSfxFile::AppendData(LPBYTE lpData,UINT nLen,SECTION_TYPE SectionType /*= SECTION_TYPE_CODE*/)
{
	DWORD dwNumberOfSections = m_NtHeader.FileHeader.NumberOfSections++;
	ChangeSectionOffset(dwNumberOfSections);
	PIMAGE_SECTION_HEADER pImageSectionHeader = new IMAGE_SECTION_HEADER;
	LPCSTR lpszSectionName = SECTION_BLOCK_NAME;
	if (SectionType == SECTION_TYPE_DATA)
		lpszSectionName = SECTION_BLOCK_DATA;

	if (FillSections(pImageSectionHeader,nLen,lpszSectionName) == FALSE)
		return FALSE;

	m_ImageSectionHeader.push_back(pImageSectionHeader);

	IMAGE_SECTION_INFO ImageSectionInfo;
	DWORD dwLen = pImageSectionHeader->SizeOfRawData;
	ImageSectionInfo.lpData = new BYTE[dwLen];
	memset(ImageSectionInfo.lpData,0,dwLen);
	ImageSectionInfo.nLen = dwLen;
	memcpy_s(ImageSectionInfo.lpData,dwLen,lpData,nLen);

	m_SectionData.push_back(ImageSectionInfo);

	return TRUE;
}

BOOL CMakeSfxFile::MakeBinaryFile(LPCTSTR lpszBinaryFile)
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	LPBYTE lpEmptyData = NULL;
	do 
	{
		hFile = CreateFile(lpszBinaryFile,GENERIC_READ|GENERIC_WRITE,NULL,NULL,CREATE_ALWAYS,NULL,NULL);

		DWORD dwRealRead = 0;

		PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)m_pDosHeader;
		//填充原始PE头
		if (WriteFile(hFile,m_pDosHeader,pImageDosHeader->e_lfanew,&dwRealRead,NULL) == FALSE)
			break;

		if (WriteFile(hFile,&m_NtHeader,sizeof(IMAGE_NT_HEADERS),&dwRealRead,NULL) == FALSE)
			break;

		//添加区段数据
		for (UINT n=0;n<m_ImageSectionHeader.size();++n)
		{
			PIMAGE_SECTION_HEADER pImageSectionHeader = m_ImageSectionHeader[n];
			WriteFile(hFile,pImageSectionHeader,sizeof(IMAGE_SECTION_HEADER),&dwRealRead,NULL);

			delete[] pImageSectionHeader;
			pImageSectionHeader = NULL;
		}

		//填充空白数据
		lpEmptyData = new BYTE[m_dwEmptyDataLen];
		ZeroMemory(lpEmptyData,m_dwEmptyDataLen);
		if (WriteFile(hFile,lpEmptyData,m_dwEmptyDataLen,&dwRealRead,NULL) == FALSE)
			break;

		//填充原始区段数据
 		if (WriteFile(hFile,m_pSectionData,m_dwSectionDataLen,&dwRealRead,NULL) == FALSE)
 			break;

		//填充新添加的区段数据
		for (UINT nIndex=0;nIndex<m_SectionData.size();++nIndex)
		{
			IMAGE_SECTION_INFO ImageSectionInfo = m_SectionData[nIndex];
			if (WriteFile(hFile,ImageSectionInfo.lpData,ImageSectionInfo.nLen,&dwRealRead,NULL) == FALSE)
				continue;

			delete[] ImageSectionInfo.lpData;
		}

		bSuccess = TRUE;
	} while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	if (lpEmptyData != NULL)
	{
		delete[] lpEmptyData;
		lpEmptyData = NULL;
	}
	return bSuccess;
}

BOOL CMakeSfxFile::ReleaseData()
{
	BOOL bSuccess = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hMapFile = INVALID_HANDLE_VALUE;
	LPVOID  lpImageBase = NULL;
	do 
	{
		TCHAR szPackageFileName[MAX_PATH];
		GetModuleFileName(NULL,szPackageFileName,_countof(szPackageFileName));

		hFile = CreateFile(_T("C:\\LSGuardClientSetup.exe")/*,szPackageFileName*/,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			break;

		DWORD dwFileSize = GetFileSize(hFile, NULL);

		hMapFile	=	CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hMapFile == NULL)
			break;

		lpImageBase	=	MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
		if (NULL == lpImageBase)
			break;

		PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)lpImageBase;
		DWORD dwNtHeaderOffset = pImageDosHeader->e_lfanew;
		memcpy_s(&m_NtHeader,sizeof(IMAGE_NT_HEADERS),(LPBYTE)lpImageBase + dwNtHeaderOffset,sizeof(IMAGE_NT_HEADERS));

		PIMAGE_SECTION_HEADER pSectionBlockHeader = NULL;
		PIMAGE_SECTION_HEADER pSectionDataHeader = NULL;

		LPBYTE lpSectionHeaderStartPos = (LPBYTE)pImageDosHeader+dwNtHeaderOffset+sizeof(IMAGE_NT_HEADERS);
		for (UINT n=0;n<m_NtHeader.FileHeader.NumberOfSections;++n)
		{
			PIMAGE_SECTION_HEADER pHeader = (PIMAGE_SECTION_HEADER)(lpSectionHeaderStartPos + sizeof(IMAGE_SECTION_HEADER)*n);
			if (memcmp(pHeader->Name,SECTION_BLOCK_NAME,strlen(SECTION_BLOCK_NAME)) ==0)
				pSectionBlockHeader = pHeader;
			if (memcmp(pHeader->Name,SECTION_BLOCK_DATA,strlen(SECTION_BLOCK_DATA)) == 0)
				pSectionDataHeader = pHeader;
		}

		if (pSectionBlockHeader != NULL && pSectionDataHeader != NULL)
			ReleaseFiles(lpImageBase,pSectionBlockHeader,pSectionDataHeader);

		bSuccess = TRUE;
	} while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (hMapFile != INVALID_HANDLE_VALUE)
		CloseHandle(hMapFile);
	if (lpImageBase != NULL)
		UnmapViewOfFile(lpImageBase);

	return bSuccess;
}

void CMakeSfxFile::ReleaseFiles(LPVOID lpImageBase,PIMAGE_SECTION_HEADER pSectionBlock,PIMAGE_SECTION_HEADER pImageSectionData)
{
	PSFX_PACKAGE_INFO pSfxPackageInfo = (PSFX_PACKAGE_INFO)((LPBYTE)lpImageBase+pImageSectionData->PointerToRawData);
	PUNPACKAGE_OPERATE_INFO pOperateInfo = PUNPACKAGE_OPERATE_INFO((LPBYTE)pSfxPackageInfo+sizeof(SFX_PACKAGE_INFO));
	TCHAR szPackageInfo[MAX_PATH];
	//GetUnPackagePath(pSfxPackageInfo,strPackageInfo);
	/*strPackageInfo = _T("C:\\test2\\t");*/
	_tcscpy_s(szPackageInfo,_countof(szPackageInfo),_T("C:\\test2\\t"));
	DWORD dwRunMode = pSfxPackageInfo->dwRunMode;

	CLzma Lzma;
	if (Lzma.LzmaDecode((LPBYTE)lpImageBase+pSectionBlock->PointerToRawData,pSectionBlock->SizeOfRawData,szPackageInfo) == FALSE)
		return;

	if (pSfxPackageInfo->UnpackageOperate == UNPACKAGE_OPERATE_OPENDIRECTORY && pOperateInfo->bOpenDirectory)
		ShellExecute(NULL,_T("open"),_T("explorer.exe"),szPackageInfo,_T("C:\\Windows"),SW_SHOW);
	else if (pSfxPackageInfo->UnpackageOperate == UNPACKAGE_OPERATE_RUN)
		PackageRunMode(pOperateInfo,szPackageInfo);
}
// 
// BOOL CMakeSfxFile::GetUnPackagePath(PSFX_PACKAGE_INFO pSfxPackageInfo,CString& strPackagePath)
// {
// 	LPCTSTR lpszPackagePath = NULL;
// 	switch (pSfxPackageInfo->UnpackageOption)
// 	{
// 	case UNPACKAGE_OPTION_TEMP:
// 			/*lpszPackagePath = gettempna*/
// 		break;
// 	case UNPACKAGE_OPTION_DESKTOP:
// 		break;
// 	case UNPACKAGE_OPTION_DOCUMENT:
// 		break;
// 	case UNPACKAGE_OPTION_LOCALTION:
// 		break;
// 	case UNPACKAGE_OPTION_SELF_SELECT:
// 		break;
// 	case UNPACKAGE_OPTION_ENVIRONMENT:
// 		break;
// 	}
// 
// 	strPackagePath = lpszPackagePath;
// }

void CMakeSfxFile::PackageRunMode(PUNPACKAGE_OPERATE_INFO pOperateInfo,LPCTSTR lpszPackagePath)
{
	TCHAR szDirectoryPath[MAX_PATH];
	TCHAR szModulePath[MAX_PATH];
	if (pOperateInfo->OperateRun.WorkDirectory == WORK_DIRECTORY_SFXMAKER_LOCALTION)
	{
		GetModuleFileName(NULL,szModulePath,_countof(szModulePath));
		/*strDirectoryPath = szModulePath;*/
		_tcscpy_s(szDirectoryPath,_countof(szDirectoryPath),szModulePath);
	}
	else if (pOperateInfo->OperateRun.WorkDirectory == WORK_DIRECTORY_UNPACKAGE_FOLDER)
	{
// 		PathRemoveFileSpec(lpszPackagePath);
// 		strDirectoryPath = szPackageInfo;
		_tcscpy_s(szDirectoryPath,_countof(szDirectoryPath),lpszPackagePath);
	}

	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	ZeroMemory(&ProcessInfo, sizeof(ProcessInfo));

	TCHAR szCommandLine[1024];
	_stprintf_s(szCommandLine,_countof(szCommandLine),_T("\"%s\" %s"),pOperateInfo->OperateRun.szFileName,pOperateInfo->OperateRun.szParam);
	if (CreateProcess(NULL,szCommandLine,NULL,NULL,FALSE,0,NULL,szDirectoryPath,&StartupInfo,&ProcessInfo) == FALSE)
		return;

	WaitForSingleObject(ProcessInfo.hProcess,INFINITE);
	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);

	if (pOperateInfo->OperateRun.DeleteUnpackage == DELETE_UNPACKAGE_EXIST)
	{
		
	}
	else if (pOperateInfo->OperateRun.DeleteUnpackage == DELETE_UNPACKAGE_REBOOT)
	{

	}
}