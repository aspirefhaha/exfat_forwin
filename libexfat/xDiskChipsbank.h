#pragma once

#define ERR_NO_ERROR                    0
#define ERR_DISK_NOT_FOUND              1
#define ERR_CFG_TABLE_NOT_FOUND         2
#define ERR_BAD_PASSWORD                3
#define ERR_PRIVATE_ZONE_NOT_FOUND      4
#define ERR_DISK_NOT_OPENNED            5
#define ERR_HIDE_ZONE_NOT_FOUND         6
#define ERR_PASSWORD_NOT_FOUND          7
#define ERR_API_NOT_SUPPORTED           8
#define ERR_BAD_AUTHORIZE_KEY           9
#define ERR_BAD_LUN                     10
#define ERR_LUN_LOCKED                  11
#define ERR_FORMAT_ERROR                12
#define ERR_BAD_FILENAME                13
#define ERR_FILE_NOT_FOUND              14
#define ERR_CANNOT_GET_SIZE             15
#define ERR_SIZE_ERROR                  16
#define ERR_ISO_WRITE_ERROR             17
#define ERR_UPDATE_CFG_TABLE_ERROR      18
#define ERR_DISK_READ_ERROR             19
#define ERR_DISK_WRITE_ERROR            20
#define ERR_MULTI_THREAD_NOT_SUPPORTED  21
#define ERR_CANNOT_REOPENED             22
#define ERR_BAD_PARAMETER               23
#define ERR_READ_PROTECTED              24
#define ERR_WRITE_PROTECTED             25
#define ERR_UNMATCH_VERSION				26
#define ERR_UNSUPPORT_SCAN_MODE			27
#define ERR_UNSUPPORT_43NMFLASH			28
//! User Mode�£���֧�ֵ�ǰӦ��
#define ERR_IN_USER_MODE                40

#define BASE_FILESYSTEM_ERR             1000
#define FSE_OPERATION_SUCCESS               ERR_NO_ERROR               //0�������ɹ���
#define FSE_FILE_NOT_EXIST                  (BASE_FILESYSTEM_ERR + 1)  //�ļ������ڡ�
#define FSE_FILE_HAD_EXIST                  (BASE_FILESYSTEM_ERR + 2)  //�ļ��Ѵ��ڡ�
#define FSE_FILE_OPEN_FAILED                (BASE_FILESYSTEM_ERR + 3)  //�ļ���ʧ�ܡ�
#define FSE_SECTOR_READ_FAILED              (BASE_FILESYSTEM_ERR + 4)  //������ʧ�ܡ�
#define FSE_SECTOR_WRITE_FAILED             (BASE_FILESYSTEM_ERR + 5)  //д����ʧ�ܡ�
#define FSE_DISK_NOT_ENOUGH_SPACE           (BASE_FILESYSTEM_ERR + 6)  //���̿ռ䲻�㡣
#define FSE_NOTFIND_DIRTABLEITEM            (BASE_FILESYSTEM_ERR + 7)  //δ�ҵ�Ŀ¼�
#define FSE_DELETE_FAT_FAILED               (BASE_FILESYSTEM_ERR + 8)  //ɾ���ļ������ʧ�ܡ�
#define FSE_NEWBUFFER_FAILED                (BASE_FILESYSTEM_ERR + 9)  //�����ڴ�ʧ�ܡ�
#define FSE_INVALID_FILENAME                (BASE_FILESYSTEM_ERR + 10) //��Ч���ļ�����
#define FSE_CREATE_DIR_FAILED               (BASE_FILESYSTEM_ERR + 11) //����·��ʧ�ܡ�
#define FSE_OPEN_FAT_FAILED                 (BASE_FILESYSTEM_ERR + 12) //��FATʧ�ܡ�
#define FSE_OPEN_FDT_FAILED                 (BASE_FILESYSTEM_ERR + 13) //��FDTʧ�ܡ�
#define FSE_INVALID_FILE_HANDLE             (BASE_FILESYSTEM_ERR + 14) //��Ч���ļ������
#define FSE_INVALID_FIND_HANDLE             (BASE_FILESYSTEM_ERR + 15) //��Ч���ļ����Ҿ����
#define FSE_READ_DATA_CLUSTER_FAILED        (BASE_FILESYSTEM_ERR + 16) //�����ݴ�ʧ�ܡ�
#define FSE_WRITE_DATA_CLUSTER_FAILED       (BASE_FILESYSTEM_ERR + 17) //д���ݴ�ʧ�ܡ�
#define FSE_REMOVEDIR_HASFILE               (BASE_FILESYSTEM_ERR + 18) //���ļ����������ļ���Ŀ¼��

typedef BOOL (WINAPI *FPZCopyFileProgress)(DWORD dwFileSize,DWORD dwHasCopyed);

