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
	CStatic m_picture;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	CPoint UserPoint2RemoteScreenPoint(const CPoint& point);
public:
	afx_msg void OnStnClickedScreen();
};
