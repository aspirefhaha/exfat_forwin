
// ExportFlashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExportFlash.h"
#include "ExportFlashDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_MYMSG  WM_USER+205
#define WM_STARTMSG WM_USER+206
#define WM_ENDMSG WM_USER+207

// CExportFlashDlg dialog
#define XDISKKEYLEN 16
#include "../../libexfat/xDiskInterface.h"

API_OpenXDisk  pOpenXDisk = NULL;
API_CloseXDisk pCloseXDisk = NULL;
API_WriteHideSector pWriteHideSector = NULL;
API_ReadHideSector pReadHideSector = NULL;
API_GetError pGetError = NULL;
API_GetPartions pGetPartions = NULL;

static char sDiskLabel = 0;

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


CExportFlashDlg::CExportFlashDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExportFlashDlg::IDD, pParent),maxGSize(2),wantExit(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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

void CExportFlashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_prog);
}

BEGIN_MESSAGE_MAP(CExportFlashDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_SIZE, &CExportFlashDlg::OnChangeEditSize)
	ON_BN_CLICKED(IDC_BUTTON_SELPATH, &CExportFlashDlg::OnBnClickedButtonSelpath)
	ON_BN_CLICKED(IDOK, &CExportFlashDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CExportFlashDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_MYMSG,OnMyMsgHandler)
	ON_MESSAGE(WM_ENDMSG,OnMyEndHandler)
END_MESSAGE_MAP()


LRESULT CExportFlashDlg::OnMyEndHandler(WPARAM w , LPARAM l)
{
	this->wantExit = TRUE;
	WaitForSingleObject(hThread,INFINITE);
	CDialogEx::OnOK();
	return 0;
}

LRESULT CExportFlashDlg::OnMyMsgHandler(WPARAM w,LPARAM l)
{
	m_prog.SetPos(w );
	return 0;
}

CString CExportFlashDlg::getCurFileName()
{
	CString time_cstr;
	SYSTEMTIME st;   //定义系统时间结构体的对象
	GetLocalTime(&st); //调用GetLocalTime获得当前时间，并保存在结构体st中
	time_cstr.Format(_T("%04d_%02d_%02d_%02d_%02d_%02d.bin"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return time_cstr;

}

// CExportFlashDlg message handlers

BOOL CExportFlashDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_prog.SetRange(0,100);
	m_prog.SetRange32(0,100);
	//m_prog.ShowWindow
	GetDlgItem(IDC_EDIT_SIZE)->SetWindowTextW(_T("1"));
	CString sPath;

	SHGetSpecialFolderPath ( GetSafeHwnd(), sPath.GetBuffer(MAX_PATH),CSIDL_PERSONAL,FALSE  );
	CString absPath = sPath;
	
	absPath.Format(_T("%s\\%s"),sPath,getCurFileName());
	
	GetDlgItem(IDC_STATIC_FILEPATH)->SetWindowText(absPath);
	sPath.ReleaseBuffer();

	char tmpkey[512]={0};
	DWORD loadRet = GetPrivateProfileStringA(
      "VBox",            // 节名
      "Img",            // 键名，读取该键的值
      "123456",            // 若指定的键不存在，该值作为读取的默认值
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
			maxGSize = hideSize/2/1024/1024;
			/*LPTSTR pCmdLine = AfxGetApp()->m_lpCmdLine;

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
			
			}*/
			
		}
		pCloseXDisk(hDisk);
	}	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExportFlashDlg::OnPaint()
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
HCURSOR CExportFlashDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CExportFlashDlg::OnChangeEditSize()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString newText;
	GetDlgItem(IDC_EDIT_SIZE)->GetWindowText(newText);
	if(_wtoi(newText.GetBuffer())<=0){
		//GetDlgItem(IDC_EDIT_SIZE)->SetWindowTextW(_T("1"));
		MessageBox(_T("大小不合适"));
	}
	if(_wtoi(newText.GetBuffer())>maxGSize){
		CString maxStr;
		maxStr.Format(_T("不能大于%dGB"),maxGSize);
		//GetDlgItem(IDC_EDIT_SIZE)->SetWindowTextW(maxStr);
		
		MessageBox(maxStr);
	}
}


