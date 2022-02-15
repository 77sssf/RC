
// RCClientDlg.cpp : implementation file
//

#include "pch.h"

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
public:
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
	, m_IsFull(FALSE)
	, m_IsDlgClosed(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRCClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS_SRV, m_addr_srv);
	DDX_Text(pDX, IDC_EDIT_PORT, m_port_srv);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CRCClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_TEST, &CRCClientDlg::OnBnClickedBtnTest)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CRCClientDlg::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BTN_FILEINFO, &CRCClientDlg::OnBnClickedBtnFileinfo)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CRCClientDlg::OnNMDblclkTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CRCClientDlg::OnNMClickTree1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CRCClientDlg::OnNMRClickList1)
	ON_COMMAND(ID_DOWNLOAD, &CRCClientDlg::OnDownload)
	ON_COMMAND(ID_DELETE, &CRCClientDlg::OnDelete)
	ON_COMMAND(ID_OPEN, &CRCClientDlg::OnOpen)
	ON_MESSAGE(WM_SEND_PACKET, &CRCClientDlg::OnSendPacket)
	ON_BN_CLICKED(IDC_BTN_MONITOR, &CRCClientDlg::OnBnClickedBtnMonitor)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_SRV, &CRCClientDlg::OnIpnFieldchangedIpaddressSrv)
	ON_EN_CHANGE(IDC_EDIT_PORT, &CRCClientDlg::OnEnChangeEditPort)
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

	UpdateData();
	CClientController::getInstance()->UpdateAddress(m_addr_srv, atoi((LPCTSTR)m_port_srv));

	m_statusDlg.Create(IDD_DLG_STATUS, this);
	m_statusDlg.ShowWindow(SW_HIDE);

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
	CClientController::getInstance()->SendCommandPacket(7777);
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
	std::list<CPkt> lstRecved;
	int ret = CClientController::getInstance()->SendCommandPacket(1, TRUE, NULL, 0, &lstRecved);
	if (ret == -1 || lstRecved.size() < 0) {
		AfxMessageBox(TEXT("OnBnClickedBtnFileinfo()"));
		return;
	}
	std::string drivers = lstRecved.front().getStrData();
	std::string td;
	m_tree.DeleteAllItems();
	for (size_t i = 0; i < drivers.size(); ++i) {
		if (drivers[i] == ',') {
			td += ":";
			HTREEITEM tmp = m_tree.InsertItem(td.c_str(), TVI_ROOT, TVI_LAST);
			m_tree.InsertItem(NULL, tmp, TVI_LAST);
			td.clear();
		}
		else {
			td += drivers[i];
		}
	}
}

void CRCClientDlg::DeleteSelectChildItem(HTREEITEM hTreeSelected) {
	HTREEITEM hSub = NULL;
	do {
		hSub = m_tree.GetChildItem(hTreeSelected);
		if (hSub) m_tree.DeleteItem(hSub);
	} while (hSub);
}


CString CRCClientDlg::GetPath(HTREEITEM hTreeItem) {
	CString res, tmp;
	do {
		tmp = m_tree.GetItemText(hTreeItem);
		res = tmp + "\\" + res;
		hTreeItem = m_tree.GetParentItem(hTreeItem);
	} while (hTreeItem);
	return res;
}

void CRCClientDlg::OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	LoadFileInfo();
}

