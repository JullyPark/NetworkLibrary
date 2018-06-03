
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

	// �񵿱⸦ �����Ѵ�
	WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CONNECT);

	// �غ������ ��ģ �� ������ �����Ѵ�
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
			// �ٸ� �̺�Ʈ�� �ޱ� ���� �񵿱⸦ �ɾ��ش�.
			WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE | FD_READ);

			AfxMessageBox(CString("������ �����߽��ϴ�"));
			AddEventString(CString("���� ���� �ߴ�!"));
			}
			else {
				AddEventString(CString("���� ���� �����ߴ�!"));
			}
		}
		break;
		case FD_READ:
		{
			// FD_READ �񵿱⸦ �����Ѵ�
			// �������� ����� �ٵ�� ������ ������ ���̱� ������
			// FD_READ �� �߻��ϴ� ���� ���ƾ��ϱ� �����̴�.
			WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE);

			char body_size = 1;
			// ����� �о�鿩 �ٵ��� ���̸� ��´�
			recv(mh_my_socket, &body_size, sizeof(body_size), 0);

			// �ٵ� �д´�
			char ip_size = 1;
			// ip�� ���̸� �д´�.
			recv(mh_my_socket, &ip_size, sizeof(ip_size), 0);

			// ip�� ���� ������ �����Ҵ��Ѵ�
			char *p_ip = new char[ip_size];
			// msg(message)�� ���� ������ �����Ҵ��Ѵ�
			char *p_msg = new char[body_size - ip_size];

			// ip ���̸�ŭ ip�� �д´�
			recv(mh_my_socket, p_ip, ip_size, 0);

			// �޽����� �д´�
			recv(mh_my_socket, p_msg, body_size - ip_size, 0);

			// ������ �޽����� ����Ʈ�� �߰��Ѵ�
			CString str;
			CString ip_cstr(p_ip);
			str.Format(_T("[%s] %s"), ip_cstr, p_msg);
			AddEventString(str);

			// ���� �Ϸ� �� FD_READ �� �񵿱⸦ �ٽ� �Ǵ�
			WSAAsyncSelect(mh_my_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE | FD_READ);

			// �����Ҵ��� IP�� MSG�� �����Ѵ�
			delete[] p_ip;
			delete[] p_msg;
			break;
		}
		case FD_CLOSE:
		{
			// ���������� FD_CLOSE�� ������ Client���ϰ� ������ ��� �����ϱ� ���� LINGER�ɼ��� �ɾ��� ��ó��
			// Client������ FD_ClOSE �� ������ ������ �����Ͽ� ������ �����Ѵ�
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

	// ws2_32.dll �� ������ ����
	WSACleanup();
}



// ����ڰ� �Է��� ���� ������ �����Ѵ�
void CJClientDlg::OnBnClickedOk()
{
	if (INVALID_SOCKET != mh_my_socket) {
		//wchar_t wstr[MAX_MESSAGE_SIZE];
		CString wstr;
		// ����â���κ��� ����ڰ� �Է��� ���� �޾ƿ´�
		int text_size = GetDlgItemText(IDC_CHAT_EDIT, wstr);

		// ���� ������ ���̰� 0�� �ƴ� ��
		if (text_size) {
			// �Է��� ���� ���̸� ��´�
			char str_len = (1 + wstr.GetLength()) * 2; // NULL + Str len // �ƽ�Ű�ڵ�� null ���ڵ� 2����Ʈ

			// �������� ũ�⸦ ����Ͽ� ���۸� �����Ҵ� �Ѵ�
			char *p_data = new char[1 + str_len];

			// ����� ���� ���̸� ��´�
			p_data[0] = str_len;
			// �ٵ� ���� ��´�
			
			// strcpy((p_data + 1), wstr);
			// wcscpy((wchar_t *)(p_data + 1), wstr);
		    memcpy(p_data + 1, wstr, str_len);
			*((wchar_t *)(p_data + 1) + wstr.GetLength()) = '\0';

			// ������ ���� ������ �����͸� �����Ѵ�
			send(mh_my_socket, p_data, 1 + str_len, 0);

			// �����Ҵ��� ���۸� �����Ѵ�
			delete[] p_data;

			SetDlgItemText(IDC_CHAT_EDIT, _T(""));
		}
	}
	else {
		AfxMessageBox(CString("�������� ������ ������ϴ�"));
	}

	// CDialogEx::OnOK();
}
