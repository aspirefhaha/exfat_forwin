
// VerifyUseDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VerifyUseData.h"
#include "VerifyUseDataDlg.h"
#include "afxdialogex.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define XDISKKEYLEN 16
#include "../../libexfat/xDiskInterface.h"
// CVerifyUseDataDlg dialog

API_OpenXDisk  pOpenXDisk = NULL;
API_CloseXDisk pCloseXDisk = NULL;
API_WriteHideSector pWriteHideSector = NULL;
API_ReadHideSector pReadHideSector = NULL;
API_GetError pGetError = NULL;
API_GetPartions pGetPartions = NULL;

static char sDiskLabel = 'E';

static char getDiskLabel()
{
	if(sDiskLabel ==0){
		char tDisk[5] ={0};
		if(GetPrivateProfileStringA("XDisk","Label",NULL,tDisk,5,"./DebugConf.ini")>0){
			sDiskLabel = tDisk[0];
		}
		else{
			char selfname[MAX_PATH]={0};
			if(GetModuleFileNameA(NULL,selfname,MAX_PATH)>0){
				char path_buffer[_MAX_PATH];
				char drive[_MAX_DRIVE];
				char dir[_MAX_DIR];
				char fname[_MAX_FNAME];
				char ext[_MAX_EXT];
				_splitpath(selfname,drive,dir,fname,ext);
				sDiskLabel = drive[0];
			}

		}
	}
	return sDiskLabel;
}



CVerifyUseDataDlg::CVerifyUseDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVerifyUseDataDlg::IDD, pParent),hDisk(INVALID_HANDLE_VALUE),glibXDisk(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	glibXDisk = LoadLibraryA("xDiskInterface.dll");
	if(glibXDisk== NULL){
		MessageBox(_T("Load xDiskInterface.dll Failed!!!!!!!!!!!\n"));
		return;
	}
	OutputDebugString(_T("Load xDiskInterface.dll Ok\n"));
	//MessageBox(NULL,"Load xDiskInterface.dll Ok","Load",MB_OK);
	pOpenXDisk = (API_OpenXDisk)GetProcAddress(glibXDisk,"OpenXDisk");
	if(pOpenXDisk==NULL){
		OutputDebugString(_T("Get OpenXDisk Failed!!!!!!!!!!!\n"));
		return;
	}
	pCloseXDisk = (API_CloseXDisk)GetProcAddress(glibXDisk,"CloseXDisk");
	if(pCloseXDisk==NULL){
		OutputDebugString(_T("Get CloseXDisk Failed!!!!!!!!!!\n"));
		return;
	}

	pWriteHideSector = (API_WriteHideSector)GetProcAddress(glibXDisk,"WriteHideSector");
	if(pWriteHideSector==NULL){
		OutputDebugString(_T("Get WriteHideSector Failed!!!!!!!!!!!\n"));
		return;
	}

	pReadHideSector = (API_ReadHideSector)GetProcAddress(glibXDisk,"ReadHideSector");
	if(pReadHideSector==NULL){
		OutputDebugString(_T("Get ReadHideSector Failed!!!!!!!!!!!\n"));
		return;
	}

	pGetError = (API_GetError)GetProcAddress(glibXDisk,"GetError");
	if(pGetError==NULL){
		OutputDebugString(_T("Get GetError Failed!!!!!!!!!!!!!\n"));
		return;
	}

	pGetPartions = (API_GetPartions)GetProcAddress(glibXDisk,"GetPartions");
	if(pGetPartions == NULL){
		OutputDebugString(_T("Get GetPartions Failed!!!!!!!!!!!!\n"));
		return;
	}
	
}

void CVerifyUseDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVerifyUseDataDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CVerifyUseDataDlg message handlers

