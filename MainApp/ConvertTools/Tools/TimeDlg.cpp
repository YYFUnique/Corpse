
// TimeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TimeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CTimeDlg �Ի���


#define  ALWAYS_GET_SYSTEM_TIME   1000

CTimeDlg::CTimeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTimeDlg::IDD, pParent)
, m_CStyleTime(_T(""))
, m_CStyleTimeLocal(_T(""))
, m_SysTime(_T(""))
, m_SysTimeLocal(_T(""))
, m_FileTimeHigh(_T(""))
, m_FileTimeLow(_T(""))
, m_FileTime(_T(""))
, m_FileTimeHighLocal(_T(""))
, m_FileTimeLowLocal(_T(""))
, m_FileTimeLocal(_T(""))
{

}

void CTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CSTYLE_TIME, m_CStyleTime);
	DDX_Text(pDX, IDC_EDIT_CSTYLE_TIME_LOCLAL, m_CStyleTimeLocal);
	DDX_Text(pDX, IDC_EDIT_SYSTEM_TIME, m_SysTime);
	DDX_Text(pDX, IDC_EDIT_SYSTEM_TIME_LOCAL, m_SysTimeLocal);
	DDX_Text(pDX, IDC_EDIT_FT_HIGH, m_FileTimeHigh);
	DDX_Text(pDX, IDC_EDIT_FT_LOW, m_FileTimeLow);
	DDX_Text(pDX, IDC_EDIT_FT, m_FileTime);
	DDX_Text(pDX, IDC_EDIT_FT_HIGH_LOCAL, m_FileTimeHighLocal);
	DDX_Text(pDX, IDC_EDIT_FT_LOW_LOCAL, m_FileTimeLowLocal);
	DDX_Text(pDX, IDC_EDIT_FT_LOCAL, m_FileTimeLocal);
}

BEGIN_MESSAGE_MAP(CTimeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_GETTIME, &CTimeDlg::OnBnClickedBtnGettime)
	ON_EN_CHANGE(IDC_EDIT_CSTYLE_TIME, &CTimeDlg::OnEnChangeEditCstyleTime)
	ON_EN_CHANGE(IDC_EDIT_CSTYLE_TIME_LOCLAL, &CTimeDlg::OnEnChangeEdicsytyleTimeLoclal)
	ON_EN_CHANGE(IDC_EDIT_SYSTEM_TIME, &CTimeDlg::OnEnChangeEditSystemTime)
	ON_EN_CHANGE(IDC_EDIT_SYSTEM_TIME_LOCAL, &CTimeDlg::OnEnChangeEditSystemTimeLocal)
	ON_EN_CHANGE(IDC_EDIT_FT_HIGH, &CTimeDlg::OnEnChangeEditFtHigh)
	ON_EN_CHANGE(IDC_EDIT_FT_LOW, &CTimeDlg::OnEnChangeEditFtLow)
	ON_EN_CHANGE(IDC_EDIT_FT_HIGH_LOCAL, &CTimeDlg::OnEnChangeEditFtHighLocal)
	ON_EN_CHANGE(IDC_EDIT_FT_LOW_LOCAL, &CTimeDlg::OnEnChangeEditFtLowLocal)
	ON_EN_CHANGE(IDC_EDIT_FT, &CTimeDlg::OnEnChangeEditFt)
	ON_EN_CHANGE(IDC_EDIT_FT_LOCAL, &CTimeDlg::OnEnChangeEditFtLocal)
	ON_BN_CLICKED(IDC_CHECK_GET_SYSTEM_TIME, &CTimeDlg::OnBnClickedCheckGetSystemTime)
END_MESSAGE_MAP()


// CTimeDlg ��Ϣ�������

BOOL CTimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetWindowText(_T("ʱ��ת������"));
	FILETIME FileTime;
	CoFileTimeNow(&FileTime);
	ShowMemberValue(FileTime,0,FALSE);
	
	((CButton*)GetDlgItem(IDC_CHECK_GET_SYSTEM_TIME))->SetCheck(TRUE);
	OnBnClickedCheckGetSystemTime();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTimeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTimeDlg::OnQueryDragIcon()
{
	//return static_cast<HCURSOR>(m_hIcon);
	return LoadCursor(NULL,IDC_ARROW);
}

void CTimeDlg::UpdateControlData()
{
	UpdateData(FALSE);
}

