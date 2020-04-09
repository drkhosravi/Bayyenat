#pragma once
#include "resource.h"

// CSplashWnd dialog

class CSplashWnd : public CDialog
{
	DECLARE_DYNAMIC(CSplashWnd)

	//public:
	//CSplashWnd(CWnd* pParent = NULL);   // standard constructor
	// Construction
public:
	BOOL Create(CWnd* pParent);

	// Dialog Data
	enum { IDD = IDD_SPLASH };

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};
