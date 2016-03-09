

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ����
#include "stdafx.h" 
#include <Stdio.h>
#include <Windows.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �궨��
#define __TRY
#define __FINALLY __Finally:
#define __LEAVE goto __Finally

#define IsNumberic(c) ((c >= '0') && (c <= '9'))
#define IsNumbericPad(c) (IsNumberic(c) || (c == ' ') || (c == '\t'))
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ԥ����


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
typedef enum tagBPPERROR
{
	BPPERROR_SUCCESS,
	BPPERROR_CREATEFILE,
	BPPERROR_GETFILESIZE,
	BPPERROR_CREATEFILEMAPPING,
	BPPERROR_MAPVIEWOFFILE,
	BPPERROR_NOTFOUND,	
}
BPPERROR;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������
char STR_VERBUILD[] = "VER_Build";
char STR_VERRELEASE[] = "VER_Release";
char STR_DEFAULTFILE[] = "Define.h";
const char *STR_BPPERRORS[] = 
{
	"Build number is increased successful.",
	"Create file error.",
	"File size error.",
	"Create file mapping error.",
	"Map view of file error.",
	"Could not found VER_BUILD value.",
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���� Build ��
BPPERROR WINAPI BuildPlusPlus(LPCSTR pszFileName, BOOL bReleaseMode)
{	
	PCHAR p;
	int iCount;
	PBYTE pEnd;
	PBYTE pFile;
	PBYTE pStart;
	HANDLE hNew;
	DWORD dwSize;
	HANDLE hFile;
	HANDLE hMapping;
	BPPERROR beResult;
	char szTemp[16];
	BOOL bVerBuild;
	BOOL bVerRelease;	
	char szSource[MAX_PATH + 2];
	char szDestination[MAX_PATH + 2];

	__TRY
	{
		// ��ʼ������
		pFile = NULL;
		hMapping = NULL;
		hNew = INVALID_HANDLE_VALUE;

		// ���������ļ�
		SearchPath(NULL, pszFileName, NULL, sizeof(szSource), szSource, NULL);
		hFile = CreateFile(szSource, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			beResult = BPPERROR_CREATEFILE;
			__LEAVE;
		}

		// ��ȡ��С
		dwSize = GetFileSize(hFile, NULL);
		if ((dwSize == INVALID_FILE_SIZE) || (dwSize == 0))
		{
			beResult = BPPERROR_GETFILESIZE;
			__LEAVE;
		}

		// �����ļ�ӳ��
		hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);
		if (hMapping == NULL)
		{
			beResult = BPPERROR_CREATEFILEMAPPING;
			__LEAVE;
		}

		// ӳ���ļ�
		pFile = (PBYTE) MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pFile == NULL)
		{
			beResult = BPPERROR_MAPVIEWOFFILE;
			__LEAVE;
		}

		// ���� VER_Build �� VER_Release �ַ���
		beResult = BPPERROR_NOTFOUND;
		pStart = pFile;
		pEnd = pFile + dwSize;
		iCount = 0;
		for (p = pStart; (p < pEnd - sizeof(STR_VERRELEASE)) && (beResult != BPPERROR_SUCCESS); p++)
		{
			bVerBuild = (_strnicmp(p, STR_VERBUILD, sizeof(STR_VERBUILD) - 1) == 0);
			bVerRelease = ((bVerBuild == FALSE) && 
				bReleaseMode && 
				(_strnicmp(p, STR_VERRELEASE, sizeof(STR_VERRELEASE) - 1) == 0));

			if (bVerBuild || bVerRelease)
			{
				// �����ո��ַ���
				for (; p < pEnd - 5; p++)
				{
					if ((*p == ' ') || (*p == '\t') || (*p == '='))
					{
						break;
					}
				}

				// �����ո������
				for (; p < pEnd - 5; p++)
				{
					if ((*p != ' ') && (*p != '\t') && (*p != '"'))
					{
						break;
					}
				}

				// �����һ���ַ�������
				if (IsNumberic(*p) == FALSE)
				{
					continue;
				}

				// ���Ӱ汾��
				wsprintf(szTemp, "%-4d", atoi(p) + 1);

				// ��ڶ��������ĸ��ַ�������/���ֲ��������û�д�����Ŀ���ļ�
				if (IsNumbericPad(*(p + 1)) && IsNumbericPad(*(p + 2)) && IsNumbericPad(*(p + 3)) && (hNew == INVALID_HANDLE_VALUE))
				{
					// д���ļ�
					*((LPDWORD) p) = *((LPDWORD) szTemp);					
				}
				else
				{
					// ����Ŀ���ļ�
					if (hNew == INVALID_HANDLE_VALUE)
					{
						wsprintf(szDestination, "%s.bpp", szSource);
						hNew = CreateFile(szDestination, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
						if (hNew == INVALID_HANDLE_VALUE)
						{
							beResult = BPPERROR_CREATEFILE;
							__LEAVE;
						}
					}

					// д��ǰ�벿������
					WriteFile(hNew, pStart, (DWORD) (p - pStart), &dwSize, NULL);

					// д���ļ�					
					WriteFile(hNew, szTemp, lstrlen(szTemp), &dwSize, NULL);
				}

				// ��ʾ��Ϣ
				printf("Build++: %s number is %s\n", bVerBuild ? "Build" : "Release", 
					szTemp);

				// ��������
				for (; (p < pEnd - 5) && IsNumberic(*p); p++);

				// ���� pStart
				pStart = p;

				// ���
				iCount++;
				if ((iCount > 1) || ((iCount == 1) && (bReleaseMode == FALSE)))
				{
					// ���óɹ���־
					beResult = BPPERROR_SUCCESS;
				}
			}
		}

		if (hFile != INVALID_HANDLE_VALUE)
		{
			// д���̲���
			WriteFile(hNew, pStart, (DWORD) (pEnd - pStart), &dwSize, NULL);
		}
	}
	__FINALLY
	{
		// �رվ��
		if (pFile)
		{
			UnmapViewOfFile(pFile);
		}

		if (hMapping)
		{
			CloseHandle(hMapping);
		}

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
		}
		
		if (hNew != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hNew);
			if (beResult == BPPERROR_SUCCESS)
			{
				DeleteFile(szSource);
				MoveFile(szDestination, szSource);
			}
		}
	}

	return beResult;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������
