#include "StdAfx.h"
#include "Settings.h"
#include <GdiPlusEnums.h>

bool stretch_wp		= STRETCH_WP_DEF;
bool show_popup		= SHOW_POPUP_DEF;//enable balloon tips
bool random_hadith	= RANDOM_HADITH_DEF;
bool fixed_image	= FIXED_IMAGE_DEF;
bool fixed_hadith	= FIXED_HADITH_DEF;
bool random_wp		= RANDOMW_WP_DEF;

int right_margin	= RIGHT_MARGIN_DEF;
int left_margin		= LEFT_MARGIN_DEF; //in %
int top_margin		= TOP_MARGIN_DEF;

int on_logon		= 0; // what to do on windows logon(0 start, 1 change aand close, 2 don't start)

char hotkey_VK		= 'W';
UINT hotkey_modifier = 0;//index of selected key in combo box
bool hotkey_enable	= true;
UINT HOTKEY_MODIFIERS[] = {MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN};
CString slide_show_interval = SLIDE_SHOW_INTERVAL_DEF;

bool hadith_auto_color = false;
bool shadow_auto_color = false;
bool trans_auto_color = false;
bool shadow_trans_auto_color = false;
bool full_width_grad = false;
bool solid_colors = false;
int  font_style_hadith = FontStyleBold;
int  font_style_trans = FontStyleBold;
bool hadith_auto_size = false;
bool trans_auto_size = false;

int enable_shadow = 0;
int enable_shadow_trans = 0;
int sz_hadith = 20; //font size of the Hadith
int sz_trans = 20;  //font size of translation
std::wstring fName_ar = FONT_HADITH_DEF;
std::wstring fName_trans = FONT_TRANS_DEF;
COLORREF clr_hadith = 0x000000;
COLORREF clr_shadow = 0xffffff;
COLORREF clr_trans = 0x000000;
COLORREF clr_shadow_trans = 0xffffff;
COLORREF clr_gradient = 0xffffffff;
wchar_t ExePath[MAX_PATH] = {0};
bool slide_show_started = false;//slide show started
bool busy = false;//indicates wallpaper is changing or not
bool dlg_inited = false;//A- problem with windows XP

int opacity = 10;//in %
bool hadith_file_changed = true;
int header_idx = 0;//index of the string that will appear in header
int last_hadith = -1; //last shown hadith (used when showing Hadithes sequentially, specially when restarting windows it should continue from last hadith)

CString str_hadith_path; //مسیر فایل احادیث
CString str_wp_path; //مسیر تصاویر زمینه
CString str_last_wp; //آخرین تصویر انتخابی


Gradient_Type gradient_type = gtFixedColorAuto;

CString lang_names[] = { L"English", L"Español", L"Portugués", L"فارسی", L"العربية" };
std::wstring lang_code[] = { L"en", L"es", L"po", L"fa", L"fa" };//فعلا از همان احادیث فراسی برای عربی هم استفاده می شود
std::wstring lang_prefix[] = { L"en:", L"es:", L"po:", L"فا:", L"عر:" };
ELang active_lang = langFA;
DWORD rtl_flags = 0;//