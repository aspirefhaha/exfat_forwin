// exfatserver.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "exfatserver.h"
#include <Windows.h>

#include <stdio.h>
#include <WinSock2.h>
#include <assert.h>
#include "../libexfat/exfat.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libexfat.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


static struct exfat *ef;

struct threadParam {
	SOCKET clientSock;
	SOCKADDR_IN addrClient;
	
};
 

void myexit()
{
	char tmpstr[MAX_LOADSTRING]={0};
	sprintf_s(tmpstr,MAX_LOADSTRING,"myexit\n");
	OutputDebugString(tmpstr);
	if(ef){
		//exfat_unmount(ef);
		//ef=NULL;
	}
}

static int clientNum = 0;
static HANDLE ghMutex = INVALID_HANDLE_VALUE;
static HANDLE ghWRMutex = INVALID_HANDLE_VALUE;

DWORD WorkerThread(LPVOID p)
{
	char tmpstr[MAX_LOADSTRING];
    struct threadParam *  clientParam  = (struct threadParam   *)p;
	SOCKET sockConn = clientParam->clientSock;
	WaitForSingleObject(ghMutex, INFINITE);
	clientNum++;
	ReleaseMutex(ghMutex);
	int sendret = 0;
	if (sockConn == INVALID_SOCKET)
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"accept() called falied!!!!!!!!!!!!!!!! The error code is: %d\n", WSAGetLastError());
		
	}
	else
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"The Server receive a new client connection [%s,%d !\n", inet_ntoa(clientParam->addrClient.sin_addr), clientParam->addrClient.sin_port);OutputDebugString(tmpstr);
		do{
			//发送数据
			//send(sockConn, sendBuf, strlen(sendBuf)+1 , 0);
			char recvCmd;
			//接收数据
			int recvlen = recv(sockConn, &recvCmd, 1, 0);
			//打印接收到的数据
			if(recvlen>0){
				//sprintf_s(tmpstr,MAX_LOADSTRING,"receive cmd %x from client side [%s,%d]\n",recvCmd, inet_ntoa(clientParam->addrClient.sin_addr), clientParam->addrClient.sin_port);
				//OutputDebugString(tmpstr);
				
				switch(recvCmd){
				case TECMD_EFFileOpen:	//
					{
						char cmdParams[TEC_EFFILEOPEN-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						
						struct exfat_node* node = NULL;
						int findres = -1;
						DWORD dwCreationDisposition=0;
						char localfilename[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
						while(tmpRecvLen < TEC_EFFILEOPEN-1 && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEOPEN-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEOPEN-1){
							assert(0);
							break;
						}
						memcpy(localfilename,cmdParams+4,EXFAT_UTF8_NAME_BUFFER_MAX);
						//dwCreationDisposition = *(DWORD *)cmdParams;
						memcpy(&dwCreationDisposition,cmdParams,sizeof(dwCreationDisposition));
						
						char * shortname  = strchr((char *)localfilename,'\\');
						while( shortname  != NULL ){
							*shortname = '/';
							shortname = strchr((char*)localfilename,'\\');
						}
						if(localfilename[0] != '/')
						{
							int len = strlen(localfilename);
							while(len>0){
								localfilename[len] = localfilename[len-1];
								len--;
							}
							localfilename[0] = '/';
						}
						
						switch(dwCreationDisposition){
						case OPEN_EXISTING:
							findres = exfat_lookup(ef,&node,localfilename);
							if(findres==0){
								rc = 0;
							}
							break;
						case CREATE_NEW:
							findres = exfat_lookup(ef,&node,localfilename);
							if(findres==0){
								exfat_put_node(ef,node);
							}
							else{
								int createres = exfat_mknod(ef,localfilename);
								if(createres == 0){
									findres = exfat_lookup(ef,&node,localfilename);
									if(findres==0){
										rc = 0;
									}
								}
							}
							break;
						case OPEN_ALWAYS:
							findres = exfat_lookup(ef,&node,localfilename);
							if(findres==0){
								rc = 0;
							}
							else{
								int createres = exfat_mknod(ef,localfilename);
								if(createres == 0){
									findres = exfat_lookup(ef,&node,localfilename);
									if(findres==0){
										rc = 0;
									}
								}
							}
							break;
						case TRUNCATE_EXISTING:
							findres = exfat_lookup(ef,&node,localfilename);
							if(findres==0){
								exfat_truncate(ef,node,0,1);
								rc = 0;
							}
							break;
						case CREATE_ALWAYS:
							findres = exfat_lookup(ef,&node,localfilename);
							if(findres==0){
								exfat_truncate(ef,node,0,1);
								rc = 0;
							}
							else{
								int createres = exfat_mknod(ef,localfilename);
								if(createres == 0){
									findres = exfat_lookup(ef,&node,localfilename);
									if(findres==0){
										rc = 0;
									}
								}
							}
							break;
						default:
							rc = -1;
							break;
						}
						do{
							char backdata[TEA_EFFILEOPEN]={0};
							backdata[0]= TECMD_EFFileOpen;
							backdata[1] = rc;
							memcpy(&backdata[2],&node,sizeof(node));
							sendret = send(sockConn, backdata, TEA_EFFILEOPEN , 0);
						}while(0);

					}
					break;
				case TECMD_EFFileClose:
					{
						char cmdParams[TEC_EFFILECLOSE-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						
						struct exfat_node* pnode = NULL;
						
						while(tmpRecvLen < TEC_EFFILECLOSE-1 && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILECLOSE-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILECLOSE-1){
							assert(0);
							break;
						}

						memcpy(&pnode,cmdParams,sizeof(struct exfat_node*));
						WaitForSingleObject(ghMutex,INFINITE);
	
						exfat_flush_node(ef,pnode);
						exfat_put_node(ef,pnode);
						
						ReleaseMutex(ghMutex);
						pnode->curpos = 0;
						do{
							char backdata[TEA_EFFILECLOSE]={0};
							backdata[0]= TECMD_EFFileClose;
							backdata[1] = 0;
							sendret = send(sockConn, backdata, TEA_EFFILECLOSE , 0);
						}while(0);
					}
					break;
				case TECMD_EFPathQueryInfoEx:
					{
						char cmdParams[TEC_EFPATHQUERYINFOEX-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						
						struct exfat_node* node = NULL;
						int findres = -1;
						DWORD dwCreationDisposition=0;
						char localfilename[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
						while(tmpRecvLen < (TEC_EFPATHQUERYINFOEX-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFPATHQUERYINFOEX-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFPATHQUERYINFOEX-1)
							break;
						memcpy(localfilename,cmdParams,EXFAT_UTF8_NAME_BUFFER_MAX);
						char * shortname  = strchr((char *)localfilename,'\\');
						while( shortname  != NULL ){
							*shortname = '/';
							shortname = strchr((char*)localfilename,'\\');
						}
						if(localfilename[0] != '/')
						{
							int len = strlen(localfilename);
							while(len>0){
								localfilename[len] = localfilename[len-1];
								len--;
							}
							localfilename[0] = '/';
        
						}
						struct exfat_node * pnode ;
						EFFSOBJINFO ObjInfo =  {0};
						rc = exfat_lookup(ef,&pnode,localfilename);
						if(rc == 0){
							ObjInfo.cbObject    = pnode->size;
							ObjInfo.cbAllocated = ROUND_UP(pnode->size, CLUSTER_SIZE(*ef->sb)) ;
							ObjInfo.AccessTime.i64NanosecondsRelativeToUnixEpoch  = pnode->atime;
							ObjInfo.BirthTime.i64NanosecondsRelativeToUnixEpoch   = pnode->mtime;
							ObjInfo.ModificationTime.i64NanosecondsRelativeToUnixEpoch = pnode->mtime;
							exfat_put_node(ef,pnode);
    
							ObjInfo.ChangeTime  = ObjInfo.ModificationTime;
						}
						do{
							char backdata[TEA_EFPATHQUERYINFOEX]={0};
							backdata[0]= TECMD_EFPathQueryInfoEx;
							memcpy(&backdata[1] ,&ObjInfo,sizeof(ObjInfo));
							sendret = send(sockConn, backdata, TEA_EFPATHQUERYINFOEX , 0);
						}while(0);
						
					}
					break;
				case TECMD_EFFileSeek:
					{
						char cmdParams[TEC_EFFILESEEK-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						int64_t offSeek;
						unsigned uMethod;	
						struct exfat_node* pnode = NULL;
						while(tmpRecvLen < (TEC_EFFILESEEK-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILESEEK-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILESEEK-1){
							assert(0);
							break;
						}
						memcpy(&pnode,&cmdParams[0],sizeof(struct exfat_node*));
						memcpy(&offSeek,&cmdParams[8],sizeof(offSeek));
						memcpy(&uMethod,&cmdParams[12],sizeof(uMethod));
						long long offActual = -1;
						WaitForSingleObject(ghMutex,INFINITE);
						switch(uMethod){
							case EFFILE_SEEK_BEGIN:
								pnode->curpos = offSeek;
								offActual = pnode->curpos;
								break;
							case EFFILE_SEEK_CURRENT:
								pnode->curpos += offSeek;
								offActual = pnode->curpos;
								break;
							case EFFILE_SEEK_END:
								pnode->curpos = pnode->size;
								pnode->curpos += offSeek;
								offActual = pnode->curpos;
								break;
							default:
								break;
						}
						ReleaseMutex(ghMutex);
						do{
							char backdata[TEA_EFFILESEEK]={0};
							backdata[0]= TECMD_EFFileSeek;
							memcpy(&backdata[1] ,&offActual,sizeof(offActual));
							
							sendret = send(sockConn, backdata, TEA_EFFILESEEK , 0);
							
						}while(0);
					}
					break;
				case TECMD_EFFileRead:
					{
						char cmdParams[TEC_EFFILEREAD-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						long long cbToRead = 0;
						struct exfat_node* pnode = NULL;
						while(tmpRecvLen < (TEC_EFFILEREAD-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEREAD-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEREAD-1){
							assert(0);
							break;
						}
						memcpy(&pnode,&cmdParams[0],sizeof(struct exfat_node*));
						memcpy(&cbToRead,&cmdParams[8],sizeof(cbToRead));
						if(((uint64_t)pnode->curpos) >= pnode->size || pnode->curpos + cbToRead > pnode->size){
							assert(-1);
						}
						unsigned char * pvBuf = (unsigned char *)malloc(cbToRead + TEA_EFFILEREAD);
						WaitForSingleObject(ghMutex,INFINITE);
						long long  retsize =  exfat_generic_pread(ef,pnode,pvBuf+TEA_EFFILEREAD,cbToRead,pnode->curpos);
						pnode->curpos += retsize;
						ReleaseMutex(ghMutex);
						do{
							pvBuf[0]= TECMD_EFFileRead;
							memcpy(&pvBuf[1] ,&retsize,sizeof(retsize));
							int sendbacksize = (int)retsize;
							sendret = send(sockConn, (const char *)pvBuf, sendbacksize + TEA_EFFILEREAD , 0);
						}while(0);
						free(pvBuf);
					}
					break;
				case TECMD_EFFileWrite:
					{
						char cmdParams[TEC_EFFILEWRITE-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						long long cbToWrite = 0;
						struct exfat_node* pnode = NULL;
						while(tmpRecvLen < (TEC_EFFILEWRITE-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEWRITE-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEWRITE-1){
							assert(0);
							break;
						}
						memcpy(&pnode,&cmdParams[0],sizeof(struct exfat_node*));
						memcpy(&cbToWrite,&cmdParams[8],sizeof(cbToWrite));
						char * pvBuf = ( char *)malloc(cbToWrite);
						curLen = 1;
						tmpRecvLen = 0;
						while(tmpRecvLen < cbToWrite && curLen >0){
							curLen =  recv(sockConn,pvBuf+tmpRecvLen,cbToWrite-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						WaitForSingleObject(ghMutex,INFINITE);
						long long  retsize = exfat_generic_pwrite(ef,pnode,pvBuf,cbToWrite,pnode->curpos);
						pnode->curpos += retsize;
						ReleaseMutex(ghMutex);
						do{
							char backdata[TEA_EFFILEWRITE]={0};
							backdata[0]= TECMD_EFFileWrite;
							memcpy(&backdata[1] ,&retsize,sizeof(retsize));
							sendret = send(sockConn, backdata, TEA_EFFILEWRITE , 0);
						}while(0);
						free(pvBuf);
					}
					break;
				case TECMD_EFFileFlush:
					{
						char cmdParams[TEC_EFFILEFLUSH-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						struct exfat_node* pnode = NULL;
						while(tmpRecvLen < (TEC_EFFILEFLUSH-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEFLUSH-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEFLUSH-1){
							assert(0);
							break;
						}
						memcpy(&pnode,&cmdParams[0],sizeof(struct exfat_node*));
						
						rc = exfat_flush_node(ef,pnode);
						
						do{
							char backdata[TEA_EFFILEFLUSH]={0};
							backdata[0]= TECMD_EFFileFlush;
							memcpy(&backdata[1] ,&rc,sizeof(rc));
							sendret = send(sockConn, backdata, TEA_EFFILEFLUSH , 0);
						}while(0);
					}
					break;
				
				case TECMD_EFFileGetSize:
					{
						char cmdParams[TEC_EFFILEGETSIZE-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						struct exfat_node* pnode = NULL;
						while(tmpRecvLen < (TEC_EFFILEGETSIZE-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEGETSIZE-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEGETSIZE-1){
							assert(0);
							break;
						}
						memcpy(&pnode,&cmdParams[0],sizeof(struct exfat_node*));
						
						//int rc = exfat_flush_node(ef,pnode);
						
						do{
							char backdata[TEA_EFFILEGETSIZE]={0};
							backdata[0]= TECMD_EFFileGetSize;
							memcpy(&backdata[1] ,&pnode->size,sizeof(pnode->size));
							sendret = send(sockConn, backdata, TEA_EFFILEGETSIZE , 0);
						}while(0);
					}
					break;
				case TECMD_EFFileIsValid:
					{
						char cmdParams[TEC_EFFILEISVALID-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						struct exfat_node* pnode = NULL;
						while(tmpRecvLen < (TEC_EFFILEISVALID-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEISVALID-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEISVALID-1){
							assert(0);
							break;
						}
						memcpy(&pnode,&cmdParams[0],sizeof(struct exfat_node*));
						
						//int rc = exfat_flush_node(ef,pnode);
						rc = pnode->is_unlinked;
						do{
							char backdata[TEA_EFFILEISVALID]={0};
							backdata[0]= TECMD_EFFileIsValid;
							memcpy(&backdata[1] ,&rc,sizeof(rc));
							sendret = send(sockConn, backdata, TEA_EFFILEISVALID , 0);
						}while(0);
					}
					break;
				case TECMD_EFFileQueryInfo:
					{
						char cmdParams[TEC_EFFILEQUERYINFO-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						struct exfat_node* pnode = NULL;
						while(tmpRecvLen < (TEC_EFFILEQUERYINFO-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEQUERYINFO-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEQUERYINFO-1){
							assert(0);
							break;
						}
						memcpy(&pnode,&cmdParams[0],sizeof(struct exfat_node*));
						
						EFFSOBJINFO ObjInfo =  {0};
						if(rc == 0){
							ObjInfo.cbObject    = pnode->size;
							ObjInfo.cbAllocated = ROUND_UP(pnode->size, CLUSTER_SIZE(*ef->sb)) ;
							ObjInfo.AccessTime.i64NanosecondsRelativeToUnixEpoch  = pnode->atime;
							ObjInfo.BirthTime.i64NanosecondsRelativeToUnixEpoch   = pnode->mtime;
							ObjInfo.ModificationTime.i64NanosecondsRelativeToUnixEpoch = pnode->mtime;
							exfat_put_node(ef,pnode);
    
							ObjInfo.ChangeTime  = ObjInfo.ModificationTime;
						}
						do{
							char backdata[TEA_EFFILEQUERYINFO]={0};
							backdata[0]= TECMD_EFFileQueryInfo;
							memcpy(&backdata[1] ,&ObjInfo,sizeof(ObjInfo));
							sendret = send(sockConn, backdata, TEA_EFFILEQUERYINFO , 0);
						}while(0);
					}
					break;
				case TECMD_EFFileDelete:
					{
						char cmdParams[TEC_EFFILEDELETE-1]={0};
						int tmpRecvLen = 0;
						int curLen = 1;
						char rc = -1;
						
						struct exfat_node* node = NULL;
						int findres = -1;
						char localfilename[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
						while(tmpRecvLen < (TEC_EFFILEDELETE-1) && curLen >0){
							curLen =  recv(sockConn,cmdParams+tmpRecvLen,TEC_EFFILEDELETE-1-tmpRecvLen,0);
							tmpRecvLen += curLen;
						}
						if(tmpRecvLen != TEC_EFFILEDELETE-1)
							break;
						memcpy(localfilename,cmdParams,EXFAT_UTF8_NAME_BUFFER_MAX);
						char * shortname  = strchr((char *)localfilename,'\\');
						while( shortname  != NULL ){
							*shortname = '/';
							shortname = strchr((char*)localfilename,'\\');
						}
						if(localfilename[0] != '/')
						{
							int len = strlen(localfilename);
							while(len>0){
								localfilename[len] = localfilename[len-1];
								len--;
							}
							localfilename[0] = '/';
        
						}
						struct exfat_node * pnode ;
						rc = exfat_lookup(ef,&pnode,localfilename);
						if(rc == 0){
							exfat_cleanup_node(ef,pnode);
							exfat_put_node(ef,pnode);
						}
						do{
							char backdata[TEA_EFFILEDELETE]={0};
							backdata[0]= TECMD_EFFileDelete;
							memcpy(&backdata[1] ,&rc,sizeof(rc));
							sendret = send(sockConn, backdata, TEA_EFFILEDELETE , 0);
						}while(0);
						
					}
					break;
				case TECMD_EFFsQuerySizes:
					{
						RTFOFF pcbTotal = ef->dev->size;
						RTFOFF pcbFree = (100 - ef->sb->allocated_percent) * ef->dev->size;
                        uint32_t pcbBlock = 1 << (ef->sb->sector_bits+ ef->sb->spc_bits);
						uint32_t pcbSector = 1 << ef->sb->sector_bits;
						do{
							char backdata[TEA_EFFSQUERYSIZES]={0};
							backdata[0]= TECMD_EFFsQuerySizes;
							memcpy(&backdata[1] ,&pcbTotal,sizeof(pcbTotal));
							memcpy(&backdata[9] ,&pcbFree,sizeof(pcbFree));
							memcpy(&backdata[17] ,&pcbBlock,sizeof(pcbBlock));
							memcpy(&backdata[21] ,&pcbSector,sizeof(pcbSector));
							sendret = send(sockConn, backdata, TEA_EFFSQUERYSIZES , 0);
						}while(0);
					}
					break;
				default:
					sendret = 0;
					break;
				}
			}
			else{
				sprintf_s(tmpstr,MAX_LOADSTRING,"receive data from client side [%s,%d] len %d\n",inet_ntoa(clientParam->addrClient.sin_addr),clientParam->addrClient.sin_port,recvlen);
				OutputDebugString(tmpstr);
				break;
			}
			if(sendret<=0){
				break;
			}

		}while(1);
		closesocket(sockConn);	//关闭连接套接字
	}
	WaitForSingleObject(ghMutex,INFINITE);
	clientNum--;
	ReleaseMutex(ghMutex);
	//Sleep(3000);
	sprintf_s(tmpstr,MAX_LOADSTRING,"Now Client Num  %d\n",clientNum);
	OutputDebugString(tmpstr);
	WaitForSingleObject(ghMutex,INFINITE);
	if(clientNum<=0){
		ReleaseMutex(ghMutex);
		sprintf_s(tmpstr,MAX_LOADSTRING,"Now Client Num  %d,Need Exit\n",clientNum);
		OutputDebugString(tmpstr);
		exit(0);
		return 0;
	}
	ReleaseMutex(ghMutex);
    return 0 ;

}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	ghMutex = CreateMutex(NULL,FALSE,NULL);
	ghWRMutex = CreateMutex(NULL,FALSE,NULL);
#if 0
 	// TODO: Place code here.
	MSG msg={0};
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXFATSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXFATSERVER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
#endif
	char tmpstr[MAX_LOADSTRING];
	
	//加载套接字库
	WORD wVersionRequested;//用于保存WinSock库的版本号
	WSADATA wsaData;
	int err;
	atexit(myexit);
	sprintf_s(tmpstr,MAX_LOADSTRING,"This is a Server side application!\n");OutputDebugString(tmpstr);
 
	wVersionRequested = MAKEWORD(2,2);
 
	err = WSAStartup( wVersionRequested, &wsaData);
	if (err != 0)
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"WSAStartup() called failed!\n");OutputDebugString(tmpstr);
		MessageBox(NULL,tmpstr,"Start Failed",MB_OK);
		return -1;
	}
	else
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"WSAStartup() called successful!\n");OutputDebugString(tmpstr);
		//MessageBox(NULL,tmpstr,"Start Failed",MB_OK);
	}
 
	if (LOBYTE(wsaData.wVersion) != 2 ||
		  HIBYTE(wsaData.wVersion) != 2)
	{
		//若不是所请求的版本号2.2，则终止WinSock库的使用
		WSACleanup();
		return -1;
	}
 
	//创建用于监听的套接字
	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if(sockServer == INVALID_SOCKET)
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"socket() called failed! ,error code is: %d", WSAGetLastError());
		MessageBox(NULL,tmpstr,"Start Failed",MB_OK);
		return -1;
	}
	else
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"socket() called successful!\n");OutputDebugString(tmpstr);
		//MessageBox(NULL,tmpstr,"Start Failed",MB_OK);
	}
	
	//填充服务器端套接字结构
	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//将主机字节顺序转换成TCP/IP网络字节顺序
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(EXFATSERVERPORT);
 
	//将套接字绑定到一个本地地址和端口上
	err = bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (err == SOCKET_ERROR)
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"bind() called failed! The error code is: %d\n", WSAGetLastError());
		return -1;
	}
	else
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"bind() called successful\n");OutputDebugString(tmpstr);
	}
	//将套接字设置为监听模式，准备接收客户请求
	err = listen(sockServer, 5);
	if (err == SOCKET_ERROR)
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"listen() called failed! The error code is: %d\n", WSAGetLastError());
		return -1;
	}
	else
	{
		sprintf_s(tmpstr,MAX_LOADSTRING,"listen() called successful!\n");OutputDebugString(tmpstr);
	}
 
	SOCKADDR_IN addrClient;	//保存发送请求连接的客户端的套接字信息
	int len = sizeof(SOCKADDR);
	DWORD loadRet = GetPrivateProfileString(
      "VBox",            // 节名
      "Img",            // 键名，读取该键的值
      "D:/Code/exfat_forwin/win/exfatDlg/vdi.img",            // 若指定的键不存在，该值作为读取的默认值
      tmpstr,      // 一个指向缓冲区的指针，接收读取的字符串
      MAX_LOADSTRING,                   // 指定lpReturnedString指向的缓冲区的大小
	  "./VBox.ini"
	);
	ef = mountFS(tmpstr);

	if(ef==NULL){
		MessageBox(NULL,tmpstr,"Mount IMG  Err",MB_OK);
		return -1;
	}
	MessageBox(NULL,tmpstr,"Mount Ok",MB_OK);
	MessageBox(NULL,"Close This Dialog Now","Start Server Ok",MB_OK);
	while(1)
	{
		//等待客户请求到来
		SOCKET SocketAccept = accept(sockServer, (SOCKADDR*)&addrClient, &len);
		if(SocketAccept>0){
			struct threadParam clientParam;
			memcpy(&(clientParam.addrClient),&addrClient,sizeof(addrClient));
			clientParam.clientSock = SocketAccept;
			DWORD dThreadID = 0;
			CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WorkerThread,&clientParam ,0,&dThreadID);
			sprintf_s(tmpstr,MAX_LOADSTRING,"accept() get client create thread id %d!\n",dThreadID);OutputDebugString(tmpstr);
		}
		else{
			break;
		}
		
	}
	if(ghMutex != NULL){
		CloseHandle(ghMutex);
		ghMutex=NULL;
	}
	if(ghWRMutex != NULL){
		CloseHandle(ghWRMutex);
		ghWRMutex = NULL;
	}
 
	return 0;
}




//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXFATSERVER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXFATSERVER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
