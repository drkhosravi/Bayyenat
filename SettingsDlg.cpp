// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"
#include "BayyenatDlg.h"
#include "Settings.h"

extern CBayyenatDlg* pHadithDlg;

IMPLEMENT_DYNAMIC(CSettings, CDialogEx)

	CSettings::CSettings(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettings::IDD, pParent)
{
	pHadithDlg = (CBayyenatDlg*) pParent;
	m_pToolTip = NULL;
	for(int i = 0; i < 5; i++){
		wcscpy(m_filter_trans[i], L"");
		wcscpy(m_filter_author[i], L"");
	}

}

CSettings::~CSettings()
{
}

void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_FILTER_TRANS1, m_filter_trans[0], 40);
	DDX_Text(pDX, IDC_FILTER_TRANS2, m_filter_trans[1], 40);
	DDX_Text(pDX, IDC_FILTER_TRANS3, m_filter_trans[2], 40);
	DDX_Text(pDX, IDC_FILTER_TRANS4, m_filter_trans[3], 40);
	DDX_Text(pDX, IDC_FILTER_TRANS5, m_filter_trans[4], 40);
	DDX_Text(pDX, IDC_FILTER_AUTHOR1, m_filter_author[0], 40);
	DDX_Text(pDX, IDC_FILTER_AUTHOR2, m_filter_author[1], 40);
	DDX_Text(pDX, IDC_FILTER_AUTHOR3, m_filter_author[2], 40);

	DDX_Radio(pDX, IDC_RADIO1, on_logon);
	DDX_Control(pDX, IDC_CMB_MODIFIER, cmb_modifier);
}


BEGIN_MESSAGE_MAP(CSettings, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CSettings::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_CHK_ENABLE_HOT_KEY, &CSettings::OnBnClickedChkEnableHotKey)
	ON_BN_CLICKED(IDC_CHK_RANDOM_HADITH, &CSettings::OnBnClickedChkRandomHadith)
	ON_BN_CLICKED(IDC_CHK_RANDOM_IMAGE, &CSettings::OnBnClickedChkRandomImage)
	ON_BN_CLICKED(IDC_CHK_FIXED_HADITH, &CSettings::OnBnClickedChkFixedHadith)
	ON_BN_CLICKED(IDC_CHK_FIXED_IMG, &CSettings::OnBnClickedChkFixedImg)
END_MESSAGE_MAP()


// CSettings message handlers


