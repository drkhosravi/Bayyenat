#pragma once
#include <atlstr.h>
#include <string>
#include <gdiplusheaders.h>
#include <vector>
#include "BUtils.h"
#include "Bayyenat.h"

using namespace Gdiplus;

#ifndef _BAYYENAT_SETTINGS_H_
#define _BAYYENAT_SETTINGS_H_

#define LEFT_MARGIN_DEF					50
#define TOP_MARGIN_DEF					20
#define RIGHT_MARGIN_DEF				5
#define RANDOM_HADITH_DEF				1
#define FIXED_IMAGE_DEF					0
#define FIXED_HADITH_DEF				0
#define RANDOMW_WP_DEF					1
#define STRETCH_WP_DEF					1
#define SHOW_POPUP_DEF					1

#define SLIDE_SHOW_INTERVAL_DEF			L"050000"
#define FONT_HADITH_DEF					L"Neirizi"
#define FONT_TRANS_DEF					L"IRANSans"
#define OPACITY_DEF						40
#define FULL_WIDTH_GRAD_DEF				0
#define SOLID_COLORS_DEF				0
#define FONT_SIZE_HADITH_DEF			18
#define FONT_SIZE_TRANS_DEF				22
#define HADITH_COLOR_DEF				0xFFFFFF
#define TRANS_COLOR_DEF					0x55FFFF
#define HADITH_ENABLE_SHADOW_DEF		1
#define HADITH_SHADOW_COLOR_DEF			0x800080
#define TRANS_ENABLE_SHADOW_DEF			1
#define TRANS_SHADOW_COLOR_DEF			0x000080
#define GRADIENT_COLOR_DEF				0x000000
#define STYLE_HADITH_DEF				FontStyleRegular
#define STYLE_TRANS_DEF					FontStyleRegular
#define	GRADIENT_DEF					gtBlur

enum		Gradient_Type		  {gtFixedColor, gtFixedColorAuto, gtBlur, gtVertical1 , gtVertical2, gtHorizon1, gtHorizon2, gtPath1, gtPath2, gtBellShape1, gtBellShape2, gtEllipse, gtNone, gtCount};

extern CBayyenatApp theApp;

extern bool stretch_wp;
extern bool show_popup;
extern bool random_hadith;
extern bool fixed_image;
extern bool fixed_hadith;
extern bool random_wp;
extern int right_margin;
extern int left_margin;
extern int top_margin;
extern int on_logon;
extern char hotkey_VK;
extern UINT hotkey_modifier;
extern bool hotkey_enable;
extern UINT HOTKEY_MODIFIERS[4];
extern CString slide_show_interval;
extern wchar_t	str_local_pathW[MAX_PATH];//the path in which wallpaper will be saved (unicode)
extern char		str_local_pathA[MAX_PATH];//the path in which wallpaper will be saved (ascii)
extern std::wstring		str_cur_image_path;

extern bool hadith_auto_color;
extern bool shadow_auto_color;
extern bool trans_auto_color;
extern bool shadow_trans_auto_color;
extern bool full_width_grad;
extern bool solid_colors;
extern int  font_style_hadith;
extern int  font_style_trans;
extern bool hadith_auto_size;
extern bool trans_auto_size;


extern int enable_shadow;
extern int enable_shadow_trans;
extern int sz_hadith; //font size of the Hadith
extern int sz_trans;  //font size of translation
extern std::wstring fName_ar;
extern std::wstring fName_trans;
extern COLORREF clr_hadith;
extern COLORREF clr_shadow;
extern COLORREF clr_trans;
extern COLORREF clr_shadow_trans;
extern COLORREF clr_gradient;
extern wchar_t ExePath[MAX_PATH];
extern bool slide_show_started;//slide show started
extern bool busy;//indicates wallpaper is changing or not
extern bool dlg_inited;//A- problem with windows XP

extern int opacity;//in %
extern bool hadith_file_changed;
extern int header_idx;//index of the string that will appear in header
extern int last_hadith; //last shown hadith (used when showing Hadithes sequentially, specially when restarting windows it should continue from last hadith)

