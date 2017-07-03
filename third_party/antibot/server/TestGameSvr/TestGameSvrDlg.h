// TestGameSvrDlg.h : header file
//

#pragma once

#include "TestGameSvrSocket.h"
#include "mainctrlbusiness.h"

// CTestGameSvrDlg dialog
class CTestGameSvrDlg : public CDialog
{
// Construction
public:
	CTestGameSvrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTGAMESVR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	int m_nTimer;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};
