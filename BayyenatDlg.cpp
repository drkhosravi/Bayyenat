
// Hadith-To-WallpaperDlg.cpp : implementation file
//
#include "stdafx.h"
#include "Bayyenat.h"
#include "BayyenatDlg.h"
#include "afxdialogex.h"
#include <cmath>
#include "Splash.h"
#include "SettingsDlg.h"
#include <WinError.h>
#include "BUtils.h"
//#include "TextDesigner/MaskColor.h"
//#include "TextDesigner/CanvasHelper.h"
//#include "TextDesigner/DrawGradient.h"

#pragma comment (lib,"gdiplus.lib")

//#ifdef _DEBUG
//#pragma comment(lib, "Debug64\\TextDesigner.lib")
//#else
//#pragma comment(lib, "Release64\\TextDesigner.lib")
//#endif

#include "afxwin.h"
#include "Settings.h"

#define	WM_ICON_NOTIFY			WM_USER+10


#ifdef _DEBUG
#pragma comment (lib, "opencv_world490d.lib")
#else
#pragma comment (lib, "opencv_world490.lib")
#endif

extern HWND g_hWnd;
extern CSplashWnd splash;

#define IDT_CHANGE_WP 110 
#define IDT_GO_TO_TRAY 111 


CSettings *pSettingsDlg = NULL;
CBayyenatDlg* pHadithDlg = NULL;

//wchar_t* str_gradients[gtCount] = {L"رنگ ثابت",  L"عمودی 1", L"عمودی 2", L"افقی 1", L"افقی 2", L"قاب 1", L"قاب 2", L"زنگوله 1", L"زنگوله 2", L"بیضی", L"هیچی!"};

int grad_color_idx = -1;

wchar_t str_cur_hadith[2000] = {0};
wchar_t str_cur_translation[2000] = {0};
wchar_t str_cur_ref[1000] = {0};
wchar_t str_cur_author[1000] = {0};

wchar_t str_must_contain[5][100] = {0};

std::wstring	str_cur_image_name = L"";
std::wstring	str_save_ext	   = L"png";
GUID			save_format = Gdiplus::ImageFormatPNG;

EncoderParameters encoderParameters;
int sz_hadith_file = 0;
int nHadith = -1;
std::vector<fpos_t> hadith_pos;//position of each hadith in the file
HANDLE hFind_Image = 0;
int nImages = 0;//number of images in the wp directory
WIN32_FIND_DATA ffd_jpg, ffd_txt;
int desktopW = 1280;
int desktopH = 1024;

bool Win7OrLess = false; //Windows 7 Version number ==> 6.1
//return a random float number between 0 and 1
float frand()
{
	float result = (float)rand()/(float)RAND_MAX;
	return result;
}

Gdiplus::Color AutoColor(cv::Scalar m, int mode = 1)
{

	int r = m[2], g = m[1], b = m[0];
	
	/*double luminance = (0.299 * r + 0.587 * g + 0.114 * b) / 255;
	if (luminance > 0.5) // bright colors - black font
	{
		r = frand() * 50;
		g = frand() * 50;
		b = frand() * 50;
	}
	else// dark colors - white font
	{
		r = 255 - frand() * 50;
		g = 255 - frand() * 50;
		b = 255 - frand() * 50;
	}
	return Color(r, g, b);*/
	if (r > 170 || r < 80)
	{
		r = (int)(255 - r);
	}
	else
		r = (int)((r > 127 ? 40 : 210)+(frand() - 0.5)*80);

	////////////////////////////////////////////////////////
	if(g > 170 || g < 80) 
		g = (int)(255-g);
	else
		g = (int)((g > 127 ? 40 : 210)+(frand() - 0.5)*80);
	
	////////////////////////////////////////////////////////
	if(b > 170 || b < 80) 
		b = (int)(255-b);
	else
		b = (int)((b > 127 ? 40 : 210)+(frand() - 0.5)*80);

	if (r < 0) r = 0; else if (r > 255) r = 255;
	if (g < 0) g = 0; else if (g > 255) g = 255;
	if (b < 0) b = 0; else if (b > 255) b = 255;

	//try for better contrast
	int bigs = (r > 127) + (g > 127) + (b > 127); //number of channels with bright values ( > 127 )

	if (bigs > 1) {
		if (r > g && r > b)
			r = 255;
		else if (g > r && g > b)
			g = 255;
		else
			b = 255;
	}
	else
	{
		if (r < g && r < b)
			r = 0;
		else if (g < r && g < b)
			g = 0;
		else
			b = 0;
	}

	return Color(r, g, b);
}

Gdiplus::Color AutoShadow(cv::Scalar m)
{
	int r = (int)((m[2] > 127 ? 255 : 50) + (frand() - 0.5) * 50);
	int g = (int)((m[1] > 127 ? 255 : 50) + (frand() - 0.5) * 50);
	int b = (int)((m[0] > 127 ? 0 : 0   ) + (frand() - 0.5) * 50);
	if(r < 0) r = 0; else if (r > 255) r = 255;
	if(g < 0) g = 0; else if (g > 255) g = 255;
	if(b < 0) b = 0; else if (b > 255) b = 255;
	Color result(r,g,b);
	return result;
}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

//////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
	       
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// CBayyenatDlg dialog
CBayyenatDlg::CBayyenatDlg(CWnd* pParent /*=NULL*/, bool load_default_settings)
	: CDialogEx(CBayyenatDlg::IDD, pParent)
	, m_strWallpapers(_T("")), m_fp_hadith(0), load_default_settings(load_default_settings)
{
	pHadithDlg = this;
	srand((unsigned int)time(0));
	m_pToolTip = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	// Save the image as a JPEG with quality level 100.
	_jpeg_quality = 100;
	encoderParameters.Parameter[0].Value = &_jpeg_quality;

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	Win7OrLess =
		((osvi.dwMajorVersion < 6) ||
		((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion <= 1)));

	if (Win7OrLess) {
		save_format = Gdiplus::ImageFormatBMP;
		str_save_ext = L"bmp";
	}
	else {
		save_format = Gdiplus::ImageFormatPNG;
		str_save_ext = L"png";
	}

}

void CBayyenatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_WP_PATH, m_strWallpapers);
	DDX_Control(pDX, IDC_COLOR_HADITH, m_clrHadith);
	DDX_Control(pDX, IDC_COLOR_TRANS, m_clrTrans);
	DDX_Control(pDX, IDC_COLOR_SHADOW, m_clrShadow);
	DDX_Control(pDX, IDC_COLOR_SHADOW_TRANS, m_clrShadowTrans);
	DDX_Control(pDX, IDC_COLOR_GRADIENT, m_clrGrad);
	DDX_Control(pDX, IDC_CMB_FN_HADITH, fnt_cmb_hadith);
	DDX_Control(pDX, IDC_CMB_FN_TRANS, fnt_cmb_trans);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_CMB_GRAD, m_cmbGrad);
	DDX_Control(pDX, IDC_CMB_LNG, m_cmbLng);
	DDX_Control(pDX, IDC_CMB_HADITH_FILES, m_cmb_hadith_files);
	DDX_Control(pDX, IDC_SLIDER_OPACITY, m_slider);
	DDX_Control(pDX, IDC_COLOR_SHADOW_GLOW, m_clrGlowTrans);
	DDX_Control(pDX, IDC_COLOR_GLOW, m_clrGlow);
	DDX_Control(pDX, IDC_CHK_SHADOW_TRANS_AUTO_COLOR, chk_trans_shadow_auto_color);
	DDX_Control(pDX, IDC_CHK_HADITH_AUTO_COLOR, chk_auto_color);
	DDX_Control(pDX, IDC_CHK_GLOW_AUTO_COLOR, chk_glow_auto_color);
	DDX_Control(pDX, IDC_CHK_SHADOW_AUTO_COLOR, chk_shadow_auto_color);
	DDX_Control(pDX, IDC_CHK_ENABLE_GLOW_TRANS, chk_trans_enable_glow);
	DDX_Control(pDX, IDC_CHK_ENABLE_SHADOW_TRANS, chk_trans_enable_shadow);
	DDX_Control(pDX, IDC_CHK_ENABLE_SHADOW, chk_enable_shadow);
	DDX_Control(pDX, IDC_CHK_ENABLE_GLOW, chk_enable_glow);
	DDX_Control(pDX, IDC_CHK_TRANS_AUTO_COLOR, chk_trans_auto_color);
	DDX_Control(pDX, IDC_CHK_GLOW_TRANS_AUTO_COLOR, chk_trans_glow_auto_color);
}

BEGIN_MESSAGE_MAP(CBayyenatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CBayyenatDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_GO_TO_TRAY, &CBayyenatDlg::OnBnClickedGoToTray)
	ON_BN_CLICKED(IDC_CHK_HADITH_AUTO_COLOR, &CBayyenatDlg::OnBnClickedHadithAutoColor)
	ON_BN_CLICKED(IDC_CHK_TRANS_AUTO_COLOR, &CBayyenatDlg::OnBnClickedTransAutoColor)
	//ON_WM_NCHITTEST()
//	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_TEXT_PROCESSING, &CBayyenatDlg::OnBnClickedTextProcessing)
	ON_BN_CLICKED(IDC_CHK_ENABLE_SHADOW, &CBayyenatDlg::OnBnClickedChkEnableShadow)
	ON_BN_CLICKED(IDC_CHK_SHADOW_AUTO_COLOR, &CBayyenatDlg::OnBnClickedChkEnableShadow)
	ON_BN_CLICKED(IDC_CHK_ENABLE_SHADOW_TRANS, &CBayyenatDlg::OnBnClickedChkEnableShadowTrans)
	ON_BN_CLICKED(IDC_CHK_SHADOW_TRANS_AUTO_COLOR, &CBayyenatDlg::OnBnClickedChkEnableShadowTrans)

	ON_BN_CLICKED(IDC_CHK_SIZE_HADITH_AUTO, &CBayyenatDlg::OnBnClickedChkSizeHadithAuto)
	ON_BN_CLICKED(IDC_CHK_SIZE_TRANS_AUTO, &CBayyenatDlg::OnBnClickedChkSizeTransAuto)
	
	ON_WM_TIMER()
	ON_COMMAND(ID_POPUP_SHOWMAINWINDOW, &CBayyenatDlg::OnPopupShowMainWindow)
	ON_BN_CLICKED(IDC_CLOSE, &CBayyenatDlg::OnBnClickedClose)
	ON_COMMAND(ID_POPUP_CHANGE, &CBayyenatDlg::OnChangeWP)
	ON_EN_CHANGE(IDC_EDT_FN_SIZE_HADITH, &CBayyenatDlg::OnEnChangeEdtFnSizeHadith)
	ON_EN_CHANGE(IDC_EDT_FN_SIZE_TRANS, &CBayyenatDlg::OnEnChangeEdtFnSizeTrans)
	ON_COMMAND(ID_POPUP_START, &CBayyenatDlg::OnPopupStart)
	ON_BN_CLICKED(IDC_CHANGE, &CBayyenatDlg::OnChangeWP)
	ON_BN_CLICKED(IDC_TRAY, &CBayyenatDlg::OnBnClickedTray)
	ON_BN_CLICKED(IDC_MINIMIZE, &CBayyenatDlg::OnBnClickedMinimize)
	ON_BN_CLICKED(IDC_ABOUT, &CBayyenatDlg::OnBnClickedAbout)
	ON_BN_CLICKED(IDC_RESET_DEFAULTS, &CBayyenatDlg::OnBnClickedResetDefaults)