BOOL CVerifyUseDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	char tmpkey[512]={0};
	DWORD loadRet = GetPrivateProfileStringA(
      "VBox",            // 节名
      "Img",            // 键名，读取该键的值
      "1q2W@_ZS",            // 若指定的键不存在，该值作为读取的默认值
      tmpkey,      // 一个指向缓冲区的指针，接收读取的字符串
      512,                   // 指定lpReturnedString指向的缓冲区的大小
	  "./VBox.ini"
	);
	
	hDisk = pOpenXDisk(tmpkey,getDiskLabel());
	if(hDisk == INVALID_HANDLE_VALUE)
		return FALSE;
	int mode = 0;
	int * pmode = &mode;
	if(glibXDisk!=NULL){
		BYTE tmpsec[512]={0};
		DWORD totalSize=0;
		DWORD hideSize=0;
		DWORD normalSize =0;
		DWORD secSize = 0;
		unsigned char savedkey[XDISKKEYLEN] = {0};
		if(pGetPartions(hDisk,&totalSize,&hideSize,&normalSize,&secSize)){
			wchar_t tmpstr[256]={0};

			LPTSTR pCmdLine = AfxGetApp()->m_lpCmdLine;

			if(wcslen(pCmdLine)>0){
				if(wcscmp(pCmdLine,_T("RESCUE"))==0){
					pReadHideSector(hDisk,hideSize - 1,tmpsec,1,FALSE,NULL,XDISK_ENCRYPT_TYPE_NONE);
					pWriteHideSector(hDisk,hideSize - 2,tmpsec,1,FALSE,NULL,XDISK_ENCRYPT_TYPE_NONE);
					OnCancel();

				}
				else{

					pWriteHideSector(hDisk,hideSize - 2,tmpsec,1,FALSE,NULL,XDISK_ENCRYPT_TYPE_NONE);
				}
				pCloseXDisk(hDisk);
				OnCancel();
				return FALSE;
			}
			else{
				pReadHideSector(hDisk,hideSize - 2,tmpsec,1,FALSE,NULL,XDISK_ENCRYPT_TYPE_NONE);
				memcpy(pmode,tmpsec,sizeof(int));
				memcpy(savedkey,tmpsec+sizeof(int),XDISKKEYLEN);
			
				wsprintf(tmpstr,_T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
							savedkey[0],savedkey[1],
							savedkey[2],savedkey[3],
							savedkey[4],savedkey[5],
							savedkey[6],savedkey[7],
							savedkey[8],savedkey[9],
							savedkey[10],savedkey[11],
							savedkey[12],savedkey[13],
							savedkey[14],savedkey[15]);
				GetDlgItem(IDC_STATIC_KEY1)->SetWindowText(tmpstr);
				wsprintf(tmpstr,_T("%s"),mode==0?_T("明文"):(mode==1?_T("AES"):(mode==2?_T("SM4"):_T("未知"))));
				GetDlgItem(IDC_STATIC_MODE1)->SetWindowText(tmpstr);
				pReadHideSector(hDisk,hideSize - 1,tmpsec,1,FALSE,NULL,XDISK_ENCRYPT_TYPE_NONE);
				memcpy(pmode,tmpsec,sizeof(int));
				memcpy(savedkey,tmpsec+sizeof(int),XDISKKEYLEN);
			
				wsprintf(tmpstr,_T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
							savedkey[0],savedkey[1],
							savedkey[2],savedkey[3],
							savedkey[4],savedkey[5],
							savedkey[6],savedkey[7],
							savedkey[8],savedkey[9],
							savedkey[10],savedkey[11],
							savedkey[12],savedkey[13],
							savedkey[14],savedkey[15]);
				GetDlgItem(IDC_STATIC_KEY2)->SetWindowText(tmpstr);
				wsprintf(tmpstr,_T("%s"),mode==0?_T("明文"):(mode==1?_T("AES"):(mode==2?_T("SM4"):_T("未知"))));
				GetDlgItem(IDC_STATIC_MODE2)->SetWindowText(tmpstr);
			}
			
		}
		pCloseXDisk(hDisk);
	}	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVerifyUseDataDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVerifyUseDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

