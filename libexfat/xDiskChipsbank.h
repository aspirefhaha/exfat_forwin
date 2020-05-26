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
//! User Mode下，不支持当前应用
#define ERR_IN_USER_MODE                40

#define BASE_FILESYSTEM_ERR             1000
#define FSE_OPERATION_SUCCESS               ERR_NO_ERROR               //0，操作成功。
#define FSE_FILE_NOT_EXIST                  (BASE_FILESYSTEM_ERR + 1)  //文件不存在。
#define FSE_FILE_HAD_EXIST                  (BASE_FILESYSTEM_ERR + 2)  //文件已存在。
#define FSE_FILE_OPEN_FAILED                (BASE_FILESYSTEM_ERR + 3)  //文件打开失败。
#define FSE_SECTOR_READ_FAILED              (BASE_FILESYSTEM_ERR + 4)  //读扇区失败。
#define FSE_SECTOR_WRITE_FAILED             (BASE_FILESYSTEM_ERR + 5)  //写扇区失败。
#define FSE_DISK_NOT_ENOUGH_SPACE           (BASE_FILESYSTEM_ERR + 6)  //磁盘空间不足。
#define FSE_NOTFIND_DIRTABLEITEM            (BASE_FILESYSTEM_ERR + 7)  //未找到目录项。
#define FSE_DELETE_FAT_FAILED               (BASE_FILESYSTEM_ERR + 8)  //删除文件分配表失败。
#define FSE_NEWBUFFER_FAILED                (BASE_FILESYSTEM_ERR + 9)  //分配内存失败。
#define FSE_INVALID_FILENAME                (BASE_FILESYSTEM_ERR + 10) //无效的文件名。
#define FSE_CREATE_DIR_FAILED               (BASE_FILESYSTEM_ERR + 11) //创建路径失败。
#define FSE_OPEN_FAT_FAILED                 (BASE_FILESYSTEM_ERR + 12) //打开FAT失败。
#define FSE_OPEN_FDT_FAILED                 (BASE_FILESYSTEM_ERR + 13) //打开FDT失败。
#define FSE_INVALID_FILE_HANDLE             (BASE_FILESYSTEM_ERR + 14) //无效的文件句柄。
#define FSE_INVALID_FIND_HANDLE             (BASE_FILESYSTEM_ERR + 15) //无效的文件查找句柄。
#define FSE_READ_DATA_CLUSTER_FAILED        (BASE_FILESYSTEM_ERR + 16) //读数据簇失败。
#define FSE_WRITE_DATA_CLUSTER_FAILED       (BASE_FILESYSTEM_ERR + 17) //写数据簇失败。
#define FSE_REMOVEDIR_HASFILE               (BASE_FILESYSTEM_ERR + 18) //该文件夹下有子文件或目录。

typedef BOOL (WINAPI *FPZCopyFileProgress)(DWORD dwFileSize,DWORD dwHasCopyed);