extern CString str_hadith_path;
extern CString str_wp_path;

extern Gradient_Type gradient_type;

extern CString lang_names[];
extern std::wstring lang_code[];
extern std::wstring lang_prefix[];
extern ELang active_lang;
extern DWORD rtl_flags;

extern int grad_color_idx;
extern std::vector<ScalarF> GradColors;

//Read color codes from Colors.inf
static void ReadColors()
{
	if (!solid_colors)
		return;
	wchar_t colors_path[MAX_PATH] = { 0 };
	wsprintf(colors_path, L"%s\\Colors.inf", ExePath);
	FILE* fp;
	_wfopen_s(&fp, colors_path, L"rt");
	ScalarF s1, s2;
	if (!fp) {
		s1 = ScalarF(0x28, 0x04, 0);
		s2 = ScalarF(0x92, 0x4E, 0);
		GradColors.push_back(s1);
		GradColors.push_back(s2);
		MessageBoxA(NULL, "File <Colors.inf> not found. Color Background will be disabled.", 0, 0);
		return;
	}
	char line[500] = { 0 };
	while (!feof(fp)) {
		fgets(line, 500, fp); //read a line
		if (strstr(line, "::")) // :: indicates comments
			continue;
		int c1 = 0, c2 = 0;
		if (strstr(line, ",")) {//there are two colores
			sscanf(line, "%x,%x", &c1, &c2);
			s1 = ScalarF((float)(c1 & 0xff), (float)((c1 >> 8) & 0xff), (float)((c1 >> 16) & 0xff));
			s2 = ScalarF((float)(c2 & 0xff), (float)((c2 >> 8) & 0xff), (float)((c2 >> 16) & 0xff));
		}
		else if(strlen(line) > 3)
		{
			sscanf(line, "%x", &c1);
			s1 = s2 = ScalarF((float)(c1 & 0xff), (float)((c1 >> 8) & 0xff), (float)((c1 >> 16) & 0xff));
		}
		GradColors.push_back(s1);
		GradColors.push_back(s2);
	}
}
//Load settings from registery
static void ReadSettings()
{
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameW(hModule, ExePath, MAX_PATH);
	PathRemoveFileSpec(ExePath);

	wchar_t def_path[MAX_PATH] = { 0 };

	slide_show_interval	= theApp.GetProfileString(_T("Settings"), L"slide_show_interval", SLIDE_SHOW_INTERVAL_DEF);
	right_margin		= theApp.GetProfileInt(_T("Settings"), L"right_margin", RIGHT_MARGIN_DEF);
	left_margin			= theApp.GetProfileInt(_T("Settings"), L"left_margin", LEFT_MARGIN_DEF);
	top_margin			= theApp.GetProfileInt(_T("Settings"), L"top_margin", TOP_MARGIN_DEF);
	hotkey_enable		= theApp.GetProfileInt(_T("Settings"), L"hotkey_enable", 1) > 0;
	hotkey_modifier		= theApp.GetProfileInt(_T("Settings"), L"hotkey_modifier", 1);
	hotkey_VK			= theApp.GetProfileInt(_T("Settings"), L"hotkey_VK", 'W');
	opacity				= theApp.GetProfileInt(_T("Settings"), L"opacity", OPACITY_DEF);

	wsprintf(def_path, L"%s\\Wallpapers", ExePath);
	str_wp_path			= theApp.GetProfileString(_T("Settings"), L"wp_path", def_path);

	last_hadith			= theApp.GetProfileInt(_T("Settings"), L"last_hadith", 0) - 1; // will increased before use
	random_hadith		= theApp.GetProfileInt(_T("Settings"), L"random_hadith", RANDOM_HADITH_DEF) > 0;
	random_wp			= theApp.GetProfileInt(_T("Settings"), L"random_wp", RANDOMW_WP_DEF) > 0;
	fixed_image			= theApp.GetProfileInt(_T("Settings"), L"fixed_image", FIXED_IMAGE_DEF) > 0;
	fixed_hadith		= theApp.GetProfileInt(_T("Settings"), L"fixed_hadith", FIXED_HADITH_DEF) > 0;
	stretch_wp			= theApp.GetProfileInt(_T("Settings"), L"stretch_wp", STRETCH_WP_DEF) > 0;
	show_popup			= theApp.GetProfileInt(_T("Settings"), L"show_popup", SHOW_POPUP_DEF) > 0;
	full_width_grad		= theApp.GetProfileInt(_T("Settings"), L"full_width_grad", FULL_WIDTH_GRAD_DEF) > 0;
	solid_colors		= theApp.GetProfileInt(_T("Settings"), L"solid_colors", SOLID_COLORS_DEF) > 0;
	sz_hadith			= theApp.GetProfileInt(_T("Settings"), L"font_size_hadith", FONT_SIZE_HADITH_DEF);
	sz_trans			= theApp.GetProfileInt(_T("Settings"), L"font_size_trans", FONT_SIZE_TRANS_DEF);
	hadith_auto_size	= (sz_hadith < 0);
	trans_auto_size		= (sz_trans < 0);

	clr_hadith			= theApp.GetProfileInt(_T("Settings"), L"hadith_color", HADITH_COLOR_DEF);
	clr_shadow			= theApp.GetProfileInt(_T("Settings"), L"hadith_shadow_color", HADITH_SHADOW_COLOR_DEF);
	clr_trans			= theApp.GetProfileInt(_T("Settings"), L"trans_color", TRANS_COLOR_DEF);
	clr_shadow_trans	= theApp.GetProfileInt(_T("Settings"), L"trans_shadow_color", TRANS_SHADOW_COLOR_DEF);
	clr_gradient		= theApp.GetProfileInt(_T("Settings"), L"gradient_color", GRADIENT_COLOR_DEF);
	enable_shadow		= theApp.GetProfileInt(_T("Settings"), L"hadith_enable_shadow", HADITH_ENABLE_SHADOW_DEF);
	enable_shadow_trans	= theApp.GetProfileInt(_T("Settings"), L"trans_enable_shadow", TRANS_ENABLE_SHADOW_DEF);
	font_style_hadith	= theApp.GetProfileInt(_T("Settings"), L"font_style_hadith", STYLE_HADITH_DEF);
	font_style_trans	= theApp.GetProfileInt(_T("Settings"), L"font_style_trans", STYLE_TRANS_DEF);
	on_logon			= theApp.GetProfileInt(_T("Settings"), L"on_logon", 0);
	gradient_type		= (Gradient_Type)theApp.GetProfileInt(_T("Settings"), L"gradient_type", GRADIENT_DEF);
	active_lang			= (ELang)theApp.GetProfileInt(_T("Settings"), L"active_lang", langFA);	

	wsprintf(def_path, L"%s\\Ahadith\\Ahadith-%s.txt", ExePath, lang_code[active_lang].c_str());
	str_hadith_path = theApp.GetProfileString(_T("Settings"), L"hadith_path", def_path);

	str_cur_image_path = theApp.GetProfileString(_T("Settings"), L"last_wp", L"");
	grad_color_idx	   = theApp.GetProfileInt(_T("Settings"), L"last_color_idx", -1);
	
	if (GradColors.size()/2 <= (size_t)grad_color_idx)
		grad_color_idx = 0;
}