void CTimeDlg::OnBnClickedBtnGettime()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	FILETIME FileTime;
	CoFileTimeNow(&FileTime);
	ShowMemberValue(FileTime,0,FALSE);
}

void CTimeDlg::OnEnChangeEditCstyleTime()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	//m_CStyleTime=GetDlgItemInt(IDC_EDIT_CSTYLE_TIME,NULL,false);

	UpdateData(TRUE);
	FILETIME FileTime,FileTimeLocal;
	if(!m_CStyleTime.IsEmpty())
	{
		INT64 iTime=0;
		_stscanf_s(m_CStyleTime,_T("%I64u"),&iTime);

		//��iTimeΪ����ʱ��
		CTime Time(iTime);

		SYSTEMTIME SysTime;
		Time.GetAsSystemTime(SysTime);

		SystemTimeToFileTime(&SysTime,&FileTimeLocal);
		LocalFileTimeToFileTime(&FileTimeLocal,&FileTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
		FileTime.dwLowDateTime=0;
	}

	ShowMemberValue(FileTime,IDC_EDIT_CSTYLE_TIME,FALSE);

}

void CTimeDlg::OnEnChangeEdicsytyleTimeLoclal()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	FILETIME FileTime,FileTimeLocal;
	if(!m_CStyleTimeLocal.IsEmpty())
	{
		INT64 iTime;
		_stscanf_s(m_CStyleTimeLocal,_T("%I64u"),&iTime);
		//�Ե�ǰ����ʱ����Ϊ����ʱ��
		CTime Time(iTime);

		SYSTEMTIME SysTime;
		Time.GetAsSystemTime(SysTime);

		SystemTimeToFileTime(&SysTime,&FileTimeLocal);
		LocalFileTimeToFileTime(&FileTimeLocal,&FileTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
		FileTime.dwLowDateTime=0;
	}
	ShowMemberValue(FileTime,IDC_EDIT_CSTYLE_TIME_LOCLAL,TRUE);
}

void CTimeDlg::OnEnChangeEditSystemTime()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	FILETIME FileTime;
	if(!m_SysTime.IsEmpty())
	{
		SYSTEMTIME SysTime;
		ZeroMemory(&SysTime,sizeof(SYSTEMTIME));
		_stscanf_s(m_SysTime,_T("%d-%d-%d %d-%d-%d"),
			&SysTime.wYear,&SysTime.wMonth,&SysTime.wDay,&SysTime.wHour,&SysTime.wMinute,&SysTime.wSecond);

		SystemTimeToFileTime(&SysTime,&FileTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
		FileTime.dwLowDateTime=0;
	}


	ShowMemberValue(FileTime,IDC_EDIT_SYSTEM_TIME,FALSE);
}

void CTimeDlg::OnEnChangeEditSystemTimeLocal()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	FILETIME FileTime;
	if(!m_SysTimeLocal.IsEmpty())
	{

		SYSTEMTIME SysTime;
		ZeroMemory(&SysTime,sizeof(SYSTEMTIME));
		_stscanf_s(m_SysTimeLocal,_T("%d-%d-%d %d-%d-%d"),&SysTime.wYear,&SysTime.wMonth,&SysTime.wDay,&SysTime.wHour,&SysTime.wMinute,&SysTime.wSecond);

		SystemTimeToFileTime(&SysTime,&FileTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
		FileTime.dwLowDateTime=0;
	}


	ShowMemberValue(FileTime,IDC_EDIT_SYSTEM_TIME_LOCAL,TRUE);
}

void CTimeDlg::OnEnChangeEditFtHigh()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	FILETIME FileTime;
	if(!m_FileTimeLow.IsEmpty())
	{
		_stscanf_s(m_FileTimeLow,_T("%lu"),&FileTime.dwLowDateTime);
	}
	else
	{
		FileTime.dwLowDateTime=0;
	}

	if(!m_FileTimeHigh.IsEmpty())
	{
		_stscanf_s(m_FileTimeHigh,_T("%lu"),&FileTime.dwHighDateTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
	}


	ShowMemberValue(FileTime,IDC_EDIT_FT_HIGH,FALSE);
}

