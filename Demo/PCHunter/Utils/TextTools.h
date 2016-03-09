#pragma once

CDuiString GetFormatIP(DWORD dwIP);
CDuiString GetFormatIPAndPort(DWORD dwIP,DWORD dwPort);
CDuiString GetConnectionState(DWORD dwConnectionState);
CDuiString GetFormatNum(DWORD dwNum);
CDuiString GetAddressFamily(DWORD dwFamily);
CDuiString GetAddressProtocol(DWORD dwProtocol);
CDuiString GetWlanAuthAlgorithm(DWORD dwAuth);
CDuiString GetWlanCipherAlgorithm(DWORD dwCipher);

BOOL CopyDataToClipboard(DWORD dwDataType,HWND hWndNewOwner,LPCTSTR lpszText);