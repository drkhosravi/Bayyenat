#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxmaskededit.h"
// CSettings dialog



class CSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CSettings)

public:
	CSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettings();

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CToolTipCtrl* m_pToolTip;
	void AddToolTip(CWnd* wnd, UINT nIDTxt)
	{
		CString strTmp;
		strTmp.LoadString(nIDTxt);
		m_pToolTip->AddTool(wnd, strTmp);
	}
	wchar_t m_filter_trans[5][40];
	wchar_t m_filter_author[5][40];

	afx_msg void OnBnClickedOk();
	CButton sss;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnApply();
	CComboBox cmb_modifier;
	afx_msg void OnBnClickedChkEnableHotKey();
	afx_msg void OnBnClickedChkRandomHadith();
	afx_msg void OnBnClickedChkRandomImage();
	afx_msg void OnBnClickedChkFixedHadith();
	afx_msg void OnBnClickedChkFixedImg();
};
