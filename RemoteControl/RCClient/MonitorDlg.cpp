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
	, m_nObjWidth(-1)
	, m_nObjHeight(-1)
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
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_STN_CLICKED(IDC_SCREEN, &CMonitorDlg::OnStnClickedScreen)
	ON_BN_CLICKED(IDC_BTN_LOCK, &CMonitorDlg::OnBnClickedBtnLock)
	ON_BN_CLICKED(IDC_BTN_UNLOCK, &CMonitorDlg::OnBnClickedBtnUnlock)
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
	if (m_nObjWidth == -1) {
		m_nObjWidth = pParent->getImage().GetWidth();
	}
	if (m_nObjHeight == -1) {
		m_nObjHeight = pParent->getImage().GetHeight();
	}
	pParent->getImage().StretchBlt(m_picture.GetDC()->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), SRCCOPY);

	m_picture.InvalidateRect(NULL);

	pParent->SetIsFull(FALSE);

	CDialog::OnTimer(nIDEvent);
}

CPoint CMonitorDlg::UserPoint2RemoteScreenPoint(const CPoint& point, BOOL isScreen) {
	
	CPoint cur = point;
	CRect clientRect = {};

	if (isScreen) {
		ScreenToClient(&cur);
	}

	//  将客户区坐标转为服务端全局坐标
	m_picture.GetWindowRect(&clientRect);
	int width0 = clientRect.Width();
	int height0 = clientRect.Height();
	int width1 = m_nObjWidth, height1 = m_nObjHeight;

	int x = cur.x * width1 / width0;
	int y = cur.y * height1 / height0;

	return { x, y };
}


void CMonitorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, FALSE);

		MOUSEVENT mouseEvent(1, 0, remotePoint);
		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}


void CMonitorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, FALSE);

		MOUSEVENT mouseEvent(2, 0, remotePoint);
		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}


void CMonitorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, FALSE);

		MOUSEVENT mouseEvent(3, 0, remotePoint);
		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
	}

	CDialog::OnLButtonUp(nFlags, point);
}


void CMonitorDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, FALSE);

		MOUSEVENT mouseEvent(1, 1, remotePoint);
		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
	}

	CDialog::OnRButtonDblClk(nFlags, point);
}


void CMonitorDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, FALSE);

		MOUSEVENT mouseEvent(2, 1, remotePoint);
		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
	}

	CDialog::OnRButtonDown(nFlags, point);
}


void CMonitorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, FALSE);

		MOUSEVENT mouseEvent(3, 1, remotePoint);
		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
	}

	CDialog::OnRButtonUp(nFlags, point);
}


void CMonitorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

// 	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
// 		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, FALSE);
// 
// 		MOUSEVENT mouseEvent(4, 3, remotePoint);
// 		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
// 		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
// 	}

	CDialog::OnMouseMove(nFlags, point);
}


void CMonitorDlg::OnStnClickedScreen()
{
	// TODO: Add your control notification handler code here
	if ((m_nObjWidth != -1) && (m_nObjHeight != -1)) {
		CPoint point = {};
		GetCursorPos(&point);

		CPoint remotePoint = UserPoint2RemoteScreenPoint(point, TRUE);

		MOUSEVENT mouseEvent(0, 0, remotePoint);
		CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
		pClientDlg->SendMessage(WM_SEND_PACKET, (5 << 1 | 1), (LPARAM)&mouseEvent);
	}
}


void CMonitorDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}


void CMonitorDlg::OnBnClickedBtnLock()
{
	// TODO: Add your control notification handler code here
	CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
	pClientDlg->SendMessage(WM_SEND_PACKET, (7 << 1 | 1));
}


void CMonitorDlg::OnBnClickedBtnUnlock()
{
	// TODO: Add your control notification handler code here
	CRCClientDlg* pClientDlg = (CRCClientDlg*)GetParent();
	pClientDlg->SendMessage(WM_SEND_PACKET, (8 << 1 | 1));
}
