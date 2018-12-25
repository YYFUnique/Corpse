#include "StdAfx.h"
#include "QRTool.h"
#include "Resource.h"
#include "QRDlg.h"
#include "QRCode/qr.h"
#include "DllCore/Utils/TextTools.h"
#include <CommDlg.h>

#define QR_BKIMG		_T("QrBkImg.bmp")

CQRTool::CQRTool()
{

}

CQRTool::~CQRTool()
{
	CMagnetFrame::GetInstance()->Release();
}

void CQRTool::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LPCTSTR CQRTool::GetWindowClassName() const
{
	return _T("QrCode");
}

UILIB_RESOURCETYPE CQRTool::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString CQRTool::GetZIPFileName() const
{
	return _T("QRTool.zip");
}

CDuiString CQRTool::GetSkinFile()
{
	return _T("QRTool.xml");
}

CDuiString CQRTool::GetSkinFolder()
{
	return _T("QRTool");
}

void CQRTool::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
}

LRESULT CQRTool::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(uMsg)
	{
		case WM_QRCODEITEM_INFO:
				bHandled = TRUE;
				return OnQRCodeItemInfo(wParam, lParam);
			break;
		default:
			return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	}
}

void CQRTool::InitWindow()
{
	SetIcon(IDI_MAINFRAME);

	CMagnetFrame::Initialize();
	CMagnetFrame::GetInstance()->SetMainWnd(m_hWnd);

	//int nCode = QR_ERR_NONE;
	//QRCode* pCode = qrInit(6, QR_EM_8BIT, QR_ECL_M, 7, &nCode);

	//if (pCode == NULL)
	//	return;

	//LPCSTR lpszText = "直接一耳巴子!";
	//LPSTR lpszUtf8 = Gb32ToUtf8(lpszText);
	//if (qrAddData(pCode, (const qr_byte_t* )lpszUtf8, strlen(lpszUtf8)) == FALSE)
	//	return;
	////注意需要调用qrFinalize函数
	//if (qrFinalize(pCode) == FALSE)
	//	return;

	//int size = 0;
	//qr_byte_t * buffer = qrSymbolToBMP(pCode, 1, 6, &size);
	//if (buffer == NULL)
	//	return;

	//m_PaintManager.AddImage(_T("BtnFace.bmp"), (LPBYTE)buffer, size);

	//qrFreeBMP(buffer);

	//CLabelUI* pLabel = (CButtonUI*)m_PaintManager.FindControl(_T("pic"));
	//if (pLabel)
	//	pLabel->SetBkImage(_T("BtnFace.bmp"));
}

void CQRTool::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_PaintManager.FindControl(_T("BtnClose")))	
		PostQuitMessage(0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnMin")))
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnSave")))
		OnSave();
	else if (msg.pSender == m_PaintManager.FindControl(_T("BtnCreate")))
		OnCreate(msg);
}

void CQRTool::OnCreate(TNotifyUI& msg)
{
	HWND hQRWnd = FindWindow(_T("QrCodeDlg"), NULL);
	if (hQRWnd != NULL)
		return;

	CQRDlg* pQRDlg = new CQRDlg(m_hWnd);
	
	pQRDlg->ShowWindow();
	CMagnetFrame::GetInstance()->AddSubWnd(pQRDlg->GetHWND(), ATTCH_MODE_RIGHT, ATTCH_ALIGN_TOP);
}

void CQRTool::OnSave()
{
	TCHAR szApkFilePath[MAX_PATH] = {0};
	_tcscpy_s(szApkFilePath, _countof(szApkFilePath), _T("未命名.bmp"));
	OPENFILENAME SaveFileName = {0};
	SaveFileName.lStructSize		= sizeof(SaveFileName); 
	SaveFileName.hwndOwner		= m_hWnd; 
	SaveFileName.lpstrFile			= szApkFilePath; 
	SaveFileName.nMaxFile			= _countof(szApkFilePath);
	SaveFileName.lpstrFilter			= _T("BMP图片\0*.bmp\0"); 
	SaveFileName.nFilterIndex		= 1; 
	SaveFileName.lpstrFileTitle	= NULL; 
	SaveFileName.nMaxFileTitle	= 0; 
	SaveFileName.lpstrTitle			= _T("请选择图片保存位置");
	SaveFileName.lpstrInitialDir	= NULL ; 
	SaveFileName.Flags				= OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST ; 

	if (GetSaveFileName(&SaveFileName) == FALSE)
		return ;

	const TImageInfo* pImage = m_PaintManager.GetImage(QR_BKIMG);
	CRenderEngine::SaveBitmapFile(pImage->hBitmap,SaveFileName.lpstrFile);
}

LRESULT CQRTool::OnQRCodeItemInfo(WPARAM wParam, LPARAM lParam)
{
	QRCodeItem* pQRCodeInfo = (QRCodeItem*)wParam;

	LRESULT lRet = FALSE;

	CButtonUI* pQRPic = (CButtonUI*)m_PaintManager.FindControl(_T("BtnPic"));

	do 
	{
		int nCode = QR_ERR_NONE;
		QRCode* pCode = qrInit(pQRCodeInfo->dwSize, QR_EM_8BIT, (qr_ecl_t)pQRCodeInfo->dwLevel, 7, &nCode);

		if (pCode == NULL)
			return FALSE;

		//LPCSTR lpszText = "直接一耳巴子!";
		CStringA strQRCode(pQRCodeInfo->strQrCode);
		LPSTR lpszUtf8 = Gb32ToUtf8(strQRCode);
		if (qrAddData(pCode, (const qr_byte_t* )lpszUtf8, strlen(lpszUtf8)) == FALSE)
		{
			CString strTipInfo(qrGetErrorInfo(pCode));
			MessageBox(m_hWnd, strTipInfo, _T("生成错误"), MB_OK|MB_ICONINFORMATION);
			break;
		}
		//注意需要调用qrFinalize函数
		if (qrFinalize(pCode) == FALSE)
			break;

		int size = 0;
		qr_byte_t* lpData = qrSymbolToBMP2(pCode, pQRCodeInfo->dwBorder, pQRCodeInfo->dwMag, &size);
		if (lpData == NULL)
			break;

		m_PaintManager.RemoveImage(QR_BKIMG);
		m_PaintManager.AddImage(QR_BKIMG, (LPBYTE)lpData, size);

		qrFreeBMP(lpData);

		if (pQRPic)
		{
			pQRPic->SetBkImage(QR_BKIMG);

			if (pQRCodeInfo->strLogoFile.IsEmpty() == FALSE)
			{
				CDuiString strLogoFile;
				strLogoFile.Format(_T("file='%s' dest='120,120,180,180'"), pQRCodeInfo->strLogoFile);
				pQRPic->SetForeImage(strLogoFile);
			}

			//由于二维码图片名称未发生改变，需要强制刷新
			pQRPic->Invalidate();
		}

		lRet = TRUE;
	} while (FALSE);

	if (lRet == FALSE)
	{
		pQRPic->SetBkImage(_T("error.png"));
		pQRPic->Invalidate();
	}

	return lRet;
}