//	ON_WM_SETFOCUS()
//	ON_WM_ACTIVATE()
//	ON_WM_NCPAINT()
//ON_WM_SHOWWINDOW()
ON_WM_DRAWITEM()
ON_EN_CHANGE(IDC_HADITH_FILE, &CBayyenatDlg::OnEnChangeHadithFile)
//ON_BN_CLICKED(IDC_CHK_RANDOM_HADITH, &CBayyenatDlg::OnBnClickedChkRandomHadith)
ON_BN_CLICKED(IDC_SAVE_WP, &CBayyenatDlg::OnBnClickedSaveWp)
//ON_BN_CLICKED(IDC_CHK_FIXED_HADITH, &CBayyenatDlg::OnBnClickedChkFixedHadith)
//ON_BN_CLICKED(IDC_CHK_FIXED_IMG, &CBayyenatDlg::OnBnClickedChkFixedImg)
ON_BN_CLICKED(IDC_BTN_SETTINGS, &CBayyenatDlg::OnBnClickedBtnSettings)
ON_BN_CLICKED(IDC_BTN_APPLY_STYLE, &CBayyenatDlg::OnBnClickedBtnApplyStyle)
ON_WM_HOTKEY()
ON_BN_CLICKED(IDC_CHK_SOLID_COLOR, &CBayyenatDlg::OnBnClickedChkSolidColor)
ON_WM_SIZE()
ON_CBN_SELCHANGE(IDC_CMB_LNG, &CBayyenatDlg::OnCbnSelchangeCmbLng)
ON_EN_CHANGE(IDC_WP_PATH, &CBayyenatDlg::OnEnChangeWpPath)
ON_BN_CLICKED(IDC_COPY_TEXT, &CBayyenatDlg::OnBnClickedCopyText)
ON_BN_CLICKED(IDC_COPY_IMAGE, &CBayyenatDlg::OnBnClickedCopyImage)
ON_CBN_SELCHANGE(IDC_CMB_HADITH_FILES, &CBayyenatDlg::OnCbnSelchangeCmbHadithFiles)

ON_BN_CLICKED(IDC_CHK_ENABLE_GLOW_TRANS, &CBayyenatDlg::OnBnClickedChkEnableGlowTrans)
ON_BN_CLICKED(IDC_CHK_GLOW_TRANS_AUTO_COLOR, &CBayyenatDlg::OnBnClickedChkEnableGlowTrans)

ON_BN_CLICKED(IDC_CHK_ENABLE_GLOW, &CBayyenatDlg::OnBnClickedChkEnableGlow)
ON_BN_CLICKED(IDC_CHK_GLOW_AUTO_COLOR, &CBayyenatDlg::OnBnClickedChkEnableGlow)
END_MESSAGE_MAP()


// CBayyenatDlg message handlers
BOOL CBayyenatDlg::OnInitDialog()
{
	std::wstring cmd = GetCommandLine();
	wchar_t* arg = wcsrchr((wchar_t*)cmd.c_str(), L' ');

	size_t idx = cmd.rfind('\\');
	std::wstring s = cmd.substr(0, idx);
	CDialogEx::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CString str;
	str.LoadString(IDS_INACTIVE);
	// Create the tray icon
	if (m_TrayIcon.Create(NULL,                            // Parent window
		WM_ICON_NOTIFY,                  // Icon notify message to use
		str,  // tooltip
		m_hIcon/*::LoadIcon(NULL, IDI_ASTERISK)*/,  // Icon to use
		IDR_POPUP_MENU))                 // ID of tray icon
	{
		m_TrayIcon.SetMenuDefaultItem(0, TRUE);
		bmpStart.LoadBitmap(IDB_START);
		bmpStop.LoadBitmap(IDB_STOP);
		bmpMain.LoadBitmap(IDB_MAIN_WND);
		bmpClose.LoadBitmap(IDB_CLOSE);
		bmpChange.LoadBitmap(IDB_CHANGE);
		m_TrayIcon.m_menu.SetMenuItemBitmaps(ID_POPUP_START, MF_BYCOMMAND, &bmpStart, &bmpStop);
		m_TrayIcon.m_menu.SetMenuItemBitmaps(ID_POPUP_SHOWMAINWINDOW, MF_BYCOMMAND, &bmpMain, 0);
		m_TrayIcon.m_menu.SetMenuItemBitmaps(IDC_CLOSE, MF_BYCOMMAND, &bmpClose, 0);
		m_TrayIcon.m_menu.SetMenuItemBitmaps(ID_POPUP_CHANGE, MF_BYCOMMAND, &bmpChange, 0);
	}	
	else
	{
		MessageBox(L"Failed to create Tray Icon");
	}

	UpdateFontList();

	ActivateToolTips();


	str.LoadString(IDS_VERSION);
	SetWindowText(str);
	SetBackgroundColor(RGB(240,240,220));
	
	if(splash.m_hWnd)
		splash.DestroyWindow();

	pSettingsDlg = new CSettings(this);
	if(wcsstr(arg, L"tray"))
	{
		theApp.windows_start_up = true;
		ShowWindow(SW_HIDE);
		SetTimer(IDT_GO_TO_TRAY, 5000, 0);
	}

	SetNewHotKey();
	if (config.active_lang == langFA || config.active_lang == langAR)
		ModifyStyleEx(0, WS_EX_LAYOUTRTL);
	
	for (int i = 0; i < 4; i++)
		m_cmbLng.AddString(lang_names[i]);
	
	m_cmbLng.SetCurSel(config.active_lang);
	
	CRect rclient;
	::GetClientRect(::GetDesktopWindow(), &rclient);
	desktopW = rclient.Width();
	desktopH = rclient.Height();

	HDC screen = GetDC()->GetSafeHdc();

	int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
	int dpiY = GetDeviceCaps(screen, LOGPIXELSY);	

	//////////////////////////////////////////////////////////////////////////
	std::wstring s2 = ExePath;
	s2 += _T("\\Ahadith\\*.txt");
	HANDLE hfind = FindFirstFile(s2.c_str(), &ffd_txt);
	
	if (INVALID_HANDLE_VALUE != hfind)
	{
		do
		{
			wcslwr(config.str_hadith_path);
			
			std::wstring fn = ExePath + std::wstring(L"\\Ahadith\\") + ffd_txt.cFileName;
			FILE* f = nullptr;
			wchar_t str_sz[20] = { 0 };
			_wfopen_s(&f, fn.c_str(), L"rb");
			if (f)
			{
				fseek(f, 0, SEEK_END);
				long sz = ftell(f);
				fclose(f);
				wsprintf(str_sz, L" (%d KB)", sz / 1000);
			}
			m_cmb_hadith_files.AddString(ffd_txt.cFileName + CString(str_sz));

			if (wcsstr(config.str_hadith_path, wcslwr(ffd_txt.cFileName)))
				m_cmb_hadith_files.SetCurSel(m_cmb_hadith_files.GetCount()-1);

		} while (FindNextFile(hfind, &ffd_txt));
	}

	fmt_rtl = new StringFormat();
	
	m_slider.SetRange(0, 255);
	UpdateUI();

	if (load_default_settings)
		OnBnClickedResetDefaults();
	dlg_inited = true;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBayyenatDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CBayyenatDlg::DrawHeaderandFooter()
{
	if (config.active_lang == langFA || config.active_lang == langAR)
		ModifyStyleEx(WS_EX_LAYOUTRTL, 0); //نقاشی ها با راست به چپ مشکل دارند
	Gdiplus::Graphics graphics(/*GetDlgItem(IDC_HEADER)->*/m_hWnd);
	Gdiplus::Font *fnt, *fnt2;
	fmt_rtl->SetAlignment(StringAlignmentCenter);
	
	if (config.active_lang == langFA || config.active_lang == langAR)
	{
		fmt_rtl->SetFormatFlags(StringFormatFlagsDirectionRightToLeft);
		//0x0429 (1065) is the LCID of fa-IR http://www.microsoft.com/resources/msdn/goglobal/default.mspx
		fmt_rtl->SetDigitSubstitution(0x0429, StringDigitSubstituteNational);

		fnt = new Gdiplus::Font(_T("B Roya"), 16, 1);
		fnt2 = new Gdiplus::Font(_T("B Roya"), 12, 1);
	}
	else
	{
		fnt = new Gdiplus::Font(_T("B Roya"), 16, 1);
		fnt2 = new Gdiplus::Font(_T("Calibri"), 10, 1);
	}
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
	const int RUNNING_SZ = 8;
	std::wstring s[RUNNING_SZ] = {
		L"اللهمّ صَلِّ علی مُحمَّـدٍ وَ آلِ محّمدٍ وَ عجّـِل فَرَجهم", 
		L"یا رب فرج امـام ما را برسان", 
		L"بر چهره‏ی دلربای مهـدی صلوات", 
		L" إِنَّ اللَّهَ وَ مَلائِكَتَهُ یُصَلُّونَ عَلَى النَّبِیِّ یا أَیُّهَا الَّذِینَ آمَنُوا صَلُّوا عَلَیْهِ وَ سَلِّمُوا تَسْلِیما",
		L"این الطـالب بدم المقتـول بکربـلاء",
		L"لا حول و لا قوة الا بالله العلی العظیم",
		L"چو خدا بود پناهت چه خطر بود ز راهت    به فلک رسد کلاهت که سر همه سرانی",
		L"نیست نشان زندگی تا نرسد نشان تو!"
	};

	header_idx++;
	if (header_idx >= RUNNING_SZ)
		header_idx = 0;

	RECT rc1;
	/*GetDlgItem(IDC_HEADER)->*/GetClientRect(&rc1);
	RectF rc;
	rc.X = (float)rc1.left;
	rc.Y = (float)rc1.top;
	rc.Width = (float)(rc1.right - rc1.left);
	rc.Height = 60;
	graphics.SetRenderingOrigin(0, 0);
	LinearGradientBrush br(rc, Color(255, 150, 200, 150), Color(255, 240, 240, 220), Gdiplus::LinearGradientModeVertical);
	graphics.FillRectangle(&br, rc);	

	SolidBrush br2(Color(255, 0, 32, 96));
	rc.Y += 10;
	graphics.DrawString(s[header_idx].c_str(), s[header_idx].length(), fnt, rc, fmt_rtl, &br2);

	//////////////////////////////////////////////////////////////////
	//Draw Footer

	CString str, str2;
	str.LoadString(IDS_VERSION);
	str2.LoadString(IDS_DOA);
	std::wstring s2 = str.GetBuffer();
	if(str2.GetLength() > 1)
		s2 = str + L" - "  + str2;
	rc.X = (float)rc1.left;
	rc.Y = (float)(rc1.bottom - 56);
	rc.Width = (float)(rc1.right - rc1.left);
	rc.Height = 56;

	LinearGradientBrush br3(rc, Color(255, 240, 240, 220), Color(255, 150, 200, 150), Gdiplus::LinearGradientModeVertical);
	graphics.FillRectangle(&br3, rc);
	rc1.left += (rc1.right - rc1.left) / 3;//یک سوم سمت چپ برای دکمه ها خالی بماند
	rc.X = (float)rc1.left;

	rc.Y += 30;

	Gdiplus::StringFormat fmt_en;
	fmt_en.SetAlignment(StringAlignmentCenter);

	rc.Width = (float)0.333*(rc1.right - rc1.left);
	Gdiplus::Font fnt3(_T("Calibri"), 10, 1);
	if (config.solid_colors)
	{
		CString str_idx;
		if (GradColors.size() > 0 && grad_color_idx > -1)
		{
			int b = (int)GradColors[grad_color_idx * 2][0];
			int g = (int)GradColors[grad_color_idx * 2][1];
			int r = (int)GradColors[grad_color_idx * 2][2];
			int b2 = (int)GradColors[grad_color_idx * 2 + 1][0];
			int g2 = (int)GradColors[grad_color_idx * 2 + 1][1];
			int r2 = (int)GradColors[grad_color_idx * 2 + 1][2];
			str_idx.Format(L"(%d) (#%06X, #%06X)", grad_color_idx, (r << 16) + (g << 8) + b, (r2 << 16) + (g2 << 8) + b2);
			graphics.DrawString(str_idx, str_idx.GetLength(), &fnt3, rc, &fmt_en, &br2);
		}
	}
	else
		graphics.DrawString(str_cur_image_name.c_str(), str_cur_image_name.length(), &fnt3, rc, &fmt_en, &br2);	


	rc.Y -= 5;
	rc.X += rc.Width;
	rc.Width += 10;
	graphics.DrawString(s2.c_str(), s2.length(), fnt2, rc, fmt_rtl, &br2);


	rc.X += rc.Width;
	rc.Width -= 20;
	wchar_t str_counter[40] = {0};
	CString str_hadith, str_of;
	str_hadith.LoadString(IDS_HADITH);
	str_of.LoadString(IDS_OF);
	if(nHadith > 0)
		wsprintf(str_counter, L"%s %d %s %d", str_hadith, config.last_hadith+1, str_of, nHadith);
	else{
		str_hadith.LoadString(IDS_HADITH_NO);
		wsprintf(str_counter, L"%s %d", str_hadith, config.last_hadith+1);
	}
	SetDlgItemInt(IDC_EDT_HADITH_NUM, config.last_hadith+1);
	graphics.DrawString(str_counter, wcslen(str_counter), fnt2, rc, fmt_rtl, &br2);	
	
	if (config.active_lang == langFA || config.active_lang == langAR)
		ModifyStyleEx(0, WS_EX_LAYOUTRTL);

	delete fnt;
	delete fnt2;
}

void CBayyenatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		DrawHeaderandFooter();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBayyenatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void VerifyROI(cv::Rect& roi, cv::Mat& img_input)
{
	if (roi.x < 0)
		roi.x = 0;
	if (roi.y < 0)
		roi.y = 0;
	if (roi.width + roi.x >= img_input.cols)
		roi.width = img_input.cols - roi.x - 1;
	if (roi.height + roi.y >= img_input.rows)
		roi.height = img_input.rows - roi.y - 1;
}


//Compute Bounding Rect of Narration
//محاسبه مستطیل محیطی حدیث و ترجمه به منظور اعمال رنگ زمینه
Gdiplus::RectF CBayyenatDlg::ComputeBBox(Gdiplus::RectF rc, Graphics* graphics, int imH, int imW)
{
	float sz_ref = std::max(config.sz_trans * 0.4f, 7.0f);// (sz_trans > 11 || sz_trans < 1) ? 10.0f : 7.0f;
	float sz_author = sz_ref;//(sz_hadith > 11 || sz_hadith < 1) ? 10.0f : 7.0f;

	Gdiplus::Font font_author(_T("IRANSansXFaNum"), sz_author, 0);
	Gdiplus::Font font_ar(config.fName_ar, config.sz_hadith, config.font_style_hadith, UnitPixel);
	Gdiplus::Font font_fa(config.fName_trans, config.sz_trans, config.font_style_trans, UnitPixel);
	Gdiplus::Font font_ref(_T("IRANSansXFaNum"), sz_ref, 0);

	std::wstring hadith = str_cur_hadith;//_T("من اخلص لله أربعین یوما ظهرت ینابیع الحكمة من قلبه على لسانه ");
	std::wstring translation = str_cur_translation;//_T("كسی كه به مدت چهل روز اعمالش را براى خداوند تبارك و تعالى خالص ‍ گرداند، چشمه هاى حكمت از دل او بر زبانش نمایان مى شود.");
	std::wstring ref = str_cur_ref;//_T("کتاب نور علی نور ص 144");
	std::wstring author = str_cur_author;//_T("رسول خدا (صلی الله علیه و آله و سلم)");



	Gdiplus::RectF b = rc;
	Gdiplus::RectF rc1 = rc;
	graphics->MeasureString(author.c_str(), author.length(), &font_author, rc, fmt_rtl, &rc1);
	b.Height = rc1.Height;
	int maxW = (int)rc1.Width;
	if (author.length() > 1)
		b.Height += 10;
	if (config.sz_hadith > 0) {//0 means no Arabic text
		graphics->MeasureString(hadith.c_str(), hadith.length(), &font_ar, rc, fmt_rtl, &rc1);
		b.Height += rc1.Height;
		if (rc1.Width > maxW) 
			maxW = (int)rc1.Width;
		if (hadith.length() > 1)
			b.Height += 15;
	}
	if (config.sz_trans > 0 && config.active_lang != langAR) {//0 means no translation
		graphics->MeasureString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &rc1);
		b.Height += rc1.Height;
		if (rc1.Width > maxW) maxW = (int)rc1.Width;
		if (translation.length() > 1)
			b.Height += 15;
	}
	graphics->MeasureString(ref.c_str(), ref.length(), &font_ref, rc, fmt_rtl, &rc1);
	b.Height += rc1.Height;
	if (rc1.Width > maxW) maxW = (int)rc1.Width;
	if (ref.length() > 1)
		b.Height += 15;

	//Add some vertical margins
	if (b.Y > 20)
	{
		b.Y = MAX(0, b.Y - 20);
		b.Height = MIN(b.Height + 40, imH);
	}

	if (config.full_width_grad)
	{
		b.X = 0;
		b.Width = (float)imW;
	}
	//Add some horizontal margins
	else if (maxW + 20 < b.Width)
	{
		b.X += 0.5f * (b.Width - maxW) - 10;
		b.Width = (float)maxW + 20;
	}

	//Check final box w.r.t target image
	if (b.Y + b.Height >= imH)
		b.Height = imH - b.Y - 1;

	//if (b.X + b.Width >= imW) //Never occures?
		//b.Width = imW - b.X - 1;

	return b;
}