void CTimeDlg::OnEnChangeEditFtLow()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	FILETIME FileTime;
	if(!m_FileTimeLow.IsEmpty())
	{
		_stscanf_s(m_FileTimeLow,_T("%lu"),&FileTime.dwLowDateTime);
	}
	else
	{
		FileTime.dwLowDateTime=0;
	}

	if(!m_FileTimeHigh.IsEmpty())
	{
		_stscanf_s(m_FileTimeHigh,_T("%lu"),&FileTime.dwHighDateTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
	}

	ShowMemberValue(FileTime,IDC_EDIT_FT_LOW,FALSE);
}

void CTimeDlg::OnEnChangeEditFtHighLocal()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	FILETIME FileTime;
	if(!m_FileTimeLowLocal.IsEmpty())
	{
		_stscanf_s(m_FileTimeLowLocal,_T("%lu"),&FileTime.dwLowDateTime);
	}
	else
	{
		FileTime.dwLowDateTime=0;
	}

	if(!m_FileTimeHighLocal.IsEmpty())
	{
		_stscanf_s(m_FileTimeHighLocal,_T("%lu"),&FileTime.dwHighDateTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
	}

	ShowMemberValue(FileTime,IDC_EDIT_FT_HIGH_LOCAL,TRUE);
}

void CTimeDlg::OnEnChangeEditFtLowLocal()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData();

	FILETIME FileTime;

	if(!m_FileTimeLowLocal.IsEmpty())
	{
		_stscanf_s(m_FileTimeLowLocal,_T("%lu"),&FileTime.dwLowDateTime);
	}
	else
	{
		FileTime.dwLowDateTime=0;
	}

	if(!m_FileTimeHighLocal.IsEmpty())
	{
		_stscanf_s(m_FileTimeHighLocal,_T("%lu"),&FileTime.dwHighDateTime);
	}
	else
	{
		FileTime.dwHighDateTime=0;
	}

	ShowMemberValue(FileTime,IDC_EDIT_FT_LOW_LOCAL,TRUE);
}

void CTimeDlg::OnEnChangeEditFt()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	GetDlgItemText(IDC_EDIT_FT,m_FileTime);
	FILETIME FileTime;
	if(m_FileTime.IsEmpty())
	{
		FileTime.dwHighDateTime=0;
		FileTime.dwLowDateTime=0;
	}
	else
	{
		swscanf_s(m_FileTime,_T("%I64u"),&FileTime);
	}

	ShowMemberValue(FileTime,IDC_EDIT_FT,FALSE);
}

void CTimeDlg::OnEnChangeEditFtLocal()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	GetDlgItemText(IDC_EDIT_FT_LOCAL,m_FileTimeLocal);
	FILETIME FileTimeLocal;
	if (m_FileTimeLocal.IsEmpty())
	{
		FileTimeLocal.dwHighDateTime=0;
		FileTimeLocal.dwLowDateTime=0;
	}
	else
	{
		_stscanf_s(m_FileTimeLocal,_T("%I64u"),&FileTimeLocal);
	}

	ShowMemberValue(FileTimeLocal,IDC_EDIT_FT_LOCAL,TRUE);
}


