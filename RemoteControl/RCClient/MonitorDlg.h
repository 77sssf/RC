#pragma once


// CMonitor dialog

class CMonitorDlg : public CDialog
{
	DECLARE_DYNAMIC(CMonitorDlg)

public:
	CMonitorDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMonitorDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SCREEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
};