#ifdef __cplusplus
extern "C" 
{
#endif

/******************************************************************************
函数声明：HANDLE OpenUsbDisk(char* AuthorizeKey, const char* pszVID, const char* pszPID)
功能描述：对闪存盘进行枚举，并判断逻辑盘的主控类型是否匹配，检查隐藏区访问授权码。
          如果主控类型不对，或者授权码不正确，均会引起函数调用失败。如果调用成功，
          则打开相应闪存盘，返回闪存盘句柄。
输入参数：AuthorizeKey 最大16个字节的带空终结符的字符串，初始值在量产时设定。
          pszVID 设备VID号。
          pszPID 设备PID号。
输出参数：无。
返 回 值：HANDLE 设备句柄
******************************************************************************/
typedef HANDLE  (WINAPI   *API_OpenUsbDisk)(char* AuthorizeKey, 
    const char* pszVID ,////= NULL 
    const char* pszPID ,////= NULL 
    BOOL bCanMultiProcess //= TRUE
	);//此参数不用，只是为了兼容旧的版本   


/******************************************************************************
函数声明：HANDLE OpenUsbDiskEx(char* AuthorizeKey, char DriverLetter, const char* pszVID, const char* pszPID)
功能描述：打开指定盘符的闪存盘设备，并判断逻辑盘的主控类型是否匹配，检查隐藏区访问授权码。
          如果主控类型不对，或者授权码不正确，均会引起函数调用失败。如果调用成功，
          则打开相应闪存盘，返回闪存盘句柄。
输入参数：AuthorizeKey 最大16个字节的带空终结符的字符串，初始值在量产时设定。
          DriverLetter 指定的盘符
          pszVID 设备VID号。
          pszPID 设备PID号。
输出参数：无。
返 回     HANDLE 设备句柄
******************************************************************************/
typedef HANDLE (WINAPI *API_OpenUsbDiskEx)(char* AuthorizeKey, char DriverLetter, 
                         const char* pszVID ,////= NULL 
                         const char* pszPID ,////= NULL 
                         BOOL bCanMultiProcess //= TRUE
						 );//此参数不用，只是为了兼容旧的版本   


/******************************************************************************
函数声明：BOOL CloseUsbDisk(HANDLE handle)
功能描述：关闭由Open类函数（OpenUsbDisk/OpenUsbDiskEx）打开的闪存盘
输入参数：handle 设备句柄
输出参数：无。
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_CloseUsbDisk)(HANDLE handle);


/******************************************************************************
函数声明：BOOL ChangeAuthorizeKey(HANDLE handle, char *OldAuthorizeKey, char *NewAuthorizeKey)
功能描述：该函数用于更改授权码。
输入参数：handle-设备句柄。
          OldAuthorizeKey-原有的16字节授权码；
          NewAuthorizeKey-更改后的16字节授权码。
输出参数：无。
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_ChangeAuthorizeKey)(HANDLE handle, char *OldAuthorizeKey, char *NewAuthorizeKey);


/******************************************************************************
函数声明：BOOL CheckAuthorizeKey(HANDLE handle, char *AuthorizeKey)
功能描述：该函数用于检查授权码是否正确。
输入参数：handle-设备句柄。
          AuthorizeKey-用于比对的16字节授权码
输出参数：无。
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_CheckAuthorizeKey)(HANDLE handle, char *AuthorizeKey);


/******************************************************************************
函数声明：BOOL ReadPrivateUserSector(HANDLE handle, DWORD SectoroffsetInBuf, BYTE *p_user_data,　BYTE SectorNumber);
功能描述：该函数从私密区指定偏移处读取用户指定个数扇区的数据到缓冲区中。由于可以
          在私密区内部任意指定偏移开始读写，意味着可以将私密区划分为不同用途的子
          区块，存放不同用户的数据，如果用户在写入数据前做过加密处理，则可以保证
          数据的安全性。如果Lun2分区已经设置了读保护，则此函数无法进行读取。
输入参数：handle-设备句柄。
          SectorOffsetInBuf-指定从私密区内第几个扇区开始读，每扇区为512字节，可以为0,1…          
          SectorNumber-指定需要读取的扇区个数，每扇区为512字节。不能大于255。
输出参数：p_user_data-用于存放从私密区读取的数据的用户缓冲区；
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_ReadPrivateUserSector)(HANDLE handle,  
                                  DWORD SectoroffsetInBuf,    // Offset sectors in private buf, must be 0,1,2,...
                                  BYTE *p_user_data,          // Receive Data buf
                                  DWORD SectorNumber,          // Sectors(512 bytes per sector) to read
                                  BYTE Lun);                  //此参数不用，只是为了兼容旧的版本    


/******************************************************************************
函数声明：BOOL WritePrivateUserSector(HANDLE handle, DWORD SectoroffsetInBuf, BYTE *p_user_data, BYTE SectorNumber)
功能描述：该函数将用户缓冲区中指定个数扇区的数据写入到私密区指定偏移处。由于可以
          在私密区内部任意指定偏移开始读写，意味着可以将私密区划分为不同用途的子
          区块，存放不同用户的数据，如果用户在写入数据前做过加密处理，则可以保证
          数据的安全性。如果Lun2分区已经设置了读写保护，则此函数无法进行写入。
          如果将私密区显示为盘符，以普通盘的方式在Windows操作系统中使用，此时，如
          果调用该接口将可能会破坏私密区的文件分区表等，导致Windows操作系统无法识别私密区的盘符。
输入参数：handle-设备句柄。
          SectorOffsetInBuf-指定从私密区内第几个扇区开始写，每扇区为512字节，可以为0,1…    
          p_user_data-用于写私密区的数据
          SectorNumber-指定需要读取的扇区个数，每扇区为512字节。不能大于255。
输出参数：无
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_WritePrivateUserSector)(HANDLE handle, 
                                   DWORD SectoroffsetInBuf,    // Offset sectors in private buf, must be 0,1,2,...,
                                   BYTE *p_user_data,          // Data buf to write
                                   DWORD SectorNumber,          // Sectors(512 bytes per sector) to write
                                   BYTE Lun,                   //此参数不用，只是为了兼容旧的版本   
                                   BOOL bVerify //= FALSE        //此参数不用，只是为了兼容旧的版本   
                                    );     


/******************************************************************************
函数声明：__int64 GetHideZoneSize(HANDLE handle)
功能描述：该函数返回以字节为单位的隐藏区大小。
输入参数：handle-设备句柄。
输出参数：无。
返 回 值：隐藏区大小，如果返回值为0，则函数调用失败。
******************************************************************************/
typedef __int64 (WINAPI *API_GetHideZoneSize)(HANDLE handle); //if return 0, no hide Zone

/******************************************************************************
函数声明：BYTE GetHideZoneLun(HANDLE handle, BYTE &Lun)
功能描述：该函数返回以字节为单位的隐藏区大小。
输入参数：handle-设备句柄。
输出参数：Lun-用于存放返回的逻辑单元号。
返 回 值：逻辑单元号。
******************************************************************************/
typedef BYTE (WINAPI *API_GetHideZoneLun)(HANDLE handle, BYTE *Lun);

/******************************************************************************
函数声明：char GetHideZoneDriveLetter(HANDLE handle, char &DriveLetter)
功能描述：该函数用于确定可以用于隐藏区映射的逻辑分区的盘符。
输入参数：handle-设备句柄。
输出参数：DriveLetter-用于存放返回的盘符。
返 回 值：盘符，注意是一个字符，不带"："。
******************************************************************************/
typedef char (WINAPI *API_GetHideZoneDriveLetter)(HANDLE handle, char *DriveLetter);


/******************************************************************************
函数声明：BOOL VerifyUserPassword(HANDLE handle, char *UserPassword)
功能描述：验证用户密码。
输入参数：HANDLE handle-设备句柄。
          char *UserPassword-最长16个字符（含'\0'）；
输出参数：无。
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_VerifyUserPassword)(HANDLE handle, char *UserPassword);


/******************************************************************************
函数声明：ChangeUserPassword(HANDLE handle, char *OldUserPassword, char *NewUserPassword)
功能描述：此函数用于更改用户密码。注意，加密盘不支持更改用户密码。
输入参数：handle-设备句柄。
          OldUserPassword-修改前的用户密码，带空终结符的字符串，最长16个字符（含'\0'）；
          NewUserPassword-新的用户密码，带空终结符的字符串，最长16个字符（含'\0'）；
输出参数：无。
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_ChangeUserPassword)(HANDLE handle, char *OldUserPassword, char *NewUserPassword);


/******************************************************************************
函数声明：BOOL MountHideZoneAsDisk(HANDLE handle, BYTE Lun)
功能描述：此函数将隐藏区映射为一个逻辑分区，被映射的分区必须是可移动磁盘类型，不能是本地盘类型
输入参数：handle-设备句柄。
          Lun-隐藏分区的逻辑单元号。
输出参数：无。
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_MountHideZoneAsDisk)(HANDLE handle, BYTE Lun);


/******************************************************************************
函数声明：BOOL UnMountHideZoneAsDisk(HANDLE handle, BYTE Lun)
功能描述：此函数与MountHideZoneAsDisk函数配对使用。用于卸载已经映射到逻辑分区的隐藏区。
输入参数：handle-设备句柄。
          Lun-隐藏分区的逻辑单元号。
输出参数：无。
返 回 值：BOOL。
******************************************************************************/
typedef BOOL (WINAPI *API_UnMountHideZoneAsDisk)(HANDLE handle, BYTE Lun);


/******************************************************************************
函数声明：BOOL IsInHideZone(HANDLE handle, BYTE Lun)
功能描述：此函数用于判断逻辑单元号为Lun的分区是否已经被映射了。
输入参数：handle-设备句柄。
          Lun-分区的逻辑单元号。
输出参数：无。
返 回 值：TRUE-处于隐藏区中，FALSE-处于非隐藏区中。
******************************************************************************/
typedef BOOL (WINAPI *API_IsInHideZone)(HANDLE handle, BYTE Lun);


/******************************************************************************
函数声明：BOOL IsCdrom(HANDLE handle, char DriveLetter)
功能描述：此函数用于判断是否是CD-ROM分区。
输入参数：handle-设备句柄。
          DriveLetter-盘符。
输出参数：无。
返 回 值：TRUE-指定的盘是CD-ROM分区，FALSE-- 指定的盘不是CD-ROM分区
******************************************************************************/
typedef BOOL (WINAPI *API_IsCdrom)(HANDLE handle, char DriveLetter);


/******************************************************************************
函数声明：BOOL SetDiskProtect(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite)
功能描述：利用此函数可以改变逻辑分区的读写访问权限。
输入参数：handle-设备句柄。
          Lun-分区的逻辑单元号。
          bCanRead-可读标志；
          bCanWrite-可写标志。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_SetDiskProtect)(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite);


/******************************************************************************
函数声明：BOOL SetDiskProtectEx(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite)
功能描述：利用此函数可以永久改变逻辑分区的读写访问权限。
输入参数：handle-设备句柄。
          Lun-分区的逻辑单元号。
          bCanRead-可读标志；
          bCanWrite-可写标志。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_SetDiskProtectEx)(HANDLE handle, BYTE Lun, BOOL bCanRead, BOOL bCanWrite);

/******************************************************************************
函数声明：void LockUsbDisk(HANDLE handle, BYTE Lun, BYTE &LockStatus)
功能描述：利用此函数可以对逻辑分区进行锁定，不能进行读、写操作。
输入参数：handle-设备句柄。
          Lun-分区的逻辑单元号。
          LockStatus-用于保存加锁前的读写状态。
输出参数：无。
返 回 值：void
******************************************************************************/
typedef void (WINAPI *API_LockUsbDisk)(HANDLE handle, BYTE Lun, BYTE *LockStatus);


/******************************************************************************
函数声明：void UnLockUsbDisk(HANDLE handle, BYTE Lun, BYTE &LockStatus)
功能描述：与LockUsbDisk函数配对，解锁被加锁的分区，解锁后将恢复加锁前的读写访问权限。
输入参数：handle-设备句柄。
          Lun-分区的逻辑单元号。
          LockStatus-用于存放需要恢复的加锁前的读写状态，由LockUsbDisk函数得到。
输出参数：无。
返 回 值：void
******************************************************************************/
typedef void (WINAPI *API_UnLockUsbDisk)(HANDLE handle, BYTE Lun, BYTE *LockStatus);


/******************************************************************************
函数声明：__int64 GetUsbDiskSize(HANDLE handle, BYTE Lun)
功能描述：此函数用于获取以字节为单位的，逻辑单元号为Lun的逻辑分区的容量。
输入参数：handle-设备句柄。
          Lun-分区的逻辑单元号。
输出参数：无。
返 回 值：逻辑分区的容量。
******************************************************************************/
typedef __int64 (WINAPI *API_GetUsbDiskSize)(HANDLE handle, BYTE Lun);


/******************************************************************************
函数声明：BOOL SetUsbDiskSize(HANDLE handle, DWORD Lun0Size,DWORD Lun1Size,DWORD hZoneSize,DWORD pZoneSize)
功能描述：此函数用于对闪存盘进行重新分区，逻辑分区0、逻辑分区1、隐藏区和私密区容量之和应该等于整个闪存盘的
          逻辑容量。重新分区后，闪存盘类型、各分区读/写保护仍与量产时指定的设置一致，但是Password值会被清为""。
          除CD-ROM区外，其他分区必须用DiskFormat()函数进行格式化后才能正常使用; CD-ROM区需要使用
          UpdateISOFileOfCDROMDriver()函数重新写入新的ISO文件。如果打算不对普通盘进行分区，那么不能设置Lun0Size为0，
          而是需要把Lun1Size设置成0。
          注意
          1、每个分区建议至少保证1M的容量，即传入的Lun0Size、Lun1Size、hZoneSize和pZoneSize的值不小于2000，
             否则调用DiskFormat接口时可能会失败。
          2、调用完此函数，再调用DiskFormat函数对分区进行格式化后，需要重新上盘才能继续使用。
输入参数：handle-设备句柄。
          Lun0Size-逻辑分区0的容量，以Sector为单位，每个Sector为512字节。
          Lun1Size-逻辑分区1的容量，以Sector为单位，每个Sector为512字节。
          hZoneSize--隐藏区的容量， 以Sector为单位，每个Sector为512字节。
          pZoneSize-私密区的容量，  以Sector为单位，每个Sector为512字节。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_SetUsbDiskSize)(HANDLE handle, DWORD Lun0Size, DWORD Lun1Size, DWORD hZoneSize, DWORD pZoneSize);


/******************************************************************************
函数声明：BOOL DiskFormat(HANDLE handle, BYTE Lun)
功能描述：格式化闪存盘的逻辑分区。二次开发接口提供逻辑分区的调节功能，并通过此
          函数对逻辑分区进行格式化。注意格式化后，闪存盘中的数据将丢失。
输入参数：handle-设备句柄。
          Lun-分区的逻辑单元号。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_DiskFormat)(HANDLE handle, BYTE Lun);//Format Current Logic Partition which has private sectors


/******************************************************************************
函数声明：int UpdateISOFileOfCDROMDriver(HANDLE handle, BYTE Lun, char* FileName)
功能描述：该函数可以用于升级AUTORUN光盘映像（ISO）文件，ISO文件可以利用UltraISO等
          工具来制作。AUTORUN盘在量产时，CD-ROM盘的大小是由ISO文件的大小来决定的，
          如果要考虑今后升级到更大的ISO文件，就必须在量产时指定CD-ROM盘的最大值。
          注意：如果在量产时选择闪存盘为AutoRun盘，则Lun号1。
输入参数：handle-设备句柄。
          Lun-闪存盘逻辑单元号。
          FileName-要写入CD-ROM的光盘映像文件名。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef int (WINAPI *API_UpdateISOFileOfCDROMDriver)(HANDLE handle, BYTE Lun, char* FileName);


/******************************************************************************
函数声明：BYTE GetUDiskSerialNumber(HANDLE handle, char *SerialNumber)
功能描述：闪存盘在量产时均具有惟一的序列号，序列号的最小长度是12字节，序列号最大
          长度是32字节，并且序列号必须是"ABCDEF0123456789"中的字符。如果在量产时
          选择自动产生序列号，则序列号的有效长度为16个字节；因此必须注意提供的接
          收缓冲区应该足够大。
输入参数：handle-设备句柄。          
输出参数：SerialNumber-用于存放返回的序列号。
返 回 值：序列号长度
******************************************************************************/
typedef BYTE (WINAPI *API_GetUDiskSerialNumber)(HANDLE handle, char *SerialNumber);


/******************************************************************************
函数声明：BOOL SetUDiskSerialNumber(HANDLE handle, char *SerialNumber)
功能描述：闪存盘在量产时均具有惟一的序列号，序列号的最小长度是12字节，最大长度
          是32字节，并且序列号必须是"ABCDEF0123456789"中的字符。该函数用于更改此
          序列号，如果给定的新序列号长度少于12个字节，该函数将自动在前补'0'，使
          新的序列号长度达到12个字节；如果给定的新序列号长度超过32个字节，该函数
          将自动截取前32个字符作为新的序列号。
输入参数：handle-设备句柄。
          SerialNumber-用于存放新的序列号。必须是带有空终结符的字符串。          
输出参数：无
返 回 值：序列号长度
******************************************************************************/
typedef BOOL (WINAPI *API_SetUDiskSerialNumber)(HANDLE handle, char *SerialNumber);


/******************************************************************************
函数声明：WORD GetPID(HANDLE handle, WORD &Pid)
功能描述：该函数返回产品的标识码，例如0x6025。
输入参数：handle-设备句柄。
输出参数：Pid-用于存放取得的产品ID
返 回 值：产品ID
******************************************************************************/
typedef WORD (WINAPI *API_GetPID)(HANDLE handle, WORD *Pid);


/******************************************************************************
函数声明：WORD SetPID(HANDLE handle, WORD &Pid)
功能描述：该函数返回产品的标识码，例如0x6025。
输入参数：handle-设备句柄。
          Pid-新的产品标识码。
输出参数：无
返 回 值：产品ID
******************************************************************************/
typedef BOOL (WINAPI *API_SetPID)(HANDLE handle, WORD PID);


/******************************************************************************
函数声明：WORD GetVID(HANDLE handle, WORD &Vid)
功能描述：该函数返回产品的标识码，例如0x6025。
输入参数：handle-设备句柄。
输出参数：用于存放取得的厂商ID。
返 回 值：厂商ID
******************************************************************************/
typedef WORD (WINAPI *API_GetVID)(HANDLE handle, WORD *Vid);


/******************************************************************************
函数声明：WORD SetVID(HANDLE handle, WORD &Pid)
功能描述：该函数用于设置厂商的标识码，例如0x0205。
输入参数：handle-设备句柄。
          VID-新的厂商标识码。
输出参数：无
返 回 值：厂商ID
******************************************************************************/
typedef BOOL (WINAPI *API_SetVID)(HANDLE handle, WORD VID);


/******************************************************************************
函数声明：char* GetProductInfo(HANDLE handle, char* ProductInfo)
功能描述：该函数返回16个字节的产品名称串，例如"Flash Disk      "
输入参数：handle-设备句柄。
输出参数：ProductInfo-产品信息串接收缓冲区。
返 回 值：产品名称串
******************************************************************************/
typedef char* (WINAPI *API_GetProductInfo)(HANDLE handle, char* ProductInfo);


/******************************************************************************
函数声明：char* SetProductInfo(HANDLE handle, char* ProductInfo)
功能描述：该函数用于设置产品名称串，新的产品名称串最大长度不能超过16个字符。
输入参数：handle-设备句柄。
          ProductInfo-新的产品信息串。
输出参数：无
返 回 值：产品名称串
******************************************************************************/
typedef BOOL (WINAPI *API_SetProductInfo)(HANDLE handle, char *ProductInfo);


/******************************************************************************
函数声明：char* GetVendorInfo(HANDLE handle, char* VendorInfo)
功能描述：该函数返回8个字节的厂商名称串，例如"Chipsbnk"。
输入参数：handle-设备句柄。
输出参数：VendorInfo-厂商信息串接收缓冲区。
返 回 值：厂商信息串
******************************************************************************/
typedef char* (WINAPI *API_GetVendorInfo)(HANDLE handle, char* VendorInfo);


/******************************************************************************
函数声明：char* SetVendorInfo(HANDLE handle, char* VendorInfo)
功能描述：该函数用于设置产品名称串，新的产品名称串最大长度不能超过16个字符。
输入参数：handle-设备句柄。
          VendorInfo-厂商信息串
输出参数：无
返 回 值：厂商信息串
******************************************************************************/
typedef BOOL (WINAPI *API_SetVendorInfo)(HANDLE handle, char *VendorInfo);


/******************************************************************************
函数声明：BOOL IsHighSpeed(HANDLE handle)
功能描述：此函数用于判断USB实际工作模式是1.1还是2.0。
输入参数：HANDLE handle - 设备句柄
输出参数：无。
返 回 值：TRUE 为USB2.0，FALSE 为USB1.1。
******************************************************************************/
typedef BOOL (WINAPI *API_IsHighSpeed)(HANDLE handle); //



/******************************************************************************
函数声明：WORD GetVendorSerial(HANDLE handle)
功能描述：此函数用于返回在量产时厂商指定的序列号，一般用于检测该设备是否由厂商生产。
输入参数：HANDLE handle - 设备句柄
输出参数：无。
返 回 值：厂商自定义的序列号。
******************************************************************************/
typedef WORD (WINAPI *API_GetVendorSerial)(HANDLE handle);


/******************************************************************************
函数声明：char* GetUDiskLetter(HANDLE handle, DWORD dwMilliseconds)
功能描述：此函数用于返回具有隐藏区的最近用OpenUsbDisk打开的闪存盘的盘符串。如果是
          分区盘，将返回多个盘符，如："JK"。由于在闪存盘上盘时有时会出现闪存盘的分
          区消失后又重新出现的现象，因此这时调用此函数可能会获得不到所有的盘符。
          通过dwMilliseconds参数可以设置需要等待的时间。
输入参数：HANDLE handle - 设备句柄
          dwMilliseconds-延时毫秒数
输出参数：无。
返 回 值：闪存盘的盘符串。如果出错则返回NULL。
******************************************************************************/
typedef char* (WINAPI *API_GetUDiskLetter)(HANDLE handle, DWORD dwMilliseconds);


/******************************************************************************
函数声明：int GetError(HANDLE handle //= NULL)
功能描述：此函数用于获得错误代码。当传入的handle句柄有效时，返回该设备API调用时
          产生的最后错误代码；当传入NULL时，返回整个API库调用时产生的最后错误代码。  
输入参数：HANDLE handle - 设备句柄
输出参数：无。
返 回 值：错误代码。
******************************************************************************/
typedef int (WINAPI *API_GetError)(HANDLE handle); //= NULL);


/******************************************************************************
函数声明：__int64 GetPrivateZoneSize(HANDLE handle)
功能描述：此函数用于获取私密区的大小。以字节为单位。
输入参数：HANDLE handle - 设备句柄
输出参数：无。
返 回 值：返回私密区的大小。
******************************************************************************/
typedef __int64 (WINAPI *API_GetPrivateZoneSize)(HANDLE handle); //if return 0, no private Zone


/******************************************************************************
函数声明：BYTE GetPrivateZoneLun(HANDLE handle, BYTE &Lun)
功能描述：此函数用于获取私密区的逻辑单元号。
输入参数：HANDLE handle - 设备句柄
输出参数：无。
返 回 值：返回私密区的逻辑单元号。
******************************************************************************/
typedef BYTE (WINAPI *API_GetPrivateZoneLun)(HANDLE handle, BYTE *Lun);


/******************************************************************************
函数声明：BOOL IsReadProtect(HANDLE handle, BYTE Lun)
功能描述：此函数用于判断指定分区是否为读保护状态。
输入参数：HANDLE handle - 设备句柄
          Lun- 逻辑单元号（这里指分区号）。
输出参数：无。
返 回 值：如果指定分区为读保护，返回TRUE，否则返回FALSE。
******************************************************************************/
typedef BOOL (WINAPI *API_IsReadProtect)(HANDLE handle, BYTE Lun);


/******************************************************************************
函数声明：BOOL IsWriteProtect(HANDLE handle, BYTE Lun)
功能描述：此函数用于判断指定分区是否为写保护状态。
输入参数：HANDLE handle - 设备句柄
          Lun- 逻辑单元号（这里指分区号）。
输出参数：无。
返 回 值：如果指定分区为写保护，返回TRUE，否则返回FALSE。
******************************************************************************/
typedef BOOL (WINAPI *API_IsWriteProtect)(HANDLE handle, BYTE Lun);


/******************************************************************************
函数声明：int PzCopyFileTo(HANDLE handle, const char* pszFromFullFileName, const char* pszToFileName, BOOL bFailIfExists)
功能描述：该接口用于将指定文件（pszFromFullFileName)以指定名称（pszToFileName）拷贝到私密区
输入参数：HANDLE handle - 设备句柄
          pszFromFullFileName-源文件全路径名。注意：这个参数指硬盘上的需要拷进私密区的文件，所以必须是全路径名，
                                                    否则会在当前工作目录下进行查找，如果仍找不到，则返回失败。
          pszToFileName-目标文件名。支持长文件名和子目录。
          BOOL bFailIfExists-如果文件存在时如何处理：TRUE，当文件存在时，返回失败。FALSE，当文件存在时，覆盖已存在的文件，并返回成功。
输出参数：无。
返 回 值：成功返回0，失败返回错误码。参见错误值列表
******************************************************************************/
typedef int (WINAPI *API_PzCopyFileTo)(HANDLE handle, const char* pszFromFullFileName, const char* pszToFileName, BOOL bFailIfExists);


/******************************************************************************
函数声明：int PzCopyFileFrom(HANDLE handle, const char* pszFromFileName, const char* pszToFullFileName)
功能描述：此接口用于将私密区根目录下的指定文件(pszFromFileName)拷贝到指定位置（pszToFullFileName）。
输入参数：HANDLE handle - 设备句柄
          pszFromFileName-源文件名。
          pszToFullFileName-目标文件全路径名。这里是要拷出的文件存放地址，全路径名，如果没有指定全路径，则将拷贝到当前工作目录下。
输出参数：无。
返 回 值：成功返回0，失败返回错误码。参见错误值列表
******************************************************************************/
typedef int (WINAPI *API_PzCopyFileFrom)(HANDLE handle, const char* pszFromFileName, const char* pszToFullFileName);


/******************************************************************************
函数声明：int PzDeleteFile(HANDLE handle, const char* pszFileName)
功能描述：此接口用于删除私密区根目录下指定文件名（pszFileName）的文件。
输入参数：HANDLE handle - 设备句柄
          pszFileName-要删除的文件名。
输出参数：无。
返 回 值：成功返回0，失败返回非零的错误码。
******************************************************************************/
typedef int (WINAPI *API_PzDeleteFile)(HANDLE handle, const char* pszFileName);


/******************************************************************************
函数声明：HANDLE PzFindFirstFile(HANDLE hUsbHandle, const char* pszFindPath, LPWIN32_FIND_DATA lpFindFileData)
功能描述：从私密区指定目录下查找第一个文件。如果pszFindPath为空字符串，则从根目录下进行查找。
输入参数：hUsbHandle-设备句柄。
          pszFindPath-指定要查找的路径。
          lpFindFileData-接收找到的文件信息。
输出参数：无。
返 回 值：成功返回文件查找句柄，返回的句柄供PzGetNextFile调用时使用，失败返回NULL。
******************************************************************************/
typedef HANDLE (WINAPI *API_PzFindFirstFile)(HANDLE hUsbHandle, const char* pszFindPath, LPWIN32_FIND_DATA lpFindFileData);


/******************************************************************************
函数声明：BOOL PzFindNextFile(HANDLE hUsbHandle, HANDLE hFildFileHandle, LPWIN32_FIND_DATA lpFindFileData)
功能描述：从私密区指定目录下查找下一个文件。
输入参数：hUsbHandle-设备句柄。
          hFildFileHandle-调用PzFindFirstFile返回的句柄。
          lpFindFileData-接收查找到的文件的相关信息。
输出参数：无。
返 回 值：BOOL，成功找到文件，返回TRUE，否则返回FALSE。
******************************************************************************/
typedef BOOL (WINAPI *API_PzFindNextFile)(HANDLE hUsbHandle, HANDLE hFildFileHandle, LPWIN32_FIND_DATA lpFindFileData);


/******************************************************************************
函数声明：BOOL PzFindClose(HANDLE hUsbHandle, HANDLE hFindFileHandle)
功能描述：此接口用于释放PzFindFirstFile接口返回的文件查找句柄。
输入参数：hUsbHandle-设备句柄。
          hFindFileHandle-文件查找句柄。
输出参数：无。
返 回 值：BOOL，成功返回TRUE，失败返回FALSE。
******************************************************************************/
typedef BOOL (WINAPI *API_PzFindClose)(HANDLE hUsbHandle, HANDLE hFindFileHandle);


/******************************************************************************
函数声明：BOOL PzCreateDir(HANDLE hUsbHandle, const char* pszDir)
功能描述：此接口用于在私密区中创建目录。
输入参数：hUsbHandle-设备句柄。
          pszDir-要创建的目录字符串。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzCreateDir)(HANDLE hUsbHandle, const char* pszDir);


/******************************************************************************
函数声明：HANDLE PzOpenFile(HANDLE hUsbHandle, const char* pszFileName, BOOL bCreateWhenNotExist)
功能描述：此接口用于打开私密区中的文件。
输入参数：hUsbHandle-设备句柄。
          pszFileName-要打开的文件名。
          bCreateWhenNotExist-当文件不存在时，是否创建一个空文件的开关。
输出参数：无。
返 回 值：HANDLE，返回文件句柄，供其它的文件读写操作接口使用。
******************************************************************************/
typedef HANDLE (WINAPI *API_PzOpenFile)(HANDLE hUsbHandle, const char* pszFileName, BOOL bCreateWhenNotExist);


/******************************************************************************
函数声明：BOOL PzReadFile(HANDLE hUsbHandle, HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
功能描述：读文件接口，用于从打开的文件中的当前游标处读取指定长度的数据。
输入参数：hUsbHandle-设备句柄。
          hFile-文件句柄，PzOpenFile接口的返回值。
          lpBuffer-接收数据缓冲区。
          nNumberOfBytesToRead-读取的数据长度（也可以理解为接收数据的缓冲区lpBuffer的大小）。
          lpNumberOfBytesRead-实际读取到的数据的长度，作为输出参数。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzReadFile)(HANDLE hUsbHandle, HANDLE hFile, LPVOID lpBuffer, 
                       DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);


/******************************************************************************
函数声明：BOOL PzWriteFile(HANDLE hUsbHandle, HANDLE hFile, LPCVOID lpBuffer, 
                           DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
功能描述：写文件接口，用于向文件中写入指定长度的数据。
输入参数：hUsbHandle-设备句柄。
          hFile-文件句柄，PzOpenFile接口的返回值。
          lpBuffer-需要写入的数据缓冲区。
          nNumberOfBytesToWrite-需要写入的数据的长度。
          lpNumberOfBytesWritten-实际写入的数据长度。作为输出参数。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzWriteFile)(HANDLE hUsbHandle, HANDLE hFile, LPCVOID lpBuffer, 
						DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten);


/******************************************************************************
函数声明：BOOL PzCloseFile(HANDLE hUsbHandle, HANDLE hFile)
功能描述：用于关闭PzOpenFile打开的文件句柄。
输入参数：hUsbHandle-设备句柄。
          hFile-文件句柄，PzOpenFile接口的返回值。
输出参数：无。
返 回 值：BOOL
******************************************************************************/
typedef BOOL (WINAPI *API_PzCloseFile)(HANDLE hUsbHandle, HANDLE hFile);


/******************************************************************************
函数声明：DWORD PzSetFilePointer(HANDLE hUsbHandle, HANDLE hFile, LONG lDistanceToMove, DWORD dwMoveMethod)
功能描述：该接口用于设置文件读写游标的位置。游标的位置取值为大于或等于0，同时小于或等于文件大小。当设置的
          游标偏移超过文件大小时，游标指向文件尾部，同样，当设置游标偏移位置小于0时，游标指向文件头。
输入参数：hUsbHandle-设备句柄。
          hFile-文件句柄，PzOpenFile接口的返回值。
          lDistanceToMove-偏移位置，以字节为单位。
          dwMoveMethod-偏侈方式。取值：0，从文件头向后偏移。1，从当前位置向后偏侈，2从文件尾向文件头方向偏侈。
输出参数：无。
返 回 值：DWORD，返回设置后的文件游标的实际位置
******************************************************************************/
typedef DWORD (WINAPI *API_PzSetFilePointer)(HANDLE hUsbHandle, HANDLE hFile, LONG lDistanceToMove, DWORD dwMoveMethod);


/******************************************************************************
函数声明：DWORD PzGetFileSize(HANDLE hUsbHandle, HANDLE hFile)
功能描述：此接口用于以字节为单位返回文件的大小。
输入参数：hUsbHandle-设备句柄。
          hFile-文件句柄，PzOpenFile接口的返回值。
输出参数：无。
返 回 值：DWORD，返回文件的实际大小，以字节为单位。
******************************************************************************/
typedef DWORD (WINAPI *API_PzGetFileSize)(HANDLE hUsbHandle, HANDLE hFile);


/******************************************************************************
函数声明：BOOL PzGetRemainSpaceSize(HANDLE hUsbHandle, DWORD& dwSize)
功能描述：此接口用于以字节为单位返回私密区剩余空间的大小。
输入参数：hUsbHandle-设备句柄。
          dwSize-私密区剩余容量大小，以字节为单位。
输出参数：无。
返 回 值：BOOL，成功返回TRUE，失败返回FALSE。
******************************************************************************/
typedef BOOL (WINAPI *API_PzGetRemainSpaceSize)(HANDLE hUsbHandle, __int64 * dwSize);

/******************************************************************************
函数声明：int PzDeleteDir(HANDLE handle, const char*pszDirName)
功能描述：删除私密区中的一个文件夹，注意该文件夹必须为空。
输入参数：hUsbHandle-设备句柄。
          pszDirName-要删除的目录名称
输出参数：无。
返 回 值：BOOL，成功返回TRUE，失败返回FALSE。
******************************************************************************/
typedef int (WINAPI *API_PzDeleteDir)(HANDLE handle, const char*pszDirName);


/******************************************************************************
函数声明：BOOL PzMoveFile(HANDLE hUsbHandle, const char* pszSrcName, const char* pszDestName)
功能描述：移动私密区中的一个文件夹或文件，可用作同级目录下的重命令。
输入参数：hUsbHandle-设备句柄。
          pszSrcName-源文件名或目录名
          pszDestName-目标文件名或目录名
输出参数：无。
返 回 值：BOOL，成功返回TRUE，失败返回FALSE。
******************************************************************************/
typedef BOOL (WINAPI *API_PzMoveFile)(HANDLE hUsbHandle, const char* pszSrcName, const char* pszDestName);



/******************************************************************************
函数声明：BOOL ReadE2prom(WORD Addr, WORD LenInDword, PVOID pOutBuff, DWORD BuffLen )
功能描述：读取I2C E2PROM中指定地址开始的指定长度的数据
输入参数：WORD Addr,         //起始地址，从0 开始
          WORD LenInDword,   //读取的DWORD个数。
          DWORD BuffLen      //缓冲区长度
输出参数：PVOID pOutBuff,    //数据的缓冲区。
返 回 值：BOOL，成功返回TRUE，否则返回FALSE。
******************************************************************************/
BOOL WINAPI ReadE2Prom(HANDLE handle, WORD Addr, WORD LenInDword, BYTE *p_user_data);

/******************************************************************************
函数声明：BOOL WriteE2prom(WORD Addr, WORD LenInDword, PVOID pOutBuff,  DWORD BuffLen )
功能描述：往I2C E2PROM中的指定地址写入指定长度的数据
输入参数：WORD Addr,         //起始地址，从0 开始
          WORD LenInDword,   //读取的DWORD个数。
          PVOID pOutBuff,    //数据的缓冲区
          DWORD BuffLen      //缓冲区长度
输出参数：无。
返 回 值：BOOL，成功返回TRUE，否则返回FALSE。
******************************************************************************/
BOOL WINAPI WriteE2Prom(HANDLE handle, WORD Addr, WORD LenInDword, BYTE *p_user_data);


typedef BOOL (WINAPI *API_Login)(HANDLE hUsbHandle, const char* szPassword);
typedef BOOL (WINAPI *API_Logout)(HANDLE hUsbHandle);

/******************************************************************************
函数声明：void GetUID(HANDLE handle, DWORD *pUid, DWORD *pUid0);
功能描述：此函数用于获得UID，UID为最大36位字符串地址。
输入参数：HANDLE handle - 设备句柄
          szUid-最大36位的UID字符串缓冲。
输出参数：无。
返 回 值：
******************************************************************************/
typedef void (WINAPI *API_GetUID)(HANDLE handle, DWORD *pUid, DWORD *pUid0);

typedef void (WINAPI *API_PzSetCopyFileProgress)(HANDLE handle, FPZCopyFileProgress fpProgress);


typedef BOOL (WINAPI *API_RefreshDisk)(HANDLE hUsbHandle);

/******************************************************************************
函数声明：BOOL IsReadProtectEx(HANDLE handle, BYTE Lun)
功能描述：此函数用于判断指定分区是否为永久读保护状态。
输入参数：HANDLE handle - 设备句柄
          Lun- 逻辑单元号（这里指分区号）。
输出参数：无。
返 回 值：如果指定分区为读保护，返回TRUE，否则返回FALSE。
******************************************************************************/
typedef BOOL (WINAPI *API_IsReadProtectEx)(HANDLE handle, BYTE Lun);


#ifdef __cplusplus
}
#endif
