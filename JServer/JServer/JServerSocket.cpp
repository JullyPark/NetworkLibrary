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
    // (사용 가능한 소켓의 버전, 가져온 정보를 저장하는 구조체 변수)
    WSAStartup(0x0202, &data);

    // AF_INET : IP_V4 체계의 네트워크 사용
    // SOCK_STREAM : tcp 사용 SOCK_STREAM, 
    // 0: 추가적인 프로토콜 사용하지 않음 
    mh_listen_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in srv_addr;

    memset(&srv_addr, 0, sizeof(struct sockaddr_in));

    srv_addr.sin_family = AF_INET;
    // 문자열 형태의 주소를 숫자로 변환
    srv_addr.sin_addr.s_addr = inet_addr(pIP);
    srv_addr.sin_port = htons(port);

    // 등록할 소켓 
    bind(mh_listen_socket, (LPSOCKADDR)&srv_addr, sizeof(struct sockaddr_in));

    // 1 : pending connection count
    listen(mh_listen_socket, 1);

    // 운영체제에게 client socket에 FD_ACCEPT가 발생하면 
    // m_hWnd를 가진 윈도우에게 "10000"이라고 메시지 보내줘
    WSAAsyncSelect(mh_listen_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_ACCEPT);

    return true;
}

BOOL JServerSocket::DestroyWindow()
{
    return CWnd::DestroyWindow();
}

// wParam 에는 소켓의 핸들값, 
// lParam 에는 이벤트 정보(하위 16비트)와 에러 정보(상위 16비트)
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

            // ip를 저장
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
        // 정보를 수신하기 전에 FD_READ가 다시 발생하지 않도록 FD_READ를 비동기에서 해제한다
        /*
        FD_READ 가 발생하는 시점?
        1. 비동기를 걸은 순간 버퍼에 데이터가 있는 경우
        2. (비동기를 걸은 후) 빈 버퍼에 데이터가 수신되었을 때
        3. (비동기를 걸은 후) 수신된 데이터보다 적은 양의 데이터를 recv 하여 데이터가 남았을 경우
        //4. 불분명..  (비동기 걸린 상태에서) 데이터가 있는 상황에서 데이터가 다시 들어운 경우?

        */
        // 의미 : 해당 소켓에 FD_CLOSE 이벤트가 발생하면 해당 윈도우에 LM_SOCKET_MESSAGE 를 보내달라
        WSAAsyncSelect(h_cur_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_CLOSE);

        // 클라이언트의 ip 길이 알아낸다
        int index = GetIndexOfClient(h_cur_socket);
        if (-1 == index) {
            AfxMessageBox(CString("잘못된 사용자 index"));
            break;
        }

        char ip_size = 1 + strlen(m_client_ip[index]);	// NULL을 제외한 문자의 길이를 반환

                                                        // 수신한 메시지의 길이를 알아낸다
        char data_size;
        recv(h_cur_socket, &data_size, 1, 0);

        // 다른 클라이언트에게 전송할 메시지를 담기 위한 버퍼를 만든다
        char *p_data = new char[2 + ip_size + data_size];

        // 버퍼에 데이터를 담는다 (길이 + 데이터)
        //	0 : body size ( data_size + ip_size + 1(len))
        p_data[0] = data_size + ip_size;	// ip의 길이를 body에 넣는데 왜 그 길이(1)는 포함시키지 않았을까
                                            // 'body의 첫 바이트는 ip의 길이'는 무언의 약속이다. 
                                            // 1 : ip_size
        p_data[1] = ip_size;
        // 2~ : ip 를 복사한다

        strcpy(p_data + 2, m_client_ip[index]);	// including the terminating null character
                                                /*	memcpy(p_data + 2, m_client_ip[index], ip_size);
                                                p_data[2 + ip_size] = '\0'; */

                                                // client의 데이터를 읽어 버퍼에 담는다
        recv(h_cur_socket, p_data + 2 + ip_size, data_size, 0);

        // 버퍼에 데이터를 다 채움.

        // 접속한 모든 사용자에게 전송한다
        for (int i = 0; i < m_client_index; i++) {
            if (INVALID_SOCKET != mh_client_sockets[i]) {
                send(mh_client_sockets[i], p_data, 2 + ip_size + data_size, 0);
            }
        }

        // 서버의 리스트박스에 ip와 메시지(UNICODE)를 출력한다
        CString str;
        // ip 는 ANSI 문자열이므로 UNICODE로 변환한다
        CString ip_cstr(m_client_ip[index]);
        str.Format(_T("[%s] %s"), ip_cstr, p_data + 2 + ip_size);

        // TBD Notify to user of server

        // !! 동적할당한 p_data를 삭제 
        delete[] p_data;

        // FD_READ 이벤트에 대한 처리를 모두 끝냈으므로 FD_READ에 대한 비동기를 다시 설정한다
        WSAAsyncSelect(h_cur_socket, m_hWnd, LM_SOCKET_MESSAGE, FD_READ | FD_CLOSE);
        break;
    }
    case FD_CLOSE:
    {
        // 소켓을 제거한다
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

    // 메시지 핸들러의 반환값은 어떤 의미가 있지?
    return 1;
}


// 소켓을 제거함으로써 통신을 종료하기 위한 함수
// Client로부터 데이터를 받을 때 해당 Client가 접속을 종료하는 경우
// Client가 정상적인 종료를 하지 못할 수 있기 때문이다. 아래 함수를 통해 LINGER 옵션을 걸어줌으로서
// 소켓을 정상적으로 삭제한다.
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