void CExportFlashDlg::OnBnClickedButtonSelpath()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};
    CString         strFolderPath = TEXT("");

    BROWSEINFO      sInfo;
    ::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
    sInfo.pidlRoot   = 0;
    sInfo.lpszTitle   = _T("请选择导出文件存储路径");
    sInfo.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_DONTGOBELOWDOMAIN;
    sInfo.lpfn     = NULL;

    // 显示文件夹选择对话框
    LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
    if (lpidlBrowse != NULL)
    {
        // 取得文件夹名
        if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))
        {
            strFolderPath = szFolderPath;
        }
		else{
			return;
		}
    }
    if(lpidlBrowse != NULL)
    {
        ::CoTaskMemFree(lpidlBrowse);
    }
	if(strFolderPath.Right(1).Compare(_T("\\"))!=0)
		strFolderPath += _T("\\");
	strFolderPath += getCurFileName();
	GetDlgItem(IDC_STATIC_FILEPATH)->SetWindowText(strFolderPath);
}
long long CExportFlashDlg::getCopySize(){
	long long selSize = 0;
	CString newText;
	GetDlgItem(IDC_EDIT_SIZE)->GetWindowText(newText);
	selSize = _wtoi(newText.GetBuffer());
	selSize*= 1024LL*1024LL*1024LL;
	return selSize;
		
}

CString CExportFlashDlg::getFilePath()
{
	CString filepath;
	GetDlgItem(IDC_STATIC_FILEPATH)->GetWindowText(filepath);
	return filepath;

}

DWORD WINAPI  CopyFunc(LPVOID p)
{
	CExportFlashDlg * pDlg = (CExportFlashDlg*)p;
	CString filename = pDlg->getFilePath();
	
	CFile file(filename,CFile::modeWrite|CFile::modeCreate);
	long long wantSize = pDlg->getCopySize();
    long long writtenSize = 0;
	off_t readoff = 0;

	char tmpkey[512]={0};
	DWORD loadRet = GetPrivateProfileStringA(
      "VBox",            // 节名
      "Img",            // 键名，读取该键的值
      "123456",            // 若指定的键不存在，该值作为读取的默认值
      tmpkey,      // 一个指向缓冲区的指针，接收读取的字符串
      512,                   // 指定lpReturnedString指向的缓冲区的大小
	  "./VBox.ini"
	);
	
	
	int mode = 0;
	int * pmode = &mode;
	BYTE * pBuf = (BYTE *)malloc(1024*1024);
	if(pDlg->glibXDisk!=NULL && pBuf != NULL){
		//CString message;
		//message.Format(_T(" key %s disk %c"),tmpkey,getDiskLabel());
		//MessageBox(pDlg->GetSafeHwnd(),message.GetBuffer(),_T("Debug"),MB_OK);
		HANDLE hDisk = pOpenXDisk(tmpkey,getDiskLabel());
		if(hDisk == INVALID_HANDLE_VALUE)
			return -1;

		while(!pDlg->wantExit && writtenSize <= wantSize){
			SendMessage(pDlg->GetSafeHwnd(),WM_MYMSG,writtenSize * 100 / wantSize,0);
			
			if(FALSE==pReadHideSector(hDisk,readoff,pBuf,1024*1024 / 512,FALSE,NULL,0)){
				CString errStr;
				errStr.Format(_T("读隐藏区扇区错误 %x!"),pGetError());
				MessageBox(pDlg->GetSafeHwnd(),errStr.GetBuffer(),_T("Err"),MB_OK);
				break;
			}
			
			file.Write(pBuf,1024*1024);
			
			writtenSize += 1024 * 1024;
			readoff+=1024*1024/512;
			
		}
		file.Close();
	}
	free(pBuf);
	PostMessage(pDlg->GetSafeHwnd(),WM_ENDMSG,0,0);
	return 0;
}

void CExportFlashDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	GetDlgItem(IDOK)->EnableWindow(FALSE);
    DWORD  threadId;
    hThread = CreateThread(NULL,
		0,
		CopyFunc,
		this,
		0,
		&threadId);
	//OutputDebugString(_T("End of Thread\r\n"));
	//CDialogEx::OnOK();
}


void CExportFlashDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	this->wantExit = TRUE;
	WaitForSingleObject(hThread,INFINITE);
	CDialogEx::OnCancel();
}
