#include "StdAfx.h"
#include "Resource.h"
#include "SDKInterface.h"
#include "ADELErrorInfo.h"

CSDKInterface::CSDKInterface()
{
	m_pADELLock = new CADELLock;
}

CSDKInterface::~CSDKInterface()
{
	m_pADELLock->Release();
}

LPCTSTR CSDKInterface::GetWindowClassName() const
{
	return _T("SDKInterface");
}

UILIB_RESOURCETYPE CSDKInterface::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

CDuiString CSDKInterface::GetZIPFileName() const
{
	return _T("SDKInterface.zip");
}

CDuiString CSDKInterface::GetSkinFile()
{
	return _T("SDKInterface.xml");
}

CDuiString CSDKInterface::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("SDKInterface");
#else
	return _T("");
#endif	
}

void CSDKInterface::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CSDKInterface::InitWindow()
{
	SetIcon(IDI_MAINFRAME);
}

void CSDKInterface::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
		OnClick(msg);
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
		OnSelectedChanged(msg);
}

void CSDKInterface::OnClick(TNotifyUI& msg)
{
	CDuiString strNotifyName = msg.pSender->GetName();
	if (strNotifyName == _T("BtnClose"))
		Close(IDOK);
	else if (strNotifyName == _T("BtnMin"))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	else if (strNotifyName == _T("BtnInit"))
		OnInit(msg);
	else if (strNotifyName == _T("BtnExit"))
		Close(IDOK);
	else if (strNotifyName == _T("BtnSetPort"))
		OnSetPort(msg);
	else if (strNotifyName == _T("BtnPopCard"))
		OnBtnPopCard(msg);
	else if (strNotifyName == _T("BtnBeep"))
		OnBtnBeep(msg);
	else if (strNotifyName == _T("BtnNewKey"))
		OnNewKey(msg, 1);
	else if (strNotifyName == _T("BtnDupKey"))
		OnNewKey(msg, 2);
	else if (strNotifyName == _T("BtnAddKey"))
		OnNewKey(msg, 3);
	else if (strNotifyName == _T("BtnReadCard"))
		OnReadCard(msg);
	else if (strNotifyName == _T("BtnEraseCard"))
		OnEraseCard(msg);
	else if (strNotifyName == _T("BtnLost"))
		OnBtnLost(msg);
	else if (strNotifyName == _T("BtnCheckOut"))
		OnBtnCheckOut(msg);
	else if (strNotifyName == _T("BtnChangeUser"))
		OnChangeUser(msg);
	else if (strNotifyName == _T("BtnReadCardId"))
		OnReadCardId(msg);
	else if (strNotifyName == _T("BtnReadData"))
		OnReadData(msg);
	else if (strNotifyName == _T("BtnWriteData"))
		OnWriteData(msg);
	else if (strNotifyName == _T("BtnGetBufInfo"))
		OnBtnGetBufInfo(msg);
	else if (strNotifyName == _T("BtnGetCardInfo"))
		OnBtnGetCardInfo(msg);
}

void CSDKInterface::OnSelectedChanged(TNotifyUI& msg)
{
	CDuiString strNotifyName = msg.pSender->GetName();
	if (strNotifyName == _T("BtnMulti"))
	{
		COptionUI* pMulti = (COptionUI*)msg.pSender->GetInterface(DUI_CTR_OPTION);
		CEditUI2* pMultiElevator = (CEditUI2*)m_PaintManager.FindControl(_T("Elevator"));
		if (pMulti != NULL && pMultiElevator != NULL)
			pMultiElevator->SetEnabled(pMulti->IsSelected());
	}
}

