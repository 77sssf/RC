
// RCClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RCClient.h"
#include "RCClientDlg.h"
#include "afxdialogex.h"


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
	, m_IsFull(FALSE)
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

	m_statudDlg.Create(IDD_DLG_STATUS, this);
	m_statudDlg.ShowWindow(SW_HIDE);

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


int CRCClientDlg::SendCommandPacket(const int nCmd, BOOL autoClose, const BYTE* pData, const int nLength) {
	
	UpdateData();

	CCliSocket* pSockCli = CCliSocket::getInstance();
	if (pSockCli->initSocket(m_addr_srv, atoi((LPCTSTR)m_port_srv)) == FALSE) {
		//  ... 
		return -1;
	}
	else {
		//TRACE(TEXT("Connected to server, CmdCode : %d\r\n"), nCmd);
		pSockCli->sendACK(CPkt(nCmd, pData, nLength));
		pSockCli->dealRequest();
		if (autoClose) {
			pSockCli->closeSock();
		}
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

	CCliSocket* pSockCli = CCliSocket::getInstance();
	CString strPath = GetPath(hTreeSelected);
	int nCmd = SendCommandPacket(2, FALSE, (const BYTE*)(LPCTSTR)strPath, strPath.GetLength());
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

		pSockCli->dealRequest();

		t = pSockCli->getPkt().getStrData();
		fInfo = {};
		memcpy(&fInfo, t.c_str(), t.size());
		Insert = TRUE;
	}

	pSockCli->closeSock();
}

void CRCClientDlg::LoadFileCurrent() {
	HTREEITEM hTree = m_tree.GetSelectedItem();
	CString filePath = GetPath(hTree);

	DeleteSelectChildItem(hTree);
	m_list.DeleteAllItems();

	CCliSocket* pSockCli = CCliSocket::getInstance();

	int nCmd = SendCommandPacket(2, FALSE, (const BYTE*)(LPCTSTR)filePath, filePath.GetLength());
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

		pSockCli->dealRequest();

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

void CRCClientDlg::ThreadEntryForDownloadFile(void* arg) {	//  static
	

	CRCClientDlg* pDlg = (CRCClientDlg*)arg;

	pDlg->ThreadDownloadFile();

	_endthread();
}

void CRCClientDlg::ThreadDownloadFile() {

	int nListSelected = m_list.GetSelectionMark();
	CString fileName = m_list.GetItemText(nListSelected, 0);

	// dialog选择路径
	CFileDialog dlg(FALSE, "*", fileName, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, NULL, this);
	if (dlg.DoModal() != IDOK) {
		return;
	}

	FILE* pf = fopen(dlg.GetPathName(), "wb+");
	if (pf == NULL) {
		AfxMessageBox(TEXT("本地打开文件失败"));
		return;
	}

	HTREEITEM hSelected = m_tree.GetSelectedItem();
	CString filePath = GetPath(hSelected);
	filePath = filePath + fileName;
	TRACE(TEXT("Download file path : %s\r\n"), filePath);
	//int ret = SendCommandPacket(4, FALSE, (BYTE*)(LPCTSTR)filePath, filePath.GetLength());
	int ret = SendMessage(WM_SEND_PACKET, (4 << 1 | 0), (LPARAM)(LPCTSTR)filePath);
	if (ret < 0) {
		AfxMessageBox(TEXT("下载命令失败\r\n"));
		TRACE(TEXT("下载命令失败, return = %d\r\n"), ret);
		return;
	}
	//  第一个包为长度
	//  最后一个包为空代表结束
	CCliSocket* pClient = CCliSocket::getInstance();
	long long nlength = *((long long*)pClient->getPkt().getStrData().c_str());
	if (nlength == 0) {
		//  文件打开失败或长度为0字节
		AfxMessageBox(TEXT("文件打开失败或长度为0字节"));
		return;
	}
	//  开始接收

	m_statudDlg.m_info.SetWindowText(TEXT("文件下载中"));
	m_statudDlg.SetActiveWindow();
	m_statudDlg.CenterWindow(this);
	m_statudDlg.ShowWindow(SW_SHOW);	//  小文件进度界面会一闪而过

	long long cnt = 0;

	// -----------------添加线程函数---------------------------------
	
	while (cnt < nlength) {
		ret = pClient->dealRequest();
		if (ret == FALSE) {
			AfxMessageBox(TEXT("断开连接或包已全部解析完毕"));
			TRACE(TEXT("文件传输失败, return = %d\r\n"), ret);
			return;
		}
		fwrite(pClient->getPkt().getStrData().c_str(), 1, pClient->getPkt().getStrData().size(), pf);
		cnt += pClient->getPkt().getStrData().size();
		if (pClient->getPkt().getStrData().size() == 0) {
			// ...
		}
	}
	// -------------------------------------------------------------

	pClient->closeSock();
	fclose(pf);
	m_statudDlg.ShowWindow(SW_HIDE);
	EndWaitCursor();
/*	MessageBox(TEXT("下载完成!"), TEXT("成功"), MB_OK);*/
}

void CRCClientDlg::OnDownload()
{
	// TODO: Add your command handler code here
	_beginthread(CRCClientDlg::ThreadEntryForDownloadFile, 0, this);

	BeginWaitCursor();	//  将光标变为等待状态图标
}


void CRCClientDlg::OnDelete()
{
	// TODO: Add your command handler code here
	HTREEITEM hTree = m_tree.GetSelectedItem();
	CString filePath = GetPath(hTree);
	int nListSelected = m_list.GetSelectionMark();
	CString fileName = m_list.GetItemText(nListSelected, 0);

	filePath = filePath + fileName;

	int ret = SendCommandPacket(9, TRUE, (BYTE*)(LPCTSTR)filePath, filePath.GetLength());

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

	int ret = SendCommandPacket(3, TRUE, (BYTE*)(LPCTSTR)filePath, filePath.GetLength());

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
		ret = SendCommandPacket(cmd, a, (BYTE*)(LPCTSTR)filePath, filePath.GetLength());
		break;
	}
	case 5: {
		ret = SendCommandPacket(cmd, a, (BYTE*)lParam, sizeof(MOUSEVENT));
		break;
	}
	case 6: {
		ret = SendCommandPacket(cmd, a, NULL, 0);
		break;
	}
	default:
		break;
	}
	
	return ret;
}