//Fill background of the narration
//این تابع، پس زمینه حدیث را با رنگ ثابت یا گرادیان پر می کند
//box: bounding box of the narraion
void CBayyenatDlg::FillBackground(Graphics* graphics, Gdiplus::RectF& box, cv::Mat& mat, CImage& img)
{
	config.opacity = m_slider.GetPos();
	Color Grad1(config.opacity, GetRValue(config.clr_gradient), GetGValue(config.clr_gradient), GetBValue(config.clr_gradient));// = (240,255,255,255);
																								  //Color Grad1(255,236,233,216);
																								  //Color Grad2(0,m[2],m[1],m[0]);
	Color Grad2(0, GetRValue(config.clr_gradient), GetGValue(config.clr_gradient), GetBValue(config.clr_gradient));// = (240,255,255,255);
	Brush* pBrush = NULL;
	int color_count = 1;

	switch (config.gradient_type)
	{
	case gtFixedColor:
		pBrush = new SolidBrush(Grad1);
		break;
	case gtFixedColorAuto:// @990116		 
		{
			cv::Rect roi((int)box.X, (int)box.Y, (int)box.Width, (int)box.Height);
			VerifyROI(roi, mat);
			cv::Mat sub = mat(roi);
			cv::Scalar m = cv::mean(sub); 
			pBrush = new SolidBrush(Color(config.opacity, m[2], m[1], m[0]));
		}
		break;
	case gtBlur:// @990116		 
		{
			cv::Rect roi((int)box.X, (int)box.Y, (int)box.Width, (int)box.Height);
			VerifyROI(roi, mat);
			cv::Mat sub = mat(roi);
			cv::GaussianBlur(sub, sub, cv::Size(23, 23), 13, 13);
			//add some color to have better effect
			double a = config.opacity / 255.0;
			cv::Scalar c(a * GetBValue(config.clr_gradient), a * GetGValue(config.clr_gradient), a * GetRValue(config.clr_gradient));
			cv::addWeighted(c, a, sub, 1-a, 0, sub);
			//sub = c * a + sub*(1-a);
			Mat2CImage(&mat, img);
		}
		break;
	case gtVertical1:
		pBrush = new LinearGradientBrush(box, Grad1, Grad2, Gdiplus::LinearGradientModeVertical);
		break;
	case gtVertical2:
		pBrush = new LinearGradientBrush(box, Grad2, Grad1, Gdiplus::LinearGradientModeVertical);
		break;
	case gtHorizon1:
		pBrush = new LinearGradientBrush(box, Grad1, Grad2, Gdiplus::LinearGradientModeHorizontal);
		break;
	case gtHorizon2:
		pBrush = new LinearGradientBrush(box, Grad2, Grad1, Gdiplus::LinearGradientModeHorizontal);
		break;
	case gtPath1:
	{
		GraphicsPath path;
		path.AddRectangle(box);
		PathGradientBrush* pb = new PathGradientBrush(&path);
		pBrush = pb;
		pb->SetCenterPoint(Point((int)(box.X + box.Width / 2), (int)(box.Y + box.Height / 4)));
		pb->SetCenterColor(Grad1);
		pb->SetFocusScales(0.7f, 0.1f);
		//pb->SetFocusScales(0.0f, 0.5f);
		Color colors[] = { Grad2 };
		pb->SetSurroundColors(colors, &color_count);
		break;
	}
	case gtPath2:
	{
		GraphicsPath path;
		path.AddRectangle(box);
		PathGradientBrush* pb = new PathGradientBrush(&path);
		pBrush = pb;
		pb->SetCenterPoint(Point((int)(box.X + box.Width / 2), (int)(box.Y + box.Height * 3 / 4)));
		pb->SetCenterColor(Grad1);
		pb->SetFocusScales(0.7f, 0.14f);
		//pb->SetFocusScales(0.0f, 0.5f);
		Color colors[] = { Grad2 };
		pb->SetSurroundColors(colors, &color_count);
		break;
	}
	case gtBellShape1:
		pBrush = new LinearGradientBrush(box, Grad2, Grad1, Gdiplus::LinearGradientModeVertical);
		((LinearGradientBrush*)pBrush)->SetBlendBellShape(0.5f, 1.0f);
		break;
	case gtBellShape2:
		pBrush = new LinearGradientBrush(box, Grad1, Grad2, Gdiplus::LinearGradientModeVertical);
		((LinearGradientBrush*)pBrush)->SetBlendBellShape(0.5f, 1.0f);
		break;
	case gtEllipse:
		GraphicsPath path;
		path.AddEllipse(box);
		PathGradientBrush* pb = new PathGradientBrush(&path);
		pBrush = pb;
		pb->SetCenterPoint(Point((int)(box.X + box.Width / 2), (int)(box.Y + box.Height / 2)));
		pb->SetCenterColor(Grad1);
		pb->SetFocusScales(0.5f, 0.5f);
		Color colors[] = { Grad2 };
		pb->SetSurroundColors(colors, &color_count);
		break;

	}
	if (pBrush) {
		graphics->FillRectangle(pBrush, box);
		delete pBrush;
	}
}