void CRCClientDlg::LoadFileInfo() {
	CPoint ptMouse = {};
	GetCursorPos(&ptMouse);	//  全局坐标
	m_tree.ScreenToClient(&ptMouse);
	HTREEITEM hTreeSelected = m_tree.HitTest(ptMouse, 0);
	if (hTreeSelected == NULL) {
		return;
	}
	if (m_tree.GetChildItem(hTreeSelected) == NULL) {
		//  没有子节点说明为文件
		return;
	}

	DeleteSelectChildItem(hTreeSelected);
	m_list.DeleteAllItems();

	CString strPath = GetPath(hTreeSelected);
	std::list<CPkt> lstRecved;
	int nCmd = CClientController::getInstance()->SendCommandPacket(2, FALSE, (const BYTE*)(LPCTSTR)strPath, strPath.GetLength(), &lstRecved);
	if (nCmd == -1) {
		//
	}

	BOOL Insert = TRUE;
	if (lstRecved.size() == 0) {
		CClientController::getInstance()->closeSock();
		return;
	}
	std::string t = lstRecved.front().getStrData();
	lstRecved.pop_front();
	FILEINFO fInfo = {};
	memcpy(&fInfo, t.c_str(), t.size());

	//  fInfo.IsDirectory == TRUE
	//  双击后若为文件, 服务端findfirst返回-1, 发送Invalid包至客户端
	//  修改服务端代码, 若为Invalid, 还应标记空或者文件

	while (fInfo.HasNext) {

		if (fInfo.IsDirectory) {
			if (!strcmp(fInfo.szFileName, ".") || !strcmp(fInfo.szFileName, "..")) {	//  名为.或..不插入
				Insert = FALSE;
			}
		}

		if (fInfo.IsInvalid == FALSE && Insert == TRUE) {
			if (fInfo.IsDirectory) {
				HTREEITEM tmp = m_tree.InsertItem(fInfo.szFileName, hTreeSelected, TVI_LAST);
				if (tmp == NULL) {
					//  
				}
				if (fInfo.IsDirectory && tmp) {
					m_tree.InsertItem("", tmp, TVI_LAST);
				}
			}
			else {
				m_list.InsertItem(0, fInfo.szFileName);	// 0代表第一列
			}
		}

		if (lstRecved.size() == 0) {
			break;
		}
		t = lstRecved.front().getStrData();
		lstRecved.pop_front();
		fInfo = {};
		memcpy(&fInfo, t.c_str(), t.size());
		Insert = TRUE;
	}

	CClientController::getInstance()->closeSock();
}

void CRCClientDlg::LoadFileCurrent() {
	HTREEITEM hTree = m_tree.GetSelectedItem();
	CString filePath = GetPath(hTree);

	DeleteSelectChildItem(hTree);
	m_list.DeleteAllItems();

	CCliSocket* pSockCli = CCliSocket::getInstance();
	int nCmd = CClientController::getInstance()->SendCommandPacket(2, FALSE, (const BYTE*)(LPCTSTR)filePath, filePath.GetLength());
	if (nCmd == -1) {

	}

	BOOL Insert = TRUE;
	//PFILEINFO pInfo = (PFILEINFO)pSockCli->getPkt().getStrData().c_str();	//  指向临时对象
	std::string t = pSockCli->getPkt().getStrData();
	FILEINFO fInfo = {};
	memcpy(&fInfo, t.c_str(), t.size());

	//  fInfo.IsDirectory == TRUE
	//  双击后若为文件, 服务端findfirst返回-1, 发送Invalid包至客户端
	//  修改服务端代码, 若为Invalid, 还应标记空或者文件

	while (fInfo.HasNext) {

		if (fInfo.IsDirectory) {
			if (!strcmp(fInfo.szFileName, ".") || !strcmp(fInfo.szFileName, "..")) {	//  名为.或..不插入
				Insert = FALSE;
			}
		}

		if (fInfo.IsInvalid == FALSE && Insert == TRUE) {
			if (fInfo.IsDirectory) {
				HTREEITEM tmp = m_tree.InsertItem(fInfo.szFileName, hTree, TVI_LAST);
				if (tmp == NULL) {
					//  
				}
				if (fInfo.IsDirectory && tmp) {
					m_tree.InsertItem("", tmp, TVI_LAST);
				}
			}
			else {
				m_list.InsertItem(0, fInfo.szFileName);	// 0代表第一列
			}
		}

		CClientController::getInstance()->DealRequest();

		t = pSockCli->getPkt().getStrData();
		fInfo = {};
		memcpy(&fInfo, t.c_str(), t.size());
		Insert = TRUE;
	}

	pSockCli->closeSock();

}


void CRCClientDlg::OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	LoadFileInfo();
}


void CRCClientDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CPoint ptMouse = {}, ptList{};
	GetCursorPos(&ptMouse);
	ptList = ptMouse;
	m_list.ScreenToClient(&ptList);
	int listSelected = m_list.HitTest(ptList);
	if (listSelected < 0) {
		return;
	}

	CMenu menu;
	menu.LoadMenuA(IDR_MENU_RCLICK);
	CMenu* pPopup = menu.GetSubMenu(0);
	if (pPopup) {
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, this);
	}
}

