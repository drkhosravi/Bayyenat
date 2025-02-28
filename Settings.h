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

#define LEFT_MARGIN_DEF					20
#define TOP_MARGIN_DEF					20
#define RIGHT_MARGIN_DEF				20
#define GLOW_SIZE						7
#define SHADOW_OFF						3
#define RANDOM_HADITH_DEF				1
#define FIXED_IMAGE_DEF					0
#define FIXED_HADITH_DEF				0
#define RANDOMW_WP_DEF					1
#define STRETCH_WP_DEF					1
#define SHOW_POPUP_DEF					1

#define SLIDE_SHOW_INTERVAL_DEF			60//minutes
#define FONT_HADITH_DEF					L"Neirizi"
#define FONT_TRANS_DEF					L"IRANSansXFaNum"
#define OPACITY_DEF						40
#define FULL_WIDTH_GRAD_DEF				0
#define SOLID_COLORS_DEF				0
#define FONT_SIZE_HADITH_DEF			30
#define FONT_SIZE_TRANS_DEF				40
#define HADITH_COLOR_DEF				0x000000
#define TRANS_COLOR_DEF					0x00007F //Red
#define HADITH_ENABLE_SHADOW_DEF		1
#define HADITH_SHADOW_COLOR_DEF			0x90C090
#define HADITH_ENABLE_GLOW_DEF			0
#define HADITH_GLOW_COLOR_DEF			0x80FFFF
#define TRANS_ENABLE_SHADOW_DEF			0
#define TRANS_SHADOW_COLOR_DEF			0xA0A0A0
#define TRANS_ENABLE_GLOW_DEF			1
#define TRANS_GLOW_COLOR_DEF			0x80FFFF
#define GRADIENT_COLOR_DEF				0x000000
#define STYLE_HADITH_DEF				FontStyleRegular
#define STYLE_TRANS_DEF					FontStyleBold
#define	GRADIENT_DEF					gtNone

enum		Gradient_Type		  {gtFixedColor, gtFixedColorAuto, gtBlur, gtVertical1 , gtVertical2, gtHorizon1, gtHorizon2, gtPath1, gtPath2, gtBellShape1, gtBellShape2, gtEllipse, gtNone, gtCount};

extern CBayyenatApp theApp;

//TODO: تنظیمات رو در یک فایل باینری ذخیره و بازیابی کن
struct Settings
{
	bool stretch_wp = STRETCH_WP_DEF;
	bool show_popup = SHOW_POPUP_DEF;//enable balloon tips
	bool random_hadith = RANDOM_HADITH_DEF;
	bool fixed_image = FIXED_IMAGE_DEF;
	bool fixed_hadith = FIXED_HADITH_DEF;
	bool random_wp = RANDOMW_WP_DEF;

	short right_margin = RIGHT_MARGIN_DEF;
	short left_margin = LEFT_MARGIN_DEF; //in %
	short top_margin = TOP_MARGIN_DEF;
	short shadow_off_x = 5;
	short shadow_off_y = 5;
	short glow_size = 7;

	int on_logon = 0; // what to do on windows logon(0 start, 1 change aand close, 2 don't start)

	char hotkey_VK = 'W';
	UINT hotkey_modifier = 0;//index of selected key in combo box
	bool hotkey_enable = true;
	short slide_show_interval = SLIDE_SHOW_INTERVAL_DEF;


	bool hadith_auto_color = false;	
	bool hadith_shadow_auto_color = false;
	bool hadith_glow_auto_color = false;

	bool trans_auto_color = false;
	bool trans_shadow_auto_color = false;
	bool trans_glow_auto_color = false;


	bool full_width_grad = false;
	bool solid_colors = false;
	short  font_style_hadith = FontStyleRegular;
	short  font_style_trans = FontStyleRegular;
	bool hadith_auto_size = false;
	bool trans_auto_size = false;

	//سایه و درخشش
	bool enable_shadow = 0;
	bool enable_shadow_trans = 0;
	bool enable_glow = 0;
	bool enable_glow_trans = 0;

	float sz_hadith = 20; //font size of the Hadith
	float sz_trans = 20;  //font size of translation
	wchar_t fName_ar[50] = FONT_HADITH_DEF;
	wchar_t fName_trans[50] = FONT_TRANS_DEF;

	COLORREF clr_hadith_text = 0x000000;
	COLORREF clr_hadith_shadow = 0xffffff;
	COLORREF clr_hadith_glow = 0x777777;//درخشش اطراف متن

	COLORREF clr_trans_text = 0x000000;
	COLORREF clr_trans_shadow = 0xffffff;
	COLORREF clr_trans_glow = 0x777777; //درخشش

	COLORREF clr_gradient = 0xffffffff;

	short opacity = 20;//0 - 255
	short grad_color_idx = -1;
	int last_hadith = -1; //last shown hadith (used when showing Hadithes sequentially, specially when restarting windows it should continue from last hadith)

	wchar_t str_hadith_path[MAX_PATH]; //مسیر فایل احادیث
	wchar_t str_wp_path[MAX_PATH]; //مسیر تصاویر زمینه

	wchar_t str_cur_image_path[MAX_PATH];//آخرین تصویر انتخابی	


	Gradient_Type gradient_type = gtFixedColorAuto;

	ELang active_lang = langFA;
};

extern Settings config;

extern UINT		HOTKEY_MODIFIERS[4];// = { MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN };
extern wchar_t	ExePath[MAX_PATH];
extern wchar_t	str_local_pathW[MAX_PATH];//the path in which wallpaper will be saved (unicode)
extern char		str_local_pathA[MAX_PATH];//the path in which wallpaper will be saved (ascii)
extern char		str_config_path[MAX_PATH];
extern CString	lang_names[];
extern std::wstring lang_code[];
extern std::wstring lang_prefix[];
extern std::vector<ScalarF> GradColors;
extern DWORD rtl_flags;
extern bool dlg_inited;

//Read color codes from Colors.inf
static void ReadColors()
{
	if (!config.solid_colors)
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
static bool ReadSettings()
{
	bool result = true;
	FILE* fp = fopen(str_config_path, "rb");
	if (!fp)
	{
		MessageBoxA(NULL, "File <BayyenatWP.dat> not found. all configs will be set to DEFAULTS.", 0, 0);
		result = false;
	}
	else
	{
		fread(&config, sizeof(Settings), 1, fp);
		fclose(fp);
	}
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameW(hModule, ExePath, MAX_PATH);
	PathRemoveFileSpec(ExePath);

	wchar_t def_path[MAX_PATH] = { 0 };

	wsprintf(def_path, L"%s\\Wallpapers", ExePath);

	wsprintf(def_path, L"%s\\Ahadith\\Ahadith-%s.txt", ExePath, lang_code[config.active_lang].c_str());
	
	if (GradColors.size()/2 <= (size_t)config.grad_color_idx)
		config.grad_color_idx = 0;

	return result;
}

static void SaveSettings()
{
	FILE* fp = fopen(str_config_path, "r+b");
	if (!fp)
		fp = fopen(str_config_path, "wb");
	if (!fp)
	{
		MessageBoxA(NULL, "File <BayyenatWP.dat> cannot be read/created.", 0, 0);
	}
	else
	{
		fseek(fp, 0, SEEK_SET);
		fwrite(&config, sizeof(Settings), 1, fp);
		fclose(fp);
	}
}

#endif