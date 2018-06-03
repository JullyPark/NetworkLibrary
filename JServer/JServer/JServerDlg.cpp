
// JServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JServer.h"
#include "JServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJServerDlg dialog
//  http://www.tipssoft.com/bulletin/tb.php/FAQ/667


CJServerDlg::CJServerDlg(CWnd* pParent):CDialogEx(IDD_JSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_server_listbox);
}

BEGIN_MESSAGE_MAP(CJServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CJServerDlg message handlers

BOOL CJServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    m_server.CreateServer(this, IDC_SERVER_SOCKET);
    m_server.StartServer("127.0.0.1", 2001);
	// 소켓의 정보를 저장하는 구조체
	AddEventString(CString("진희 짱"));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJServerDlg::OnPaint()
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
HCURSOR CJServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CJServerDlg::OnDestroy()
{
    m_server.DestroyWindow();
	CDialogEx::OnDestroy();

	// 소켓에 대한 정보를 더이상 사용하지 않겠다. 
	// ws2_32.dll과의 연결을 해지
	WSACleanup();
}


void CJServerDlg::AddEventString(const wchar_t *str)
{ 
	int index = m_server_listbox.InsertString(-1, str);
	m_server_listbox.SetCurSel(index);
}