BOOL CSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)GetDlgItem(IDC_CHK_RANDOM_HADITH))->SetCheck(random_hadith);

	((CButton*)GetDlgItem(IDC_CHK_FIXED_IMG))->SetCheck(fixed_image);

	((CButton*)GetDlgItem(IDC_CHK_FIXED_HADITH))->SetCheck(fixed_hadith);

	((CButton*)GetDlgItem(IDC_CHK_RANDOM_IMAGE))->SetCheck(random_wp);

	((CButton*)GetDlgItem(IDC_CHK_STRETCH))->SetCheck(stretch_wp);

	((CButton*)GetDlgItem(IDC_CHK_POPUP))->SetCheck(show_popup);

	SetDlgItemInt(IDC_EDT_RIGHT_MARGIN, right_margin);

	SetDlgItemInt(IDC_EDT_LEFT_MARGIN, left_margin);

	SetDlgItemInt(IDC_EDT_TOP_MARGIN, top_margin);

	wchar_t temp[10] = L"00:00:00";//resolve the problem with : (GetDlgItemText does not return :'s but in SetDlgItemText it must have :'s !)
	temp[0] = slide_show_interval[0]; temp[1] = slide_show_interval[1];
	temp[3] = slide_show_interval[2]; temp[4] = slide_show_interval[3];
	temp[6] = slide_show_interval[4]; temp[7] = slide_show_interval[5];
	SetDlgItemText(IDC_INTERVAL, temp);

	//static CFont fnt;	
	if(!m_pToolTip)
	{	
		//fnt.CreateFont(24,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
			//CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("B Mitra"));
		m_pToolTip = new CToolTipCtrl();
	}
	if(!m_pToolTip->m_hWnd)
	{
		DWORD flags = WS_EX_NOINHERITLAYOUT;
		if (active_lang == langFA || active_lang == langAR)
			flags = WS_EX_RTLREADING | WS_EX_LAYOUTRTL;
		if(!m_pToolTip->CreateEx(this, 0, flags))
		{
			TRACE("Unable To create ToolTip\n");
			return TRUE;
		}
	}
	//m_pToolTip->SetFont(&fnt);
	m_pToolTip->SetTipTextColor(RGB(200,0, 0));

	AddToolTip(GetDlgItem(IDC_CHK_FIXED_IMG), IDS_FIXED_IMG);//  _T("تصویر ثابت/حدیث متغیر: با انتخاب این گزینه، با تغییر حدیث، تصویر زمینه تغییر نخواهد کرد"));
	AddToolTip(GetDlgItem(IDC_CHK_FIXED_HADITH), IDS_FIXED_HADITH);// _T("حدیث ثابت/تصویر متغیر: با انتخاب این گزینه، حدیث تغییر نخواهد کرد ولی تصویر زمینه تغییر می کند"));
	AddToolTip(GetDlgItem(IDC_CHK_RANDOM_HADITH), IDS_RAND_HADITH);// _T("انتخاب حدیث به صورت تصادفی"));
	AddToolTip(GetDlgItem(IDC_CHK_RANDOM_IMAGE), IDS_RAND_IMG);//_T("انتخاب تصویر زمینه دسکتاپ به صورت تصادفی از میان عکسهای موجود در پوشه تصاویر زمینه"));
	AddToolTip(GetDlgItem(IDC_CHK_STRETCH), IDS_STRETCH);// _T("با انتخاب این گزینه، تصویر کاغذ دیواری به ابعاد صفحه دسکتاپ تغییر اندازه می دهد"));
	AddToolTip(GetDlgItem(IDC_CHK_POPUP), IDS_NOTIFY);// _T("با انتخاب این گزینه، هرگاه حدیث عوض شود، در قالب جعبه‏ی پیام کوچکی در گوشه دسکتاپ (پایین-راست) نشان داده می شود"));
	AddToolTip(GetDlgItem(IDC_INTERVAL), IDS_INTERVAL);// _T("تعیین بازه زمانی که پس از آن حدیث عوض خواهد شد"));

	AddToolTip(GetDlgItem(IDC_EDT_LEFT_MARGIN), IDS_LEFT_MARGIN);// _T("عددی بین 0 و 100 بیانگر حاشیه از سمت چپ دسکتاپ (به درصد)"));
	AddToolTip(GetDlgItem(IDC_EDT_RIGHT_MARGIN), IDS_RIGHT_MARGIN);// _T("عددی بین 0 و 100 بیانگر حاشیه از سمت راست دسکتاپ (به درصد)"));
	AddToolTip(GetDlgItem(IDC_EDT_TOP_MARGIN), IDS_TOP_MARGIN);// _T("عددی بین 0 و 100 بیانگر حاشیه از بالای دسکتاپ (به درصد)"));


	AddToolTip(GetDlgItem(IDC_FILTER_TRANS1), IDS_FILTER_TRANS);// _T("با گزینه های این قسمت می توانید احادیث را فیلتر کنید، طوریکه تنها احادیث با مشخصات خواسته شده، نشان داده شود. مثلا برای دیدن احادیث شامل کلمه نماز یا قرآن در اولین خانه بنویسید نماز و در دومی بنویسید قرآن "));
	AddToolTip(GetDlgItem(IDC_FILTER_AUTHOR1), IDS_FILTER_AUTHOR);// _T("با گزینه های این قسمت می توانید احادیث را فیلتر کنید، طوریکه تنها احادیث نقل شده از افراد خاص، نشان داده شود. مثلا برای دیدن احادیث امام حسین ع در اولین خانه بنویسید حسین "));

	AddToolTip(GetDlgItem(IDC_RADIO1), IDS_START_BY_WINDOWS);// _T("شروع به کار برنامه هنگام شروع ویندوز (پیش فرض)"));
	AddToolTip(GetDlgItem(IDC_RADIO2), IDS_CHANGE_WP_AND_EXIT);// _T("تغییر تصویر پس زمینه ویندوز و خروج"));
	AddToolTip(GetDlgItem(IDC_RADIO3), IDS_DONT_START_AT_BOOT);// _T("عدم شروع به کار برنامه هنگام شروع ویندوز!"));

	m_pToolTip->SetMaxTipWidth(240);

	m_pToolTip->Activate(TRUE);
	
	((CButton*)GetDlgItem(IDC_CHK_ENABLE_HOT_KEY))->SetCheck(hotkey_enable);
	GetDlgItem(IDC_CMB_MODIFIER)->EnableWindow(hotkey_enable);
	GetDlgItem(IDC_EDT_VK)->EnableWindow(hotkey_enable);

	cmb_modifier.SetCurSel(hotkey_modifier);
	char str[3] = "W";
	str[0] = hotkey_VK;
	SetDlgItemTextA(m_hWnd, IDC_EDT_VK, str);
	return TRUE;  
}


