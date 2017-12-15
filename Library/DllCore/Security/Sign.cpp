#include "StdAfx.h"
#include "Sign.h"

#include <atlstr.h>
#include <SoftPub.h>
#include <WinCrypt.h>
#include <WinTrust.h>
#pragma comment(lib,"wintrust")

CSign::CSign()
{

}

CSign::~CSign()
{

}

LONG CSign::VerifySignTrust(LPCTSTR lpszFilePath)
{
	CString strFilePath(lpszFilePath);

	// Initialize the WINTRUST_FILE_INFO structure.
	WINTRUST_FILE_INFO FileData;
	ZeroMemory(&FileData, sizeof(FileData));
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.hFile = INVALID_HANDLE_VALUE;
	FileData.pcwszFilePath = strFilePath;
	FileData.pgKnownSubject = NULL;

	/*
	WVTPolicyGUID specifies the policy to apply on the file
	WINTRUST_ACTION_GENERIC_VERIFY_V2 policy checks:
    
	1) The certificate used to sign the file chains up to a root 
	certificate located in the trusted root certificate store. This 
	implies that the identity of the publisher has been verified by 
	a certification authority.
    
	2) In cases where user interface is displayed (which this example
	does not do), WinVerifyTrust will check for whether the  
	end entity certificate is stored in the trusted publisher store,  
	implying that the user trusts content from this publisher.
    
	3) The end entity certificate has sufficient permission to sign 
	code, as indicated by the presence of a code signing EKU or no 
	EKU.
	*/

	GUID wvTPolicyGUID= WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA WinTrustData;

	ZeroMemory(&WinTrustData, sizeof(WINTRUST_DATA));
	WinTrustData.cbStruct = sizeof(WINTRUST_DATA);
	// Use default code signing EKU.
	WinTrustData.pPolicyCallbackData = NULL;
	//No data to pass to SIP
	WinTrustData.pSIPClientData = NULL;
	//Disable WVT UI
	WinTrustData.dwUIChoice = WTD_UI_NONE;
	//No Revocation checking
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
	//	Verify an embedded signature on a file
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
	//Default Verification
	WinTrustData.dwStateAction = 0;
	// Not applicable for default verification of embedded signature
	WinTrustData.hWVTStateData = NULL;
	// Not used
	WinTrustData.pwszURLReference = NULL;
	// Default 
	WinTrustData.dwProvFlags = WTD_SAFER_FLAG;
	// This is not applicable if there is no UI because it changes 
	// the UI to accommodate running applications instead of 
	// installing applications.
	WinTrustData.dwUIContext = 0;

	// Set pFile
	WinTrustData.pFile = &FileData;
	// WinVerifyTrust verifies signatures as specified by the GUID 
	// and Wintrust_Data.

	//ERROR_SUCCESS:
	//          Signed file:
	//              - Hash that represents the subject is trusted.
	//              - Trusted publisher without any verification errors.
	//              - UI was disabled in dwUIChoice. No publisher or 
	//                  time stamp chain errors.
	//              - UI was enabled in dwUIChoice and the user clicked 
	//                  "Yes" when asked to install and run the signed 
	//                  subject.
	//      case TRUST_E_NOSIGNATURE:
	//          // The file was not signed or had a signature 
	//          // that was not valid.

	//          // Get the reason for no signature.
	//          dwLastError = GetLastError();
	//          if (TRUST_E_NOSIGNATURE == dwLastError ||  TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError ||
	//                  TRUST_E_PROVIDER_UNKNOWN == dwLastError) 
	//          {
	//              // The file was not signed.
	//          } 
	//          else 
	//          {
	//              // The signature was not valid or there was an error  opening the file.
	//          }

	//      case TRUST_E_EXPLICIT_DISTRUST:
	//          // The hash that represents the subject or the publisher 
	//          // is not allowed by the admin or user.
	//      case TRUST_E_SUBJECT_NOT_TRUSTED:
	//          // The user clicked "No" when asked to install and run.
	//      case CRYPT_E_SECURITY_SETTINGS:
	//          The hash that represents the subject or the publisher 
	//          was not explicitly trusted by the admin and the 
	//          admin policy has disabled user trust. No signature, 
	//          publisher or time stamp errors.

	//      default:
	//           The UI was disabled in dwUIChoice or the admin policy 
	//           has disabled user trust. lStatus contains the 
	//           publisher or time stamp chain error.

	return WinVerifyTrustEx(NULL, &wvTPolicyGUID, &WinTrustData);
}