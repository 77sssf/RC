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
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_STN_CLICKED(IDC_SCREEN, &CMonitorDlg::OnStnClickedScreen)
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

CPoint CMonitorDlg::UserPoint2RemoteScreenPoint(const CPoint& point) {
	CPoint cur = point;
	CRect clientRect = {};
	ScreenToClient(&cur);

	//  将客户区坐标转为服务端全局坐标
	m_picture.GetWindowRect(&clientRect);
	int width0 = clientRect.Width();
	int height0 = clientRect.Height();
	int width1 = 1920, height1 = 1080;	//  TO DO : 

	int x = cur.x * width1 / width0;
	int y = cur.y * height1 / height0;

	return { x, y };
}


void CMonitorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint remotePoint = UserPoint2RemoteScreenPoint(point);

	MOUSEVENT mouseEvent(1, 0, remotePoint);
	CCliSocket::getInstance()->sendACK(CPkt(5, (BYTE*)&mouseEvent, sizeof(mouseEvent)));
	
	CDialog::OnLButtonDblClk(nFlags, point);
}


void CMonitorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint remotePoint = UserPoint2RemoteScreenPoint(point);

	MOUSEVENT mouseEvent(2, 0, remotePoint);
	CCliSocket::getInstance()->sendACK(CPkt(5, (BYTE*)&mouseEvent, sizeof(mouseEvent)));

	CDialog::OnLButtonDown(nFlags, point);
}


void CMonitorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint remotePoint = UserPoint2RemoteScreenPoint(point);

	MOUSEVENT mouseEvent(3, 0, remotePoint);
	CCliSocket::getInstance()->sendACK(CPkt(5, (BYTE*)&mouseEvent, sizeof(mouseEvent)));

	CDialog::OnLButtonUp(nFlags, point);
}


void CMonitorDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint remotePoint = UserPoint2RemoteScreenPoint(point);

	MOUSEVENT mouseEvent(1, 1, remotePoint);
	CCliSocket::getInstance()->sendACK(CPkt(5, (BYTE*)&mouseEvent, sizeof(mouseEvent)));

	CDialog::OnRButtonDblClk(nFlags, point);
}


void CMonitorDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint remotePoint = UserPoint2RemoteScreenPoint(point);

	MOUSEVENT mouseEvent(2, 1, remotePoint);
	CCliSocket::getInstance()->sendACK(CPkt(5, (BYTE*)&mouseEvent, sizeof(mouseEvent)));

	CDialog::OnRButtonDown(nFlags, point);
}


void CMonitorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CPoint remotePoint = UserPoint2RemoteScreenPoint(point);

	MOUSEVENT mouseEvent(3, 1, remotePoint);
	CCliSocket::getInstance()->sendACK(CPkt(5, (BYTE*)&mouseEvent, sizeof(mouseEvent)));

	CDialog::OnRButtonUp(nFlags, point);
}


void CMonitorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnMouseMove(nFlags, point);
}


void CMonitorDlg::OnStnClickedScreen()
{
	// TODO: Add your control notification handler code here
	
	CPoint point = {};
	GetCursorPos(&point);

	CPoint remotePoint = UserPoint2RemoteScreenPoint(point);

	MOUSEVENT mouseEvent(0, 0, remotePoint);
	CCliSocket::getInstance()->sendACK(CPkt(5, (BYTE*)&mouseEvent, sizeof(mouseEvent)));
}
