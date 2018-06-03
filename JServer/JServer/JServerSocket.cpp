// JServerSocket.cpp : implementation file
//

#include "stdafx.h"
#include "JServerSocket.h"


// JServerSocket

IMPLEMENT_DYNAMIC(JServerSocket, CWnd)

JServerSocket::JServerSocket()
{
    mh_listen_socket = INVALID_SOCKET;
    memset(mh_client_sockets, INVALID_SOCKET, sizeof(mh_client_sockets));
    m_client_index = 0;
}

JServerSocket::~JServerSocket()
{
}


BEGIN_MESSAGE_MAP(JServerSocket, CWnd)
    ON_MESSAGE(LM_SOCKET_MESSAGE, OnSocketMessage)
END_MESSAGE_MAP()



// JServerSocket message handlers

bool JServerSocket::CreateServer(CWnd *p_parent_wnd, int n_id)
{
    RECT rect;
    rect.bottom = 0;
    rect.left = 0;
    rect.right = 0;
    rect.top = 0;

    return Create(NULL, NULL, WS_CHILD, rect, p_parent_wnd, n_id);
}

bool JServerSocket::StartServer(const char *pIP, short port)
{
    WSADATA data;
    // (��� ������ ������ ����, ������ ������ �����ϴ� ����ü ����)
    WSAStartup(0x0202, &data);

    // AF_INET : IP_V4 ü���� ��Ʈ��ũ ���
    // SOCK_STREAM : tcp ��� SOCK_STREAM, 
    // 0: �߰����� �������� ������� ���� 
    mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in srv_addr;

    memset(&srv_addr, 0, sizeof(struct sockaddr_in));

    srv_addr.sin_family = AF_INET;
    // ���ڿ� ������ �ּҸ� ���ڷ� ��ȯ
    srv_addr.sin_addr.s_addr = inet_addr(pIP);
    srv_addr.sin_port = htons(port);

    // ����� ���� 
    bind(mh_listen_socket, (LPSOCKADDR)&srv_addr, sizeof(struct sockaddr_in));

    // 1 : pending connection count
    listen(mh_listen_socket, 1);

    // �ü������ client socket�� FD_ACCEPT�� �߻��ϸ� 
    // m_hWnd�� ���� �����쿡�� "10000"�̶�� �޽��� ������
    WSAAsyncSelect(mh_listen_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_ACCEPT);

    return true;
}

BOOL JServerSocket::DestroyWindow()
{
    return CWnd::DestroyWindow();
}

