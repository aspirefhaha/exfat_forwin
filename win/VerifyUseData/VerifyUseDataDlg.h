
// VerifyUseDataDlg.h : header file
//

#pragma once


// CVerifyUseDataDlg dialog
class CVerifyUseDataDlg : public CDialogEx
{
// Construction
public:
	CVerifyUseDataDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VERIFYUSEDATA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HANDLE hDisk;
	HMODULE glibXDisk;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
