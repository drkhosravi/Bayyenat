#include "StdAfx.h"
#include "Settings.h"
#include <GdiPlusEnums.h>

UINT			HOTKEY_MODIFIERS[4] = { MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN };
wchar_t			ExePath[MAX_PATH] = { 0 };
wchar_t			str_local_pathW[MAX_PATH] = { 0 };//the path in which wallpaper will be saved (Unicode)
char			str_local_pathA[MAX_PATH] = { 0 };//the path in which wallpaper will be saved (ascii)
char			str_config_path[MAX_PATH];
CString			lang_names[] = { L"English", L"Portugués", L"فارسی", L"العربية" };
std::wstring	lang_code[] = { L"en", L"po", L"fa", L"fa" };//فعلا از همان احادیث فارسی برای عربی هم استفاده می شود
std::wstring	lang_prefix[] = { L"en:", L"po:", L"فا:", L"عر:" };
std::vector<ScalarF> GradColors;
DWORD rtl_flags = 0;
bool dlg_inited = false;//A problem with windows XP


Settings config;