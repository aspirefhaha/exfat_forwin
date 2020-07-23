
// ExportFlashDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CExportFlashDlg dialog
class CExportFlashDlg : public CDialogEx
{
// Construction
public:
	CExportFlashDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EXPORTFLASH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnMyMsgHandler(WPARAM w,LPARAM l);
	afx_msg LRESULT OnMyEndHandler(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()
	CString getCurFileName();

	HANDLE hDisk;
	int maxGSize ;

public:
	afx_msg void OnChangeEditSize();
	afx_msg void OnBnClickedButtonSelpath();
	afx_msg void OnBnClickedOk();

	BOOL wantExit;
	afx_msg void OnBnClickedCancel();
	HANDLE  hThread;

	CString getFilePath();
	long long getCopySize();
	HMODULE glibXDisk;
	CProgressCtrl m_prog;
};