#ifdef __cplusplus
extern "C" 
{
#endif

/******************************************************************************
����������HANDLE OpenUsbDisk(char* AuthorizeKey, const char* pszVID, const char* pszPID)
�����������������̽���ö�٣����ж��߼��̵����������Ƿ�ƥ�䣬���������������Ȩ�롣
          ����������Ͳ��ԣ�������Ȩ�벻��ȷ����������������ʧ�ܡ�������óɹ���
          �����Ӧ�����̣����������̾����
���������AuthorizeKey ���16���ֽڵĴ����ս�����ַ�������ʼֵ������ʱ�趨��
          pszVID �豸VID�š�
          pszPID �豸PID�š�
����������ޡ�
�� �� ֵ��HANDLE �豸���
******************************************************************************/
typedef HANDLE  (WINAPI   *API_OpenUsbDisk)(char* AuthorizeKey, 
    const char* pszVID ,////= NULL 
    const char* pszPID ,////= NULL 
    BOOL bCanMultiProcess //= TRUE
	);//�˲������ã�ֻ��Ϊ�˼��ݾɵİ汾   


/******************************************************************************
����������HANDLE OpenUsbDiskEx(char* AuthorizeKey, char DriverLetter, const char* pszVID, const char* pszPID)
������������ָ���̷����������豸�����ж��߼��̵����������Ƿ�ƥ�䣬���������������Ȩ�롣
          ����������Ͳ��ԣ�������Ȩ�벻��ȷ����������������ʧ�ܡ�������óɹ���
          �����Ӧ�����̣����������̾����
���������AuthorizeKey ���16���ֽڵĴ����ս�����ַ�������ʼֵ������ʱ�趨��
          DriverLetter ָ�����̷�
          pszVID �豸VID�š�
          pszPID �豸PID�š�
����������ޡ�
�� ��     HANDLE �豸���
******************************************************************************/
typedef HANDLE (WINAPI *API_OpenUsbDiskEx)(char* AuthorizeKey, char DriverLetter, 
                         const char* pszVID ,////= NULL 
                         const char* pszPID ,////= NULL 
                         BOOL bCanMultiProcess //= TRUE
						 );//�˲������ã�ֻ��Ϊ�˼��ݾɵİ汾   


/******************************************************************************
����������BOOL CloseUsbDisk(HANDLE handle)
�����������ر���Open�ຯ����OpenUsbDisk/OpenUsbDiskEx���򿪵�������
���������handle �豸���
����������ޡ�
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_CloseUsbDisk)(HANDLE handle);


/******************************************************************************
����������BOOL ChangeAuthorizeKey(HANDLE handle, char *OldAuthorizeKey, char *NewAuthorizeKey)
�����������ú������ڸ�����Ȩ�롣
���������handle-�豸�����
          OldAuthorizeKey-ԭ�е�16�ֽ���Ȩ�룻
          NewAuthorizeKey-���ĺ��16�ֽ���Ȩ�롣
����������ޡ�
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_ChangeAuthorizeKey)(HANDLE handle, char *OldAuthorizeKey, char *NewAuthorizeKey);


/******************************************************************************
����������BOOL CheckAuthorizeKey(HANDLE handle, char *AuthorizeKey)
�����������ú������ڼ����Ȩ���Ƿ���ȷ��
���������handle-�豸�����
          AuthorizeKey-���ڱȶԵ�16�ֽ���Ȩ��
����������ޡ�
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_CheckAuthorizeKey)(HANDLE handle, char *AuthorizeKey);


/******************************************************************************
����������BOOL ReadPrivateUserSector(HANDLE handle, DWORD SectoroffsetInBuf, BYTE *p_user_data,��BYTE SectorNumber);
�����������ú�����˽����ָ��ƫ�ƴ���ȡ�û�ָ���������������ݵ��������С����ڿ���
          ��˽�����ڲ�����ָ��ƫ�ƿ�ʼ��д����ζ�ſ��Խ�˽��������Ϊ��ͬ��;����
          ���飬��Ų�ͬ�û������ݣ�����û���д������ǰ�������ܴ�������Ա�֤
          ���ݵİ�ȫ�ԡ����Lun2�����Ѿ������˶���������˺����޷����ж�ȡ��
���������handle-�豸�����
          SectorOffsetInBuf-ָ����˽�����ڵڼ���������ʼ����ÿ����Ϊ512�ֽڣ�����Ϊ0,1��          
          SectorNumber-ָ����Ҫ��ȡ������������ÿ����Ϊ512�ֽڡ����ܴ���255��
���������p_user_data-���ڴ�Ŵ�˽������ȡ�����ݵ��û���������
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_ReadPrivateUserSector)(HANDLE handle,  
                                  DWORD SectoroffsetInBuf,    // Offset sectors in private buf, must be 0,1,2,...
                                  BYTE *p_user_data,          // Receive Data buf
                                  DWORD SectorNumber,          // Sectors(512 bytes per sector) to read
                                  BYTE Lun);                  //�˲������ã�ֻ��Ϊ�˼��ݾɵİ汾    


/******************************************************************************
����������BOOL WritePrivateUserSector(HANDLE handle, DWORD SectoroffsetInBuf, BYTE *p_user_data, BYTE SectorNumber)
�����������ú������û���������ָ����������������д�뵽˽����ָ��ƫ�ƴ������ڿ���
          ��˽�����ڲ�����ָ��ƫ�ƿ�ʼ��д����ζ�ſ��Խ�˽��������Ϊ��ͬ��;����
          ���飬��Ų�ͬ�û������ݣ�����û���д������ǰ�������ܴ�������Ա�֤
          ���ݵİ�ȫ�ԡ����Lun2�����Ѿ������˶�д��������˺����޷�����д�롣
          �����˽������ʾΪ�̷�������ͨ�̵ķ�ʽ��Windows����ϵͳ��ʹ�ã���ʱ����
          �����øýӿڽ����ܻ��ƻ�˽�������ļ�������ȣ�����Windows����ϵͳ�޷�ʶ��˽�������̷���
���������handle-�豸�����
          SectorOffsetInBuf-ָ����˽�����ڵڼ���������ʼд��ÿ����Ϊ512�ֽڣ�����Ϊ0,1��    
          p_user_data-����д˽����������
          SectorNumber-ָ����Ҫ��ȡ������������ÿ����Ϊ512�ֽڡ����ܴ���255��
�����������
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_WritePrivateUserSector)(HANDLE handle, 
                                   DWORD SectoroffsetInBuf,    // Offset sectors in private buf, must be 0,1,2,...,
                                   BYTE *p_user_data,          // Data buf to write
                                   DWORD SectorNumber,          // Sectors(512 bytes per sector) to write
                                   BYTE Lun,                   //�˲������ã�ֻ��Ϊ�˼��ݾɵİ汾   
                                   BOOL bVerify //= FALSE        //�˲������ã�ֻ��Ϊ�˼��ݾɵİ汾   
                                    );     


/******************************************************************************
����������__int64 GetHideZoneSize(HANDLE handle)
�����������ú����������ֽ�Ϊ��λ����������С��
���������handle-�豸�����
����������ޡ�
�� �� ֵ����������С���������ֵΪ0����������ʧ�ܡ�
******************************************************************************/
typedef __int64 (WINAPI *API_GetHideZoneSize)(HANDLE handle); //if return 0, no hide Zone

/******************************************************************************
����������BYTE GetHideZoneLun(HANDLE handle, BYTE &Lun)
�����������ú����������ֽ�Ϊ��λ����������С��
���������handle-�豸�����
���������Lun-���ڴ�ŷ��ص��߼���Ԫ�š�
�� �� ֵ���߼���Ԫ�š�
******************************************************************************/
typedef BYTE (WINAPI *API_GetHideZoneLun)(HANDLE handle, BYTE *Lun);

/******************************************************************************
����������char GetHideZoneDriveLetter(HANDLE handle, char &DriveLetter)
�����������ú�������ȷ����������������ӳ����߼��������̷���
���������handle-�豸�����
���������DriveLetter-���ڴ�ŷ��ص��̷���
�� �� ֵ���̷���ע����һ���ַ�������"��"��
******************************************************************************/
typedef char (WINAPI *API_GetHideZoneDriveLetter)(HANDLE handle, char *DriveLetter);


/******************************************************************************
����������BOOL VerifyUserPassword(HANDLE handle, char *UserPassword)
������������֤�û����롣
���������HANDLE handle-�豸�����
          char *UserPassword-�16���ַ�����'\0'����
����������ޡ�
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_VerifyUserPassword)(HANDLE handle, char *UserPassword);


/******************************************************************************
����������ChangeUserPassword(HANDLE handle, char *OldUserPassword, char *NewUserPassword)
�����������˺������ڸ����û����롣ע�⣬�����̲�֧�ָ����û����롣
���������handle-�豸�����
          OldUserPassword-�޸�ǰ���û����룬�����ս�����ַ������16���ַ�����'\0'����
          NewUserPassword-�µ��û����룬�����ս�����ַ������16���ַ�����'\0'����
����������ޡ�
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_ChangeUserPassword)(HANDLE handle, char *OldUserPassword, char *NewUserPassword);


/******************************************************************************
����������BOOL MountHideZoneAsDisk(HANDLE handle, BYTE Lun)
�����������˺�����������ӳ��Ϊһ���߼���������ӳ��ķ��������ǿ��ƶ��������ͣ������Ǳ���������
���������handle-�豸�����
          Lun-���ط������߼���Ԫ�š�
����������ޡ�
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_MountHideZoneAsDisk)(HANDLE handle, BYTE Lun);


/******************************************************************************
����������BOOL UnMountHideZoneAsDisk(HANDLE handle, BYTE Lun)
�����������˺�����MountHideZoneAsDisk�������ʹ�á�����ж���Ѿ�ӳ�䵽�߼���������������
���������handle-�豸�����
          Lun-���ط������߼���Ԫ�š�
����������ޡ�
�� �� ֵ��BOOL��
******************************************************************************/
typedef BOOL (WINAPI *API_UnMountHideZoneAsDisk)(HANDLE handle, BYTE Lun);


/******************************************************************************
����������BOOL IsInHideZone(HANDLE handle, BYTE Lun)
�����������˺��������ж��߼���Ԫ��ΪLun�ķ����Ƿ��Ѿ���ӳ���ˡ�
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
����������ޡ�
�� �� ֵ��TRUE-�����������У�FALSE-���ڷ��������С�
******************************************************************************/
typedef BOOL (WINAPI *API_IsInHideZone)(HANDLE handle, BYTE Lun);


/******************************************************************************
����������BOOL IsCdrom(HANDLE handle, char DriveLetter)
�����������˺��������ж��Ƿ���CD-ROM������
���������handle-�豸�����
          DriveLetter-�̷���
����������ޡ�
�� �� ֵ��TRUE-ָ��������CD-ROM������FALSE-- ָ�����̲���CD-ROM����
******************************************************************************/
typedef BOOL (WINAPI *API_IsCdrom)(HANDLE handle, char DriveLetter);


/******************************************************************************
����������BOOL SetDiskProtect(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite)
�������������ô˺������Ըı��߼������Ķ�д����Ȩ�ޡ�
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
          bCanRead-�ɶ���־��
          bCanWrite-��д��־��
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_SetDiskProtect)(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite);


/******************************************************************************
����������BOOL SetDiskProtectEx(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite)
�������������ô˺����������øı��߼������Ķ�д����Ȩ�ޡ�
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
          bCanRead-�ɶ���־��
          bCanWrite-��д��־��
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_SetDiskProtectEx)(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite);

/******************************************************************************
����������void LockUsbDisk(HANDLE handle, BYTE Lun, BYTE &LockStatus)
�������������ô˺������Զ��߼������������������ܽ��ж���д������
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
          LockStatus-���ڱ������ǰ�Ķ�д״̬��
����������ޡ�
�� �� ֵ��void
******************************************************************************/
typedef void (WINAPI *API_LockUsbDisk)(HANDLE handle, BYTE Lun, BYTE *LockStatus);


/******************************************************************************
����������void UnLockUsbDisk(HANDLE handle, BYTE Lun, BYTE &LockStatus)
������������LockUsbDisk������ԣ������������ķ����������󽫻ָ�����ǰ�Ķ�д����Ȩ�ޡ�
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
          LockStatus-���ڴ����Ҫ�ָ��ļ���ǰ�Ķ�д״̬����LockUsbDisk�����õ���
����������ޡ�
�� �� ֵ��void
******************************************************************************/
typedef void (WINAPI *API_UnLockUsbDisk)(HANDLE handle, BYTE Lun, BYTE *LockStatus);


/******************************************************************************
����������__int64 GetUsbDiskSize(HANDLE handle, BYTE Lun)
�����������˺������ڻ�ȡ���ֽ�Ϊ��λ�ģ��߼���Ԫ��ΪLun���߼�������������
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
����������ޡ�
�� �� ֵ���߼�������������
******************************************************************************/
typedef __int64 (WINAPI *API_GetUsbDiskSize)(HANDLE handle, BYTE Lun);


/******************************************************************************
����������BOOL SetUsbDiskSize(HANDLE handle, DWORD Lun0Size,DWORD Lun1Size,DWORD hZoneSize,DWORD pZoneSize)
�����������˺������ڶ������̽������·������߼�����0���߼�����1����������˽��������֮��Ӧ�õ������������̵�
          �߼����������·��������������͡���������/д������������ʱָ��������һ�£�����Passwordֵ�ᱻ��Ϊ""��
          ��CD-ROM���⣬��������������DiskFormat()�������и�ʽ�����������ʹ��; CD-ROM����Ҫʹ��
          UpdateISOFileOfCDROMDriver()��������д���µ�ISO�ļ���������㲻����ͨ�̽��з�������ô��������Lun0SizeΪ0��
          ������Ҫ��Lun1Size���ó�0��
          ע��
          1��ÿ�������������ٱ�֤1M���������������Lun0Size��Lun1Size��hZoneSize��pZoneSize��ֵ��С��2000��
             �������DiskFormat�ӿ�ʱ���ܻ�ʧ�ܡ�
          2��������˺������ٵ���DiskFormat�����Է������и�ʽ������Ҫ�������̲��ܼ���ʹ�á�
���������handle-�豸�����
          Lun0Size-�߼�����0����������SectorΪ��λ��ÿ��SectorΪ512�ֽڡ�
          Lun1Size-�߼�����1����������SectorΪ��λ��ÿ��SectorΪ512�ֽڡ�
          hZoneSize--�������������� ��SectorΪ��λ��ÿ��SectorΪ512�ֽڡ�
          pZoneSize-˽������������  ��SectorΪ��λ��ÿ��SectorΪ512�ֽڡ�
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_SetUsbDiskSize)(HANDLE handle, DWORD Lun0Size, DWORD Lun1Size, DWORD hZoneSize, DWORD pZoneSize);


/******************************************************************************
����������BOOL DiskFormat(HANDLE handle, BYTE Lun)
������������ʽ�������̵��߼����������ο����ӿ��ṩ�߼������ĵ��ڹ��ܣ���ͨ����
          �������߼��������и�ʽ����ע���ʽ�����������е����ݽ���ʧ��
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_DiskFormat)(HANDLE handle, BYTE Lun);//Format Current Logic Partition which has private sectors


/******************************************************************************
����������int UpdateISOFileOfCDROMDriver(HANDLE handle, BYTE Lun, char* FileName)
�����������ú���������������AUTORUN����ӳ��ISO���ļ���ISO�ļ���������UltraISO��
          ������������AUTORUN��������ʱ��CD-ROM�̵Ĵ�С����ISO�ļ��Ĵ�С�������ģ�
          ���Ҫ���ǽ�������������ISO�ļ����ͱ���������ʱָ��CD-ROM�̵����ֵ��
          ע�⣺���������ʱѡ��������ΪAutoRun�̣���Lun��1��
���������handle-�豸�����
          Lun-�������߼���Ԫ�š�
          FileName-Ҫд��CD-ROM�Ĺ���ӳ���ļ�����
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef int (WINAPI *API_UpdateISOFileOfCDROMDriver)(HANDLE handle, BYTE Lun, char* FileName);


/******************************************************************************
����������BYTE GetUDiskSerialNumber(HANDLE handle, char *SerialNumber)
����������������������ʱ������Ωһ�����кţ����кŵ���С������12�ֽڣ����к����
          ������32�ֽڣ��������кű�����"ABCDEF0123456789"�е��ַ������������ʱ
          ѡ���Զ��������кţ������кŵ���Ч����Ϊ16���ֽڣ���˱���ע���ṩ�Ľ�
          �ջ�����Ӧ���㹻��
���������handle-�豸�����          
���������SerialNumber-���ڴ�ŷ��ص����кš�
�� �� ֵ�����кų���
******************************************************************************/
typedef BYTE (WINAPI *API_GetUDiskSerialNumber)(HANDLE handle, char *SerialNumber);


/******************************************************************************
����������BOOL SetUDiskSerialNumber(HANDLE handle, char *SerialNumber)
����������������������ʱ������Ωһ�����кţ����кŵ���С������12�ֽڣ���󳤶�
          ��32�ֽڣ��������кű�����"ABCDEF0123456789"�е��ַ����ú������ڸ��Ĵ�
          ���кţ���������������кų�������12���ֽڣ��ú������Զ���ǰ��'0'��ʹ
          �µ����кų��ȴﵽ12���ֽڣ���������������кų��ȳ���32���ֽڣ��ú���
          ���Զ���ȡǰ32���ַ���Ϊ�µ����кš�
���������handle-�豸�����
          SerialNumber-���ڴ���µ����кš������Ǵ��п��ս�����ַ�����          
�����������
�� �� ֵ�����кų���
******************************************************************************/
typedef BOOL (WINAPI *API_SetUDiskSerialNumber)(HANDLE handle, char *SerialNumber);


/******************************************************************************
����������WORD GetPID(HANDLE handle, WORD &Pid)
�����������ú������ز�Ʒ�ı�ʶ�룬����0x6025��
���������handle-�豸�����
���������Pid-���ڴ��ȡ�õĲ�ƷID
�� �� ֵ����ƷID
******************************************************************************/
typedef WORD (WINAPI *API_GetPID)(HANDLE handle, WORD *Pid);


/******************************************************************************
����������WORD SetPID(HANDLE handle, WORD &Pid)
�����������ú������ز�Ʒ�ı�ʶ�룬����0x6025��
���������handle-�豸�����
          Pid-�µĲ�Ʒ��ʶ�롣
�����������
�� �� ֵ����ƷID
******************************************************************************/
typedef BOOL (WINAPI *API_SetPID)(HANDLE handle, WORD PID);


/******************************************************************************
����������WORD GetVID(HANDLE handle, WORD &Vid)
�����������ú������ز�Ʒ�ı�ʶ�룬����0x6025��
���������handle-�豸�����
������������ڴ��ȡ�õĳ���ID��
�� �� ֵ������ID
******************************************************************************/
typedef WORD (WINAPI *API_GetVID)(HANDLE handle, WORD *Vid);


/******************************************************************************
����������WORD SetVID(HANDLE handle, WORD &Pid)
�����������ú����������ó��̵ı�ʶ�룬����0x0205��
���������handle-�豸�����
          VID-�µĳ��̱�ʶ�롣
�����������
�� �� ֵ������ID
******************************************************************************/
typedef BOOL (WINAPI *API_SetVID)(HANDLE handle, WORD VID);


/******************************************************************************
����������char* GetProductInfo(HANDLE handle, char* ProductInfo)
�����������ú�������16���ֽڵĲ�Ʒ���ƴ�������"Flash Disk      "
���������handle-�豸�����
���������ProductInfo-��Ʒ��Ϣ�����ջ�������
�� �� ֵ����Ʒ���ƴ�
******************************************************************************/
typedef char* (WINAPI *API_GetProductInfo)(HANDLE handle, char* ProductInfo);


/******************************************************************************
����������char* SetProductInfo(HANDLE handle, char* ProductInfo)
�����������ú����������ò�Ʒ���ƴ����µĲ�Ʒ���ƴ���󳤶Ȳ��ܳ���16���ַ���
���������handle-�豸�����
          ProductInfo-�µĲ�Ʒ��Ϣ����
�����������
�� �� ֵ����Ʒ���ƴ�
******************************************************************************/
typedef BOOL (WINAPI *API_SetProductInfo)(HANDLE handle, char *ProductInfo);


/******************************************************************************
����������char* GetVendorInfo(HANDLE handle, char* VendorInfo)
�����������ú�������8���ֽڵĳ������ƴ�������"Chipsbnk"��
���������handle-�豸�����
���������VendorInfo-������Ϣ�����ջ�������
�� �� ֵ��������Ϣ��
******************************************************************************/
typedef char* (WINAPI *API_GetVendorInfo)(HANDLE handle, char* VendorInfo);


/******************************************************************************
����������char* SetVendorInfo(HANDLE handle, char* VendorInfo)
�����������ú����������ò�Ʒ���ƴ����µĲ�Ʒ���ƴ���󳤶Ȳ��ܳ���16���ַ���
���������handle-�豸�����
          VendorInfo-������Ϣ��
�����������
�� �� ֵ��������Ϣ��
******************************************************************************/
typedef BOOL (WINAPI *API_SetVendorInfo)(HANDLE handle, char *VendorInfo);


/******************************************************************************
����������BOOL IsHighSpeed(HANDLE handle)
�����������˺��������ж�USBʵ�ʹ���ģʽ��1.1����2.0��
���������HANDLE handle - �豸���
����������ޡ�
�� �� ֵ��TRUE ΪUSB2.0��FALSE ΪUSB1.1��
******************************************************************************/
typedef BOOL (WINAPI *API_IsHighSpeed)(HANDLE handle); //



/******************************************************************************
����������WORD GetVendorSerial(HANDLE handle)
�����������˺������ڷ���������ʱ����ָ�������кţ�һ�����ڼ����豸�Ƿ��ɳ���������
���������HANDLE handle - �豸���
����������ޡ�
�� �� ֵ�������Զ�������кš�
******************************************************************************/
typedef WORD (WINAPI *API_GetVendorSerial)(HANDLE handle);


/******************************************************************************
����������char* GetUDiskLetter(HANDLE handle, DWORD dwMilliseconds)
�����������˺������ڷ��ؾ����������������OpenUsbDisk�򿪵������̵��̷����������
          �����̣������ض���̷����磺"JK"������������������ʱ��ʱ����������̵ķ�
          ����ʧ�������³��ֵ����������ʱ���ô˺������ܻ��ò������е��̷���
          ͨ��dwMilliseconds��������������Ҫ�ȴ���ʱ�䡣
���������HANDLE handle - �豸���
          dwMilliseconds-��ʱ������
����������ޡ�
�� �� ֵ�������̵��̷�������������򷵻�NULL��
******************************************************************************/
typedef char* (WINAPI *API_GetUDiskLetter)(HANDLE handle, DWORD dwMilliseconds);


/******************************************************************************
����������int GetError(HANDLE handle //= NULL)
�����������˺������ڻ�ô�����롣�������handle�����Чʱ�����ظ��豸API����ʱ
          ��������������룻������NULLʱ����������API�����ʱ��������������롣  
���������HANDLE handle - �豸���
����������ޡ�
�� �� ֵ��������롣
******************************************************************************/
typedef int (WINAPI *API_GetError)(HANDLE handle); //= NULL);


/******************************************************************************
����������__int64 GetPrivateZoneSize(HANDLE handle)
�����������˺������ڻ�ȡ˽�����Ĵ�С�����ֽ�Ϊ��λ��
���������HANDLE handle - �豸���
����������ޡ�
�� �� ֵ������˽�����Ĵ�С��
******************************************************************************/
typedef __int64 (WINAPI *API_GetPrivateZoneSize)(HANDLE handle); //if return 0, no private Zone


/******************************************************************************
����������BYTE GetPrivateZoneLun(HANDLE handle, BYTE &Lun)
�����������˺������ڻ�ȡ˽�������߼���Ԫ�š�
���������HANDLE handle - �豸���
����������ޡ�
�� �� ֵ������˽�������߼���Ԫ�š�
******************************************************************************/
typedef BYTE (WINAPI *API_GetPrivateZoneLun)(HANDLE handle, BYTE *Lun);


/******************************************************************************
����������BOOL IsReadProtect(HANDLE handle, BYTE Lun)
�����������˺��������ж�ָ�������Ƿ�Ϊ������״̬��
���������HANDLE handle - �豸���
          Lun- �߼���Ԫ�ţ�����ָ�����ţ���
����������ޡ�
�� �� ֵ�����ָ������Ϊ������������TRUE�����򷵻�FALSE��
******************************************************************************/
typedef BOOL (WINAPI *API_IsReadProtect)(HANDLE handle, BYTE Lun);


/******************************************************************************
����������BOOL IsWriteProtect(HANDLE handle, BYTE Lun)
�����������˺��������ж�ָ�������Ƿ�Ϊд����״̬��
���������HANDLE handle - �豸���
          Lun- �߼���Ԫ�ţ�����ָ�����ţ���
����������ޡ�
�� �� ֵ�����ָ������Ϊд����������TRUE�����򷵻�FALSE��
******************************************************************************/
typedef BOOL (WINAPI *API_IsWriteProtect)(HANDLE handle, BYTE Lun);


/******************************************************************************
����������int PzCopyFileTo(HANDLE handle, const char* pszFromFullFileName, const char* pszToFileName, BOOL bFailIfExists)
�����������ýӿ����ڽ�ָ���ļ���pszFromFullFileName)��ָ�����ƣ�pszToFileName��������˽����
���������HANDLE handle - �豸���
          pszFromFullFileName-Դ�ļ�ȫ·������ע�⣺�������ָӲ���ϵ���Ҫ����˽�������ļ������Ա�����ȫ·������
                                                    ������ڵ�ǰ����Ŀ¼�½��в��ң�������Ҳ������򷵻�ʧ�ܡ�
          pszToFileName-Ŀ���ļ�����֧�ֳ��ļ�������Ŀ¼��
          BOOL bFailIfExists-����ļ�����ʱ��δ���TRUE�����ļ�����ʱ������ʧ�ܡ�FALSE�����ļ�����ʱ�������Ѵ��ڵ��ļ��������سɹ���
����������ޡ�
�� �� ֵ���ɹ�����0��ʧ�ܷ��ش����롣�μ�����ֵ�б�
******************************************************************************/
typedef int (WINAPI *API_PzCopyFileTo)(HANDLE handle, const char* pszFromFullFileName, const char* pszToFileName, BOOL bFailIfExists);


/******************************************************************************
����������int PzCopyFileFrom(HANDLE handle, const char* pszFromFileName, const char* pszToFullFileName)
�����������˽ӿ����ڽ�˽������Ŀ¼�µ�ָ���ļ�(pszFromFileName)������ָ��λ�ã�pszToFullFileName����
���������HANDLE handle - �豸���
          pszFromFileName-Դ�ļ�����
          pszToFullFileName-Ŀ���ļ�ȫ·������������Ҫ�������ļ���ŵ�ַ��ȫ·���������û��ָ��ȫ·�����򽫿�������ǰ����Ŀ¼�¡�
����������ޡ�
�� �� ֵ���ɹ�����0��ʧ�ܷ��ش����롣�μ�����ֵ�б�
******************************************************************************/
typedef int (WINAPI *API_PzCopyFileFrom)(HANDLE handle, const char* pszFromFileName, const char* pszToFullFileName);


/******************************************************************************
����������int PzDeleteFile(HANDLE handle, const char* pszFileName)
�����������˽ӿ�����ɾ��˽������Ŀ¼��ָ���ļ�����pszFileName�����ļ���
���������HANDLE handle - �豸���
          pszFileName-Ҫɾ�����ļ�����
����������ޡ�
�� �� ֵ���ɹ�����0��ʧ�ܷ��ط���Ĵ����롣
******************************************************************************/
typedef int (WINAPI *API_PzDeleteFile)(HANDLE handle, const char* pszFileName);


/******************************************************************************
����������HANDLE PzFindFirstFile(HANDLE hUsbHandle, const char* pszFindPath, LPWIN32_FIND_DATA lpFindFileData)
������������˽����ָ��Ŀ¼�²��ҵ�һ���ļ������pszFindPathΪ���ַ�������Ӹ�Ŀ¼�½��в��ҡ�
���������hUsbHandle-�豸�����
          pszFindPath-ָ��Ҫ���ҵ�·����
          lpFindFileData-�����ҵ����ļ���Ϣ��
����������ޡ�
�� �� ֵ���ɹ������ļ����Ҿ�������صľ����PzGetNextFile����ʱʹ�ã�ʧ�ܷ���NULL��
******************************************************************************/
typedef HANDLE (WINAPI *API_PzFindFirstFile)(HANDLE hUsbHandle, const char* pszFindPath, LPWIN32_FIND_DATA lpFindFileData);


/******************************************************************************
����������BOOL PzFindNextFile(HANDLE hUsbHandle, HANDLE hFildFileHandle, LPWIN32_FIND_DATA lpFindFileData)
������������˽����ָ��Ŀ¼�²�����һ���ļ���
���������hUsbHandle-�豸�����
          hFildFileHandle-����PzFindFirstFile���صľ����
          lpFindFileData-���ղ��ҵ����ļ��������Ϣ��
����������ޡ�
�� �� ֵ��BOOL���ɹ��ҵ��ļ�������TRUE�����򷵻�FALSE��
******************************************************************************/
typedef BOOL (WINAPI *API_PzFindNextFile)(HANDLE hUsbHandle, HANDLE hFildFileHandle, LPWIN32_FIND_DATA lpFindFileData);


/******************************************************************************
����������BOOL PzFindClose(HANDLE hUsbHandle, HANDLE hFindFileHandle)
�����������˽ӿ������ͷ�PzFindFirstFile�ӿڷ��ص��ļ����Ҿ����
���������hUsbHandle-�豸�����
          hFindFileHandle-�ļ����Ҿ����
����������ޡ�
�� �� ֵ��BOOL���ɹ�����TRUE��ʧ�ܷ���FALSE��
******************************************************************************/
typedef BOOL (WINAPI *API_PzFindClose)(HANDLE hUsbHandle, HANDLE hFindFileHandle);


/******************************************************************************
����������BOOL PzCreateDir(HANDLE hUsbHandle, const char* pszDir)
�����������˽ӿ�������˽�����д���Ŀ¼��
���������hUsbHandle-�豸�����
          pszDir-Ҫ������Ŀ¼�ַ�����
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzCreateDir)(HANDLE hUsbHandle, const char* pszDir);


/******************************************************************************
����������HANDLE PzOpenFile(HANDLE hUsbHandle, const char* pszFileName, BOOL bCreateWhenNotExist)
�����������˽ӿ����ڴ�˽�����е��ļ���
���������hUsbHandle-�豸�����
          pszFileName-Ҫ�򿪵��ļ�����
          bCreateWhenNotExist-���ļ�������ʱ���Ƿ񴴽�һ�����ļ��Ŀ��ء�
����������ޡ�
�� �� ֵ��HANDLE�������ļ���������������ļ���д�����ӿ�ʹ�á�
******************************************************************************/
typedef HANDLE (WINAPI *API_PzOpenFile)(HANDLE hUsbHandle, const char* pszFileName, BOOL bCreateWhenNotExist);


/******************************************************************************
����������BOOL PzReadFile(HANDLE hUsbHandle, HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
�������������ļ��ӿڣ����ڴӴ򿪵��ļ��еĵ�ǰ�α괦��ȡָ�����ȵ����ݡ�
���������hUsbHandle-�豸�����
          hFile-�ļ������PzOpenFile�ӿڵķ���ֵ��
          lpBuffer-�������ݻ�������
          nNumberOfBytesToRead-��ȡ�����ݳ��ȣ�Ҳ�������Ϊ�������ݵĻ�����lpBuffer�Ĵ�С����
          lpNumberOfBytesRead-ʵ�ʶ�ȡ�������ݵĳ��ȣ���Ϊ���������
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzReadFile)(HANDLE hUsbHandle, HANDLE hFile, LPVOID lpBuffer, 
                       DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);


/******************************************************************************
����������BOOL PzWriteFile(HANDLE hUsbHandle, HANDLE hFile, LPCVOID lpBuffer, 
                           DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
����������д�ļ��ӿڣ��������ļ���д��ָ�����ȵ����ݡ�
���������hUsbHandle-�豸�����
          hFile-�ļ������PzOpenFile�ӿڵķ���ֵ��
          lpBuffer-��Ҫд������ݻ�������
          nNumberOfBytesToWrite-��Ҫд������ݵĳ��ȡ�
          lpNumberOfBytesWritten-ʵ��д������ݳ��ȡ���Ϊ���������
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzWriteFile)(HANDLE hUsbHandle, HANDLE hFile, LPCVOID lpBuffer, 
						DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten);


/******************************************************************************
����������BOOL PzCloseFile(HANDLE hUsbHandle, HANDLE hFile)
�������������ڹر�PzOpenFile�򿪵��ļ������
���������hUsbHandle-�豸�����
          hFile-�ļ������PzOpenFile�ӿڵķ���ֵ��
����������ޡ�
�� �� ֵ��BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzCloseFile)(HANDLE hUsbHandle, HANDLE hFile);


/******************************************************************************
����������DWORD PzSetFilePointer(HANDLE hUsbHandle, HANDLE hFile, LONG lDistanceToMove, DWORD dwMoveMethod)
�����������ýӿ����������ļ���д�α��λ�á��α��λ��ȡֵΪ���ڻ����0��ͬʱС�ڻ�����ļ���С�������õ�
          �α�ƫ�Ƴ����ļ���Сʱ���α�ָ���ļ�β����ͬ�����������α�ƫ��λ��С��0ʱ���α�ָ���ļ�ͷ��
���������hUsbHandle-�豸�����
          hFile-�ļ������PzOpenFile�ӿڵķ���ֵ��
          lDistanceToMove-ƫ��λ�ã����ֽ�Ϊ��λ��
          dwMoveMethod-ƫ�޷�ʽ��ȡֵ��0�����ļ�ͷ���ƫ�ơ�1���ӵ�ǰλ�����ƫ�ޣ�2���ļ�β���ļ�ͷ����ƫ�ޡ�
����������ޡ�
�� �� ֵ��DWORD���������ú���ļ��α��ʵ��λ��
******************************************************************************/
typedef DWORD (WINAPI *API_PzSetFilePointer)(HANDLE hUsbHandle, HANDLE hFile, LONG lDistanceToMove, DWORD dwMoveMethod);


/******************************************************************************
����������DWORD PzGetFileSize(HANDLE hUsbHandle, HANDLE hFile)
�����������˽ӿ��������ֽ�Ϊ��λ�����ļ��Ĵ�С��
���������hUsbHandle-�豸�����
          hFile-�ļ������PzOpenFile�ӿڵķ���ֵ��
����������ޡ�
�� �� ֵ��DWORD�������ļ���ʵ�ʴ�С�����ֽ�Ϊ��λ��
******************************************************************************/
typedef DWORD (WINAPI *API_PzGetFileSize)(HANDLE hUsbHandle, HANDLE hFile);


/******************************************************************************
����������BOOL PzGetRemainSpaceSize(HANDLE hUsbHandle, DWORD& dwSize)
�����������˽ӿ��������ֽ�Ϊ��λ����˽����ʣ��ռ�Ĵ�С��
���������hUsbHandle-�豸�����
          dwSize-˽����ʣ��������С�����ֽ�Ϊ��λ��
����������ޡ�
�� �� ֵ��BOOL���ɹ�����TRUE��ʧ�ܷ���FALSE��
******************************************************************************/
typedef BOOL (WINAPI *API_PzGetRemainSpaceSize)(HANDLE hUsbHandle, __int64 * dwSize);

/******************************************************************************
����������int PzDeleteDir(HANDLE handle, const char*pszDirName)
����������ɾ��˽�����е�һ���ļ��У�ע����ļ��б���Ϊ�ա�
���������hUsbHandle-�豸�����
          pszDirName-Ҫɾ����Ŀ¼����
����������ޡ�
�� �� ֵ��BOOL���ɹ�����TRUE��ʧ�ܷ���FALSE��
******************************************************************************/
typedef int (WINAPI *API_PzDeleteDir)(HANDLE handle, const char*pszDirName);


/******************************************************************************
����������BOOL PzMoveFile(HANDLE hUsbHandle, const char* pszSrcName, const char* pszDestName)
�����������ƶ�˽�����е�һ���ļ��л��ļ���������ͬ��Ŀ¼�µ������
���������hUsbHandle-�豸�����
          pszSrcName-Դ�ļ�����Ŀ¼��
          pszDestName-Ŀ���ļ�����Ŀ¼��
����������ޡ�
�� �� ֵ��BOOL���ɹ�����TRUE��ʧ�ܷ���FALSE��
******************************************************************************/
typedef BOOL (WINAPI *API_PzMoveFile)(HANDLE hUsbHandle, const char* pszSrcName, const char* pszDestName);



/******************************************************************************
����������BOOL ReadE2prom(WORD Addr, WORD LenInDword, PVOID pOutBuff, DWORD BuffLen )
������������ȡI2C E2PROM��ָ����ַ��ʼ��ָ�����ȵ�����
���������WORD Addr,         //��ʼ��ַ����0 ��ʼ
          WORD LenInDword,   //��ȡ��DWORD������
          DWORD BuffLen      //����������
���������PVOID pOutBuff,    //���ݵĻ�������
�� �� ֵ��BOOL���ɹ�����TRUE�����򷵻�FALSE��
******************************************************************************/
BOOL WINAPI ReadE2Prom(HANDLE handle, WORD Addr, WORD LenInDword, BYTE *p_user_data);

/******************************************************************************
����������BOOL WriteE2prom(WORD Addr, WORD LenInDword, PVOID pOutBuff,  DWORD BuffLen )
������������I2C E2PROM�е�ָ����ַд��ָ�����ȵ�����
���������WORD Addr,         //��ʼ��ַ����0 ��ʼ
          WORD LenInDword,   //��ȡ��DWORD������
          PVOID pOutBuff,    //���ݵĻ�����
          DWORD BuffLen      //����������
����������ޡ�
�� �� ֵ��BOOL���ɹ�����TRUE�����򷵻�FALSE��
******************************************************************************/
BOOL WINAPI WriteE2Prom(HANDLE handle, WORD Addr, WORD LenInDword, BYTE *p_user_data);


typedef BOOL (WINAPI *API_Login)(HANDLE hUsbHandle, const char* szPassword);
typedef BOOL (WINAPI *API_Logout)(HANDLE hUsbHandle);

/******************************************************************************
����������void GetUID(HANDLE handle, DWORD *pUid, DWORD *pUid0);
�����������˺������ڻ��UID��UIDΪ���36λ�ַ�����ַ��
���������HANDLE handle - �豸���
          szUid-���36λ��UID�ַ������塣
����������ޡ�
�� �� ֵ��
******************************************************************************/
typedef void (WINAPI *API_GetUID)(HANDLE handle, DWORD *pUid, DWORD *pUid0);

typedef void (WINAPI *API_PzSetCopyFileProgress)(HANDLE handle, FPZCopyFileProgress fpProgress);


typedef BOOL (WINAPI *API_RefreshDisk)(HANDLE hUsbHandle);

/******************************************************************************
����������BOOL IsReadProtectEx(HANDLE handle, BYTE Lun)
�����������˺��������ж�ָ�������Ƿ�Ϊ���ö�����״̬��
���������HANDLE handle - �豸���
          Lun- �߼���Ԫ�ţ�����ָ�����ţ���
����������ޡ�
�� �� ֵ�����ָ������Ϊ������������TRUE�����򷵻�FALSE��
******************************************************************************/
typedef BOOL (WINAPI *API_IsReadProtectEx)(HANDLE handle, BYTE Lun);


#ifdef __cplusplus
}
#endif
