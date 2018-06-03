
// JClientDlg.h : header file
//

#pragma once

#include <Winsock2.h>
#include "afxwin.h"
#pragma comment (lib, "WS2_32.lib")
#define LM_SOCKET_MESSAGE	28000

#define MAX_MESSAGE_SIZE		1000


// CJClientDlg dialog
class CJClientDlg : public CDialogEx
{
// Construction
private: 
	SOCKET mh_my_socket;
	
public:
	CJClientDlg(CWnd* pParent = NULL);	// standard constructor

	void AddEventString(const wchar_t *str);
	void DestroySocket(SOCKET a_socket);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JCLIENT_DIALOG };
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
	LRESULT OnClientMessage(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnDestroy();
//	afx_msg void OnLbnSelchangeList1();
	CListBox m_client_listbox;
	afx_msg void OnBnClickedOk();
};
