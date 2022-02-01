// Monitor.cpp : implementation file
//

#include "pch.h"
#include "RCClient.h"
#include "RCClientDlg.h"
#include "MonitorDlg.h"
#include "afxdialogex.h"

// CMonitor dialog

IMPLEMENT_DYNAMIC(CMonitorDlg, CDialog)

CMonitorDlg::CMonitorDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_SCREEN, pParent)
{

}

CMonitorDlg::~CMonitorDlg()
{
}

void CMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCREEN, m_picture);
}


BEGIN_MESSAGE_MAP(CMonitorDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMonitor message handlers

BOOL CMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	SetTimer(0, 30, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent != 0) {
		CDialog::OnTimer(nIDEvent);
		return;
	}

	CRCClientDlg* pParent = (CRCClientDlg*)GetParent();

	if (pParent->getIsFull() == FALSE) {
		return;
	}
	
	//  得到picture 控件的DC
	CRect rect = {};
	m_picture.GetWindowRect(&rect);
	//pParent->getImage().BitBlt(m_picture.GetDC()->GetSafeHdc(), 0, 0, SRCCOPY);	//
	//  大小不适配
	pParent->getImage().StretchBlt(m_picture.GetDC()->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), SRCCOPY);

	m_picture.InvalidateRect(NULL);

	pParent->getImage().Destroy();
	pParent->SetIsFull(FALSE);

	CDialog::OnTimer(nIDEvent);
}