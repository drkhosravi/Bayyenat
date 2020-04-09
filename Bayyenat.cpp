
// Bayyenat.cpp : Defines the class behaviors for the application.
// Hossein Khosravi

#include "stdafx.h"
#include "Bayyenat.h"
#include "BayyenatDlg.h"
#include "Splash.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(linker, "/SECTION:.shr,RWS")
#pragma data_seg(".shr")
HWND g_hWnd = NULL;
#pragma data_seg()

CSplashWnd splash;
// CBayyenatApp
BEGIN_MESSAGE_MAP(CBayyenatApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinAppEx::OnHelp)
END_MESSAGE_MAP()


// CBayyenatApp construction

CBayyenatApp::CBayyenatApp()
{
	restart = false;
	windows_start_up = false;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBayyenatApp object
HANDLE hMutexOneInstance;
CBayyenatApp theApp;
wchar_t		str_local_pathW[MAX_PATH] = {0};//the path in which wallpaper will be saved (Unicode)
char		str_local_pathA[MAX_PATH] = { 0 };//the path in which wallpaper will be saved (ascii)

void CBayyenatApp::SetLanguage(ELang lang)
{
	HINSTANCE hDll = 0;
	if (lang == langFA)
	{
		hDll = AfxGetApp()->m_hInstance;
	}
	else if (lang == langEN)
	{
		hDll = LoadLibrary(L"BayyenatEN.dll");
	}
	else if (lang == langES)
	{
		hDll = LoadLibrary(L"BayyenatSPA.dll");
	}
	else if (lang == langPO)
	{
		hDll = LoadLibrary(L"BayyenatPOR.dll");
	}
	else if (lang == langAR)
	{
		hDll = LoadLibrary(L"BayyenatAR.dll");
	}
	if (hDll != NULL)
	{
		//Unload Resource Dll (Unloads possibly currently loaded Resource DLL)
		if (m_hDll != NULL)
		{
			// Restore the EXE as the resource container.
			// Tells MFC where the resources are stored.
			AfxSetResourceHandle(AfxGetApp()->m_hInstance);
#if _MFC_VER>=0x0700
			_AtlBaseModule.SetResourceInstance(AfxGetApp()->m_hInstance);
#endif
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}

		// Set loaded DLL as default resource container
		m_hDll = hDll;
		AfxSetResourceHandle(m_hDll);
#if _MFC_VER>=0x0700
		_AtlBaseModule.SetResourceInstance(m_hDll);
#endif
	}

	if (active_lang == langFA || active_lang == langAR)
		rtl_flags = MB_RTLREADING | MB_RIGHT;
	else
		rtl_flags = 0;
}
// CBayyenatApp initialization
BOOL CBayyenatApp::InitInstance()
{

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();
	
	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	//CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	//////////////////////////////////////////////////////////////////////////
	//For single instance checking
	hMutexOneInstance = ::CreateMutex( NULL, TRUE, _T("Bayyenat-Wallpaper-AndishehOnline.ir-1395")); 
	bool AlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS); 
	if (hMutexOneInstance != NULL) 
		::ReleaseMutex(hMutexOneInstance);
	if (AlreadyRunning)
	{
		if (g_hWnd) /* pop up */
		{ 
			::SetForegroundWindow(g_hWnd);
			//if (IsIconic(g_hWnd)) ::ShowWindow(g_hWnd, SW_RESTORE);
			::ShowWindow(g_hWnd, SW_SHOW);
		}
		else
			AfxMessageBox(L"Already running (see notification bar)");

		return FALSE; // terminates the creation
	}
	//////////////////////////////////////////////////////////////////////////

	// Change the registry key under which our settings are stored
	//SetRegistryKey(_T("Andisheh Online"));
	//To save settings in ini file instead of registry we must not call SetRegistryKey and to specify the path of ini file we must do the following

	//Construct a directory for settings and wallpaper ( C:\Users\Mahdiar\AppData\Local\BayyenatWP )
	//Note that in win 7 we cannot write directly in C: drive and must write in user profile directory
	wchar_t sw[MAX_PATH] = {0};
	GetTempPath(MAX_PATH, sw);
	if( sw [wcslen(sw)-1] == L'\\') 
		sw [wcslen(sw)-1] = 0;
	wchar_t* p = wcsrchr(sw, L'\\');
	sw [ p-sw ] = 0;
	wsprintf(str_local_pathW, L"%s\\%s", sw, L"BayyenatWP");
	CreateDirectory(str_local_pathW, 0);
	
	WideCharToMultiByte(CP_ACP, 0, str_local_pathW, -1, str_local_pathA, wcslen(str_local_pathW), NULL, NULL);

	wchar_t full_path[MAX_PATH] = {0};
	wsprintf(full_path, L"%s\\%s", str_local_pathW, L"BayyenatWP.ini");
	/*GetFullPathName(_T(), MAX_PATH, full_path, 0);*/
	free((void*)m_pszProfileName);
	m_pszProfileName = ::_tcsdup(full_path);
	//InitTooltipManager ();//it seems not to work in dialogs

	/////////////////////////
	ReadSettings();
	SetLanguage(active_lang);
	/////////////////////////

	wchar_t* cmd = GetCommandLine();
	wchar_t* arg = wcsrchr(cmd, L' ');
	if(wcsstr(arg, L"tray") && (on_logon == 2)/*don't start*/)
		exit(0);
	if(wcsstr(arg, L"tray") && (on_logon != 1)/*just change wp, so don't show splash*/)
	{
		splash.Create(AfxGetMainWnd());
		splash.ShowWindow(SW_SHOW);
	}

	dlg_inited = false;
	CBayyenatDlg dlg;
	m_pMainWnd = &dlg;
	//dlg.ModifyStyleEx(WS_EX_LAYOUTRTL, 0);
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}
	/*else if (nResponse == 20)//change language
	{		
		CString str;
		str.LoadString(IDS_CHANGE_HADITH_FILE);
		MessageBox(NULL, str, L":::", MB_ICONINFORMATION);
		SetLanguage(active_lang);
	}*/
	// Delete the shell manager created above.
	/*if (pShellManager != NULL)
	{
		delete pShellManager;
	}*/

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}



int CBayyenatApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	if (restart)
	{
		CloseHandle(hMutexOneInstance);
		wchar_t fn[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, fn, MAX_PATH);
		::ShellExecute(NULL, _T("open"), fn, NULL, NULL, SW_RESTORE);
	}
	return CWinAppEx::ExitInstance();
}