void CTimeDlg::ShowMemberValue(FILETIME& FileTimePass,int nControlId/*=0*/,BOOL bLocalTime/*=FALSE*/)
{
	LARGE_INTEGER LargeInt,LargeIntLocal;
	FILETIME FileTime,FileTimeLocal;
	time_t CStyleTime,CStyleTimeLocal;
	if(bLocalTime)
	{
		LocalFileTimeToFileTime(&FileTimePass,&FileTime);
		LargeInt.HighPart=FileTime.dwHighDateTime;
		LargeInt.LowPart=FileTime.dwLowDateTime;


		INT64 tVal=LargeInt.QuadPart/10000000-11644473600;
		if(tVal<0)
		{
			CStyleTime=0;
		}
		else
		{
			CStyleTime=tVal;
		}

		LargeIntLocal.HighPart=FileTimePass.dwHighDateTime;
		LargeIntLocal.LowPart=FileTimePass.dwLowDateTime;

		INT64 tValLocal=LargeIntLocal.QuadPart/10000000-11644473600;
		if(tValLocal<0)
		{
			CStyleTimeLocal=0;
		}
		else
		{
			CStyleTimeLocal=tValLocal;
		}
	}
	else
	{
		LargeInt.HighPart=FileTimePass.dwHighDateTime;
		LargeInt.LowPart=FileTimePass.dwLowDateTime;

		INT64 tVal=LargeInt.QuadPart/10000000-11644473600;
		if(tVal<0)
		{
			CStyleTime=0;
		}
		else
		{
			CStyleTime=tVal;
		}

		FileTimeToLocalFileTime(&FileTimePass,&FileTimeLocal);
		LargeIntLocal.HighPart=FileTimeLocal.dwHighDateTime;
		LargeIntLocal.LowPart=FileTimeLocal.dwLowDateTime;

		INT64 tValLocal=LargeIntLocal.QuadPart/10000000-11644473600;
		if(tValLocal<0)
		{
			CStyleTimeLocal=0;
		}
		else
		{
			CStyleTimeLocal=tValLocal;
		}
	}


	if (nControlId!=IDC_EDIT_CSTYLE_TIME)
	{
		if(CStyleTime)
			m_CStyleTime.Format(_T("%I64u"),CStyleTime);
		else
			m_CStyleTime=_T("");
	}

	if(nControlId!=IDC_EDIT_CSTYLE_TIME_LOCLAL)
	{
		if (CStyleTimeLocal)
			m_CStyleTimeLocal.Format(_T("%I64u"),CStyleTimeLocal);
		else
			m_CStyleTimeLocal=_T("");
	}

	SYSTEMTIME SysTime,SysTimeLocal;
	BOOL bEmptySysTime=FALSE,bEmptySysTimelocal=FALSE;
	if(bLocalTime)
	{
		LocalFileTimeToFileTime(&FileTimePass,&FileTime);

		LargeInt.HighPart=FileTime.dwHighDateTime;
		LargeInt.LowPart=FileTime.dwLowDateTime;
		if(LargeInt.QuadPart/10000000<=0)
		{
			ZeroMemory(&SysTime,sizeof(SYSTEMTIME));
			bEmptySysTime=TRUE;
		}
		else
		{
			FileTimeToSystemTime(&FileTime,&SysTime);
		}

		LargeIntLocal.HighPart=FileTimePass.dwHighDateTime;
		LargeIntLocal.LowPart=FileTimePass.dwLowDateTime;

		if(LargeIntLocal.QuadPart/10000000<=0)
		{
			ZeroMemory(&SysTimeLocal,sizeof(SYSTEMTIME));
			bEmptySysTimelocal=TRUE;
		}
		else
		{
			FileTimeToSystemTime(&FileTimePass,&SysTimeLocal);
		}
	}
	else
	{
		LargeInt.HighPart=FileTimePass.dwHighDateTime;
		LargeInt.LowPart=FileTimePass.dwLowDateTime;
		if(LargeInt.QuadPart/10000000<=0)
		{
			ZeroMemory(&SysTime,sizeof(SYSTEMTIME));
			bEmptySysTime=TRUE;
		}
		else
		{
			FileTimeToSystemTime(&FileTimePass,&SysTime);
		}

		FileTimeToLocalFileTime(&FileTimePass,&FileTimeLocal);

		LargeIntLocal.HighPart=FileTimeLocal.dwHighDateTime;
		LargeIntLocal.LowPart=FileTimeLocal.dwLowDateTime;
		INT64 i=LargeIntLocal.QuadPart/10000000-28800;
		if(i<=0)
		{
			ZeroMemory(&SysTimeLocal,sizeof(SYSTEMTIME));
			bEmptySysTimelocal=TRUE;
		}
		else
		{
			FileTimeToSystemTime(&FileTimeLocal,&SysTimeLocal);
		}
	}

	if(nControlId!=IDC_EDIT_SYSTEM_TIME)
	{
		if(!bEmptySysTime)
			m_SysTime.Format(_T("%0.4d-%0.2d-%0.2d %0.2d-%0.2d-%0.2d"),SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
		else
			m_SysTime=_T("");
	}
	if(nControlId!=IDC_EDIT_SYSTEM_TIME_LOCAL)
	{
		if(!bEmptySysTimelocal)
			m_SysTimeLocal.Format(_T("%0.4d-%0.2d-%0.2d %0.2d-%0.2d-%0.2d"),SysTimeLocal.wYear,SysTimeLocal.wMonth,SysTimeLocal.wDay,SysTimeLocal.wHour,SysTimeLocal.wMinute,SysTimeLocal.wSecond);
		else
			m_SysTimeLocal=_T("");
	}

	BOOL bStandardTimeNull=FALSE;
	if(bLocalTime)
	{
		LocalFileTimeToFileTime(&FileTimePass,&FileTime);

		LargeInt.HighPart=FileTime.dwHighDateTime;
		LargeInt.LowPart=FileTime.dwLowDateTime;

		if(LargeInt.QuadPart<0)
		{
			ZeroMemory(&FileTime,sizeof(FILETIME));
		}
		FileTimeLocal=FileTimePass;
	}
	else
	{
		FileTime=FileTimePass;
		FileTimeToLocalFileTime(&FileTime,&FileTimeLocal);
		if(!FileTime.dwHighDateTime&&!FileTime.dwLowDateTime)
			bStandardTimeNull=TRUE;
	}


	if(nControlId!=IDC_EDIT_FT_HIGH)
	{
		if(FileTime.dwHighDateTime)
			m_FileTimeHigh.Format(_T("%lu"),FileTime.dwHighDateTime);
		else
			m_FileTimeHigh=_T("");
	}

	if(nControlId!=IDC_EDIT_FT_LOW)
	{
		if(FileTime.dwLowDateTime)
			m_FileTimeLow.Format(_T("%lu"),FileTime.dwLowDateTime);
		else
			m_FileTimeLow=_T("");
	}

	if (nControlId!=IDC_EDIT_FT)
	{
		LargeInt.HighPart=FileTime.dwHighDateTime;
		LargeInt.LowPart=FileTime.dwLowDateTime;
		if(LargeInt.QuadPart)
			m_FileTime.Format(_T("%I64u"),FileTime);
		else
			m_FileTime=_T("");
	}


	if(nControlId!=IDC_EDIT_FT_HIGH_LOCAL)
	{
		if (FileTimeLocal.dwHighDateTime&&!bStandardTimeNull)
			m_FileTimeHighLocal.Format(_T("%lu"),FileTimeLocal.dwHighDateTime);
		else
			m_FileTimeHighLocal=_T("");
	}

	if(nControlId!=IDC_EDIT_FT_LOW_LOCAL)
	{
		if(FileTimeLocal.dwLowDateTime&&!bStandardTimeNull)
			m_FileTimeLowLocal.Format(_T("%lu"),FileTimeLocal.dwLowDateTime);
		else
			m_FileTimeLowLocal=_T("");
	}

	if (nControlId!=IDC_EDIT_FT_LOCAL)
	{
		LargeIntLocal.HighPart=FileTimeLocal.dwHighDateTime;
		LargeIntLocal.LowPart=FileTimeLocal.dwLowDateTime;
		if(LargeIntLocal.QuadPart&&!bStandardTimeNull)
			m_FileTimeLocal.Format(_T("%I64u"),FileTimeLocal);
		else
			m_FileTimeLocal=_T("");
	}

	UpdateControlData();
}
void CTimeDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	//�����ڴ�
	//delete this;
}

