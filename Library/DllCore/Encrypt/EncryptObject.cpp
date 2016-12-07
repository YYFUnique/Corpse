#include "stdafx.h"
#include "EncryptObject.h"
#include "../Utils/ErrorInfo.h"
#include "Rijndael.h"


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