static void SaveSettings()
{
	theApp.WriteProfileString(_T("Settings"), L"wp_path", str_wp_path);
	theApp.WriteProfileString(_T("Settings"), L"hadith_path", str_hadith_path);
	theApp.WriteProfileInt	 (_T("Settings"), L"last_hadith", last_hadith);
	theApp.WriteProfileString(_T("Settings"), L"last_wp", str_cur_image_path.c_str());
	theApp.WriteProfileInt	 (_T("Settings"), L"last_color_idx", grad_color_idx);

	theApp.WriteProfileString(_T("Settings"), L"slide_show_interval", slide_show_interval);
	theApp.WriteProfileInt	 (_T("Settings"), L"right_margin", right_margin);
	theApp.WriteProfileInt	 (_T("Settings"), L"left_margin", left_margin);
	theApp.WriteProfileInt	 (_T("Settings"), L"top_margin", top_margin);
	theApp.WriteProfileInt	 (_T("Settings"), L"random_hadith", random_hadith);
	theApp.WriteProfileInt	 (_T("Settings"), L"fixed_image", fixed_image);
	theApp.WriteProfileInt	 (_T("Settings"), L"fixed_hadith", fixed_hadith);
	theApp.WriteProfileInt	 (_T("Settings"), L"random_wp", random_wp);
	theApp.WriteProfileInt	 (_T("Settings"), L"stretch_wp", stretch_wp);
	theApp.WriteProfileInt	 (_T("Settings"), L"show_popup", show_popup);

	theApp.WriteProfileInt	 (_T("Settings"), L"opacity", opacity);

	theApp.WriteProfileInt	 (_T("Settings"), L"full_width_grad", full_width_grad);
	theApp.WriteProfileInt	 (_T("Settings"), L"solid_colors", solid_colors);


	//Hadith Style Parameters
	theApp.WriteProfileInt	 (_T("Settings"), L"font_style_hadith", font_style_hadith);

	if (hadith_auto_color)
		theApp.WriteProfileInt(_T("Settings"), L"hadith_color", 0xffffffff);
	else
		theApp.WriteProfileInt(_T("Settings"), L"hadith_color", clr_hadith);

	theApp.WriteProfileInt	 (_T("Settings"), L"hadith_enable_shadow", enable_shadow);

	if (shadow_auto_color)
		theApp.WriteProfileInt(_T("Settings"), L"hadith_shadow_color", 0xffffffff);
	else
		theApp.WriteProfileInt(_T("Settings"), L"hadith_shadow_color", clr_shadow);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Trans Style Parameters
	theApp.WriteProfileInt	 (_T("Settings"), L"font_style_trans", font_style_trans);

	if (trans_auto_color)
		theApp.WriteProfileInt(_T("Settings"), L"trans_color", 0xffffffff);
	else
		theApp.WriteProfileInt(_T("Settings"), L"trans_color", clr_trans);

	theApp.WriteProfileInt	 (_T("Settings"), L"trans_enable_shadow", enable_shadow_trans);

	if (shadow_trans_auto_color)
		theApp.WriteProfileInt(_T("Settings"), L"trans_shadow_color", 0xffffffff);
	else
		theApp.WriteProfileInt(_T("Settings"), L"trans_shadow_color", clr_shadow_trans);

	//////////////////////////////////////////////////////////////////////////
	// Gradient Params
	theApp.WriteProfileInt	 (_T("Settings"), L"gradient_color", clr_gradient);

	theApp.WriteProfileInt	 (_T("Settings"), L"gradient_type", (int)gradient_type);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//Font Size Parameters
	if (hadith_auto_size || sz_hadith < 0)
		theApp.WriteProfileInt(_T("Settings"), L"font_size_hadith", -1);
	else
		theApp.WriteProfileInt(_T("Settings"), L"font_size_hadith", sz_hadith);


	if (trans_auto_size || sz_trans < 0)
		theApp.WriteProfileInt(_T("Settings"), L"font_size_trans", -1);
	else
		theApp.WriteProfileInt(_T("Settings"), L"font_size_trans", sz_trans);

	theApp.WriteProfileString(_T("Settings"), L"font_hadith", fName_ar.c_str());

	theApp.WriteProfileString(_T("Settings"), L"font_trans", fName_trans.c_str());

	theApp.WriteProfileInt	 (_T("Settings"), L"on_logon", on_logon);

	theApp.WriteProfileInt	 (_T("Settings"), L"hotkey_modifier", hotkey_modifier);

	theApp.WriteProfileInt	 (_T("Settings"), L"hotkey_VK", hotkey_VK);

	theApp.WriteProfileInt	 (_T("Settings"), L"hotkey_enable", hotkey_enable);

	theApp.WriteProfileInt	 (_T("Settings"), L"active_lang", active_lang);
}

#endif