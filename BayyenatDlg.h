
// Hadith-To-WallpaperDlg.h : header file
//

#pragma once
#include "afxcolorbutton.h"
#include <string>
#include "afxfontcombobox.h"
#include "afxmaskededit.h"
#include "SystemTray.h"
#include "afxwin.h"
#include <opencv.hpp>

// CBayyenatDlg dialog
class CBayyenatDlg : public CDialogEx
{
// Construction
public:
	CBayyenatDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BAYYENAT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	

	// Generated message map functions
	virtual BOOL OnInitDialog();

	BOOL ActivateToolTips();

	void LoadParams();

	void UpdateFontList();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CImage img;
	ULONG  _jpeg_quality;

	CToolTipCtrl* m_pToolTip;
	void AddToolTip(CWnd* wnd, UINT nIDTxt)
	{
		CString strTmp;
		strTmp.LoadString(nIDTxt);
		m_pToolTip->AddTool(wnd, strTmp);
	}
	CMFCToolTipInfo params;

	CString m_strWallpapers;
	FILE* m_fp_hadith;
	CSystemTray m_TrayIcon;
	CComboBox fnt_cmb_hadith, fnt_cmb_trans;	
	CMFCColorButton m_clrShadowTrans;
	CMFCButton m_btnStart;
	CMFCColorButton m_clrGrad;
	CComboBox m_cmbGrad;
	CMFCColorButton m_clrHadith;
	CMFCColorButton m_clrTrans;
	CMFCColorButton m_clrShadow;

	CBitmap bmpStart, bmpStop, bmpMain, bmpClose, bmpChange;

	void ApplySettings();
	void GenerateHadithWp(std::wstring img_path);

	Gdiplus::RectF BoundRect(Gdiplus::RectF rc, Gdiplus::Graphics* graphics, Gdiplus::StringFormat& fmt_rtl, int imH, int imW);
	static void FillBackground(Gdiplus::Graphics* graphics, Gdiplus::RectF& box, cv::Mat& im, CImage& _img);

	void SetNextWallpaper();
	void DrawHeaderandFooter();
	void SetNewHotKey();

	afx_msg void OnBnClickedStart();

	void ProcessHadithFile();

	afx_msg void OnBnClickedGoToTray();
	afx_msg void OnBnClickedHadithAutoColor();
	afx_msg void OnBnClickedTransAutoColor();
	//afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedTextProcessing();
	afx_msg void OnBnClickedChkEnableShadow();
	afx_msg void OnBnClickedChkEnableShadowTrans();
	afx_msg void OnBnClickedChkSizeHadithAuto();
	afx_msg void OnBnClickedChkSizeTransAuto();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPopupShowMainWindow();
	afx_msg void OnBnClickedClose();
	afx_msg void OnChangeWP();
	afx_msg void OnEnChangeEdtFnSizeHadith();
	afx_msg void OnEnChangeEdtFnSizeTrans();
	afx_msg void OnPopupStart();
	afx_msg void OnUpdatePopupStart(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedTray();
	afx_msg void OnBnClickedMinimize();
	afx_msg void OnBnClickedResetDefaults();
	afx_msg void OnBnClickedAbout();
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg void OnNcPaint();
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnEnChangeHadithFile();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedChkRandomHadith();
	afx_msg void OnBnClickedSaveWp();
	afx_msg void OnBnClickedChkFixedHadith();
	afx_msg void OnBnClickedChkFixedImg();
	afx_msg void OnBnClickedBtnSettings();
	afx_msg void OnBnClickedBtnApplyStyle();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnBnClickedChkSolidColor();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CComboBox m_cmbLng;
	afx_msg void OnCbnSelchangeCmbLng();
	afx_msg void OnEnChangeWpPath();
	afx_msg void OnBnClickedCopyText();
	afx_msg void OnBnClickedCopyImage();
	CComboBox m_cmb_hadith_files;
	afx_msg void OnCbnSelchangeCmbHadithFiles();
};
