
// RCClientDlg.h : header file
//

#pragma once

#include "CCliSock.h"
#include "StatusDlg.h"
#include "MonitorDlg.h"

#define WM_SEND_PACKET (WM_USER + 1)

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
	int SendCommandPacket(const int, BOOL autoClose = TRUE, const BYTE* = 0, const int = 0);
	CString GetPath(HTREEITEM hTreeItem);
	void DeleteSelectChildItem(HTREEITEM hTreeSelected);
	void LoadFileInfo();
	void LoadFileCurrent();
	static void ThreadEntryForDownloadFile(void* arg);
	void ThreadDownloadFile();
	
	afx_msg LRESULT OnSendPacket(WPARAM wParam, LPARAM lParam);
	
	static void ThreadEntryForWatchData(void* args);
	void ThreadWatchData();

public:
	BOOL getIsFull() const ;
	CImage& getImage();

private:
	CStatusDlg m_statudDlg;
	BOOL m_IsFull;
	CImage m_image;

public:
	afx_msg void OnBnClickedBtnTest();
	DWORD m_addr_srv;
	CString m_port_srv;
	afx_msg void OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnFileinfo();
	CTreeCtrl m_tree;
	afx_msg void OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_list;
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDownload();
	afx_msg void OnDelete();
	afx_msg void OnOpen();
	afx_msg void OnBnClickedBtnMonitor();
};
