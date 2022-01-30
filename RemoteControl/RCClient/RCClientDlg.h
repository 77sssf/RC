
// RCClientDlg.h : header file
//

#pragma once


// CRCClientDlg dialog
class CRCClientDlg : public CDialogEx
{
// Construction
public:
	CRCClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RCCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	int SendCommandPacket(const int, const BYTE* = 0, const int = 0);

public:
	afx_msg void OnBnClickedBtnTest();
	DWORD m_addr_srv;
	CString m_port_srv;
	afx_msg void OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnFileinfo();
	CTreeCtrl m_tree;
};
