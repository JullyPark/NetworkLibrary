
// JServerDlg.h : header file
//

#pragma once
#include <WinSock2.h>
#include "afxwin.h"
#pragma comment (lib, "ws2_32.lib")
#define LM_SOCKET_MESSAGE	27000

#define MAX_CLIENT_COUNT	5

// CJServerDlg dialog
class CJServerDlg : public CDialogEx
{
private:
	SOCKET mh_listen_socket;
	SOCKET mh_client_sockets[MAX_CLIENT_COUNT];
	char m_client_ip[MAX_CLIENT_COUNT][24];

	int m_client_index;
	// 왜 ip를 24길이로 했을까? 3*4 + 3(.)

public:
	CJServerDlg(CWnd* pParent = NULL);	// standard constructor
	void AddEventString(const wchar_t *str);
	int GetIndexOfClient(SOCKET h_sock);
	void DestroySocket(SOCKET a_socket);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	LRESULT OnSocketMessage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
	CListBox m_server_listbox;
};
