#pragma once

#include <WinSock2.h>
#include "afxwin.h"
#pragma comment (lib, "ws2_32.lib")

#define LM_SOCKET_MESSAGE	27000

#define MAX_CLIENT_COUNT	5

// JServerSocket

class JServerSocket : public CWnd
{
	DECLARE_DYNAMIC(JServerSocket)

private:
    SOCKET mh_listen_socket;
    SOCKET mh_client_sockets[MAX_CLIENT_COUNT];
    char m_client_ip[MAX_CLIENT_COUNT][24];

    int m_client_index;
    
    LRESULT OnSocketMessage(WPARAM wParam, LPARAM lParam);

    void DestroySocket(SOCKET a_socket);
    int GetIndexOfClient(SOCKET h_sock);

public:
	JServerSocket();
	virtual ~JServerSocket();

    bool CreateServer(CWnd *p_parent_wnd, int n_id);
    bool StartServer(const char *pIP, short port);

    virtual BOOL DestroyWindow();

protected:
	DECLARE_MESSAGE_MAP()
};