void CBayyenatDlg::GenerateHadithWp(std::wstring img_path)
{	
	///////////////////////////////////////////////////////////////////////////////////
	// Update user data and save them in registry
	ApplySettings();
	SaveSettings(); 
	///////////////////////////////////////////////////////////////////////////////////
	float sz_ref = std::max(config.sz_trans * 0.4f, 7.0f);// (sz_trans > 11 || sz_trans < 1) ? 10.0f : 7.0f;
	float sz_author = sz_ref;//(sz_hadith > 11 || sz_hadith < 1) ? 10.0f : 7.0f;

	Gdiplus::Font font_author(_T("IRANSansXFaNum"), sz_author, 0);
	Gdiplus::Font font_ar(config.fName_ar, config.sz_hadith, config.font_style_hadith, UnitPixel);
	Gdiplus::Font font_fa(config.fName_trans, config.sz_trans, config.font_style_trans, UnitPixel);
	Gdiplus::Font font_ref(_T("IRANSansXFaNum"), sz_ref, 0);
	
	std::wstring hadith = str_cur_hadith;
	std::wstring translation = str_cur_translation;
	std::wstring ref = str_cur_ref;
	std::wstring author = str_cur_author;

	if(translation.length() < 2 && hadith.length() < 2)
	{
		hadith = L"فَاقْرَ‌ءُوا مَا تَیَسَّرَ‌ مِنَ الْقُرْ‌آنِ";
		translation = L"پس آنچه برایتان میسر است قرآن بخوانید";
		author = L"قرآن کریم";
		ref = L"سوره مبارکه مزمل، آیه 20";
	}

	/////////////////////////////////////////////////////////////////////////////
	// This block is here only because opencv doesnt support unicode path!	

	cv::Mat mat;// , mat_black;
	wchar_t wp_nameW[MAX_PATH] = {0};

	if(config.solid_colors){
		mat = CreateGradient(desktopH, desktopW, GradColors[grad_color_idx*2], GradColors[grad_color_idx*2+1]);
		wsprintf(wp_nameW, L"%s\\wp-temp.png", str_local_pathW);
		WriteImage(mat, wp_nameW); // Background Only, without quotes
	}
	else
	{
		mat = ReadImage(img_path.c_str());
		std::wstring ext = img_path.substr(img_path.length()-3,3);
		wsprintf(wp_nameW, L"%s\\wp-temp.%s", str_local_pathW, ext.c_str());
		CopyFile(img_path.c_str(), wp_nameW, false);// Background Only, without quotes
	}

	//mat_black = cv::Mat::zeros(mat.size(), mat.type());
	
	if( (mat.cols != desktopW) || (mat.rows != desktopH)){
		if(config.stretch_wp)
			cv::resize( mat, mat, cv::Size( desktopW, desktopH ), 0, 0, cv::INTER_LINEAR);
		else
			cv::resize( mat, mat, cv::Size( desktopW, mat.rows * desktopW / mat.cols ), 0, 0, cv::INTER_LINEAR);		
	}
	/////////////////////////////////////////////////////////////////
	
	if(!img.IsNull())
		img.Destroy();
	int imW = mat.cols;
	int imH = mat.rows;

	Gdiplus::RectF rc((float)imW* config.left_margin/100, (float)imH* config.top_margin/100, (float)imW*(100- config.left_margin- config.right_margin)/100, (float)imH*(100- config.top_margin)/100);
	Gdiplus::RectF rcBk = rc;
	Graphics *graphics = NULL;

	Mat2CImage(&mat, img);
	graphics = new Graphics(img.GetDC());

	//graphics.SetSmoothingMode(SmoothingModeAntiAlias); //useful for lines and arcs
	graphics->SetTextRenderingHint(TextRenderingHintAntiAlias);
	//////////////////////////////////////////////////////////////////////////

	//Draw Gradients
	Gdiplus::RectF box = rc;
	if (config.gradient_type != gtNone)
	{
		box = ComputeBBox(rc, graphics, imH, imW);
		FillBackground(graphics, box, mat, img);
		if(config.gradient_type != gtBlur) //in the case of gtBlur, gradient applyes on mat and then it converts to img
			CImage2Mat(img, mat); //to better compute mean color of the background
	}
	/////////////////////////////////////////////////////////////////////////////////////////	
	SolidBrush br_hadith( CLR2Color(config.clr_hadith_text) );
	if(config.hadith_auto_color)
	{
		//Automatic Coloring based on mean of the background
		cv::Mat sub(mat, cv::Rect((int)box.X, (int)box.Y, (int)box.Width, (int)box.Height/2) );
		cv::Scalar m = cv::mean(sub);

		if(config.hadith_auto_color){
			Color clr = AutoColor(m);
			br_hadith.SetColor(clr);//Color(0, 0, 50));			
		}
	}
	//Draw author
	if(author.length() < 2){
		rc.Height = 0;
		goto DRAW_HADITH;
	}
	else{
		rc.Y += 10;
		rc.Height = (float) (imH*2/10);
	}
	graphics->MeasureString(author.c_str(), author.length(), &font_author, rc, fmt_rtl, &rc);
	graphics->DrawString(author.c_str(), author.length(), &font_author, rc, fmt_rtl, &br_hadith);

DRAW_HADITH:
	rc.X = rcBk.X; rc.Width = rcBk.Width; //return to the original width
	DrawHadith(hadith, rc, imH, mat, box, graphics, font_ar);

	/////////////////////////////////////////////////////////////////////////////////////////
	//Translation
DRAW_TRANS:
	rc.X = rcBk.X; rc.Width = rcBk.Width; //return to the original width
	DrawTrans(translation, rc, imH, mat, box, graphics, font_fa);

	/////////////////////////

DRAW_REF:
	Gdiplus::SolidBrush br_ref(CLR2Color(config.clr_trans_text));

	//Draw Reference
	rc.X = rcBk.X; rc.Width = rcBk.Width; //return to the original width
	rc.Y += rc.Height + 15;
	rc.Height = rc.Height = MAX(imH-rc.Y-20, 0);
	graphics->MeasureString(ref.c_str(), ref.length(), &font_ref, rc, fmt_rtl, &rc);
	graphics->DrawString(ref.c_str(), ref.length(), &font_ref, rc, fmt_rtl, &br_ref);

DRAW_LOGO:
	Gdiplus::RectF rc_logo((float)imW*0.8f, (float)imH-50.0f, (float)imW*0.2f, 50.0f);
	SolidBrush br_logo(Color(220, 200, 200));
	Gdiplus::Font font_bayyenat_logo(_T("Cambria"), 12, FontStyleBoldItalic);
	graphics->DrawString(L"BAYYENAT Wallpaper", 20, &font_bayyenat_logo, rc_logo, fmt_rtl, &br_logo);

	wchar_t new_wp_path[MAX_PATH] = {0};
	/**************************************************************************** 
	 * Output must be saved in PNG format because Windows 10 over compresses 
	 * other formats like jpg and its quality is awful
	 ****************************************************************************/	
	
	wsprintf(new_wp_path, _T("%s\\wp-hadith.%s"), str_local_pathW, str_save_ext.c_str());
	img.Save(new_wp_path, save_format);
	img.ReleaseDC();
	BOOL result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, new_wp_path, SPIF_UPDATEINIFILE);

	DrawHeaderandFooter();
	delete graphics;
}

void CBayyenatDlg::DrawHadith(std::wstring& hadith, Gdiplus::RectF& rc, int imH, cv::Mat& mat, Gdiplus::RectF& box, Gdiplus::Graphics* graphics, Gdiplus::Font& font_ar)
{
	SolidBrush br_hadith(CLR2Color(config.clr_hadith_text, 255));
	SolidBrush br_shadow(CLR2Color(config.clr_hadith_shadow, 255));
	Pen pen_glow(CLR2Color(config.clr_hadith_glow, 255), config.glow_size); 

	if (config.hadith_auto_color || config.hadith_shadow_auto_color)
	{
		//Automatic Coloring based on mean of the background
		cv::Mat sub(mat, cv::Rect((int)box.X, (int)box.Y, (int)box.Width, (int)box.Height / 2));
		cv::Scalar m = cv::mean(sub);

		if (config.hadith_auto_color) {
			Color clr = AutoColor(m);
			br_hadith.SetColor(clr);//Color(0, 0, 50));			
		}
		if (config.hadith_shadow_auto_color)
			br_shadow.SetColor(AutoShadow(m));
	}

	if (hadith.length() < 2 || config.sz_hadith <= 0) {
		return;
	}
	//Draw shadows of the Hadith
	rc.Y += rc.Height + 15;
	rc.Height = MAX(imH - rc.Y - 20, 0);
	int n = hadith.length();
	graphics->MeasureString(hadith.c_str(), hadith.length(), &font_ar, rc, fmt_rtl, &rc);

	//graphics->DrawString(hadith.c_str(), hadith.length(), &font_ar, rc, fmt_rtl, &br_hadith);
	//return;
	/*if (config.enable_shadow)
		graphics->DrawString(hadith.c_str(), hadith.length(), &font_ar, rc, fmt_rtl, &br_shadow);

	//Draw Hadith
	rc.X = MAX(rc.X - 1, 0);
	rc.Y = MAX(rc.Y - 1, 0);
	graphics->DrawString(hadith.c_str(), hadith.length(), &font_ar, rc, fmt_rtl, &br_hadith);*/
	FontFamily fontFamily;
	font_ar.GetFamily(&fontFamily);
	DrawTextWithEffects(*graphics, hadith.c_str(), fontFamily, config.sz_hadith, config.font_style_hadith, rc, config.enable_shadow, config.enable_glow, br_hadith, br_shadow, pen_glow);

}

void CBayyenatDlg::DrawTextWithEffects(Graphics& graphics, const WCHAR* text, FontFamily& fontFamily, float font_sz, int font_style, 
										RectF& rc, bool doShadow, bool doGlow, SolidBrush& textBrush, SolidBrush& shadowBrush, Pen& glowPen) {
	
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	Gdiplus::Font font(&fontFamily, font_sz, font_style, UnitPixel);//Glow only works with UnitPixel
	
	WCHAR fname[260] = { 0 };
	fontFamily.GetFamilyName(fname);
	wcslwr(fname);
	// Shadow effect	
	if (doShadow)
	{
		RectF shadowRect = rc;
		shadowRect.X += config.shadow_off_x;
		shadowRect.Y += config.shadow_off_y;

		if(wcsstr(fname, L"irannastaliq"))
			graphics.DrawString(text, -1, &font, shadowRect, fmt_rtl, &shadowBrush);
		else
		{
			GraphicsPath path;
			path.AddString(text, -1, &fontFamily, font_style, font_sz, shadowRect, fmt_rtl);
			graphics.FillPath(&shadowBrush, &path);
		}
	}
	if (doGlow)
	{
		// Glow effect
		glowPen.SetLineJoin(LineJoinRound);
		GraphicsPath path;		
		path.AddString(text, -1, &fontFamily, font_style, font_sz, rc, fmt_rtl);
		graphics.DrawPath(&glowPen, &path);
	}

	//DrawPath usually takes more width than DrawString! so FillPath is preferred
	//except for Irannastaliq
	if (wcsstr(fname, L"irannastaliq"))
		graphics.DrawString(text, -1, &font, rc, fmt_rtl, &textBrush);
	else
	{
		GraphicsPath path;
		path.AddString(text, -1, &fontFamily, font_style, font_sz, rc, fmt_rtl);
		graphics.FillPath(&textBrush, &path);
	}
}

void CBayyenatDlg::DrawTrans(std::wstring& translation, Gdiplus::RectF& rc, int imH, cv::Mat& mat, Gdiplus::RectF& box, Gdiplus::Graphics* graphics, Gdiplus::Font& font_fa)
{
	Gdiplus::SolidBrush br_trans(CLR2Color(config.clr_trans_text));
	Gdiplus::SolidBrush br_shadow_trans(CLR2Color(config.clr_trans_shadow));
	Pen pen_glow(CLR2Color(config.clr_trans_glow, 255), config.glow_size); // Yellow glow

	Gdiplus::GraphicsPath path;
	FontFamily fontFamily(config.fName_trans);
	////////////////////////////

	if (translation.length() < 2 || config.sz_trans <= 0 || config.active_lang == langAR) {
		return;
	}
	rc.Y += rc.Height + 15;
	rc.Height = MAX(imH - rc.Y - 20, 0);
	if (config.trans_auto_color || config.trans_shadow_auto_color)
	{
		//Automatic Coloring based on mean of the background
		cv::Mat sub(mat, cv::Rect((int)box.X, (int)box.Y + (int)box.Height / 2, (int)box.Width, (int)box.Height / 2));
		cv::Scalar m = cv::mean(sub);
		if (config.trans_auto_color) {
			Color clr = AutoColor(m);
			br_trans.SetColor(clr);//Color(0, 0, 50));	
		}
		if (config.trans_shadow_auto_color)
			br_shadow_trans.SetColor(AutoShadow(m));
	}
	graphics->MeasureString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &rc);

	//graphics->DrawString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &br_trans);
	//return;
#if 0
		graphics->DrawString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &br_shadow_trans);
		//For Translation
		rc.X = MAX(rc.X - 1, 0);
		rc.Y = MAX(rc.Y - 1, 0);
		graphics->DrawString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &br_trans);
#elif 0
		graphics->DrawString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &br_shadow_trans);		
		rc.X = MAX(rc.X - 2, 0);
		rc.Y = MAX(rc.Y - 2, 0); 
		graphics->DrawString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &br_shadow_trans);

		//دو سطر قبلی به نوعی حاشیه بالا و پایین را می سازند و در وسط متن را با رنک اصلی ترسیم می کنیم
		rc.X += 1;
		rc.Y += 1;
		graphics->DrawString(translation.c_str(), translation.length(), &font_fa, rc, fmt_rtl, &br_trans);

#elif 1
		/*graphics->SetSmoothingMode(SmoothingModeAntiAlias);
		graphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		path.AddString(translation.c_str(), -1, &fontFamily,
			config.font_style_trans, config.sz_trans, rc, fmt_rtl);
		
		Pen pen(CLR2Color(config.clr_trans_shadow), 5);
		pen.SetLineJoin(LineJoinRound);//رفع مشکل تیزیهای اضافه اطراف متن

		if (enable_shadow_trans)
			graphics->DrawPath(&pen, &path);

		graphics->FillPath(&br_trans, &path);*/

		DrawTextWithEffects(*graphics, translation.c_str(), fontFamily, config.sz_trans, config.font_style_trans, rc, 
			config.enable_shadow_trans, config.enable_glow_trans, br_trans, br_shadow_trans, pen_glow);

#else //bad results
		Gdiplus::Font font_bold(fName_trans.c_str(), config.sz_trans, FontStyleBold);
		graphics->DrawString(translation.c_str(), translation.length(), &font_bold, rc, &fmt_rtl, &br_shadow_trans);
		graphics->DrawString(translation.c_str(), translation.length(), &font_fa, rc, &fmt_rtl, &br_trans);
#endif
}

