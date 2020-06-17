#pragma once

#include "resource.h"
#include "../libexfat/exfat.h"
#include "extra_defs.h"

#define EXFATSERVERPORT 6543	//·þÎñ¶Ë¿ÚºÅ

enum TCPEXFAT_CMD {	//TCP Exfat Command
	TECMD_NEGSIZE=0,	//negotiate variable length or constant length ,default 0=constant length,1=variable length
	TECMD_EFFileOpen,	// 
	TECMD_EFFileClose,
	TECMD_EFPathQueryInfoEx,
	TECMD_EFFileSeek,
	TECMD_EFFileRead,
	TECMD_EFFileWrite,
	TECMD_EFFileFlush,
	TECMD_EFFileGetSize,
	TECMD_EFFileIsValid,
	TECMD_EFFileQueryInfo,
	TECMD_EFFileDelete,
	TECMD_EFFsQuerySizes,
	TECMD_EFFileSetSize,
	TECMD_EFDirList,
	TECMD_MAX
};

//unsigned char tcpexfat_param_bytes[] ={
//TCP EXFAT COMMAND
#define	TEC_NEGSIZE  2 	//TECMD_NEGSIZE=1,	//negotiate variable length or constant length ,default constant length
#define	TEC_EFFILEOPEN (1+4+EXFAT_UTF8_NAME_BUFFER_MAX)	//TECMD_EFFileOpen, (EXFAT_UTF8_NAME_BUFFER_MAX + 8 bytes flag) = 255+8 = 263 bytes
#define TEC_EFFILECLOSE	(1+8)	//TECMD_EFFileClose, 64 bit pointer = 8 bytes
#define TEC_EFPATHQUERYINFOEX 	(1+EXFAT_UTF8_NAME_BUFFER_MAX)	//TECMD_EFPathQueryInfoEx, EXFAT_NAME_MAX + 4(fFlags) = 255 + 4 = 259
#define TEC_EFFILESEEK	(1+8+8+4)	//TECMD_EFFileSeek,
#define TEC_EFFILEREAD	(1+8+8)	//TECMD_EFFileRead, file handle and cbToRead
#define TEC_EFFILEWRITE	(1+8+8)	//TECMD_EFFileWrite,file handle and cbToWrite
#define TEC_EFFILEFLUSH	(1+8)	//TECMD_EFFileFlush,
#define TEC_EFFILEGETSIZE	(1+8)	//TECMD_EFFileGetSize,
#define TEC_EFFILEISVALID	(1+8)	//TECMD_EFFileIsValid,
#define TEC_EFFILEQUERYINFO	(1+8)	//TECMD_EFFileQueryInfo,
#define TEC_EFFILEDELETE	(1+EXFAT_UTF8_NAME_BUFFER_MAX)	//TECMD_EFFileDelete,
#define TEC_EFFSQUERYSIZES	(1)
#define TEC_EFFILESETSIZE	(1+8+8)	//
#define TEC_EFFILELIST		(1+EXFAT_UTF8_NAME_BUFFER_MAX)
#define TEC_CMDMAX	1	//TECMD_MAX
//};

//TCP EXFAT ACK
//unsigned char tcpexfat_return_bytes[] ={
#define TEA_NEGSIZE  (2)	//TECMD_NEGSIZE=1,	//negotiate variable length or constant length ,default constant length
#define TEA_EFFILEOPEN 	(1+1+8)	//TECMD_EFFileOpen,HANDLE as 64 bit pointer
#define TEA_EFFILECLOSE	2	//TECMD_EFFileClose, 64 bit pointer = 8 bytes
#define TEA_EFPATHQUERYINFOEX	(1+sizeof(EFFSOBJINFO))	//TECMD_EFPathQueryInfoEx, EXFAT_NAME_MAX + 4(fFlags) = 255 + 4 = 259
#define TEA_EFFILESEEK	(1+8)	//TECMD_EFFileSeek,
#define TEA_EFFILEREAD	(1+8)	//TECMD_EFFileRead,
#define TEA_EFFILEWRITE	(1+8)	//TECMD_EFFileWrite,
#define TEA_EFFILEFLUSH	(1+4)	//TECMD_EFFileFlush,
#define TEA_EFFILEGETSIZE	(1+8)	//TECMD_EFFileGetSize,
#define TEA_EFFILEISVALID	(1+4)	//TECMD_EFFileIsValid,
#define TEA_EFFILEQUERYINFO	(1+sizeof(EFFSOBJINFO))	//TECMD_EFFileQueryInfo,
#define TEA_EFFILEDELETE	(1+4)	//TECMD_EFFileDelete,
#define TEA_EFFSQUERYSIZES	(1+8+8+4+4)
#define TEA_EFFILESETSIZE	(1+8+4)
#define TEA_EFDIRLIST		(1+EXFAT_UTF8_NAME_BUFFER_MAX)
#define TEA_CMDMAX	1	//TECMD_MAX



// RTR3DECL(int) EFFileOpen(PRTFILE pFile, const char *pszFilename, uint64_t fOpen)
// RTR3DECL(int) EFFileClose(RTFILE hFile)
// RTR3DECL(int) EFPathQueryInfoEx(const char *pszPath, PRTFSOBJINFO pObjInfo, RTFSOBJATTRADD enmAdditionalAttribs, uint32_t fFlags)
// RTR3DECL(int) EFFsQuerySizes(const char *pszFsPath, RTFOFF *pcbTotal, RTFOFF *pcbFree,uint32_t *pcbBlock, uint32_t *pcbSector)
// RTR3DECL(int) EFFileSeek(RTFILE hFile, int64_t offSeek, unsigned uMethod, uint64_t *poffActual)
// RTR3DECL(int) EFFileRead(RTFILE hFile, void *pvBuf, size_t cbToRead, size_t *pcbRead)
// RTR3DECL(int) EFFileWrite(RTFILE hFile, const void *pvBuf, size_t cbToWrite, size_t *pcbWritten)
// RTR3DECL(int) EFFileFlush(RTFILE hFile)
// RTR3DECL(int) EFFileQuerySize(const char * filename ,uint64_t *pcbSize)
// RTR3DECL(int) EFFileGetSize(RTFILE hFile, uint64_t *pcbSize)
// RTR3DECL(bool) EFFileIsValid(RTFILE hFile)
// RTR3DECL(int) EFFileQueryInfo(RTFILE hFile, PRTFSOBJINFO pObjInfo, RTFSOBJATTRADD enmAdditionalAttribs)
// RTR3DECL(int) EFFileDelete(const char *pszFilename)
// RTR3DECL(int) EFFsQuerySizes(const char *pszFsPath, RTFOFF *pcbTotal, RTFOFF *pcbFree, uint32_t *pcbBlock, uint32_t *pcbSector)