void CSDKInterface::OnInit(TNotifyUI& msg)
{
	CDuiString strSQLAddr;
	int nPort = 0;
	CEditUI2* pSQLAddr = (CEditUI2*)m_PaintManager.FindControl(_T("SQLAddr"));
	if (pSQLAddr != NULL)
		strSQLAddr = pSQLAddr->GetText();
	CComboUI* pComType = (CComboUI*)m_PaintManager.FindControl(_T("ComType"));
	if (pComType != NULL)
		nPort = pComType->GetCurSel();

	CComboUI* pSoftware = (CComboUI*)m_PaintManager.FindControl(_T("Software"));
	int nSelect = pSoftware->GetCurSel();
	int nSoftware = 11;
	if (nSelect == 1)
		nSoftware = 14;
	else if (nSelect == 2)
		nSoftware = 18;

	CComboUI* pEncoder = (CComboUI*)m_PaintManager.FindControl(_T("EncoderType"));
	int nEncoderType =pEncoder->GetCurSel();

	CComboUI* pTMEncoder = (CComboUI*)m_PaintManager.FindControl(_T("TMEncoderType"));
	int nTMEncoder = pEncoder->GetCurSel();
	int nTMEncoderType = 1;
	if (nTMEncoder == 1)
		nTMEncoderType = 5;

	int nRet = m_pADELLock->Init(nSoftware, strSQLAddr, _T("test"), nPort, nEncoderType, nTMEncoderType);

	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("Init调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	else
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("Init调用成功，返回值[%d]"), nRet);
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnSetPort(TNotifyUI& msg)
{
	int nPort = 0;
	CComboUI* pComType = (CComboUI*)m_PaintManager.FindControl(_T("ComType"));
	if (pComType != NULL)
		nPort = pComType->GetCurSel();

	CComboUI* pSoftware = (CComboUI*)m_PaintManager.FindControl(_T("Software"));
	int nSelect = pSoftware->GetCurSel();
	int nSoftware = 11;
	if (nSelect == 1)
		nSoftware = 14;
	else if (nSelect == 2)
		nSoftware = 18;

	CComboUI* pEncoder = (CComboUI*)m_PaintManager.FindControl(_T("EncoderType"));
	int nEncoderType =pEncoder->GetCurSel();

	CComboUI* pTMEncoder = (CComboUI*)m_PaintManager.FindControl(_T("TMEncoderType"));
	int nTMEncoder = pEncoder->GetCurSel();
	int nTMEncoderType = 1;
	if (nTMEncoder == 1)
		nTMEncoderType = 5;

	int nRet = m_pADELLock->SetPort(nSoftware, nPort, nEncoderType, nTMEncoderType);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("SetPort调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnBtnPopCard(TNotifyUI& msg)
{
	int nRet = m_pADELLock->PopCard();
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("PopCard调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnBtnBeep(TNotifyUI& msg)
{
	CComboUI* pBeepType = (CComboUI*)m_PaintManager.FindControl(_T("BeepType"));
	int nBeep = _ttoi(pBeepType->GetText());
	int nRet = m_pADELLock->Reader_Beep(nBeep);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("Reader_Beep调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnNewKey(TNotifyUI& msg, int nNewKey/* = 1*/)
{
	CDuiString strRoomNum, strValidTime, strGuestName, strGuestId, strElevator, strGate;
	LONG lCardNo = 0, nBreakfast = 0, nOpenWay = 0;
	BOOL bCover = FALSE;
	BOOL bGetFinger = FALSE;

	// 房号
	CEditUI2* pRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("RoomNum"));
	if (pRoomNum != NULL)
		strRoomNum = pRoomNum->GetText();

	// 有效期
	CEditUI2* pValidTime = (CEditUI2*)m_PaintManager.FindControl(_T("ValidTime"));
	if (pValidTime != NULL)
		strValidTime = pValidTime->GetText();

	// 是否覆盖
	COptionUI* pCoverCard = (COptionUI*)m_PaintManager.FindControl(_T("BtnCoverCard"));
	if (pCoverCard != NULL)
		bCover	= pCoverCard->IsSelected();

	// 卡号
	CEditUI2* pCardNum = (CEditUI2*)m_PaintManager.FindControl(_T("CardNum"));
	if (pCardNum != NULL)
		lCardNo = _ttoi(pCardNum->GetText());
		
	// 客人姓名和ID
	CEditUI2* pGuestName = (CEditUI2*)m_PaintManager.FindControl(_T("UserName"));
	if (pGuestName != NULL)
		strGuestName = pGuestName->GetText();
	CEditUI2* pGuestId = (CEditUI2*)m_PaintManager.FindControl(_T("UserID"));
	if (pGuestId != NULL)
		strGuestId = pGuestId->GetText();

	// 通道
	CEditUI2* pGate = (CEditUI2*)m_PaintManager.FindControl(_T("Gate"));
	if (pGate != NULL)
		strGate = pGate->GetText();

	// 早餐数量
	CComboUI* pBreakfast = (CComboUI*)m_PaintManager.FindControl(_T("Breakfast"));
	if (pBreakfast != NULL)
		nBreakfast = pBreakfast->GetCurSel();

	// 开门方式
	/*CComboUI* pOpenWay = (CComboUI*)m_PaintManager.FindControl(_T("OpenWay"));
	if (pOpenWay != NULL)
		nOpenWay = pOpenWay->GetCurSel();*/

	// 是否是多路电梯
	/*COptionUI* pMuitl = (COptionUI*)m_PaintManager.FindControl(_T("BtnMulti"));
	if (pMuitl != NULL && pMuitl->IsSelected())
	{
		
	}*/

	CEditUI2* pMuitlElevator = (CEditUI2*)m_PaintManager.FindControl(_T("Elevator"));
	if (pMuitlElevator != NULL)
		strElevator = pMuitlElevator->GetText();

	// 是否采集指纹
	/*COptionUI* pGetFinger = (COptionUI*)m_PaintManager.FindControl(_T("BtnCollecte"));
	if (pGetFinger != NULL)
		bGetFinger = pGetFinger->IsSelected();*/

	int nRet = 0;
	if (nNewKey == 1)
		nRet = m_pADELLock->NewKey(strRoomNum, strGate, strValidTime, strGuestName, strGuestId,
																strElevator, bCover, nBreakfast, &lCardNo, NULL, NULL);
	else if (nNewKey == 2)
		nRet = m_pADELLock->DupKey(strRoomNum, strGate, strValidTime, strGuestName, strGuestId,
																strElevator, bCover, nBreakfast, &lCardNo, NULL, NULL);
	/*else
		nRet = m_pADELLock->AddKey(strRoomNum, strGate, strValidTime, strGuestName, strGuestId, 
															bCover, nOpenWay, &lCardNo, nBreakfast, NULL, NULL, strElevator, bGetFinger);*/
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("NewKey/DupKey调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnReadCard(TNotifyUI& msg)
{
	CARD_DATA CardData;

	int nRet = m_pADELLock->ReadCard(&CardData);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("ReadCard调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pRoom = (CEditUI2*)m_PaintManager.FindControl(_T("GetRoomNum"));
	if (pRoom != NULL)
		pRoom->SetText(CardData.strRoom);

	CEditUI2* pVaildTime = (CEditUI2*)m_PaintManager.FindControl(_T("GetValidTime"));
	if (pVaildTime != NULL)
		pVaildTime->SetText(CardData.strVaildTime);

	CEditUI2* pCardNum = (CEditUI2*)m_PaintManager.FindControl(_T("GetCardNum"));
	if (pCardNum != NULL)
	{
		CDuiString strCardNo;
		strCardNo.Format(_T("%d"), CardData.lCardNo);
		pCardNum->SetText(strCardNo);
	}

	CEditUI2* pGuestName = (CEditUI2*)m_PaintManager.FindControl(_T("GetUserName"));
	if (pGuestName != NULL)
		pGuestName->SetText(CardData.strGuestName);

	CEditUI2* pGuestId = (CEditUI2*)m_PaintManager.FindControl(_T("GetUserID"));
	if (pGuestId != NULL)
		pGuestId->SetText(CardData.strGuestId);

	CEditUI2* pGate = (CEditUI2*)m_PaintManager.FindControl(_T("GetGate"));
	if (pGate != NULL)
		pGate->SetText(CardData.strGate);

	CEditUI2* pBreakfast = (CEditUI2*)m_PaintManager.FindControl(_T("GetBreakfast"));
	if (pBreakfast != NULL)
	{
		CDuiString strBreakfast;
		strBreakfast.Format(_T("%d"), CardData.nBreakfast);
		pBreakfast->SetText(strBreakfast);
	}

	/*CEditUI2* pOpenWay = (CEditUI2*)m_PaintManager.FindControl(_T("GetOpenWay"));
	if (pOpenWay != NULL)
	{
		CDuiString strOpenWay;
		strOpenWay.Format(_T("%d"), CardData.OpenWay);
		pOpenWay->SetText(strOpenWay);
	}*/

	CEditUI2* pMutilElevator = (CEditUI2*)m_PaintManager.FindControl(_T("GetMuitl"));
	if (pMutilElevator != NULL)
		pMutilElevator->SetText(CardData.strElevator);

	// 填写注销用的房号和卡号
	CEditUI2* pSetRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("SetRoomNum"));
	if (pSetRoomNum != NULL)
		pSetRoomNum->SetText(CardData.strRoom);

	CEditUI2* pSetCardId = (CEditUI2*)m_PaintManager.FindControl(_T("SetCardNum"));
	if (pSetCardId != NULL)
	{
		CDuiString strCardId;
		strCardId.Format(_T("%d"), CardData.lCardNo);
		pSetCardId->SetText(strCardId);
	}
}

void CSDKInterface::OnBtnLost(TNotifyUI& msg)
{
	CDuiString strRoomNum;
	LONG lCardNo = 0;
	CEditUI2* pRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("SetRoomNum"));
	if (pRoomNum != NULL)
		strRoomNum = pRoomNum->GetText();

	CEditUI2* pCardNo = (CEditUI2*)m_PaintManager.FindControl(_T("SetCardNum"));
	if (pCardNo != NULL)
		lCardNo = _ttoi(pCardNo->GetText());

	if (lCardNo == 0)
	{
		MessageBox(m_hWnd, _T("请输入卡号^_^"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		pCardNo->SetFocus();
		return;
	}

	int nRet = m_pADELLock->LostCard(strRoomNum, lCardNo);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("LostCard调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		//return;
	}
}

void CSDKInterface::OnEraseCard(TNotifyUI& msg)
{
	CDuiString strRoomNum;
	LONG lCardNo = 0;
	CEditUI2* pRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("SetRoomNum"));
	if (pRoomNum != NULL)
		strRoomNum = pRoomNum->GetText();

	CEditUI2* pCardNo = (CEditUI2*)m_PaintManager.FindControl(_T("SetCardNum"));
	if (pCardNo != NULL)
		lCardNo = _ttoi(pCardNo->GetText());

	if (lCardNo == 0)
	{
		MessageBox(m_hWnd, _T("请输入卡号^_^"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		pCardNo->SetFocus();
		return;
	}

	int nRet = m_pADELLock->EraseCard(lCardNo, NULL, NULL);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("EraseCard调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	/*pCardNo->SetText(_T(""));*/
}

void CSDKInterface::OnBtnCheckOut(TNotifyUI& msg)
{
	CDuiString strRoomNum;
	LONG lCardNo = 0;
	CEditUI2* pRoomNum = (CEditUI2*)m_PaintManager.FindControl(_T("SetRoomNum"));
	if (pRoomNum != NULL)
		strRoomNum = pRoomNum->GetText();

	CEditUI2* pCardNo = (CEditUI2*)m_PaintManager.FindControl(_T("SetCardNum"));
	if (pCardNo != NULL)
		lCardNo = _ttoi(pCardNo->GetText());

	if (lCardNo == 0)
	{
		MessageBox(m_hWnd, _T("请输入卡号^_^"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		pCardNo->SetFocus();
		return;
	}

	int nRet = m_pADELLock->CheckOut(strRoomNum, lCardNo);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("CheckOut调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		//return;
	}
}

void CSDKInterface::OnChangeUser(TNotifyUI& msg)
{
	CDuiString strAdmin;
	CEditUI2* pAdmin = (CEditUI2*)m_PaintManager.FindControl(_T("SetAdmin"));
	if (pAdmin != NULL)
		strAdmin = pAdmin->GetText();

	if (strAdmin.IsEmpty())
	{
		MessageBox(m_hWnd, _T("请输入操作员名称。"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		pAdmin->SetFocus();
		return;
	}

	m_pADELLock->ChangeUser(strAdmin);
}

void CSDKInterface::OnReadCardId(TNotifyUI& msg)
{
	LONG lCardId = 0;
	int nRet = m_pADELLock->ReadCardId(&lCardId);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("ReadCardId调用失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
	CEditUI2* pCardId = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardId"));
	if (pCardId != NULL)
	{
		CDuiString strCardId;
		strCardId.Format(_T("0x%X"), lCardId);
		pCardId->SetText(strCardId);
	}
}

void CSDKInterface::OnReadData(TNotifyUI& msg)
{
	int nLen = 0, nBlock = 0, nSection = 0;

	CComboUI* pSection = (CComboUI*)m_PaintManager.FindControl(_T("ComboSection"));
	if (pSection != NULL)
		nSection = pSection->GetCurSel();

	CComboUI* pBlock = (CComboUI*)m_PaintManager.FindControl(_T("ComboBlock"));
	if (pBlock != NULL)
		nBlock = pBlock->GetCurSel();

	CComboUI* pLen = (CComboUI*)m_PaintManager.FindControl(_T("ComboLen"));
	if (pLen != NULL)
		nLen = _ttoi(pLen->GetText());

	int nStart = MAKEWORD(nSection,nBlock);//nBlock | (nSection < 8);
	CString strData;
	int nRet = m_pADELLock->ReadIC(nStart, nLen, strData);

	if (nRet > 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("调用ReadCardData失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pReadData = (CEditUI2*)m_PaintManager.FindControl(_T("SetData"));
	if (pReadData != NULL)
		pReadData->SetText(strData);
}

void CSDKInterface::OnWriteData(TNotifyUI& msg)
{
	int nAuth = 0, nLen = 0, nBlock = 0, nSection = 0;
	CDuiString strPassword, strWriteData;

	// 获取密码
	CEditUI2* pCardPassword = (CEditUI2*)m_PaintManager.FindControl(_T("Password"));
	if (pCardPassword != NULL)
		strPassword = pCardPassword->GetText();

	// 获取授权类型
	CComboUI* pAuthType = (CComboUI*)m_PaintManager.FindControl(_T("AuthType"));
	if (pAuthType != NULL)
		nAuth = pAuthType->GetCurSel();

	// 获取扇区
	CComboUI* pSection = (CComboUI*)m_PaintManager.FindControl(_T("ComboSection"));
	if (pSection != NULL)
		nSection = pSection->GetCurSel();

	// 获取块
	CComboUI* pBlock = (CComboUI*)m_PaintManager.FindControl(_T("ComboBlock"));
	if (pBlock != NULL)
		nBlock = pBlock->GetCurSel();

	// 获取长度
	CComboUI* pLen = (CComboUI*)m_PaintManager.FindControl(_T("ComboLen"));
	if (pLen != NULL)
		nLen = _ttoi(pLen->GetText());

	int nStart = MAKEWORD(nSection, nBlock);//nBlock | (nSection < 8);
	// 获取写入数据
	CEditUI2* pWriteData = (CEditUI2*)m_PaintManager.FindControl(_T("SetData"));
	if (pWriteData != NULL)
		strWriteData = pWriteData->GetText();

	CDuiString strWriteCardData;
	strWriteCardData.Format(_T("%d%s%s"), nAuth, (LPCTSTR)strPassword.Left(7), (LPCTSTR)strWriteData.Left(nLen));

	int nRet = m_pADELLock->WriteIC(nStart, nLen, strWriteCardData);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("调用WriteCardData失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}
}

void CSDKInterface::OnBtnGetBufInfo(TNotifyUI& msg)
{
	BUFF_INFO BuffInfo;
	int nRet = m_pADELLock->GetBuffInfo(&BuffInfo);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("调用GetBuffInfo失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pReadCardNum = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardNum"));
	if (pReadCardNum != NULL)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("%d"), BuffInfo.lCardNo);
		pReadCardNum->SetText(strTipInfo);
	}

	CEditUI2* pReadCardType = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardType"));
	if (pReadCardType)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("%d"), BuffInfo.CardType);
		pReadCardType->SetText(strTipInfo);
		GetCardType((CARD_TYPE)BuffInfo.CardType, strTipInfo);
		pReadCardType->SetToolTip(strTipInfo);
	}

	CEditUI2* pReadCardState = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardState"));
	if (pReadCardState)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("%d"), BuffInfo.CardState);
		pReadCardState->SetText(strTipInfo);
		GetCardState((CARD_STATE)BuffInfo.CardState, strTipInfo);
		pReadCardState->SetToolTip(strTipInfo);
	}

	CEditUI2* pReadCardUser = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardUser"));
	if (pReadCardUser)
		pReadCardUser->SetText(BuffInfo.strGName);

	CEditUI2* pReadCardRoom = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardRoom"));
	if (pReadCardRoom)
		pReadCardRoom->SetText(BuffInfo.strRoomNum);
	
	CEditUI2* pReadCardTime = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardTime"));
	if (pReadCardTime)
		pReadCardTime->SetText(BuffInfo.strValidTime);
}

void CSDKInterface::OnBtnGetCardInfo(TNotifyUI& msg)
{
	BUFF_INFO BuffInfo;
	CEditUI2* pReadCardNum = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardNum"));
	if (pReadCardNum->GetText().IsEmpty())
	{
		MessageBox(m_hWnd, _T("卡号不能为空，请重新输入"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		pReadCardNum->SetFocus();
		return;
	}
	
	BuffInfo.lCardNo = _ttoi(pReadCardNum->GetText());

	CEditUI2* pReadCardType = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardType"));
	if (pReadCardType->GetText().IsEmpty())
	{
		MessageBox(m_hWnd, _T("卡状态不能为空，请重新输入"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		pReadCardType->SetFocus();
		return;
	}

	BuffInfo.CardType = _ttoi(pReadCardType->GetText());

	int nRet = m_pADELLock->GetBuffInfo(&BuffInfo);
	if (nRet != 0)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("调用GetBuffInfo失败，返回值[%d]:%s"), nRet, GetADELErrorInfo(nRet));
		MessageBox(m_hWnd, strTipInfo, _T("提示"), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CEditUI2* pReadCardState = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardState"));
	if (pReadCardState)
	{
		CDuiString strTipInfo;
		strTipInfo.Format(_T("%d"), BuffInfo.CardState);
		pReadCardState->SetText(strTipInfo);
		GetCardState((CARD_STATE)BuffInfo.CardState, strTipInfo);
		pReadCardState->SetToolTip(strTipInfo);
	}

	CEditUI2* pReadCardUser = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardUser"));
	if (pReadCardUser)
		pReadCardUser->SetText(BuffInfo.strGName);

	CEditUI2* pReadCardRoom = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardRoom"));
	if (pReadCardRoom)
		pReadCardRoom->SetText(BuffInfo.strRoomNum);

	CEditUI2* pReadCardTime = (CEditUI2*)m_PaintManager.FindControl(_T("ReadCardTime"));
	if (pReadCardTime)
		pReadCardTime->SetText(BuffInfo.strValidTime);
}

LRESULT CSDKInterface::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (m_pADELLock != NULL)
		m_pADELLock->EndSession();
	PostQuitMessage(0);
	// 需要返回bHandle = FALSE , 操作系统才能给窗口发送WM_NCDESTROY消息，从而完全退出软件
	bHandled = FALSE;
	return 0;
}

void CSDKInterface::GetCardType(CARD_TYPE CardType, CDuiString& strTipInfo)
{
	LPCTSTR lpszTipInfo = _T("未定义");
	switch (CardType)
	{
		case CARD_TYPE_PRESIDENT:
				lpszTipInfo = _T("总裁卡");
			break;
		case CARD_TYPE_VILLA:
				lpszTipInfo = _T("别墅卡");
			break;
		case CARD_TYPE_MANAGE:
				lpszTipInfo = _T("管理卡");
			break;
		case CARD_TYPE_CONTROL:
				lpszTipInfo = _T("总控卡");
			break;
		case CARD_TYPE_ROOM_TIME:
				lpszTipInfo = _T("时钟卡");
			break;
		case CARD_TYPE_LEADER:
				lpszTipInfo = _T("邻班卡");
			break;
		case CARD_TYPE_FLOOR:
				lpszTipInfo = _T("楼层卡");
			break;
		case CARD_TYPE_CLEAR:
				lpszTipInfo = _T("清洁卡");
			break;
		case CARD_TYPE_CUSTOM:
				lpszTipInfo = _T("客人卡");
			break;
		case CARD_TYPE_FINISH:
				lpszTipInfo = _T("终止卡");
			break;
		case CARD_TYPE_MEETING:
				lpszTipInfo = _T("会议卡");
			break;
		case CARD_TYPE_EMERGENCY:
				lpszTipInfo = _T("应急卡");
			break;
		case CARD_TYPE_CHECK_OUT:
				lpszTipInfo = _T("退房卡");
			break;
		case CARD_TYPE_RESVERED:
				lpszTipInfo = _T("备用卡");
			break;
		default:
			lpszTipInfo = _T("未定义");
	}

	strTipInfo = lpszTipInfo;
}

void CSDKInterface::GetCardState(CARD_STATE CardState, CDuiString& strTipInfo)
{
	LPCTSTR lpszTipInfo = _T("未定义");
	switch (CardState)
	{
		case CARD_STATE_NORMAL:
				lpszTipInfo = _T("正常使用");
			break;
		case CARD_STATE_LOGOUT:
				lpszTipInfo = _T("正常注销");
			break;
		case CARD_STATE_LOST:
				lpszTipInfo = _T("遗失注销");
			break;
		case CARD_STATE_BROKEN:
				lpszTipInfo = _T("损毁注销");
			break;
		case CARD_STATE_AUTO:
				lpszTipInfo = _T("自动注销");
			break;
		default:
			lpszTipInfo = _T("未定义");
	}

	strTipInfo = lpszTipInfo;
}