void CBayyenatDlg::OnBnClickedStart()
{	
	CWaitCursor wait;
	if(slide_show_started) //Then stop it!
	{
		BOOL result = KillTimer(IDT_CHANGE_WP);
		if(result == 0)
			MessageBox(L"Timer can not be stopped", L"");
		slide_show_started = false;
		CString str;
		str.LoadString(IDS_INACTIVE);//"بیـّـنات - غیر فعال"
		m_TrayIcon.SetTooltipText(str);
		str.LoadString(IDS_START);
		str = L"⏯️ " + str;
		SetDlgItemText(IDC_START, str);
		AddToolTip(GetDlgItem(IDC_START), IDS_START_HINT);// _T("آغاز به کار برنامه - در فاصله های زمانی که مشخص کرده اید، حدیث عوض خواهد شد")

		m_TrayIcon.m_menu.ModifyMenu(ID_POPUP_START, MF_BYCOMMAND, ID_POPUP_START, L"شروع");
		m_TrayIcon.m_menu.CheckMenuItem(ID_POPUP_START, MF_BYCOMMAND|MF_UNCHECKED);

		//HICON icon = AfxGetApp()->LoadIcon(IDI_START);
		//m_btnStart.SetIcon(icon);

		wait.Restore();
		m_btnStart.SetTextColor(RGB(0, 128, 0));
		return;
	}
	m_btnStart.SetTextColor(RGB(128, 0, 0));

	UpdateData();
	ProcessHadithFile();

	//wchar_t path[MAX_PATH] = {0};
	//SystemParametersInfo(SPI_GETDESKWALLPAPER, 300, path, 0);
	//////////////////////////////////////////////////////////////////
	m_strWallpapers += _T("\\");
	std::wstring str = m_strWallpapers + _T("\\*.jpg");
	//SetCurrentDirectoryA(path.c_str());

	//if(!solid_colors)
	//{
	hFind_Image = FindFirstFile(str.c_str(), &ffd_jpg);

	if (INVALID_HANDLE_VALUE == hFind_Image && !config.solid_colors)
	{
		MessageBox(L"No image (*.jpg) found in the wallpaper directory", L"Error", MB_ICONASTERISK);
		return;
	}
	else
	{
		nImages = 0;
		do
		{
			nImages++;
		} while (FindNextFile(hFind_Image, &ffd_jpg));

		hFind_Image = FindFirstFile(str.c_str(), &ffd_jpg);
	}
	//}
	BOOL result = SetTimer(IDT_CHANGE_WP, ((config.slide_show_interval) * 60) * 1000, 0);
	if(result == 0)
		MessageBox(L"Timer can not be started", L"");

	ReadColors();
	SetNextWallpaper();//Set the new wallpaper

	slide_show_started = true;
	CString strTmp;
	strTmp.LoadString(IDS_ACTIVE);//"بیـّـنات - فعال"
	m_TrayIcon.SetTooltipText(strTmp);
	strTmp.LoadString(IDS_STOP);//L"توقف"
	strTmp = L"⏹ " + strTmp;
	SetDlgItemText(IDC_START, strTmp);
	AddToolTip(GetDlgItem(IDC_START), IDS_STOP_HINT);//_T("توقف برنامه")
	//HICON icon = AfxGetApp()->LoadIcon(IDI_STOP);
	//m_btnStart.SetIcon(icon);


	m_TrayIcon.m_menu.ModifyMenu(ID_POPUP_START, MF_BYCOMMAND, ID_POPUP_START, L"توقف");
	m_TrayIcon.m_menu.CheckMenuItem(ID_POPUP_START, MF_BYCOMMAND|MF_CHECKED);

	wait.Restore();
}

//Analyze hadith file to find the number of narrations (Hadiths) and save start position of each Hadith in the text file
void CBayyenatDlg::ProcessHadithFile()
{
	if (!hadith_file_changed)
		return;

	if (m_fp_hadith) {
		fclose(m_fp_hadith);
		m_fp_hadith = 0;
	}
	
	GetDlgItemText(IDC_HADITH_FILE, config.str_hadith_path, MAX_PATH);
	_wfopen_s(&m_fp_hadith, config.str_hadith_path, L"rb,ccs=UNICODE");//rb because fgetpos and fsetpos will be confused in text mode (CR,LF changes to \n in windows )
	if (!m_fp_hadith)
	{
		MessageBox(L"Hadith file can not be opened.", L"Error", MB_ICONASTERISK);
		return;
	}
	fseek(m_fp_hadith, 0, SEEK_END);
	sz_hadith_file = ftell(m_fp_hadith);
	fseek(m_fp_hadith, 0, SEEK_SET);

	int i = 0;
	wchar_t line[2000] = { 0 };
	fpos_t pos;
	hadith_pos.clear();
	while (!feof(m_fp_hadith)) {
		fgetws(line, 2000, m_fp_hadith); //read a line
		fgetpos(m_fp_hadith, &pos);
		wchar_t* p = wcsstr(line, L".:."); // .:. indicates start of new hadith
		if (p && abs(p - line) < 4)
			hadith_pos.push_back(pos);

	}
	nHadith = hadith_pos.size();
	hadith_file_changed = false;
}

void CBayyenatDlg::OnBnClickedGoToTray()
{
	ShowWindow(SW_HIDE);
}


void CBayyenatDlg::OnBnClickedHadithAutoColor()
{
	config.hadith_auto_color = ((CButton*)GetDlgItem(IDC_CHK_HADITH_AUTO_COLOR))->GetCheck() > 0;
	m_clrHadith.EnableWindow(!config.hadith_auto_color);
}


void CBayyenatDlg::OnBnClickedTransAutoColor()
{
	config.trans_auto_color = chk_trans_auto_color.GetCheck() > 0;
	m_clrTrans.EnableWindow(!config.trans_auto_color);
}


void CBayyenatDlg::OnBnClickedChkEnableShadow()
{
	config.enable_shadow = chk_enable_shadow.GetCheck() > 0;

	chk_shadow_auto_color.EnableWindow(config.enable_shadow);

	config.hadith_shadow_auto_color = chk_shadow_auto_color.GetCheck() > 0;

	m_clrShadow.EnableWindow(config.enable_shadow && !config.hadith_shadow_auto_color);
}


void CBayyenatDlg::OnBnClickedChkEnableShadowTrans()
{
	config.enable_shadow_trans = ((CButton*)GetDlgItem(IDC_CHK_ENABLE_SHADOW_TRANS))->GetCheck() > 0;
	
	((CButton*) GetDlgItem(IDC_CHK_SHADOW_TRANS_AUTO_COLOR))->EnableWindow(config.enable_shadow_trans);

	config.trans_shadow_auto_color = chk_trans_shadow_auto_color.GetCheck() > 0;
	
	m_clrShadowTrans.EnableWindow(config.enable_shadow_trans && !config.trans_shadow_auto_color);
}

void CBayyenatDlg::OnBnClickedChkSizeHadithAuto()
{
	config.hadith_auto_size = ((CButton*)GetDlgItem(IDC_CHK_SIZE_HADITH_AUTO))->GetCheck() > 0;
	((CButton*) GetDlgItem(IDC_EDT_FN_SIZE_HADITH))->SetCheck(!config.hadith_auto_size);
}


void CBayyenatDlg::OnBnClickedChkSizeTransAuto()
{
	config.trans_auto_size = ((CButton*)GetDlgItem(IDC_CHK_SIZE_TRANS_AUTO))->GetCheck() > 0;

	((CButton*) GetDlgItem(IDC_EDT_FN_SIZE_TRANS))->SetCheck(!config.trans_auto_size);
}


/*LRESULT CBayyenatDlg::OnNcHitTest(CPoint point)
{
	return HTCAPTION;//CDialogEx::OnNcHitTest(point);
}*/


//BOOL CBayyenatDlg::OnEraseBkgnd(CDC* pDC)
//{
//	/*CRect rclient;
//	GetClientRect(&rclient);
//	Gdiplus::Rect rc(rclient.left, rclient.top, rclient.Width(), rclient.Height());
//	Graphics gBack(pDC->m_hDC);
//	Color Grad(255, 200, 197, 180);
//	LinearGradientBrush brGrad(rc, Color(255,236,233,216), Grad, 90);
//	gBack.FillRectangle(&brGrad, rc);
//
//	return TRUE;*/
//	DrawHeaderandFooter();
//	return CDialogEx::OnEraseBkgnd(pDC);
//}


//custom function used sometimes to refine text files - unnecessary
void CBayyenatDlg::OnBnClickedTextProcessing()
{
	/*FILE *fp = 0, *fpout = 0;
	_wfopen_s(&fp, L"D:\\Projects\\Bayyenat 4.6.3 Setup\\Bayyenat 5-x64\\Ahadith\\Ahadith-New-Fa.txt", L"rt,ccs=UNICODE");
	_wfopen_s(&fpout, L"D:\\Projects\\Bayyenat 4.6.3 Setup\\Bayyenat 5-x64\\Ahadith\\Ahadith-New-Fa2.txt", L"wt,ccs=UNICODE");
	wchar_t line[2000] = {0};
	wchar_t num[10] = {0};
	int i = 0, n = 0;
	while(!feof(fp))
	{
		i++;
		fgetws(line, 2000, fp);
		if (wcslen(line) < 4)
			continue;
		for (auto i = 0; i < wcslen(line); i++)
			if (line[i] == L'\r')
				line[i] = L'\n';
		if (line[0] == L' ')
			wcscpy(line, line + 1);
		if (wcslen(line) > 5 && (line[wcslen(line) - 2] == L' '  || line[wcslen(line) - 2] == L'.' || line[wcslen(line) - 2] == L':'))
		{
			line[wcslen(line) - 2] = line[wcslen(line) - 1];
			line[wcslen(line) - 1] = 0;
		}
		fwprintf(fpout, line);
		continue;

		wchar_t * p = wcschr(line, L'_');//
		assert(p);
		if(p){
			wcsncpy(num, line, p-line);
			n = _wtoi(num);
			//assert(n == i);

			fwprintf(fpout, L".:.\n");
			fwprintf(fpout, L"»رسول خدا (صلی الله علیه و آله و سلم)\n");
			fwprintf(fpout, p+1);

			fgetws(line, 2000, fp);//read translation
			p = wcschr(line, L'_');//
			assert(!p);
			fwprintf(fpout, line);
			fwprintf(fpout, L"نهج الفصاحه حدیث شماره  %d\n", n);
		}
	}

	fclose(fp);
	fclose(fpout);*/
}

