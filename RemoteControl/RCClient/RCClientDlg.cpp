
// RCClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RCClient.h"
#include "RCClientDlg.h"
#include "afxdialogex.h"
#include "CCliSock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRCClientDlg dialog



CRCClientDlg::CRCClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RCCLIENT_DIALOG, pParent)
	, m_addr_srv(0)
	, m_port_srv(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRCClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS_SRV, m_addr_srv);
	DDX_Text(pDX, IDC_EDIT_PORT, m_port_srv);
	DDX_Control(pDX, IDC_TREE1, m_tree);
}

BEGIN_MESSAGE_MAP(CRCClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_TEST, &CRCClientDlg::OnBnClickedBtnTest)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CRCClientDlg::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BTN_FILEINFO, &CRCClientDlg::OnBnClickedBtnFileinfo)
END_MESSAGE_MAP()


// CRCClientDlg message handlers

BOOL CRCClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	UpdateData();
	m_port_srv = TEXT("7070");
	m_addr_srv = 0x7F000001;
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRCClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRCClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRCClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRCClientDlg::OnBnClickedBtnTest()
{
	// TODO: Add your control notification handler code here

	SendCommandPacket(7777);
}


int CRCClientDlg::SendCommandPacket(const int nCmd, const BYTE* pData, const int nLength) {
	
	UpdateData();

	CCliSocket* pSockCli = CCliSocket::getInstance();
	if (pSockCli->initSocket(m_addr_srv, atoi((LPCTSTR)m_port_srv)) == FALSE) {
		//  ... 
		return -1;
	}
	else {
		AfxMessageBox(TEXT("已连接至服务器"));
		pSockCli->sendACK(CPkt(nCmd, pData, nLength));
		pSockCli->dealRequest();
		TRACE(TEXT("ack : %d \r\n"), pSockCli->getPkt().getCmd());
	}

	return pSockCli->getPkt().getCmd();
}

void CRCClientDlg::OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CRCClientDlg::OnBnClickedBtnFileinfo()
{
	// TODO: Add your control notification handler code here
	CCliSocket* pSockCli = CCliSocket::getInstance();
	int ret = SendCommandPacket(1);
	if (ret == -1) {
		AfxMessageBox(TEXT("OnBnClickedBtnFileinfo()"));
		return;
	}
	std::string drivers = pSockCli->getPkt().getStrData();
	std::string td;
	m_tree.DeleteAllItems();
	for (size_t i = 0; i < drivers.size(); ++i) {
		if (drivers[i] == ',') {
			td += ":";
			m_tree.InsertItem(td.c_str(), TVI_ROOT, TVI_LAST);
			td.clear();
		}
		else {
			td += drivers[i];
		}
	}
}
