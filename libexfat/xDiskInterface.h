//////////////////////////////////////////////////////////////////////////////////////////////
/*++ BUILD Version: 0001

Copyright (c) 2016

Module Name:

    xDiskInterface.h

Abstract:

    Defines the interface for xDisk device.

--*/
/*++ Change log
VERSION    DATE           CHANGE
1.0.0.0    20160901       创建动态库	
1.0.0.1    20160915       V1.0.0.1首个版本发行；
1.0.0.2	   20160925		  授权码验证方式修改；
1.0.0.3    20160928       设备ID获取方式修改；
--*/
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef XDISK_API

// xDiskInterface should be defined in all of the DLL's source
// code modules before this header file is included.

// All functions/variables are being exported.

#else

// Indicate that all functions/variables are being imported.

#define XDISK_API extern "C" __declspec(dllimport)

#endif


//////////////////////////////////////////////////////////////////////////////////////////////
// Define exported function prototypes here


/*********************************************************************************************
函数声明：	HANDLE OpenXDisk(char* AuthorizeKey, char DriverLetter)
功能描述：	打开指定盘符的设备，检查访问授权码。
			如果设备类型不对，或者授权码不正确，均会引起函数调用失败。
			如果调用成功，则打开相应设备，返回设备句柄。
输入参数：	AuthorizeKey 最大12个字节的带空终结符的字符串，初始值在初始化时设定。
			DriverLetter 指定的盘符。
输出参数：	无。
返 回 值：	HANDLE 设备句柄
**********************************************************************************************/
//XDISK_API
//HANDLE 
//OpenXDisk(char* AuthorizeKey, char DriverLetter);   

typedef HANDLE (* API_OpenXDisk)(char* AuthorizeKey, char DriverLetter);


/*********************************************************************************************
函数声明：	BOOL CloseXDisk(HANDLE handle)
功能描述：	关闭由Open类函数（OpenXDisk）打开的设备
输入参数：	handle 设备句柄
输出参数：	无。
返 回 值：	BOOL。
*********************************************************************************************/
//XDISK_API
//BOOL 
//CloseXDisk(HANDLE handle);

typedef BOOL (* API_CloseXDisk)(HANDLE handle);

/*********************************************************************************************
函数声明：	BOOL ReadHideSector(HANDLE handle, DWORD Sectoroffset, BYTE *pData, 
								DWORD SectorNumber, BOOL bDecrypt = FALSE, BYTE *pKey)
功能描述：	该函数从隐藏区指定偏移处读取用户指定个数扇区的数据到缓冲区中。由于可以
			在隐藏区内部任意指定偏移开始读写，意味着可以将隐藏区划分为不同用途的子
			区块，存放不同用户的数据。
输入参数：	handle-设备句柄。
			Sectoroffset-指定从隐藏区内第几个扇区开始读，每扇区为512字节，可以为0,1…          
			SectorNumber-指定需要读取的扇区个数，每扇区为512字节。
			bDecrypt-是否需要对读取的数据进行解密。
			pKey-如果bDecrypt为TRUE，这里需要指定解密秘钥，16字节（128位）。
输出参数：	pData-用于存放从隐藏区读取的数据的用户缓冲区；
返 回 值：	BOOL。
*********************************************************************************************/
//XDISK_API
//BOOL 
//ReadHideSector(HANDLE handle,  
//               DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...
//               BYTE *pData,           // Receive Data buf
//               DWORD SectorNumber,    // Sectors(512 bytes per sector) to read
//               BOOL bDecrypt,
//               BYTE *pKey);   

typedef BOOL (* API_ReadHideSector)(HANDLE handle,  
               DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...
               BYTE *pData,           // Receive Data buf
               DWORD SectorNumber,    // Sectors(512 bytes per sector) to read
               BOOL bDecrypt,
               BYTE *pKey);   


/*********************************************************************************************
函数声明：	BOOL WriteHideSector(HANDLE handle, DWORD Sectoroffset, BYTE *pData, 
								 DWORD SectorNumber, BOOL bEncrypt = FALSE, BYTE *pKey)
功能描述：	该函数将用户缓冲区中指定个数扇区的数据写入到隐藏区指定偏移处。由于可以
			在隐藏区内部任意指定偏移开始读写，意味着可以将隐藏区划分为不同用途的子
			区块，存放不同用户的数据。
输入参数：	handle-设备句柄。
			SectorOffsetInBuf-指定从隐藏区内第几个扇区开始写，每扇区为512字节，可以为0,1…    
			pData-用于写隐藏区的数据
			SectorNumber-指定需要写入的扇区个数，每扇区为512字节。
			bEncrypt-是否需要对写入的数据进行加密。
			pKey-如果bEncrypt为TRUE，这里需要指定加密秘钥，16字节（128位）。
输出参数：	无
返 回 值：	BOOL。
*********************************************************************************************/
//XDISK_API
//BOOL 
//WriteHideSector(HANDLE handle, 
//			    DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...,
//				BYTE *pData,           // Data buf to write
//				DWORD SectorNumber,    // Sectors(512 bytes per sector) to write
//				BOOL bEncrypt, 
//				BYTE *pKey);    

typedef
BOOL 
(* API_WriteHideSector)(HANDLE handle, 
			    DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...,
				BYTE *pData,           // Data buf to write
				DWORD SectorNumber,    // Sectors(512 bytes per sector) to write
				BOOL bEncrypt, 
				BYTE *pKey);  

/*********************************************************************************************
函数声明：	DWORD GetError()
功能描述：	此函数用于获得错误代码。  
输入参数：	无。
输出参数：	无。
返 回 值：	错误代码。
*********************************************************************************************/
//XDISK_API
//DWORD 
//GetError();

typedef DWORD (* API_GetError)();

////////////////////////////////////////// End of File ///////////////////////////////////////
