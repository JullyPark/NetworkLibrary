
// JServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "JServer.h"
#include "JServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJServerApp

BEGIN_MESSAGE_MAP(CJServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CJServerApp construction

CJServerApp::CJServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CJServerApp object

CJServerApp theApp;


// CJServerApp initialization

BOOL CJServerApp::InitInstance()
{
	CWinApp::InitInstance();

	CJServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