int main(int argc, char *argv[])
{
	BOOL bReleaseMode;
	BPPERROR beResult;
	PCHAR pszFileName;

	if (argc < 2)
	{
		pszFileName = STR_DEFAULTFILE;
		bReleaseMode = FALSE;
	}
	else if (argc == 2)
	{
		if ((lstrcmpi(argv[1], "-Release") == 0) || (lstrcmpi(argv[1], "/Release") == 0))
		{
			pszFileName = STR_DEFAULTFILE;
			bReleaseMode = TRUE;
		}
		else
		{
			pszFileName = argv[1];
			bReleaseMode = FALSE;
		}
	}
	else if (argc == 3)
	{
		if ((lstrcmpi(argv[1], "-Release") == 0) || (lstrcmpi(argv[1], "/Release") == 0))
		{
			pszFileName = argv[2];
			bReleaseMode = TRUE;
		}
		else if ((lstrcmpi(argv[2], "-Release") == 0) || (lstrcmpi(argv[2], "/Release") == 0))
		{
			pszFileName = argv[1];
			bReleaseMode = TRUE;
		}
		else
		{
			pszFileName = argv[1];
			bReleaseMode = FALSE;
		}
	}
	else
	{
		printf("Build++: Too many arguments.\n");
		return 0;
	}

	// ���ʧ�ܣ���ʾ����
	beResult = BuildPlusPlus(pszFileName, bReleaseMode);
	if (beResult)
	{
		printf("Build++: %s\n", STR_BPPERRORS[beResult]);
	}

	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////