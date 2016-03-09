
#include "StdAfx.h"
#include "Xor.h"

void memxor(void* dest,size_t count)
{
	BYTE* pData=(BYTE*)dest;
	while(count>=sizeof(DWORD))
	{
		*(DWORD*)(pData)^=0xffffffff;
		count-=4;
		pData+=4;
	}

	while(count>=sizeof(WORD))
	{
		*(WORD*)(pData)^=0xffff;
		count-=2;
		pData+=2;
	}

	while(count)
	{
		*(pData)^=0xff;
		count-=1;
		pData+=1;
	}
}

BOOL XOROperate(TCHAR *Input,DWORD dwOperateNum)
{
	DWORD dwSize = (DWORD)_tcslen(Input);
	for(DWORD i =0;i<dwSize;i++)
	{
		*(Input+i) ^= dwOperateNum;
	}

	return TRUE;
}