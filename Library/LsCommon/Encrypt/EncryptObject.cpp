// EncryptObject.cpp : implementation file
//
#include "stdafx.h"
#include "EncryptObject.h"
#include "..\ErrorInfo.h"
#include "..\DebugInfo.h"
#include "Rijndael.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncryptObject

CEncryptObject::CEncryptObject()
{
}

CEncryptObject::~CEncryptObject()
{
}

CEncryptObject* CEncryptObject::Create(DWORD dwAlgId)
{
	CEncryptObject* pEncryptObject=NULL;
	if (dwAlgId==LS_ENCRYPT_ALG_AES)
		pEncryptObject=new CRijndael;
	else
	{
		SetErrorInfo(CUSTOM_ERROR,0,_T("无效的加密算法ID(%d)!"),dwAlgId);
		OutputDebugStringWhenDebugMode(GetThreadErrorInfoString());
	}

	return pEncryptObject;
}

void CEncryptObject::Release()
{
	if (this)
		delete this;
}
/////////////////////////////////////////////////////////////////////////////
// CEncryptObject message handlers
