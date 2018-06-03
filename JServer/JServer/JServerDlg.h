
// JServerDlg.h : header file
//
#include "JServerSocket.h"

// CJServerDlg dialog
class CJServerDlg : public CDialogEx
{
private:
    JServerSocket m_server;

public:
	CJServerDlg(CWnd* pParent = NULL);	// standard constructor
	void AddEventString(const wchar_t *str);

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
public:
	afx_msg void OnDestroy();
	CListBox m_server_listbox;
};
