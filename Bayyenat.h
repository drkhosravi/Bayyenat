
// Hadith-To-Wallpaper.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBayyenatApp:
// See Hadith-To-Wallpaper.cpp for the implementation of this class
//
enum ELang {langEN, langPO, langFA, langAR};

class CBayyenatApp : public CWinAppEx
{
public:
	CBayyenatApp();
	HINSTANCE m_hDll; // Satellite DLL for multilanguage
	bool restart;
	bool windows_start_up;
// Overrides
public:
	virtual BOOL InitInstance();
	void SetLanguage(ELang lang);
// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};