void CRCClientDlg::OnDownload()
{
	//  TODO: Add your command handler code here
	//  获取服务端文件路径
	int nListSelected = m_list.GetSelectionMark();
	CString fileName = m_list.GetItemText(nListSelected, 0);
	HTREEITEM hSelected = m_tree.GetSelectedItem();
	CString filePath = GetPath(hSelected);
	filePath = filePath + fileName;
	//  获取客户端文件路径
	CFileDialog dlg(FALSE, "*", fileName, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, NULL, this);
	if (dlg.DoModal() != IDOK) {
		return;
	}

	int ret = CClientController::getInstance()->DownloadFile(filePath, dlg.GetPathName());
	if (ret < 0) {
			
	}
}


void CRCClientDlg::OnDelete()
{
	// TODO: Add your command handler code here
	HTREEITEM hTree = m_tree.GetSelectedItem();
	CString filePath = GetPath(hTree);
	int nListSelected = m_list.GetSelectionMark();
	CString fileName = m_list.GetItemText(nListSelected, 0);

	filePath = filePath + fileName;
	int ret = CClientController::getInstance()->SendCommandPacket(9, TRUE, (BYTE*)(LPCTSTR)filePath, filePath.GetLength());

	if (ret < 0) {
		AfxMessageBox(TEXT("删除文件命令失败"));
		return;
	}

	LoadFileCurrent();
}


void CRCClientDlg::OnOpen()
{
	// TODO: Add your command handler code here
	HTREEITEM hTree = m_tree.GetSelectedItem();
	CString filePath = GetPath(hTree);
	int nListSelected = m_list.GetSelectionMark();
	CString fileName = m_list.GetItemText(nListSelected, 0);

	filePath = filePath + fileName;

	int ret = CClientController::getInstance()->SendCommandPacket(3, TRUE, (BYTE*)(LPCTSTR)filePath, filePath.GetLength());

	if (ret < 0) {
		AfxMessageBox(TEXT("打开文件命令失败"));
		return;
	}
}

afx_msg LRESULT CRCClientDlg::OnSendPacket(WPARAM wParam, LPARAM lParam) {
	
	int cmd = wParam >> 1;
	BOOL a = (BOOL)(wParam & 0x1);
	int ret = 0;

	switch (cmd)
	{
	case 4: {
		CString filePath = (LPCTSTR)lParam;
		ret = CClientController::getInstance()->SendCommandPacket(cmd, a, (BYTE*)(LPCTSTR)filePath, filePath.GetLength());
		break;
	}
	case 5: {
		ret = CClientController::getInstance()->SendCommandPacket(cmd, a, (BYTE*)lParam, sizeof(MOUSEVENT));
		break;
	}
	case 6: {
		ret = CClientController::getInstance()->SendCommandPacket(cmd, a, NULL, 0);
		break;
	}
	case 7: {
		ret = CClientController::getInstance()->SendCommandPacket(cmd, a, NULL, 0);
		break;
	}
	case 8: {
		ret = CClientController::getInstance()->SendCommandPacket(cmd, a, NULL, 0);
		break;
	}
	default:
		break;
	}
	
	return ret;
}


void CRCClientDlg::OnBnClickedBtnMonitor()
{
	// TODO: Add your control notification handler code here
	CClientController::getInstance()->OpenMonitor();
}

BOOL CRCClientDlg::getIsFull() const {
	return m_IsFull;
}

CImage& CRCClientDlg::getImage() {
	return m_image;
}

BOOL CRCClientDlg::SetIsFull(BOOL b) {
	m_IsFull = b;
	return TRUE;
}

void CRCClientDlg::OnIpnFieldchangedIpaddressSrv(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateData();
	CClientController::getInstance()->UpdateAddress(m_addr_srv, atoi((LPCTSTR)m_port_srv));
	*pResult = 0;
}


void CRCClientDlg::OnEnChangeEditPort()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData();
	CClientController::getInstance()->UpdateAddress(m_addr_srv, atoi((LPCTSTR)m_port_srv));
}