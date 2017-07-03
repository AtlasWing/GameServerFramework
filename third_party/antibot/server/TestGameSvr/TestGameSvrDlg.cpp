// TestGameSvrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestGameSvr.h"
#include "TestGameSvrDlg.h"


#include "iantibotinterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CALLBACK ProcessInPutTimer(HWND hWnd, UINT uMsg, unsigned int idEvent, DWORD dwTime);

//extern IAntiBotChecker * GetAntiBotInterface();

IAntibotInterface * g_pIAntibotChecker ;
static CMainCtrlBusiness g_cMainCtrlBusiness ;
std::vector< unsigned int> g_vecUin;
CTestGameSvrSocket *g_pSvrListenSocket = NULL;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestGameSvrDlg dialog




CTestGameSvrDlg::CTestGameSvrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestGameSvrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestGameSvrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestGameSvrDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTestGameSvrDlg message handlers

BOOL CTestGameSvrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	g_pSvrListenSocket = new CTestGameSvrSocket();
	g_pSvrListenSocket->CreateSvrSocket(6666);

	g_pIAntibotChecker = CreateAntiServerObj();//GetAntiBotInterface();
	g_pIAntibotChecker ->Initialize(&g_cMainCtrlBusiness,"1");
	m_nTimer = ::SetTimer(NULL, 1, 2000, ProcessInPutTimer);
	CTestGameSvrSocket::ClearSvrSocket();

	::SetWindowText(this->m_hWnd, "TestGameSvr running...");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestGameSvrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestGameSvrDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestGameSvrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestGameSvrDlg::OnClose()
{
	if (g_pSvrListenSocket)
	{
		delete g_pSvrListenSocket;
		g_pSvrListenSocket = NULL;
	}

	CDialog::OnClose();
}


void CALLBACK ProcessInPutTimer(HWND hWnd, UINT uMsg, unsigned int idEvent, DWORD dwTime)
{
	g_pIAntibotChecker->ProcessAntiBotInput();

	static int count = 1;
	std::vector<unsigned int>::iterator iter;
	for (iter = g_vecUin.begin(); iter != g_vecUin.end(); iter++)
	{
		unsigned int iUin =  (unsigned int )(*iter);
		g_pIAntibotChecker->CheckTimer(1,1,iUin);

		/*if(! ( (count ++)%16))
		{
			GAMESVR_SOCKET_INFO *pSocketInfo = g_pSvrListenSocket->GetSvrSocket(iUin);
			ASSERT(pSocketInfo);

			BYTE abySendBuffer[] = "1234567890";
			DWORD nSize = sizeof (abySendBuffer) ;

			BYTE *pbyGameProtoData = NULL;
			DWORD dwGameProtoSize = 0;

			pSocketInfo->pSocket->EncodeGameProto(abySendBuffer, nSize, GAMEPROTO_TEST_DP, &pbyGameProtoData, dwGameProtoSize);

			BYTE *pbyGameProtoAntiBotData = NULL;
			DWORD dwGameProtoAntiBotSize = 0;

			BYTE byTmp[3000] = {0};
			WORD wTmp = 3000;

			g_pIAntibotChecker->RecvBufferForEncrypt(pSocketInfo->iKey, 
			pbyGameProtoData, 
			dwGameProtoSize,
			byTmp,
			wTmp);
			delete[] pbyGameProtoData;
			pSocketInfo->pSocket->EncodeGameProto(byTmp, wTmp, GAMEPROTO_DPROTO, &pbyGameProtoAntiBotData, dwGameProtoAntiBotSize);

			//pSocketInfo->pSocket->EncodeGameProto_Dynamic(abySendBuffer, nSize, &pbyGameProtoAntiBotData, dwGameProtoAntiBotSize);
			pSocketInfo->pSocket->SendData(pbyGameProtoAntiBotData, dwGameProtoAntiBotSize);
			delete[] pbyGameProtoAntiBotData;
		}*/
	}
	//	DebugOut("CacheTimerProc timer.");
}