void CBayyenatDlg::SetNextWallpaper()
{
	UpdateData();
	//if( !((CButton*)GetDlgItem(IDC_CHK_RANDOM_HADITH))->GetCheck() )
	if(!config.random_hadith)
		config.last_hadith = GetDlgItemInt(IDC_EDT_HADITH_NUM)-1;

	if (!m_fp_hadith || sz_hadith_file < 10)
		return;
	
	mu.lock();
	if (config.solid_colors && !config.fixed_image)//when fixed_image is true, solid_color also must be fixed
	{
		if(GradColors.size() > 0)
			grad_color_idx = (++grad_color_idx) % (GradColors.size() / 2); //index of SolidColors array
	}
	bool check_filter_trans = false, check_filter_author = false;
	
	//Check for search queries
	std::wstring fTrans[5];
	std::wstring fAuthor[5];
	for(int k = 0; k < 5; k++){
		fTrans[k] = pSettingsDlg->m_filter_trans[k];
		fAuthor[k] = pSettingsDlg->m_filter_author[k];
		if (fTrans[k].length() > 1) 
			check_filter_trans = true;
		if(fAuthor[k].length() > 1) 
			check_filter_author = true;
	}

	bool check_filters = check_filter_author || check_filter_trans;

	int rnd_hadith = config.random_hadith;//((CButton*)GetDlgItem(IDC_CHK_RANDOM_HADITH))->GetCheck();

	if(check_filters) 
		rnd_hadith = false;

	//static bool start_of_new_hadith = false;// in the case of going to the region of the next hadith (reading .:. unusually)

	if(rnd_hadith){
		//start_of_new_hadith = false;
		double frnd = (double)rand()/(double)RAND_MAX;
		int rnd = (int)(frnd*sz_hadith_file);
		if(rnd % 2 == 1)//because of Unicode file, each character contains 2 bytes
			rnd -= 1;
		
		//////////////////////////////////////////////////////////////////////////
		if(nHadith > 0)
			rnd = MIN((int)(frnd*nHadith), nHadith-1);

		config.last_hadith = rnd-1;//it will be increased by 1 later
	}
	
	wchar_t line[1000] = {0};
	int filter_check_counter = 0;
	//skip until next valid hadith
FIND_VALID_HADITH:
	//fixed_hadith = ((CButton*)GetDlgItem(IDC_CHK_FIXED_HADITH))->GetCheck();
	if(config.fixed_hadith && (wcslen(str_cur_hadith) > 1  || wcslen(str_cur_translation) > 1 ) )
		goto FIND_WP;
	int i = 0;
	config.last_hadith++;
	if(config.last_hadith > nHadith-1)
		config.last_hadith = 0;
	fsetpos(m_fp_hadith, &hadith_pos[config.last_hadith]);
	
	i = 0;
	memset(str_cur_author, 0, 1000*2);
	memset(str_cur_ref, 0, 1000*2);
	memset(str_cur_hadith, 0, 1000*2);
	memset(str_cur_translation, 0, 1000*2);
	for(i = 0; i < 5; i++){
		int pos = ftell(m_fp_hadith);
		if(config.last_hadith < nHadith-1 && pos >= hadith_pos[config.last_hadith+1])
			break;
		if(feof(m_fp_hadith))
			break;
		int k = 0;
		do{
			fgetws(line, 2000, m_fp_hadith);//read a line
			if(k++ > 100){//prevent from infinite loop
				break;
			}
		}while( (wcslen(line) < 2) || (line[0] == L':' && line[1] == L':') );//skip empty lines and comment lines started by ::

		if(line[0] == L'.' && line[1] == L':')//(.:. indicates start of new hadith. used to prevent next search)
		{
			//int newpos = ftell(m_fp_hadith);
			//fgetws(line[i], 1000, m_fp_hadith);//read a line
			//fseek(m_fp_hadith, (pos-newpos)/2, SEEK_CUR);//go back to the start of the next hadith
			//newpos = ftell(m_fp_hadith);
			//fgetws(line[i], 1000, m_fp_hadith);//read a line
			//start_of_new_hadith = true;
			//assert(false);
			break;
		}
		if (line[0] == L'»'){
			if(i < 2)
				wcscpy(str_cur_author, &line[1]);
			else
				wcscpy(str_cur_ref, &line[1]);
		}
		else
			/*if(wcsstr(line, lang_prefix))
				continue;
			else*/ if(wcsstr(line, lang_prefix[config.active_lang].c_str()))
				wcscpy(str_cur_translation, &line[4]);
			else if(wcslen(str_cur_hadith) < 2)
				wcscpy(str_cur_hadith, line);
			else if(wcslen(str_cur_translation) < 2)//برای فایلهایی که چند ترجمه دارند این شرط باید چک شود
				wcscpy(str_cur_translation, line);
	}

	if(check_filters)
	{
		bool filter_trans_ok = !check_filter_trans; //if there is no filter for translation, its OK! and no checking required
		if(check_filter_trans){ //
			for(int i = 0; i < 5; i++)
			{
				if(fTrans[i].length() < 2)
					continue;
				if( wcsstr(str_cur_translation, fTrans[i].c_str()) ){
					filter_trans_ok = 1;
					break;
				}
			}
		}

		if(!filter_trans_ok && filter_check_counter++ < 400)
			goto FIND_VALID_HADITH;

		if(!check_filter_author)
			goto FIND_WP;//if there is no filter for the author, its OK! and no checking required
			
		for(int i = 0; i < 3; i++)
		{
			if(fAuthor[i].length() < 2)
				continue;
			if( wcsstr(str_cur_author, fAuthor[i].c_str()) )
				goto FIND_WP;
		}

		if(filter_check_counter++ < 400)
			goto FIND_VALID_HADITH;

		if(filter_check_counter >= 400){
			m_TrayIcon.ShowBallonTip(L"::خطا::", L"حدیثی مطابق با فیلترهای اعمالی یافت نشد", 5);
			Sleep(2000);
		}

	}

FIND_WP:
	//fixed_image = ((CButton*)GetDlgItem(IDC_CHK_FIXED_IMG))->GetCheck();
	std::wstring fn = L"";
	if(!config.solid_colors){
		if(config.fixed_image && wcslen(config.str_cur_image_path) > 1)
			fn = config.str_cur_image_path;
		else
		{
			int rnd_image = config.random_wp;//((CButton*)GetDlgItem(IDC_CHK_RANDOM_IMAGE))->GetCheck();
			str_cur_image_name = ffd_jpg.cFileName;
			if(rnd_image)
			{
				double frnd = (double)rand()/(double)RAND_MAX;
				int rnd = (int)(frnd*nImages);

				std::wstring s = m_strWallpapers + _T("\\*.jpg");
				hFind_Image = FindFirstFile(s.c_str(), &ffd_jpg);
				for(int i = 0; i < rnd-1; i++)
					FindNextFile(hFind_Image, &ffd_jpg);

				fn = m_strWallpapers + _T("\\") + ffd_jpg.cFileName;
			}
			else
			{
				fn = m_strWallpapers + _T("\\") + ffd_jpg.cFileName;
				if(FindNextFile(hFind_Image, &ffd_jpg) == 0){
					UpdateData();
					m_strWallpapers += _T("\\");
					std::wstring s = m_strWallpapers + _T("\\*.jpg");
					hFind_Image = FindFirstFile(s.c_str(), &ffd_jpg);
				}
			}			
			wcscpy(config.str_cur_image_path, fn.c_str());
		}
	}
	GenerateHadithWp(fn);
	if(config.show_popup)
		m_TrayIcon.ShowBallonTip(str_cur_author, str_cur_translation, 20);
	mu.unlock();
}

void CBayyenatDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == IDT_CHANGE_WP)
	{
		SetNextWallpaper();
	}
	else if(nIDEvent == IDT_GO_TO_TRAY)//when windows starts
	{
		try{
			KillTimer(IDT_GO_TO_TRAY);
			OnBnClickedStart();
			if(config.on_logon == 0)
				ShowWindow(SW_HIDE);
			else if(config.on_logon == 1)
				exit(0);
		}
		catch(...)
		{
			AfxMessageBox(L"Some problems!!");
		}
	}
	CDialogEx::OnTimer(nIDEvent);	
}


void CBayyenatDlg::OnPopupShowMainWindow()
{
	ShowWindow(SW_NORMAL);
}


void CBayyenatDlg::OnBnClickedClose()
{
	CString msg;
	msg.LoadStringW(IDS_CLOSE_PROMPT);//L"برنامه بسته خواهد شد، آیا مطمئن هستید؟"
	DWORD flags = MB_YESNO | MB_ICONINFORMATION;
	if(MessageBox(msg, L"اللهم صل علی محمد و آل محمد", flags | rtl_flags) == IDYES)
		CDialogEx::OnCancel();	
}


void CBayyenatDlg::OnChangeWP()
{
	UpdateData();
	
	m_strWallpapers += _T("\\");

	if(slide_show_started)
		SetNextWallpaper();
	else{
		OnBnClickedStart();//Start (Change)
		OnBnClickedStart();//Stop!
	}
	
}


void CBayyenatDlg::OnEnChangeEdtFnSizeHadith()
{	
	if(!dlg_inited)
		return;
	((CButton*)GetDlgItem(IDC_CHK_SIZE_HADITH_AUTO))->SetCheck(0);
}


void CBayyenatDlg::OnEnChangeEdtFnSizeTrans()
{
	if(!dlg_inited)
		return;
	((CButton*)GetDlgItem(IDC_CHK_SIZE_TRANS_AUTO))->SetCheck(0);
}


void CBayyenatDlg::OnPopupStart()
{
	OnBnClickedStart();
}
/*static CMFCToolTipInfo params;

params.m_bBoldLabel = FALSE;
params.m_bDrawDescription = FALSE;
params.m_bDrawIcon = FALSE;
params.m_bRoundedCorners = TRUE;
params.m_bDrawSeparator = FALSE;
params.m_clrFill = RGB (255, 255, 255);
params.m_clrFillGradient = RGB (228, 228, 240);
params.m_clrText = RGB (61, 83, 80);
params.m_clrBorder = RGB (144, 149, 168);

static CMFCToolTipCtrl tipCtrl(&params);
if(!tipCtrl.m_hWnd)
tipCtrl.Create(this);
tipCtrl.SetDescription(_T("tool tip control"));
tipCtrl.SetFixedWidth(100,150);
RECT rc;
GetClientRect(&rc);
EnableToolTips();
tipCtrl.AddTool( this, L"سلام اخوی");
tipCtrl.Activate(1);
tipCtrl.Popup();
Sleep(3000);*/




void CBayyenatDlg::OnBnClickedTray()
{
	ShowWindow(SW_HIDE);
}


void CBayyenatDlg::OnBnClickedMinimize()
{
	ShowWindow(SW_MINIMIZE);
}


void CBayyenatDlg::OnBnClickedAbout()
{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
}


void CBayyenatDlg::OnBnClickedResetDefaults()
{
	config.slide_show_interval = SLIDE_SHOW_INTERVAL_DEF;
	config.right_margin = RIGHT_MARGIN_DEF;
	config.left_margin = LEFT_MARGIN_DEF;
	config.top_margin = TOP_MARGIN_DEF;
	config.glow_size = GLOW_SIZE;
	config.shadow_off_x = config.shadow_off_y = SHADOW_OFF;
	config.on_logon = 0;
	config.hotkey_modifier = 1;
	config.hotkey_VK = 'W';
	config.hotkey_enable = true;
	SetNewHotKey();

	config.opacity = OPACITY_DEF;
	m_slider.SetPos(config.opacity);

	config.last_hadith = -1;

	config.random_hadith = RANDOM_HADITH_DEF;
	config.random_wp = RANDOMW_WP_DEF;
	config.fixed_image = FIXED_IMAGE_DEF;
	config.fixed_hadith = FIXED_HADITH_DEF;
	config.stretch_wp = STRETCH_WP_DEF;
	config.show_popup = SHOW_POPUP_DEF;
	
	((CComboBox*)GetDlgItem(IDC_CMB_HADITH_STYLE))->SetCurSel(STYLE_HADITH_DEF);
	((CComboBox*)GetDlgItem(IDC_CMB_TRANS_STYLE))->SetCurSel(STYLE_TRANS_DEF);
	

	config.full_width_grad = FULL_WIDTH_GRAD_DEF;
	((CButton*)GetDlgItem(IDC_CHK_FULL_WIDTH))->SetCheck(config.full_width_grad);

	config.solid_colors = SOLID_COLORS_DEF;
	((CButton*)GetDlgItem(IDC_CHK_SOLID_COLOR))->SetCheck(config.solid_colors);
	GetDlgItem(IDC_WP_PATH)->EnableWindow(!config.solid_colors);

	SetDlgItemInt(IDC_EDT_FN_SIZE_HADITH, FONT_SIZE_HADITH_DEF);
	if(!FONT_SIZE_HADITH_DEF)
		((CButton*)GetDlgItem(IDC_CHK_SIZE_HADITH_AUTO))->SetCheck(1);

	SetDlgItemInt(IDC_EDT_FN_SIZE_TRANS, FONT_SIZE_TRANS_DEF);
	if(!FONT_SIZE_TRANS_DEF)
		((CButton*)GetDlgItem(IDC_CHK_SIZE_TRANS_AUTO))->SetCheck(1);

	fnt_cmb_hadith.SelectString(0, FONT_HADITH_DEF);
	if(config.active_lang >= langFA)
		fnt_cmb_trans.SelectString(0, FONT_TRANS_DEF);
	else
		fnt_cmb_trans.SelectString(0, L"Tahoma");
	////////////////////////////////////////////////////////////////////////////
	//Hadith Style Parameters
	config.hadith_auto_color = false;
	chk_auto_color.SetCheck(config.hadith_auto_color);
	m_clrHadith.EnableWindow(!config.hadith_auto_color);
	//if(!config.hadith_auto_color)
	m_clrHadith.SetColor(HADITH_COLOR_DEF);

	chk_enable_shadow.SetCheck(HADITH_ENABLE_SHADOW_DEF);
	m_clrShadow.EnableWindow(HADITH_ENABLE_SHADOW_DEF);
	m_clrShadow.SetColor(HADITH_SHADOW_COLOR_DEF);
	chk_shadow_auto_color.EnableWindow(HADITH_ENABLE_SHADOW_DEF);

	chk_enable_glow.SetCheck(HADITH_ENABLE_GLOW_DEF);
	m_clrGlow.EnableWindow(HADITH_ENABLE_GLOW_DEF);
	m_clrGlow.SetColor(HADITH_GLOW_COLOR_DEF);
	chk_glow_auto_color.EnableWindow(HADITH_ENABLE_GLOW_DEF);

	config.hadith_shadow_auto_color = false;
	chk_shadow_auto_color.SetCheck(config.hadith_shadow_auto_color);
	////////////////////////////////////////////////////////////////////////////
	//Translation Style Parameters
	config.trans_auto_color = false;
	chk_trans_auto_color.SetCheck(config.trans_auto_color);
	m_clrTrans.EnableWindow(!config.trans_auto_color);
	//if(!config.trans_auto_color)//Automatic coloring
		m_clrTrans.SetColor(TRANS_COLOR_DEF);

	
	chk_trans_enable_shadow.SetCheck(TRANS_ENABLE_SHADOW_DEF);
	m_clrShadowTrans.EnableWindow(TRANS_ENABLE_SHADOW_DEF);
	m_clrShadowTrans.SetColor(TRANS_SHADOW_COLOR_DEF);
	chk_trans_shadow_auto_color.EnableWindow(TRANS_ENABLE_SHADOW_DEF);

	chk_trans_enable_glow.SetCheck(TRANS_ENABLE_GLOW_DEF);
	m_clrGlowTrans.EnableWindow(TRANS_ENABLE_GLOW_DEF);
	m_clrGlowTrans.SetColor(TRANS_GLOW_COLOR_DEF);
	chk_trans_glow_auto_color.EnableWindow(TRANS_ENABLE_GLOW_DEF);

	config.trans_shadow_auto_color = false;
	chk_trans_shadow_auto_color.SetCheck(config.trans_shadow_auto_color);
	m_clrShadowTrans.EnableWindow(!config.trans_shadow_auto_color);

	
	m_clrGrad.SetColor(GRADIENT_COLOR_DEF);
	
	m_cmbGrad.SetCurSel(GRADIENT_DEF);
}


void CBayyenatDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	//if(nIDCtl == IDC_HEADER)
		//DrawHeaderandFooter();
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CBayyenatDlg::OnEnChangeHadithFile()
{
	hadith_file_changed = true;
}


void CBayyenatDlg::OnCancel()
{	
	OnBnClickedClose();
}

BOOL CBayyenatDlg::PreTranslateMessage(MSG* pMsg)
{
	if (NULL != m_pToolTip)
		m_pToolTip->RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


//void CBayyenatDlg::OnBnClickedChkRandomHadith()
//{
//	int rnd = ((CButton*)GetDlgItem(IDC_CHK_RANDOM_HADITH))->GetCheck();
//	GetDlgItem(IDC_EDT_HADITH_NUM)->EnableWindow(!rnd);
//}


void CBayyenatDlg::OnBnClickedSaveWp()
{
	if(img.IsNull())
		return;
	CFileDialog fdlg(FALSE, _T(".jpg"), 0, 0, _T("JPEG Image (*.jpg)|*.jpg|PNG Image (*.png)|*.png|Bitmap Image - Best Quality (*.bmp)|*.bmp||"));
	if(fdlg.DoModal() != IDOK)
		return;
	
	std::wstring ext = fdlg.GetPathName().Right(3);
	if(ext[0] == L'j' || ext[0] == L'J'){
		img.Save(fdlg.GetPathName(), Gdiplus::ImageFormatJPEG);
		/*CLSID encoderClsid;
		GetEncoderClsid(L"image/jpeg", &encoderClsid);
		Gdiplus::Bitmap bm((HBITMAP)img, NULL);
		assert(bm.GetLastStatus() == Gdiplus::Ok);
		bm.Save( fdlg.GetPathName(), &encoderClsid, &encoderParameters );*/
		//img.Save(fdlg.GetPathName(), Gdiplus::ImageFormatJPEG);
	}
	else if (ext[0] == L'p' || ext[0] == L'P')
		img.Save(fdlg.GetPathName(), Gdiplus::ImageFormatPNG);
	else
		img.Save(fdlg.GetPathName(), Gdiplus::ImageFormatBMP);
}


//void CBayyenatDlg::OnBnClickedChkFixedHadith()
//{
//	int f = ((CButton*)GetDlgItem(IDC_CHK_FIXED_HADITH))->GetCheck();
//	
//	if(f)
//		((CButton*)GetDlgItem(IDC_CHK_FIXED_IMG))->SetCheck(false);
//}
//
//
//void CBayyenatDlg::OnBnClickedChkFixedImg()
//{
//	int f = ((CButton*)GetDlgItem(IDC_CHK_FIXED_IMG))->GetCheck();
//
//	if(f)
//		((CButton*)GetDlgItem(IDC_CHK_FIXED_HADITH))->SetCheck(false);
//}


void CBayyenatDlg::OnBnClickedBtnSettings()
{
	pSettingsDlg->DoModal();
}

void CBayyenatDlg::ApplySettings()
{
	UpdateData();

	config.opacity = m_slider.GetPos();
	config.full_width_grad = ((CButton*)GetDlgItem(IDC_CHK_FULL_WIDTH))->GetCheck() > 0;

	config.font_style_hadith = ((CComboBox*)GetDlgItem(IDC_CMB_HADITH_STYLE))->GetCurSel();

	config.clr_hadith_text   = m_clrHadith.GetColor();
	config.clr_hadith_glow   = m_clrGlow.GetColor();
	config.clr_hadith_shadow = m_clrShadow.GetColor();

	config.enable_shadow = chk_enable_shadow.GetCheck() > 0;
	config.hadith_shadow_auto_color = chk_shadow_auto_color.GetCheck() > 0;
	config.enable_glow = chk_enable_glow.GetCheck() > 0;
	config.hadith_glow_auto_color = chk_glow_auto_color.GetCheck() > 0;
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Trans Style Parameters
	config.font_style_trans = ((CComboBox*)GetDlgItem(IDC_CMB_TRANS_STYLE))->GetCurSel();

	config.clr_trans_text   = m_clrTrans.GetColor();
	config.clr_trans_shadow = m_clrShadowTrans.GetColor();
	config.clr_trans_glow = m_clrGlowTrans.GetColor();

	config.enable_shadow_trans = chk_trans_enable_shadow.GetCheck() > 0;
	config.trans_shadow_auto_color = chk_trans_shadow_auto_color.GetCheck() > 0;
	config.enable_glow_trans = chk_trans_enable_glow.GetCheck() > 0;
	config.trans_glow_auto_color = chk_trans_glow_auto_color.GetCheck() > 0;
	//////////////////////////////////////////////////////////////////////////
	// Gradient Params
	config.clr_gradient = m_clrGrad.GetColor();
	
	int sel = m_cmbGrad.GetCurSel();
	if(sel > -1)
		config.gradient_type = (Gradient_Type) sel;
	else
		config.gradient_type = gtNone;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Font Size Parameters
	config.sz_hadith = GetDlgItemInt(IDC_EDT_FN_SIZE_HADITH);
	if(config.hadith_auto_size || config.sz_hadith < 0)
		config.sz_hadith = 20*desktopW/1366;


	config.sz_trans = GetDlgItemInt(IDC_EDT_FN_SIZE_TRANS);
	if(config.trans_auto_size || config.sz_trans < 0)
		config.sz_trans = 20*desktopW/1366;

	int i = fnt_cmb_hadith.GetCurSel();
	//CString s;
	if(i > -1){
		fnt_cmb_hadith.GetLBText(i, config.fName_ar);//Get List Box Text
		//config.fName_ar = s;
	}

	i = fnt_cmb_trans.GetCurSel();
	if(i > -1){
		fnt_cmb_trans.GetLBText(i, config.fName_trans);
		//fName_trans = s;
	}
}

void CBayyenatDlg::OnBnClickedBtnApplyStyle()
{
	//if(slide_show_started)
		GenerateHadithWp(config.str_cur_image_path); 
	//else
		//OnChangeWP();
}


void CBayyenatDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default
	OnChangeWP();
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

void CBayyenatDlg::UpdateFontList()
{
	//////////////////////////////////////////////////////////////////////////
	//enumerate installed fonts
	//note that using MFC Font Combo Box USES A LOT OF RESOURCE AND MAKE THE PROGRAM TO START <VERY SLOWLY>
	//so I enumerate them manually
	INT          found = 0;
	WCHAR        familyName[LF_FACESIZE];  // enough space for one family name
	FontFamily*  pFontFamily = NULL;
	InstalledFontCollection installedFontCollection;
	int count = installedFontCollection.GetFamilyCount();
	// Allocate a buffer to hold the array of FontFamily
	// objects returned by GetFamilies.
	pFontFamily = new FontFamily[count];
	// Get the array of FontFamily objects.
	installedFontCollection.GetFamilies(count, pFontFamily, &found);
	for(INT j = 0; j < count; ++j)
	{
		pFontFamily[j].GetFamilyName(familyName);
		fnt_cmb_hadith.AddString(familyName);
		fnt_cmb_trans.AddString(familyName);
	}

	delete [] pFontFamily;
}

//Must be called after ReadSettings
void CBayyenatDlg::UpdateUI()
{	
	m_slider.SetPos(config.opacity);
	SetDlgItemInt(IDC_EDT_HADITH_NUM, config.last_hadith+1);
	SetDlgItemText(IDC_WP_PATH, config.str_wp_path);
	SetDlgItemText(IDC_HADITH_FILE, config.str_hadith_path);
	((CButton*)GetDlgItem(IDC_CHK_FULL_WIDTH))->SetCheck(config.full_width_grad);
	((CButton*)GetDlgItem(IDC_CHK_SOLID_COLOR))->SetCheck(config.solid_colors);
	GetDlgItem(IDC_WP_PATH)->EnableWindow(!config.solid_colors);
	SetDlgItemInt(IDC_EDT_FN_SIZE_HADITH, config.sz_hadith);
	((CButton*)GetDlgItem(IDC_CHK_SIZE_HADITH_AUTO))->SetCheck(config.hadith_auto_size);

	SetDlgItemInt(IDC_EDT_FN_SIZE_TRANS, config.sz_trans);
	((CButton*)GetDlgItem(IDC_CHK_SIZE_TRANS_AUTO))->SetCheck(config.trans_auto_size);

	fnt_cmb_hadith.SelectString(-1, config.fName_ar);

	fnt_cmb_trans.SelectString(-1, config.fName_trans);
	////////////////////////////////////////////////////////////////////////////
	//Hadith Style Parameters	
	((CComboBox*)GetDlgItem(IDC_CMB_HADITH_STYLE))->SetCurSel(config.font_style_hadith);
	m_clrHadith.SetColor(config.clr_hadith_text);
	m_clrShadow.SetColor(config.clr_hadith_shadow);
	m_clrGlow.SetColor(config.clr_hadith_glow);
	chk_auto_color.SetCheck(config.hadith_auto_color);
	chk_enable_shadow.SetCheck(config.enable_shadow);
	chk_enable_glow.SetCheck(config.enable_glow);
	chk_shadow_auto_color.SetCheck(config.hadith_shadow_auto_color);
	chk_glow_auto_color.SetCheck(config.hadith_glow_auto_color);

	m_clrHadith.EnableWindow(!config.hadith_auto_color);
	m_clrShadow.EnableWindow(config.enable_shadow && !config.trans_auto_color);
	m_clrGlow.EnableWindow(config.enable_glow && !config.hadith_glow_auto_color);

	/*if (config.hadith_auto_color) {//Automatic coloring
		chk_auto_color.SetCheck(1);
		m_clrHadith.EnableWindow(false);
	}	
	
	
	m_clrShadow.EnableWindow(config.enable_shadow);
	chk_shadow_auto_color.EnableWindow(config.enable_shadow );

	m_clrGlow.EnableWindow(config.enable_glow);
	chk_glow_auto_color.EnableWindow(config.enable_glow);

	if(config.hadith_shadow_auto_color){//Automatic coloring		
		chk_shadow_auto_color.SetCheck(1);
		m_clrShadow.EnableWindow(false);
	}*/

	////////////////////////////////////////////////////////////////////////////
	//Translation Style Parameters
	((CComboBox*)GetDlgItem(IDC_CMB_TRANS_STYLE))->SetCurSel(config.font_style_trans);
	m_clrTrans.SetColor(config.clr_trans_text);
	m_clrShadowTrans.SetColor(config.clr_trans_shadow);
	m_clrGlowTrans.SetColor(config.clr_trans_glow);
	chk_trans_auto_color.SetCheck(config.trans_auto_color);
	chk_trans_enable_shadow.SetCheck(config.enable_shadow_trans);
	chk_trans_enable_glow.SetCheck(config.enable_glow_trans);
	chk_trans_shadow_auto_color.SetCheck(config.trans_shadow_auto_color);
	chk_trans_glow_auto_color.SetCheck(config.trans_glow_auto_color);

	m_clrTrans.EnableWindow(!config.trans_auto_color);
	m_clrShadowTrans.EnableWindow(config.enable_shadow_trans && !config.trans_shadow_auto_color);
	m_clrGlowTrans.EnableWindow(config.enable_glow_trans && !config.trans_glow_auto_color);

	/*chk_trans_glow_auto_color.EnableWindow(config.enable_glow_trans);

	config.trans_glow_auto_color = chk_trans_glow_auto_color.GetCheck() > 0;


	if (config.trans_auto_color)//Automatic coloring
	{		
		chk_trans_auto_color.SetCheck(1);
		m_clrTrans.EnableWindow(false);
	}

	if(config.active_lang == langAR)
		config.enable_shadow_trans = false;

	((CButton*)GetDlgItem(IDC_CHK_ENABLE_SHADOW_TRANS))->SetCheck(config.enable_shadow_trans);
	m_clrShadowTrans.EnableWindow(config.enable_shadow_trans);
	
	chk_trans_shadow_auto_color.EnableWindow(config.enable_shadow_trans);

	if (config.trans_shadow_auto_color)//Automatic coloring
	{
		chk_trans_shadow_auto_color.SetCheck(1);
		m_clrShadowTrans.EnableWindow(false);
	}*/

	m_clrGrad.SetColor(config.clr_gradient);
	for(int i = 0; i < gtCount; i++)
	{
		CString str;
		str.LoadString(IDS_GRAD_FIXED+i);
		m_cmbGrad.AddString(str);
	}

	m_cmbGrad.SetCurSel(config.gradient_type);

	//SetWindowPos(0, 0, 0, 633, 439, SWP_NOMOVE);

	g_hWnd = m_hWnd;

	static CFont fnt;
	fnt.CreatePointFont(100, L"IRANSansXFaNum");
	m_btnStart.SetFont(&fnt, 0);
	m_btnStart.SetTextColor(RGB(0, 128, 0));
}

BOOL CBayyenatDlg::ActivateToolTips()
{
	/////////////////////////////////////////////////////////////////
	/*params.m_bBoldLabel = FALSE;
	params.m_bDrawDescription = FALSE;
	params.m_bDrawIcon = TRUE;
	params.m_bRoundedCorners = TRUE;
	params.m_bDrawSeparator = FALSE;
	params.m_clrFill = RGB (255, 255, 255);
	params.m_clrFillGradient = RGB (228, 228, 240);
	params.m_clrText = RGB (61, 83, 80);
	params.m_clrBorder = RGB (144, 149, 168);
	params.m_bVislManagerTheme = TRUE;*/

	//Set up the tooltip
	m_pToolTip = new CToolTipCtrl();

	DWORD flags = WS_EX_NOINHERITLAYOUT;//از ساختار دیالوگ پدر پیروی نکن. ممکن است دیالوگ پدر راست چین باشد
	if (config.active_lang == langFA || config.active_lang == langAR)
		flags = WS_EX_RTLREADING | WS_EX_LAYOUTRTL;
	if (!m_pToolTip->CreateEx(this, 0, flags))
	{
		TRACE("Unable To create ToolTip\n");
		return TRUE;
	}
	//static CFont fnt;
	//fnt.CreateFont(24,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
	//CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("B Mitra"));
	//GetDlgItem(IDC_START)->SetFont(&fnt);

	//m_pToolTip->SetFont(&fnt);
	m_pToolTip->SetTipTextColor(RGB(100, 100, 200));
	AddToolTip(GetDlgItem(IDC_CHK_FULL_WIDTH), IDS_FULL_WIDTH_HINT);// _T("با انتخاب این گزینه، رنگ پس زمینه، کل عرض صفحه دسکتاپ را پوشش می دهد"));
	AddToolTip(GetDlgItem(IDC_CHK_SOLID_COLOR), IDS_SOLID_COLOR_HINT);// _T("با انتخاب این گزینه، به جای تصویر پس زمینه از رنگهای ثابت استفاده می‏شود"));
	AddToolTip(GetDlgItem(IDC_SLIDER_OPACITY), IDS_OPACITY_HINT);// _T("عددی بین 0 و 255 که میزان شفافیت رنگ پس زمینه حدیث را بیان می کند"));
	AddToolTip(GetDlgItem(IDC_COLOR_GRADIENT), IDS_COLOR_GRAD_HINT);// _T("انتخاب رنگ گرادیان پس زمینه حدیث"));
	AddToolTip(GetDlgItem(IDC_EDT_HADITH_NUM), IDS_HADITH_NUM_HINT);// _T("تعیین شماره حدیث (مناسب برای نمایش غیر تصادفی احادیث)"));
	AddToolTip(GetDlgItem(IDC_START), IDS_START_HINT);// _T("آغاز به کار برنامه - در فاصله های زمانی که مشخص کرده اید، حدیث عوض خواهد شد"));	

	AddToolTip(GetDlgItem(IDC_RESET_DEFAULTS), IDS_RESET_DEFAULT_HINT);// _T("بازگشت به تنظیمات اولیه"));
	AddToolTip(GetDlgItem(IDC_BTN_APPLY_STYLE), IDS_APPLY_HINT);// _T("اعمال تنظیمات به حدیث جاری"));
	AddToolTip(GetDlgItem(IDC_CHANGE), IDS_CHANGE_HOTKEY_HINT);// _T("تغییر فوری حدیث پس زمینه ویندوز\n (دکمه میانبر: CTRL+W)"));
	AddToolTip(GetDlgItem(IDC_SAVE_WP), IDS_SAVE_HADITH_IMG);// _T("ذخیره تصویر دارای حدیث"));
	AddToolTip(GetDlgItem(IDC_BTN_SETTINGS), IDS_SETTINGS_HINT);// _T("نمایش تنظیمات بیشتر"));

	AddToolTip(GetDlgItem(IDC_EDT_FN_SIZE_HADITH), IDS_HADITH_FONT_SZ);// _T("اندازه قلم - به منظور عدم نمایش متن عربی حدیث، مقدار صفر وارد کنید"));
	AddToolTip(GetDlgItem(IDC_EDT_FN_SIZE_TRANS), IDS_TRANS_FONT_SZ);//_T("اندازه قلم - به منظور عدم نمایش ترجمه، مقدار صفر وارد کنید"));
	

	AddToolTip(GetDlgItem(IDC_ABOUT), IDS_ABOUT);
	AddToolTip(GetDlgItem(IDC_TRAY), IDS_GO_TO_NOTIFICATION);
	//AddToolTip(GetDlgItem(IDC_CHK_FULL_WIDTH), _T(""));

	//int x = m_pToolTip->ModifyStyleEx(0, WS_EX_RTLREADING);
	m_pToolTip->SetMaxTipWidth(240);

	//does not work!!
	/*params.m_bBalloonTooltip = TRUE;
	theApp.GetTooltipManager ()->SetTooltipParams (
	AFX_TOOLTIP_TYPE_ALL,
	RUNTIME_CLASS (CMFCToolTipCtrl),
	&params);*/
	m_pToolTip->Activate(TRUE);
	return TRUE;
}

void CBayyenatDlg::SetNewHotKey()
{
	CString tmp;
	tmp.LoadString(IDS_CHANGE);
	tmp = L"✅ " + tmp;
	if(config.hotkey_enable){
		BOOL b = UnregisterHotKey( m_hWnd, 1);  
		b = RegisterHotKey( m_hWnd, 1, HOTKEY_MODIFIERS[config.hotkey_modifier]|MOD_NOREPEAT, config.hotkey_VK );//hotkey to change wallpaper
		if(b){
			CString key_name[] = {L"ALT", L"CTRL", L"SHIFT", L"WIN"};
			tmp.AppendFormat(L" <%s+%c>", key_name[config.hotkey_modifier], config.hotkey_VK);
			SetDlgItemText(IDC_CHANGE, tmp);
		}
		else{
			DWORD err = GetLastError();
			MessageBox(L"Error while assigning HotKey, Please change it.", L"Warning", MB_OK);
		}
	}
	else
		SetDlgItemText(IDC_CHANGE, tmp);
	
}


void CBayyenatDlg::OnBnClickedChkSolidColor()
{
	config.solid_colors = ((CButton*)GetDlgItem(IDC_CHK_SOLID_COLOR))->GetCheck() > 0;
	if(config.solid_colors)
		ReadColors();
	GetDlgItem(IDC_WP_PATH)->EnableWindow(!config.solid_colors);
}


void CBayyenatDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if(pSettingsDlg){//if oninitdlg finished!
		Invalidate();
	}
	// TODO: Add your message handler code here
}


