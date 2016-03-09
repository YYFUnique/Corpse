#pragma once

//xml sample:<SkinPikerPictureItem name="" width="118" height="70" bkimage="UI\BKImage\1small.jpg" bkname="����" author="Redrain" />
//�����ͽӿ�������CreateControl�����л��õ�
const TCHAR kSkinPickerPictureItemClassName[] =	_T("SkinPikerPictureItemUI");
const TCHAR kSkinPickerPictureItemInterface[] =	_T("SkinPikerPictureItem");

//��ɫ��ǰ��ͼ��λ��
const TCHAR kSkinPickerPictureItemForeImage[] =	_T("file='UI\\LeftTab\\listitem\\ListBk.png' fade='150'");

//�߿����ɫ��ͼƬ���Ƶ�������ɫ��������Ϣ��������ɫ
const DWORD kBorderColor					  = 0xFF64B0FA;
const DWORD kBkNameColor					  = 0xFFFFFFFF;
const DWORD kAuthorColor					  = 0xFFAAAAAA;

class CSkinPikerPictureItemUI : public CButtonUI
{
public:
	CSkinPikerPictureItemUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintStatusImage(HDC hDC);

private:
	CDuiString m_BkName;
	CDuiString m_Author;
};
