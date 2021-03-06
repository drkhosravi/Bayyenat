// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MessageDlg.h"
#include "afxdialogex.h"


// CMessageDlg dialog

IMPLEMENT_DYNAMIC(CMessageDlg, CDialogEx)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMessageDlg::IDD, pParent)
{

}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_OK, &CMessageDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMessageDlg message handlers


void CMessageDlg::OnBnClickedOk()
{
	OnOK();
}


BOOL CMessageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SYSTEMTIME t;
	GetLocalTime(&t);

	if( (t.wYear >= 2015) || ( t.wYear >= 2014 && t.wMonth >= 7 ) )
	SetDlgItemText(IDC_MSG, L"بسم الله الرحمن الرحیم\n\
با عرض سلام و احترام، اعتبار نسخه‏ی جاری برنامه تمام شده است. لطفا نسخه جدید را با کلیک روی پیوند پایین صفحه دانلود کنید\
\n\rاین محدودیت به این منظور قرار داده شده است که نقاط ضعف و قوت برنامه مشخص شود.\
 انشاءالله در نسخه های آتی چنین محدودیتی نخواهد بود. لطفا اگر نظر یا پیشنهادی برای بهبود نرم افزار دارید یا مشکلی در نرم افزار مشاهده کرده اید\
 در سایت وارد بفرمایید. التماس دعا");
	else
	SetDlgItemText(IDC_MSG, L"بسم الله الرحمن الرحیم\n\
با عرض سلام و احترام، اعتبار نسخه‏ی جاری برنامه کمتر از یک ماه دیگر تمام می شود لطفا نسخه جدید را با کلیک روی پیوند پایین صفحه دانلود کنید\
\n\rاین محدودیت تنها برای نسخه های  قبل از ٣ قرار داده شده است تا نقاط ضعف و قوت برنامه مشخص شود.\
 انشاءالله در نسخه های آتی چنین محدودیتی نخواهد بود. لطفا اگر نظر یا پیشنهادی برای بهبود نرم افزار دارید یا مشکلی در نرم افزار مشاهده کرده اید\
 در سایت وارد بفرمایید. التماس دعا");


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CMessageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	SYSTEMTIME t;
	GetLocalTime(&t);	
	
	if(t.wYear >= 2013 && t.wMonth >= 5 && t.wDay >= 21)
		pDC->SetTextColor(RGB(127,0,0));
	else
		pDC->SetTextColor(RGB(0,0,127));

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