void CBayyenatDlg::OnCbnSelchangeCmbLng()
{
	int sel = m_cmbLng.GetCurSel();
	if (sel < 0)
		return;
	if (config.active_lang == (ELang)sel)
		return;
	config.active_lang = (ELang)sel;
	if (config.active_lang < langFA && wcscmp(config.fName_trans, FONT_TRANS_DEF) == 0)
		wcscpy(config.fName_trans, L"Tahoma");//Default font is not OK for other languages


	wsprintf(config.str_hadith_path, L"%s\\Ahadith\\Ahadith-%s.txt", ExePath, lang_code[config.active_lang].c_str());

	SaveSettings();
	theApp.restart = true;

	//این خط تنها برای اینکه رشته تغییر فایل حدیث که در دو خط بعد است درست کار کند، لازم است
	theApp.SetLanguage(config.active_lang);
	
	CString str;
	str.LoadString(IDS_CHANGE_HADITH_FILE);
	MessageBox(str, L".:. Warning .:.", rtl_flags|MB_ICONINFORMATION);

	EndDialog(20);
}

void CBayyenatDlg::OnEnChangeWpPath()
{
	GetDlgItemText(IDC_WP_PATH, config.str_wp_path, MAX_PATH);
}


void CBayyenatDlg::OnBnClickedCopyText()
{
	CWaitCursor wait;
	std::wstring s = str_cur_author;
	if (s[s.length() - 1] != L'\n')
		s += L"\n";
	s += str_cur_hadith;
	if (s[s.length() - 1] != L'\n')
		s += L"\n";
	s += str_cur_translation;
	if (s[s.length() - 1] != L'\n')
		s += L"\n";
	s += str_cur_ref;
	if (s[s.length() - 1] != L'\n')
		s += L"\n";

	CString str;
	str.LoadString(IDS_VERSION);
	s += str;
	if(CopyToClipboard(m_hWnd, s))
		MessageBox(L"Text Copied Successfully!");
	wait.Restore();
}


void CBayyenatDlg::OnBnClickedCopyImage()
{
	CWaitCursor wait;
	CBitmap* pBitmap = CBitmap::FromHandle(img);
	if(CopyImageToClipboard(img))
		MessageBox(L"Image Copied Successfully!");
	wait.Restore();
}


void CBayyenatDlg::OnCbnSelchangeCmbHadithFiles()
{
	CWaitCursor wait;

	CString str;
	GetDlgItemText(IDC_CMB_HADITH_FILES, str);
	int n = str.Find(L"(");
	if (n > 0)
		str = str.Left(n-1);
	std::wstring fn = ExePath + std::wstring(L"\\Ahadith\\") + str.GetBuffer();

	SetDlgItemText(IDC_HADITH_FILE, fn.c_str());

	hadith_file_changed = true;
	ProcessHadithFile();

	wait.Restore();
}


void CBayyenatDlg::OnBnClickedChkEnableGlowTrans()
{
	config.enable_glow_trans =  chk_trans_enable_glow.GetCheck() > 0;

	chk_trans_glow_auto_color.EnableWindow(config.enable_glow_trans);

	config.trans_glow_auto_color = chk_trans_glow_auto_color.GetCheck() > 0;

	m_clrGlowTrans.EnableWindow(config.enable_glow_trans && !config.trans_glow_auto_color);
}


void CBayyenatDlg::OnBnClickedChkEnableGlow()
{
	config.enable_glow = chk_enable_glow.GetCheck() > 0;
	
	chk_glow_auto_color.EnableWindow(config.enable_glow);

	config.hadith_glow_auto_color = chk_glow_auto_color.GetCheck() > 0;

	m_clrGlow.EnableWindow(config.enable_glow && !config.hadith_glow_auto_color);
}
