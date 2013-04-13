// IPFindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPFind.h"
#include "IPFindDlg.h"
#include "logMsg.h"
#include <winsock2.h>
#include <iphlpapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define MSG_CUR_ADAPTER      _T("当前网卡信息:  ")
#define MAC_ADDR_ERROR_TITLE _T("警告")
#define MAC_ADDR_ERROR_MSG   _T("Mac地址错误,请确认后重试!")

#define Field_01 _T("IP地址")
#define Field_02 _T("MAC地址")
#define COLNUM 2

const TCHAR* LISTTITLE[COLNUM] = 
{
	Field_01,Field_02,
};

bool CheckMac(const char *mac, char *token)
{
	if(strlen(mac) != 17) return false;

	char arrMac[17+1] = {0};
	strcpy(arrMac, mac);

	char *tmp = strtok(arrMac, token);
	while(tmp != NULL)
	{
		if(strlen(tmp) != 2) return false;
		if((tmp[0] >= '0' && tmp[0] <= '9') || (tmp[0] >= 'A' && tmp[0] <= 'F') || (tmp[0] >= 'a' && tmp[0] <= 'f'))
		{
			if((tmp[1] >= '0' && tmp[1] <= '9') || (tmp[1] >= 'A' && tmp[1] <= 'F') || (tmp[1] >= 'a' && tmp[1] <= 'f'))
			{
				tmp = strtok(NULL, token);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

CIPFindDlg::CIPFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPFindDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strFindIP = "";
	m_strFindMask = "";
}

void CIPFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_DEVICE, m_Device);
}

BEGIN_MESSAGE_MAP(CIPFindDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CIPFindDlg::OnBnClickedButtonStart)
	ON_CBN_SELCHANGE(IDC_CMB_DEVICE, &CIPFindDlg::OnCbnSelchangeCmbDevice)
	ON_BN_CLICKED(IDC_BUTTON_COPY_TO_CLIPBOARD, &CIPFindDlg::OnBnClickedButtonCopyToClipboard)
END_MESSAGE_MAP()


// CIPFindDlg message handlers

BOOL CIPFindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	InitAdapter();
#if defined(_DEBUG)
	SetDlgItemText(IDC_EDIT_MAC, _T("08:20:3e:9a:5f:17"));
#endif

	InitListCtrl();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIPFindDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIPFindDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIPFindDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIPFindDlg::InitListCtrl()
{
	CListCtrl* m_pList=(CListCtrl*)GetDlgItem(IDC_LIST_IPMAC);

	CRect rt;
	int nCount = COLNUM;
	m_pList->GetClientRect(&rt);
	int nWidth = rt.right/nCount;
	for(int i=0; i<nCount; i++)
	{
		m_pList->InsertColumn(i, LISTTITLE[i], LVCFMT_LEFT, nWidth, i);
	}

	DWORD dwNewStyle = 0;
	dwNewStyle = m_pList->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_pList->SetExtendedStyle(dwNewStyle);
}

bool CIPFindDlg::CheckMACAddr(CString strMAC)
{
	CStringA stra(strMAC.GetBuffer(0));
	strMAC.ReleaseBuffer();

	string str=stra.GetBuffer(0);
	stra.ReleaseBuffer();
	
	if (-1 != str.find(":")) return CheckMac(str.c_str(), ":");
	if (-1 != str.find("-")) return CheckMac(str.c_str(), "-");

	return false;
}


void CIPFindDlg::OnBnClickedButtonStart()
{
	UpdateData(TRUE);
	CString strMac;
	GetDlgItemText(IDC_EDIT_MAC, strMac);
	if (0 != strMac.GetLength() && !CheckMACAddr(strMac))
	{
		MessageBox(MAC_ADDR_ERROR_MSG, MAC_ADDR_ERROR_TITLE);
		return;
	}

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(false);

	CListCtrl *m_pList =(CListCtrl*)GetDlgItem(IDC_LIST_IPMAC);
	if (!m_pList) 
	{
		AfxMessageBox(_T("Failed to get point of CListCtrl"));
		return ;
	}

	// Delete all items and update immediately
	m_pList->DeleteAllItems();
	ASSERT(m_pList->GetItemCount() == 0);
	m_pList->SetRedraw(TRUE);
	m_pList->Invalidate();
	m_pList->UpdateWindow();
	
	// do find
	vtIpMac.swap(vector<st_ip_mac>());
	IPFind(m_strFindIP, m_strFindMask, vtIpMac);

	int nItem;
	st_ip_mac ipmac;
	int len = vtIpMac.size();

	USES_CONVERSION;

	if (FALSE == strMac.IsEmpty())
	{
		strMac.MakeUpper();
		strMac.Replace(_T(":"), _T("-"));
		string strMacFind = T2A(strMac);

		for (int i=0; i<len; i++)
		{
			if (0 == strMacFind.compare(vtIpMac.at(i).mac))
			{
				nItem = m_pList->GetItemCount();
				m_pList->InsertItem(nItem, A2T(vtIpMac.at(i).ip.c_str()));
				m_pList->SetItemText(nItem, 1, A2T(vtIpMac.at(i).mac.c_str()));
			}
		}
	}
	else
	{
		for (int i=0; i<len; i++)
		{
			nItem = m_pList->GetItemCount();
			m_pList->InsertItem(nItem, A2T(vtIpMac.at(i).ip.c_str()));
			m_pList->SetItemText(nItem, 1, A2T(vtIpMac.at(i).mac.c_str()));
		}
	}

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
}

void CIPFindDlg::InitAdapter()
{
	m_Device.ResetContent();
	USES_CONVERSION;

	ULONG len = 0; 
	GetAdaptersInfo(NULL, &len);
	PIP_ADAPTER_INFO p = static_cast<PIP_ADAPTER_INFO>(malloc(len));
	GetAdaptersInfo(p, &len);
	for (PIP_ADAPTER_INFO q = p; q != NULL; q = q->Next)
	{
		m_Device.AddString(A2T(q->Description));
	}

	m_Device.SetCurSel (0);
	GetCurSelIPMask(p->IpAddressList.IpAddress.String, p->IpAddressList.IpMask.String);

	free(p);
}

void CIPFindDlg::OnCbnSelchangeCmbDevice()
{
	UpdateData(FALSE);

	int nCurSel = m_Device.GetCurSel();

	ULONG len = 0; 
	GetAdaptersInfo(NULL, &len);
	PIP_ADAPTER_INFO p = static_cast<PIP_ADAPTER_INFO>(malloc(len));
	GetAdaptersInfo(p, &len);

	int i=0;
	for (PIP_ADAPTER_INFO q = p; q != NULL; q = q->Next)
	{
		if (i++ == nCurSel)
		{
			GetCurSelIPMask(q->IpAddressList.IpAddress.String, q->IpAddressList.IpMask.String);
		}
	}

	free(p);
}

void CIPFindDlg::GetCurSelIPMask(char *ip, char *mask)
{
	USES_CONVERSION;

	CString strMsg = MSG_CUR_ADAPTER;
	strMsg += "IP:";
	strMsg += A2T(ip);
	strMsg += " Mask:";
	strMsg += A2T(mask);

	SetDlgItemText(IDC_STATIC_ADAPTER_MSG, strMsg);

	m_strFindIP = ip;
	m_strFindMask = mask;
}


void CIPFindDlg::OnBnClickedButtonCopyToClipboard()
{
	string strIpMacCopy = "";
	int len = vtIpMac.size();
	for (int i=0; i<len; i++)
	{
		strIpMacCopy += vtIpMac.at(i).mac;
		strIpMacCopy += "  ";
		strIpMacCopy += vtIpMac.at(i).ip;
		strIpMacCopy += "\r\n";
	}

	if(OpenClipboard())  
	{  
		HGLOBAL   clipbuffer;  
		char   *   buffer;  
		EmptyClipboard();  
		clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,strIpMacCopy.length()+1);  
		buffer   =   (char*)GlobalLock(clipbuffer);  
		strcpy(buffer,   strIpMacCopy.c_str());  
		GlobalUnlock(clipbuffer);  
		SetClipboardData(CF_TEXT,clipbuffer);  
		CloseClipboard();  
	}     
}
