


#include <Windows.h>
#include <WinDef.h>

#include "../../libexfat/exfat.h"

static struct exfat* ef = NULL;

BOOL APIENTRY DllMain(HANDLE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
switch( ul_reason_for_call ) {
case DLL_PROCESS_ATTACH:
	break;
case DLL_THREAD_ATTACH:
	break;
case DLL_THREAD_DETACH:
	break;
case DLL_PROCESS_DETACH:
	{
		if(ef!=NULL){
			exfat_unmount(ef);
			ef = NULL;
		}
	}
	break;
}
return TRUE;
}

__declspec(dllexport) void set_dllexfat(struct exfat * ief){
	
	if(ef==NULL)
		ef = ief;
};