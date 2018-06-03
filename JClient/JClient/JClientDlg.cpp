
// JClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JClient.h"
#include "JClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJClientDlg dialog



CJClientDlg::CJClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mh_my_socket = INVALID_SOCKET;
}

void CJClientDlg::AddEventString(const wchar_t * str)
{
	int index = m_client_listbox.InsertString(-1, str);
	m_client_listbox.SetCurSel(index);
}

void CJClientDlg::DestroySocket(SOCKET a_socket)
{
	if (INVALID_SOCKET == a_socket) return;

	LINGER linger = { TRUE, 0 };
	setsockopt(mh_my_socket, SOL_SOCKET, SO_LINGER, (char FAR *)&linger, sizeof(linger));

	DestroySocket(mh_my_socket);
}

void CJClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHAT_LIST, m_client_listbox);
}

BEGIN_MESSAGE_MAP(CJClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(LM_SOCKET_MESSAGE, OnClientMessage)
	ON_BN_CLICKED(IDOK, &CJClientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CJClientDlg message handlers

BOOL CJClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	WSADATA data;
	
	WSAStartup(0x0202, &data);

	mh_my_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(2001);

	// 비동기를 설정한다
	WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CONNECT);

	// 준비과정을 마친 후 서버에 연결한다
	connect(mh_my_socket, (LPSOCKADDR)&addr, sizeof(struct sockaddr_in));


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJClientDlg::OnPaint()
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
HCURSOR CJClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CJClientDlg::OnClientMessage(WPARAM wParam, LPARAM lParam)
{
	SOCKET h_cur_sock = (SOCKET)wParam;

	switch (WSAGETSELECTEVENT(lParam)) {
		case FD_CONNECT:
		{	if (0 == WSAGETSELECTERROR(lParam)) {
			// 다른 이벤트를 받기 위해 비동기를 걸어준다.
			WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE | FD_READ);

			AfxMessageBox(CString("서버에 접속했습니다"));
			AddEventString(CString("서버 접속 했당!"));
			}
			else {
				AddEventString(CString("서버 접속 실패했당!"));
			}
		}
		break;
		case FD_READ:
		{
			// FD_READ 비동기를 해제한다
			// 프레임을 헤더와 바디로 나누어 수신할 것이기 때문에
			// FD_READ 가 발생하는 것을 막아야하기 때문이다.
			WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE);

			char body_size = 1;
			// 헤더를 읽어들여 바디의 길이를 얻는다
			recv(mh_my_socket, &body_size, sizeof(body_size), 0);

			// 바디를 읽는다
			char ip_size = 1;
			// ip의 길이를 읽는다.
			recv(mh_my_socket, &ip_size, sizeof(ip_size), 0);

			// ip를 얻을 공간을 동적할당한다
			char *p_ip = new char[ip_size];
			// msg(message)를 얻을 공간을 동적할당한다
			char *p_msg = new char[body_size - ip_size];

			// ip 길이만큼 ip를 읽는다
			recv(mh_my_socket, p_ip, ip_size, 0);

			// 메시지를 읽는다
			recv(mh_my_socket, p_msg, body_size - ip_size, 0);

			// 수신한 메시지를 리스트에 추가한다
			CString str;
			CString ip_cstr(p_ip);
			str.Format(_T("[%s] %s"), ip_cstr, p_msg);
			AddEventString(str);

			// 수신 완료 후 FD_READ 의 비동기를 다시 건다
			WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE | FD_READ);

			// 동적할당한 IP와 MSG를 해제한다
			delete[] p_ip;
			delete[] p_msg;
			break;
		}
		case FD_CLOSE:
		{
			// 서버에서도 FD_CLOSE가 들어오면 Client소켓과 연결을 즉시 종료하기 위해 LINGER옵션을 걸어준 것처럼
			// Client에서도 FD_ClOSE 가 들어오면 소켓을 삭제하여 연결을 종료한다
			DestroySocket(mh_my_socket);
			mh_my_socket = INVALID_SOCKET;
			break;
		}
		default:
		{
			break;
		}
	}

	return 1;
}



void CJClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// ws2_32.dll 과 연결을 해지
	WSACleanup();
}



// 사용자가 입력한 글을 서버에 전송한다
void CJClientDlg::OnBnClickedOk()
{
	if (INVALID_SOCKET != mh_my_socket) {
		//wchar_t wstr[MAX_MESSAGE_SIZE];
		CString wstr;
		// 에딧창으로부터 사용자가 입력한 글을 받아온다
		int text_size = GetDlgItemText(IDC_CHAT_EDIT, wstr);

		// 보낼 문자의 길이가 0이 아닐 때
		if (text_size) {
			// 입력한 글의 길이를 얻는다
			char str_len = (1 + wstr.GetLength()) * 2; // NULL + Str len // 아스키코드는 null 문자도 2바이트

			// 프레임의 크기를 계산하여 버퍼를 동적할당 한다
			char *p_data = new char[1 + str_len];

			// 헤더에 글의 길이를 담는다
			p_data[0] = str_len;
			// 바디에 글을 담는다
			
			// strcpy((p_data + 1), wstr);
			// wcscpy((wchar_t *)(p_data + 1), wstr);
		    memcpy(p_data + 1, wstr, str_len);
			*((wchar_t *)(p_data + 1) + wstr.GetLength()) = '\0';

			// 소켓을 통해 버퍼의 데이터를 전송한다
			send(mh_my_socket, p_data, 1 + str_len, 0);

			// 동적할당한 버퍼를 헤제한다
			delete[] p_data;

			SetDlgItemText(IDC_CHAT_EDIT, _T(""));
		}
	}
	else {
		AfxMessageBox(CString("서버와의 접속이 끊겼습니다"));
	}

	// CDialogEx::OnOK();
}
