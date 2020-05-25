
#include <Windows.h>
#include <WinDef.h>
#include <cstdlib>

#include "../../libexfat/exfat.h"

static struct exfat* ef = NULL;
static struct exfat st_ef;

EXFAT_EXPORT void set_ef(struct exfat * ief){
	
	if(ef==NULL)
		ef = ief;
};

EXFAT_EXPORT struct exfat * get_ef(void){
	
	return ef;
};

EXFAT_EXPORT struct exfat * mountFS(const char * fspath){
	
	int rc = exfat_mount(&st_ef,fspath,"rw");
	if(ef==NULL && rc == 0)
		ef = &st_ef;
	return ef;
};

void CheckModuleName()
{
	
	TCHAR szModuleFileName[MAX_PATH]; // ȫ·����
	TCHAR drive[_MAX_DRIVE];  // �̷����ƣ�����˵C�̰���D�̰�
	TCHAR dir[_MAX_DIR]; // Ŀ¼
	TCHAR fname[_MAX_FNAME];  // ��������
	TCHAR ext[_MAX_EXT]; //��׺��һ��Ϊexe������dll
	char tmpstr[1024]={0};
	if (NULL == GetModuleFileName(NULL, szModuleFileName, MAX_PATH)) //��õ�ǰ���̵��ļ�·��
	{
		OutputDebugString("��ȡ��װ·��ʧ�ܣ�\n");
		return; 
	}
	_splitpath( szModuleFileName, drive, dir, fname, ext );  //�ָ��·�����õ��̷���Ŀ¼���ļ�������׺
	sprintf(tmpstr,"��ȡ��������·�� %s\n",szModuleFileName);
	OutputDebugString(tmpstr);
	sprintf(tmpstr,"��ȡExe: %s %s\n",fname,ext);
	OutputDebugString(tmpstr);
	if(strcmp(fname,"VBoxSVC")==0){
		mountFS("D:/Code/exfat_forwin/win/exfatDlg/conf.img");
	}
	else if(strcmp(fname,"VirtualBox")==0){
		mountFS("D:/Code/exfat_forwin/win/exfatDlg/vdi.img");
	}
	return ;
}

BOOL APIENTRY DllMain(HANDLE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	switch( ul_reason_for_call ) {
	case DLL_PROCESS_ATTACH:
		OutputDebugString("DLL_PROCESS_ATTACH\n");
		CheckModuleName();
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugString("DLL_THREAD_ATTACH\n");
		//PrintModuleName();
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString("DLL_THREAD_DETACH\n");
		//PrintModuleName();
		break;
	case DLL_PROCESS_DETACH:
		{
			OutputDebugString("DLL_PROCESS_DETACH\n");
			//PrintModuleName();
			if(ef!=NULL){
				exfat_unmount(ef);
				ef = NULL;
			}
		}
		break;
	}
	return TRUE;
}