BOOL CSettings::PreTranslateMessage(MSG* pMsg)
{
	if (NULL != m_pToolTip)
		m_pToolTip->RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSettings::OnBnClickedOk()
{
	OnBnClickedBtnApply();
	CDialogEx::OnOK();
}


void CSettings::OnBnClickedBtnApply()
{
	UpdateData();

	random_hadith = ((CButton*)GetDlgItem(IDC_CHK_RANDOM_HADITH))->GetCheck() > 0;

	fixed_image = ((CButton*)GetDlgItem(IDC_CHK_FIXED_IMG))->GetCheck() > 0;

	fixed_hadith = ((CButton*)GetDlgItem(IDC_CHK_FIXED_HADITH))->GetCheck() > 0;

	random_wp = ((CButton*)GetDlgItem(IDC_CHK_RANDOM_IMAGE))->GetCheck() > 0;

	stretch_wp = ((CButton*)GetDlgItem(IDC_CHK_STRETCH))->GetCheck() > 0;

	show_popup = ((CButton*)GetDlgItem(IDC_CHK_POPUP))->GetCheck() > 0;

	GetDlgItemText(IDC_INTERVAL, slide_show_interval);
	slide_show_interval.Remove(L':');

	right_margin = MIN(GetDlgItemInt(IDC_EDT_RIGHT_MARGIN), 50);

	left_margin = MIN(GetDlgItemInt(IDC_EDT_LEFT_MARGIN), 50);

	top_margin = MIN(GetDlgItemInt(IDC_EDT_TOP_MARGIN), 50);
		
	CString str;
	GetDlgItemText(IDC_EDT_VK, str);
	hotkey_VK = (char)str[0];

	hotkey_modifier = cmb_modifier.GetCurSel();

	pHadithDlg->SetNewHotKey();

	if(str_cur_image_path.length() < 2)
		pHadithDlg->OnChangeWP();
	else
		pHadithDlg->GenerateHadithWp(str_cur_image_path);

	SaveSettings();
}


void CSettings::OnBnClickedChkEnableHotKey()
{
	hotkey_enable = ((CButton*)GetDlgItem(IDC_CHK_ENABLE_HOT_KEY))->GetCheck() > 0;	
	GetDlgItem(IDC_CMB_MODIFIER)->EnableWindow(hotkey_enable);
	GetDlgItem(IDC_EDT_VK)->EnableWindow(hotkey_enable);
}

void CSettings::OnBnClickedChkFixedImg()
{
	fixed_image = ((CButton*)GetDlgItem(IDC_CHK_FIXED_IMG))->GetCheck() > 0;
	if(fixed_image)
		((CButton*)GetDlgItem(IDC_CHK_RANDOM_IMAGE))->SetCheck(0);
}


void CSettings::OnBnClickedChkFixedHadith()
{
	fixed_hadith = ((CButton*)GetDlgItem(IDC_CHK_FIXED_HADITH))->GetCheck() > 0;	
	if(fixed_hadith)
		((CButton*)GetDlgItem(IDC_CHK_RANDOM_HADITH))->SetCheck(0);
}


void CSettings::OnBnClickedChkRandomHadith()
{
	random_hadith = ((CButton*)GetDlgItem(IDC_CHK_RANDOM_HADITH))->GetCheck() > 0;
	if(random_hadith)
		((CButton*)GetDlgItem(IDC_CHK_FIXED_HADITH))->SetCheck(0);
}


void CSettings::OnBnClickedChkRandomImage()
{
	random_wp = ((CButton*)GetDlgItem(IDC_CHK_RANDOM_IMAGE))->GetCheck() > 0;
	if(random_wp)
		((CButton*)GetDlgItem(IDC_CHK_FIXED_IMG))->SetCheck(0);
}
