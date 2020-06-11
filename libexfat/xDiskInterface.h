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
1.0.0.0    20160901       ������̬��	
1.0.0.1    20160915       V1.0.0.1�׸��汾���У�
1.0.0.2	   20160925		  ��Ȩ����֤��ʽ�޸ģ�
1.0.0.3    20160928       �豸ID��ȡ��ʽ�޸ģ�
2.0.0.1    20200605       ���⻯ר���豸�ӿ�ʵ�֣�
--*/
//////////////////////////////////////////////////////////////////////////////////////////////
//�����㷨����
#define XDISK_ENCRYPT_TYPE_NONE		0x00
#define XDISK_ENCRYPT_TYPE_AES		0x01
#define XDISK_ENCRYPT_TYPE_SM4		0x02
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
����������	HANDLE OpenXDisk(char* AuthorizeKey, char DriverLetter)
����������	��ָ���̷����豸����������Ȩ�롣
			����豸���Ͳ��ԣ�������Ȩ�벻��ȷ����������������ʧ�ܡ�
			������óɹ��������Ӧ�豸�������豸�����
���������	AuthorizeKey ���12���ֽڵĴ����ս�����ַ�������ʼֵ�ڳ�ʼ��ʱ�趨��
			DriverLetter ָ�����̷���
���������	�ޡ�
�� �� ֵ��	HANDLE �豸���
**********************************************************************************************/
//XDISK_API
//HANDLE 
//OpenXDisk(char* AuthorizeKey, char DriverLetter);   

typedef HANDLE (* API_OpenXDisk)(char* AuthorizeKey, char DriverLetter);


/*********************************************************************************************
����������	BOOL CloseXDisk(HANDLE handle)
����������	�ر���Open�ຯ����OpenXDisk���򿪵��豸
���������	handle �豸���
���������	�ޡ�
�� �� ֵ��	BOOL��
*********************************************************************************************/
//XDISK_API
//BOOL 
//CloseXDisk(HANDLE handle);

typedef BOOL (* API_CloseXDisk)(HANDLE handle);
/*********************************************************************************************
����������	BOOL GetPartions(HANDLE hDevice, DWORD* pTotalSector, DWORD* pSector0, DWORD* pSector1, DWORD* pSector2)
����������	�ú�����ȡ��ȫ�豸��������������������
���������	handle-�豸�����
���������	pTotalSector-���ڷ����豸��������512�ֽ�������������ָ�룻
		    pSector0-���ڷ��ط���0����������������512�ֽ�������������ָ�룻
			pSector1-���ڷ��ط���1����������������512�ֽ�������������ָ�룻
			pSector2-���ڷ��ط���2����ͨ����������512�ֽ�������������ָ�룻
�� �� ֵ��	BOOL��
*********************************************************************************************/
//XDISK_API
//BOOL 
//GetPartions(HANDLE hDevice, 
//			DWORD* pTotalSector, 
//			DWORD* pSector0, 
//			DWORD* pSector1, 
//			DWORD* pSector2); 

typedef BOOL (*API_GetPartions)(HANDLE hDevice,
							DWORD* pTotalSector, 
							DWORD* pSector0, 
							DWORD* pSector1, 
							DWORD* pSector2);
/*********************************************************************************************
����������	BOOL ReadHideSector(HANDLE handle, DWORD Sectoroffset, BYTE *pData, 
								DWORD SectorNumber, BOOL bDecrypt = FALSE, BYTE *pKey, BYTE DecryptType)
����������	�ú�����������ָ��ƫ�ƴ���ȡ�û�ָ���������������ݵ��������С����ڿ���
			���������ڲ�����ָ��ƫ�ƿ�ʼ��д����ζ�ſ��Խ�����������Ϊ��ͬ��;����
			���飬��Ų�ͬ�û������ݡ�
���������	handle-�豸�����
			Sectoroffset-ָ�����������ڵڼ���������ʼ����ÿ����Ϊ512�ֽڣ�����Ϊ0,1��          
			SectorNumber-ָ����Ҫ��ȡ������������ÿ����Ϊ512�ֽڡ�
			bDecrypt-�Ƿ���Ҫ�Զ�ȡ�����ݽ��н��ܡ�
			pKey-���bDecryptΪTRUE��������Ҫָ��������Կ��16�ֽڣ�128λ����
			DecryptType-���bDecryptΪTRUE��������Ҫָ�������㷨���ͣ�����㷨���ͺ궨�塣
���������	pData-���ڴ�Ŵ���������ȡ�����ݵ��û���������
�� �� ֵ��	BOOL��
*********************************************************************************************/
//XDISK_API
//BOOL 
//ReadHideSector(HANDLE handle,  
//               DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...
//               BYTE *pData,           // Receive Data buf
//               DWORD SectorNumber,    // Sectors(512 bytes per sector) to read
//               BOOL bDecrypt,
//               BYTE *pKey��
//				 BYTE DecryptType);   

typedef BOOL (* API_ReadHideSector)(HANDLE handle,  
               DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...
               BYTE *pData,           // Receive Data buf
               DWORD SectorNumber,    // Sectors(512 bytes per sector) to read
               BOOL bDecrypt,
               BYTE *pKey, 
			   BYTE DecryptType);   


/*********************************************************************************************
����������	BOOL WriteHideSector(HANDLE handle, DWORD Sectoroffset, BYTE *pData, 
								 DWORD SectorNumber, BOOL bEncrypt = FALSE, BYTE *pKey)
����������	�ú������û���������ָ����������������д�뵽������ָ��ƫ�ƴ������ڿ���
			���������ڲ�����ָ��ƫ�ƿ�ʼ��д����ζ�ſ��Խ�����������Ϊ��ͬ��;����
			���飬��Ų�ͬ�û������ݡ�
���������	handle-�豸�����
			SectorOffsetInBuf-ָ�����������ڵڼ���������ʼд��ÿ����Ϊ512�ֽڣ�����Ϊ0,1��    
			pData-����д������������
			SectorNumber-ָ����Ҫд�������������ÿ����Ϊ512�ֽڡ�
			bEncrypt-�Ƿ���Ҫ��д������ݽ��м��ܡ�
			pKey-���bEncryptΪTRUE��������Ҫָ��������Կ��16�ֽڣ�128λ����
���������	��
�� �� ֵ��	BOOL��
*********************************************************************************************/
//XDISK_API
//BOOL 
//WriteHideSector(HANDLE handle, 
//			    DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...,
//				BYTE *pData,           // Data buf to write
//				DWORD SectorNumber,    // Sectors(512 bytes per sector) to write
//				BOOL bEncrypt, 
//				BYTE *pKey,
//				BYTE EncryptType);    

typedef
BOOL 
(* API_WriteHideSector)(HANDLE handle, 
			    DWORD Sectoroffset,    // Offset sectors in private buf, must be 0,1,2,...,
				BYTE *pData,           // Data buf to write
				DWORD SectorNumber,    // Sectors(512 bytes per sector) to write
				BOOL bEncrypt, 
				BYTE *pKey, 
				BYTE EncryptType);     

/*********************************************************************************************
����������	DWORD GetError()
����������	�˺������ڻ�ô�����롣  
���������	�ޡ�
���������	�ޡ�
�� �� ֵ��	������롣
*********************************************************************************************/
//XDISK_API
//DWORD 
//GetError();

typedef DWORD (* API_GetError)();

////////////////////////////////////////// End of File ///////////////////////////////////////