// wParam ���� ������ �ڵ鰪, 
// lParam ���� �̺�Ʈ ����(���� 16��Ʈ)�� ���� ����(���� 16��Ʈ)
LRESULT JServerSocket::OnSocketMessage(WPARAM wParam, LPARAM lParam)
{
    SOCKET h_cur_socket = wParam;

    switch (WSAGETSELECTEVENT(lParam)) {
    case FD_ACCEPT:
    {
        if (m_client_index < MAX_CLIENT_COUNT) {
            struct sockaddr_in client_addr;
            int client_addr_size = sizeof(struct sockaddr_in);
            mh_client_sockets[m_client_index] = accept(mh_listen_socket, (LPSOCKADDR)&client_addr, &client_addr_size);

            WSAAsyncSelect(mh_client_sockets[m_client_index], m_hWnd, LM_SOCKET_MESSAGE, FD_READ | FD_CLOSE);

            // ip�� ����
            char temp_ip[24] = { 0, };
            strcpy(temp_ip, inet_ntoa(client_addr.sin_addr));

            strcpy(m_client_ip[m_client_index++], temp_ip);

            // TBD Notify to user of server
        }
        else {
            // TBD Notify to user of server
        }
        break;
    }
    case FD_READ:
    {
        // ������ �����ϱ� ���� FD_READ�� �ٽ� �߻����� �ʵ��� FD_READ�� �񵿱⿡�� �����Ѵ�
        /*
        FD_READ �� �߻��ϴ� ����?
        1. �񵿱⸦ ���� ���� ���ۿ� �����Ͱ� �ִ� ���
        2. (�񵿱⸦ ���� ��) �� ���ۿ� �����Ͱ� ���ŵǾ��� ��
        3. (�񵿱⸦ ���� ��) ���ŵ� �����ͺ��� ���� ���� �����͸� recv �Ͽ� �����Ͱ� ������ ���
        //4. �Һи�..  (�񵿱� �ɸ� ���¿���) �����Ͱ� �ִ� ��Ȳ���� �����Ͱ� �ٽ� ���� ���?

        */
        // �ǹ� : �ش� ���Ͽ� FD_CLOSE �̺�Ʈ�� �߻��ϸ� �ش� �����쿡 LM_SOCKET_MESSAGE �� �����޶�
        WSAAsyncSelect(h_cur_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE);

        // Ŭ���̾�Ʈ�� ip ���� �˾Ƴ���
        int index = GetIndexOfClient(h_cur_socket);
        if (-1 == index) {
            AfxMessageBox(CString("�߸��� ����� index"));
            break;
        }

        char ip_size = 1 + strlen(m_client_ip[index]);	// NULL�� ������ ������ ���̸� ��ȯ

                                                        // ������ �޽����� ���̸� �˾Ƴ���
        char data_size;
        recv(h_cur_socket, &data_size, 1, 0);

        // �ٸ� Ŭ���̾�Ʈ���� ������ �޽����� ��� ���� ���۸� �����
        char *p_data = new char[2 + ip_size + data_size];

        // ���ۿ� �����͸� ��´� (���� + ������)
        //	0 : body size ( data_size + ip_size + 1(len))
        p_data[0] = data_size + ip_size;	// ip�� ���̸� body�� �ִµ� �� �� ����(1)�� ���Խ�Ű�� �ʾ�����
                                            // 'body�� ù ����Ʈ�� ip�� ����'�� ������ ����̴�. 
                                            // 1 : ip_size
        p_data[1] = ip_size;
        // 2~ : ip �� �����Ѵ�

        strcpy(p_data + 2, m_client_ip[index]);	// including the terminating null character
                                                /*	memcpy(p_data + 2, m_client_ip[index], ip_size);
                                                p_data[2 + ip_size] = '\0'; */

                                                // client�� �����͸� �о� ���ۿ� ��´�
        recv(h_cur_socket, p_data + 2 + ip_size, data_size, 0);

        // ���ۿ� �����͸� �� ä��.

        // ������ ��� ����ڿ��� �����Ѵ�
        for (int i = 0; i < m_client_index; i++) {
            if (INVALID_SOCKET != mh_client_sockets[i]) {
                send(mh_client_sockets[i], p_data, 2 + ip_size + data_size, 0);
            }
        }

        // ������ ����Ʈ�ڽ��� ip�� �޽���(UNICODE)�� ����Ѵ�
        CString str;
        // ip �� ANSI ���ڿ��̹Ƿ� UNICODE�� ��ȯ�Ѵ�
        CString ip_cstr(m_client_ip[index]);
        str.Format(_T("[%s] %s"), ip_cstr, p_data + 2 + ip_size);

        // TBD Notify to user of server

        // !! �����Ҵ��� p_data�� ���� 
        delete[] p_data;

        // FD_READ �̺�Ʈ�� ���� ó���� ��� �������Ƿ� FD_READ�� ���� �񵿱⸦ �ٽ� �����Ѵ�
        WSAAsyncSelect(h_cur_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_READ | FD_CLOSE);
        break;
    }
    case FD_CLOSE:
    {
        // ������ �����Ѵ�
        DestroySocket(h_cur_socket);

        int index = GetIndexOfClient(h_cur_socket);
        if (--m_client_index != index) {
            mh_client_sockets[index] = mh_client_sockets[m_client_index];


            // TBD Notify to user of server
            strcpy(m_client_ip[index], m_client_ip[m_client_index]);
        }
        break;
    }
    default:
        return 0;
        break;
    }

    // �޽��� �ڵ鷯�� ��ȯ���� � �ǹ̰� ����?
    return 1;
}


// ������ ���������ν� ����� �����ϱ� ���� �Լ�
// Client�κ��� �����͸� ���� �� �ش� Client�� ������ �����ϴ� ���
// Client�� �������� ���Ḧ ���� ���� �� �ֱ� �����̴�. �Ʒ� �Լ��� ���� LINGER �ɼ��� �ɾ������μ�
// ������ ���������� �����Ѵ�.
void JServerSocket::DestroySocket(SOCKET a_socket)
{
    if (a_socket == INVALID_SOCKET) return;

    LINGER linger = { TRUE, 0 };

    setsockopt(a_socket, SOL_SOCKET, SO_LINGER, (char FAR *)&linger, sizeof(linger));

    closesocket(a_socket);
}



int JServerSocket::GetIndexOfClient(SOCKET h_sock)
{
    for (int i = 0; i < m_client_index; i++) {
        if (h_sock == mh_client_sockets[i]) {
            return i;
        }
    }
    return -1;
}