void CRCClientDlg::ThreadEntryForWatchData(void* args) {
	CRCClientDlg* pDlg = (CRCClientDlg*)args;

	pDlg->ThreadWatchData();

	_endthread();
}

void CRCClientDlg::ThreadWatchData() {
	
	Sleep(50);
	CCliSocket* pClient = NULL;

	do {
		pClient = CCliSocket::getInstance();
	} while (pClient == NULL);

	//  InitSocket

	ULONGLONG tick = GetTickCount64();

	while (TRUE) {
		
		while (m_IsFull) {
			Sleep(35);
		}

		int ret = SendMessage(WM_SEND_PACKET, (6 << 1 | 1));
		//
		if (ret < 0) {
			Sleep(500);
		}
		else {
			//ret = pClient->dealRequest();
			if (ret == 6 && !m_IsFull) {
				//  TODO : 存入CImage
				std::string data = pClient->getPkt().getStrData();	///////////////
				
				HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
				if (hMem == NULL) {
					TRACE(TEXT("内存不足"));
					Sleep(500);
					continue;
				}
				IStream* pStream = NULL;
				HRESULT hRet = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
				if (hRet == S_OK) {
					ULONG length = 0;
					pStream->Write(data.c_str(), data.size(), &length);
					LARGE_INTEGER bg = {};
					pStream->Seek(bg, STREAM_SEEK_SET, NULL);
					m_image.Load(pStream);
					m_IsFull = TRUE;
					pStream->Release();
					GlobalFree(hMem);
				}
			}
		}

	}

}

void CRCClientDlg::OnBnClickedBtnMonitor()
{
	// TODO: Add your control notification handler code here
	CMonitorDlg dlg(this);
	_beginthread(CRCClientDlg::ThreadEntryForWatchData, 0, this);
	//GetDlgItem(IDC_BTN_MONITOR)->EnableWindow(FALSE);
	//  启动Dlg显示截图
	dlg.DoModal();
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