void CTimeDlg::OnOK()
{
	//return FALSE;
}

void CTimeDlg::OnCancel()
{
	//return FALSE;
}

void CTimeDlg::OnBnClickedCheckGetSystemTime()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CButton* pCheckBox = (CButton*)GetDlgItem(IDC_CHECK_GET_SYSTEM_TIME);
	BOOL bEnable = pCheckBox->GetCheck();
	if (bEnable == TRUE)
		SetTimer(ALWAYS_GET_SYSTEM_TIME,1000,NULL);
	else
		KillTimer(ALWAYS_GET_SYSTEM_TIME);

	EnableAllWindow(bEnable == FALSE);
}

void CTimeDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
		case ALWAYS_GET_SYSTEM_TIME:
				OnBnClickedBtnGettime();
			break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CTimeDlg::EnableAllWindow(BOOL bEnable)
{
	UINT uCtrlID[] = {IDC_EDIT_CSTYLE_TIME,IDC_EDIT_CSTYLE_TIME_LOCLAL,IDC_EDIT_SYSTEM_TIME,
								IDC_EDIT_SYSTEM_TIME_LOCAL,IDC_EDIT_FT_HIGH,IDC_EDIT_FT_LOW,IDC_EDIT_FT,
								IDC_EDIT_FT_HIGH_LOCAL,IDC_EDIT_FT_LOW_LOCAL,IDC_EDIT_FT_LOCAL,IDC_BTN_GETTIME};
	for (int i=0;i<_countof(uCtrlID);++i)
		GetDlgItem(uCtrlID[i])->EnableWindow(bEnable);
}
