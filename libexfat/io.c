/*
	io.c (02.09.09)
	exFAT file system implementation library.

	Free exFAT implementation.
	Copyright (C) 2010-2018  Andrew Nayenko

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "exfat.h"

#if defined(WIN32) && _MSC_VER < 1900
#include "../win/libexfat/inttypes.h"
#else
#include <inttypes.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if !defined(WIN32)
#include <unistd.h>
#else
#include <Windows.h>
#include "xDiskInterface.h"
#include <io.h>
#include <process.h>
#endif
#include <string.h>
#include <errno.h>
#if defined(__APPLE__)
#include <sys/disk.h>
#elif defined(__OpenBSD__)
#include <sys/param.h>
#include <sys/disklabel.h>
#include <sys/dkio.h>
#include <sys/ioctl.h>
#elif __linux__
#include <sys/mount.h>
#endif
#ifdef USE_UBLIO
#include <sys/uio.h>
#include <ublio.h>
#endif

#ifdef WIN32

HINSTANCE  glibXDisk = NULL;
API_OpenXDisk  pOpenXDisk = NULL;
API_CloseXDisk pCloseXDisk = NULL;
API_WriteHideSector pWriteHideSector = NULL;
API_ReadHideSector pReadHideSector = NULL;
API_GetError pGetError = NULL;
static void xdisk_init()
{
	if(glibXDisk==NULL){
		glibXDisk = LoadLibraryA("xDiskInterface.dll");
		if(glibXDisk== NULL){
			OutputDebugString("Load xDiskInterface.dll Failed!!!!!!!!!!!\n");
			return;
		}
		OutputDebugString("Load xDiskInterface.dll Ok\n");
		MessageBox(NULL,"Load xDiskInterface.dll Ok","Load",MB_OK);
		pOpenXDisk = (API_OpenXDisk)GetProcAddress(glibXDisk,"OpenXDisk");
		if(pOpenXDisk==NULL){
			OutputDebugString("Get OpenXDisk Failed!!!!!!!!!!!\n");
			return;
		}
		pCloseXDisk = (API_CloseXDisk)GetProcAddress(glibXDisk,"CloseXDisk");
		if(pCloseXDisk==NULL){
			OutputDebugString("Get CloseXDisk Failed!!!!!!!!!!\n");
			return;
		}

		pWriteHideSector = (API_WriteHideSector)GetProcAddress(glibXDisk,"WriteHideSector");
		if(pWriteHideSector==NULL){
			OutputDebugString("Get WriteHideSector Failed!!!!!!!!!!!\n");
			return;
		}

		pReadHideSector = (API_ReadHideSector)GetProcAddress(glibXDisk,"ReadHideSector");
		if(pReadHideSector==NULL){
			OutputDebugString("Get ReadHideSector Failed!!!!!!!!!!!\n");
			return;
		}

		pGetError = (API_GetError)GetProcAddress(glibXDisk,"GetError");
		if(pGetError==NULL){
			OutputDebugString("Get GetError Failed!!!!!!!!!!!!!\n");
			return;
		}
	}
}

static void xdisk_uninit()
{
	if(glibXDisk != NULL){
		FreeLibrary(glibXDisk);
		glibXDisk = NULL;
		OutputDebugString("xdisk_uninit !!!!!!!!!!!!!!!\n");
	}
}
//TODO need change to USB Flash
int fsync(int fd) 
{
	//sync();
	return 0;
}
#if USEXDISK==1
#define XDISKSECSIZE 512
#define XDISKCURTOTALSIZE (50000LL*1024LL*1024L)
#endif
int pread(HANDLE fd, char * buf, size_t size, off_t off)
{
#if USEXDISK==0
	off_t ret = lseek((int)fd,  off, SEEK_SET);
	if (ret == (off_t)-1) {
		return 0;
	}
	return read((int)fd, buf, size);
#else
	BYTE tmpbuf[XDISKSECSIZE];
	char errbuf[XDISKSECSIZE];
	int readsec = 0;
	size_t needsize = size;
	size_t firstsecsize = 0;
	sprintf((char *)errbuf,"\t\twant read at %llx size %u\n",off,size);
	OutputDebugString((LPCSTR)errbuf);
	if(off%XDISKSECSIZE!=0){ //start pos is not XDISKSECSIZE aligned
		off_t startoff = off / XDISKSECSIZE * XDISKSECSIZE;
		size_t firstsecsize = (off - startoff + needsize) > XDISKSECSIZE ? (startoff + XDISKSECSIZE - off) : needsize ;
		if(pReadHideSector(fd, startoff / XDISKSECSIZE,tmpbuf,1,FALSE,NULL)){
			
			memcpy(buf,&tmpbuf[off - startoff],firstsecsize);
		}
		else{
			sprintf((char *)tmpbuf,"first read failed at 0x%llx size %d errcode 0x%x\n",off,firstsecsize,pGetError());
			OutputDebugString((LPCSTR)tmpbuf);
		}
		needsize -= firstsecsize;
		off += firstsecsize;
	}
	//off is now XDISKSECSIZE aligned
	while(needsize>=XDISKSECSIZE){
		off_t curoff = off / XDISKSECSIZE;
		if(pReadHideSector(fd, curoff,tmpbuf,1,FALSE,NULL))
			memcpy(buf + firstsecsize + readsec * XDISKSECSIZE,tmpbuf,XDISKSECSIZE);
		else{
			sprintf((char *)tmpbuf,"mid read failed at 0x%llx  size %d errcode 0x%x\n",off,XDISKSECSIZE,pGetError());
			OutputDebugString((LPCSTR)tmpbuf);
		}
		needsize -= XDISKSECSIZE;
		readsec++;
		off += XDISKSECSIZE;
	}
	//left some tail bytes
	if(needsize >0){
		if(pReadHideSector(fd, off / XDISKSECSIZE,tmpbuf,1,FALSE,NULL))
			memcpy(buf + firstsecsize + readsec * XDISKSECSIZE,tmpbuf,needsize);
		else{
			sprintf((char *)tmpbuf,"last read failed at 0x%llx  size %d errcode 0x%x\n",off,XDISKSECSIZE,pGetError());
			OutputDebugString((LPCSTR)tmpbuf);
		}
		needsize = 0;
	}
	return size;
#endif
}

int pwrite(HANDLE fd, char * buf, size_t size, off_t off)
{
#if USEXDISK==0
	off_t ret = lseek((int)fd,  off,SEEK_SET );
	size_t writtensize = 0,readsize = 0;
	unsigned char * readbuf = NULL;
	if (ret == (off_t)-1) {
		return 0;
	}
	writtensize =  write((int)fd, buf, size);
	if(writtensize != size){
		return 0;
	}
	//readbuf = (unsigned char*)malloc(size);
	//ret = lseek((int)fd,off,SEEK_SET);
	//readsize = read((int)fd,readbuf,size);
	//if(memcmp(buf,readbuf,size)!=0){
	//	ret = -1;
	//}
	//else
	//	ret = size;
	//free(readbuf);
	return ret;
#else
	BYTE tmpbuf[XDISKSECSIZE];
	char errbuf[XDISKSECSIZE];
	int readsec = 0;
	size_t needsize = size;
	size_t firstsecsize = 0;
	sprintf((char *)errbuf,"\t\twant write at %llx size %u\n",off,size);
	OutputDebugString((LPCSTR)errbuf);
	if(off%XDISKSECSIZE!=0){ //start pos is not XDISKSECSIZE aligned
		off_t startoff = off / XDISKSECSIZE * XDISKSECSIZE;
		size_t firstsecsize = (off - startoff + needsize) > XDISKSECSIZE ? (startoff + XDISKSECSIZE - off) : needsize ;
		if(pReadHideSector(fd, startoff / XDISKSECSIZE,tmpbuf,1,FALSE,NULL)){
			memcpy(&tmpbuf[off - startoff],buf,firstsecsize);
			if(pWriteHideSector(fd,startoff / XDISKSECSIZE,tmpbuf,1,FALSE,NULL)==FALSE){
				sprintf((char *)errbuf,"first write at write failed at 0x%llx size %d errcode 0x%x\n",off,firstsecsize,pGetError());
				OutputDebugString((LPCSTR)errbuf);
				if(pWriteHideSector(fd,startoff / XDISKSECSIZE,tmpbuf,1,FALSE,NULL)==FALSE){
					sprintf((char *)errbuf,"\ttry again first write at write failed at 0x%llx  size %d errcode 0x%x\n",off,firstsecsize,pGetError());
					OutputDebugString((LPCSTR)errbuf);
				}
			}
		}
		else{
			sprintf((char *)tmpbuf,"first read at write failed at 0x%llx size %d errcode 0x%x\n",off,firstsecsize,pGetError());
			OutputDebugString((LPCSTR)tmpbuf);
		}
		needsize -= firstsecsize;
		off += firstsecsize;
	}
	//off is now XDISKSECSIZE aligned
	while(needsize>=XDISKSECSIZE){
		off_t curoff = off / XDISKSECSIZE;
		size_t cursecnum = needsize/XDISKSECSIZE;
		if(pWriteHideSector(fd,curoff,(BYTE *)buf+firstsecsize,cursecnum,FALSE,NULL)==FALSE){
			sprintf((char *)errbuf,"mid write at write failed at 0x%llx size %d errcode 0x%x\n",off,XDISKSECSIZE*cursecnum,pGetError());
			OutputDebugString((LPCSTR)errbuf);
			if(pWriteHideSector(fd,curoff,(BYTE *)buf+firstsecsize,cursecnum,FALSE,NULL)==FALSE){
				sprintf((char *)errbuf,"\ttry again mid write at write failed at 0x%llx size %d errcode 0x%x\n",off,XDISKSECSIZE*cursecnum,pGetError());
				OutputDebugString((LPCSTR)errbuf);
			}
		}
		
		needsize -= cursecnum* XDISKSECSIZE;
		readsec  += cursecnum;
		off += cursecnum*XDISKSECSIZE;
	}
	//left some tail bytes
	if(needsize >0){
		if(pReadHideSector(fd, off / XDISKSECSIZE,tmpbuf,1,FALSE,NULL)){
			memcpy(tmpbuf,buf + firstsecsize + readsec * XDISKSECSIZE,needsize);
			if(pWriteHideSector(fd,off/XDISKSECSIZE,tmpbuf,1,FALSE,NULL)==FALSE){
				sprintf((char *)errbuf,"last write at write failed at 0x%llx size %d errcode 0x%x\n",off,XDISKSECSIZE,pGetError());
				OutputDebugString((LPCSTR)errbuf);
				if(pWriteHideSector(fd,off/XDISKSECSIZE,tmpbuf,1,FALSE,NULL)==FALSE){
					sprintf((char *)errbuf,"try again last write at write failed at 0x%llx size %d errcode 0x%x\n",off,XDISKSECSIZE,pGetError());
					OutputDebugString((LPCSTR)errbuf);
				}
			}
		}
		else{
			sprintf((char *)tmpbuf,"last read at write failed at 0x%llx size %d errcode 0x%x\n",off,XDISKSECSIZE,pGetError());
			OutputDebugString((LPCSTR)tmpbuf);
		}
		needsize = 0;
	}
	return size;
#endif
}

#endif


#if !defined(WIN32)
static bool is_open(int fd)
{
	return fcntl(fd, F_GETFD) != -1;
}

#define HANDLE int
#define INVALID_HANDLE_VALUE -1
#endif

static HANDLE open_ro(const char* spec)
{
#ifdef WIN32
	OutputDebugString("open ro\n");
#if USEXDISK
	xdisk_init();
	if(glibXDisk!=NULL){
		HANDLE hDisk = pOpenXDisk((char *)spec,'E');
		return hDisk;
	}
#else
	return (HANDLE)open(spec, O_RDONLY | O_BINARY);
#endif
#else
	return open(spec, O_RDONLY);
#endif
}

static HANDLE open_rw(const char* spec)
{
#ifdef WIN32
#ifdef _DEBUG
	//OutputDebugString("open rw\n");
#endif
#if USEXDISK
	xdisk_init();
	if(glibXDisk!=NULL){
		HANDLE hDisk = pOpenXDisk((char *)spec,'E');
		return hDisk;
	}
	return INVALID_HANDLE_VALUE;//
#else
	int fd = open(spec, O_RDWR | O_BINARY );
	return (HANDLE)fd;
#endif
#else
	int fd = open(spec, O_RDWR);
#endif
#ifdef __linux__
	int ro = 0;

	/*
	   This ioctl is needed because after "blockdev --setro" kernel still
	   allows to open the device in read-write mode but fails writes.
	*/
	if (fd != -1 && ioctl(fd, BLKROGET, &ro) == 0 && ro)
	{
		close(fd);
		errno = EROFS;
		return -1;
	}
	return fd;
#endif
	
}

struct exfat_dev* exfat_open(const char* spec, enum exfat_mode mode)
{
	struct exfat_dev* dev;
	struct stat stbuf;
#ifdef USE_UBLIO
	struct ublio_param up;
#endif
#if !defined(WIN32)
	/* The system allocates file descriptors sequentially. If we have been
	   started with stdin (0), stdout (1) or stderr (2) closed, the system
	   will give us descriptor 0, 1 or 2 later when we open block device,
	   FUSE communication pipe, etc. As a result, functions using stdin,
	   stdout or stderr will actually work with a different thing and can
	   corrupt it. Protect descriptors 0, 1 and 2 from such misuse. */
	while (!is_open(STDIN_FILENO)
		|| !is_open(STDOUT_FILENO)
		|| !is_open(STDERR_FILENO))
	{
		/* we don't need those descriptors, let them leak */
		if (open("/dev/null", O_RDWR) == -1)
		{
			exfat_error("failed to open /dev/null");
			return NULL;
		}
	}
#endif
	dev = (struct exfat_dev *)malloc(sizeof(struct exfat_dev));
	if (dev == NULL)
	{
		exfat_error("failed to allocate memory for device structure");
		return NULL;
	}

	switch (mode)
	{
	case EXFAT_MODE_RO:
		dev->fd = open_ro(spec);
		if (dev->fd == INVALID_HANDLE_VALUE)
		{
			free(dev);
			exfat_error("failed to open '%s' in read-only mode: %s", spec,
					strerror(errno));
			return NULL;
		}
		dev->mode = EXFAT_MODE_RO;
		break;
	case EXFAT_MODE_RW:
		dev->fd = open_rw(spec);
		if (dev->fd == INVALID_HANDLE_VALUE)
		{
			free(dev);
			exfat_error("failed to open '%s' in read-write mode: %s", spec,
					strerror(errno));
			return NULL;
		}
		dev->mode = EXFAT_MODE_RW;
		break;
	case EXFAT_MODE_ANY:
		dev->fd = open_rw(spec);
		if (dev->fd != INVALID_HANDLE_VALUE)
		{
			dev->mode = EXFAT_MODE_RW;
			break;
		}
		dev->fd = open_ro(spec);
		if (dev->fd != INVALID_HANDLE_VALUE)
		{
			dev->mode = EXFAT_MODE_RO;
			exfat_warn("'%s' is write-protected, mounting read-only", spec);
			break;
		}
		free(dev);
		exfat_error("failed to open '%s': %s", spec, strerror(errno));
		return NULL;
	}
#if USEXDISK==0
	if (fstat(dev->fd, &stbuf) != 0)
	{
		close(dev->fd);
		free(dev);
		exfat_error("failed to fstat '%s'", spec);
		return NULL;
	}
	
	if (!S_ISBLK(stbuf.st_mode) &&
		!S_ISCHR(stbuf.st_mode) &&
		!S_ISREG(stbuf.st_mode))
	{
		close(dev->fd);
		free(dev);
		exfat_error("'%s' is neither a device, nor a regular file", spec);
		return NULL;
	}
#endif
#if defined(__APPLE__)
	if (!S_ISREG(stbuf.st_mode))
	{
		uint32_t block_size = 0;
		uint64_t blocks = 0;

		if (ioctl(dev->fd, DKIOCGETBLOCKSIZE, &block_size) != 0)
		{
			close(dev->fd);
			free(dev);
			exfat_error("failed to get block size");
			return NULL;
		}
		if (ioctl(dev->fd, DKIOCGETBLOCKCOUNT, &blocks) != 0)
		{
			close(dev->fd);
			free(dev);
			exfat_error("failed to get blocks count");
			return NULL;
		}
		dev->size = blocks * block_size;
	}
	else
#elif defined(__OpenBSD__)
	if (!S_ISREG(stbuf.st_mode))
	{
		struct disklabel lab;
		struct partition* pp;
		char* partition;

		if (ioctl(dev->fd, DIOCGDINFO, &lab) == -1)
		{
			close(dev->fd);
			free(dev);
			exfat_error("failed to get disklabel");
			return NULL;
		}

		/* Don't need to check that partition letter is valid as we won't get
		   this far otherwise. */
		partition = strchr(spec, '\0') - 1;
		pp = &(lab.d_partitions[*partition - 'a']);
		dev->size = DL_GETPSIZE(pp) * lab.d_secsize;

		if (pp->p_fstype != FS_NTFS)
			exfat_warn("partition type is not 0x07 (NTFS/exFAT); "
					"you can fix this with fdisk(8)");
	}
	else
#endif
	{
		/* works for Linux, FreeBSD, Solaris */
		//TODO
#if defined(WIN32) && !defined(WIN64)
#if 1
#if USEXDISK==0
		struct mystat {
        _dev_t     st_dev;
        _ino_t     st_ino;
        unsigned short st_mode;
        short      st_nlink;
        short      st_uid;
        short      st_gid;
        _dev_t     st_rdev;
        size_t     st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
        };
		struct _stat64 statbuf;
		//fstat(dev->fd,(struct stat *)&statbuf);
		_fstat64(dev->fd, &statbuf);
		dev->size = statbuf.st_size;
#else
		dev->size = XDISKCURTOTALSIZE;
#endif
#else
		FILE * fp = NULL;
		if(dev->mode = EXFAT_MODE_RO)
			fp = fdopen(dev->fd,"r");
		else 
			fp = fdopen(dev->fd,"r+");
		_fseeki64(fp,0,SEEK_END);
 
        dev->size = _ftelli64(fp);
		_fseeki64(fp,0,SEEK_SET);
		//fclose(fp);
#endif
		//dev->size = exfat_seek(dev, 0, SEEK_END);
#else
		//dev->size = exfat_seek(dev, 0, SEEK_END);
		
		
#ifdef WIN32
		struct _stat64 statbuf;		
		_fstat64(dev->fd, &statbuf);
#else
		struct stat statbuf;
		fstat(dev->fd,(struct stat *)&statbuf);
#endif
		dev->size = statbuf.st_size;
#endif
		if (dev->size <= 0)
		{
#if USEXDISK==0
			close(dev->fd);
#else
			xdisk_uninit();
#endif
			free(dev);
			exfat_error("failed to get size of '%s'", spec);
			return NULL;
		}
		if (exfat_seek(dev, 0, SEEK_SET) == -1)
		{
#if USEXDISK==0
			close(dev->fd);
#else
			xdisk_uninit();
#endif
			free(dev);
			exfat_error("failed to seek to the beginning of '%s'", spec);
			return NULL;
		}
	}

#ifdef USE_UBLIO
	memset(&up, 0, sizeof(struct ublio_param));
	up.up_blocksize = 256 * 1024;
	up.up_items = 64;
	up.up_grace = 32;
	up.up_priv = &dev->fd;

	dev->pos = 0;
	dev->ufh = ublio_open(&up);
	if (dev->ufh == NULL)
	{
		close(dev->fd);
		free(dev);
		exfat_error("failed to initialize ublio");
		return NULL;
	}
#endif

	return dev;
}

int exfat_close(struct exfat_dev* dev)
{
	int rc = 0;

#ifdef USE_UBLIO
	if (ublio_close(dev->ufh) != 0)
	{
		exfat_error("failed to close ublio");
		rc = -EIO;
	}
#endif
	
#if USEXDISK==0
	if (close(dev->fd) != 0)
	{
		exfat_error("failed to close device: %s", strerror(errno));
		rc = -EIO;
	}
#else
	xdisk_init();
	if(glibXDisk != NULL){
		pCloseXDisk(dev->fd);
		xdisk_uninit();
	}
#endif
	free(dev);
	return rc;
}

int exfat_fsync(struct exfat_dev* dev)
{
	int rc = 0;

#ifdef USE_UBLIO
	if (ublio_fsync(dev->ufh) != 0)
	{
		exfat_error("ublio fsync failed");
		rc = -EIO;
	}
#endif
#if USEXDISK==0
	if (fsync(dev->fd) != 0)
	{
		exfat_error("fsync failed: %s", strerror(errno));
		rc = -EIO;
	}
#endif
	return rc;
}

enum exfat_mode exfat_get_mode(const struct exfat_dev* dev)
{
	return dev->mode;
}

off_t exfat_get_size(const struct exfat_dev* dev)
{
	return dev->size;
}

off_t exfat_seek(struct exfat_dev* dev, off_t offset, int whence)
{
#ifdef USE_UBLIO
	/* XXX SEEK_CUR will be handled incorrectly */
	return dev->pos = lseek(dev->fd, offset, whence);
#else
#if USEXDISK==0	
	return lseek(dev->fd, offset, whence);
#else
	switch(whence){
	case SEEK_SET:
		dev->curpos = offset;
		break;
	case SEEK_END:
		dev->curpos = XDISKCURTOTALSIZE + offset;
		break;
	case SEEK_CUR:
		dev->curpos += offset;
		break;
	}
#endif
#endif
}

long long  exfat_read(struct exfat_dev* dev, void* buffer, size_t size)
{
#ifdef USE_UBLIO
	ssize_t result = ublio_pread(dev->ufh, buffer, size, dev->pos);
	if (result >= 0)
		dev->pos += size;
	return result;
#else
#if USEXDISK==0
	return read(dev->fd,buffer,size);
#else
	int readsize = pread(dev->fd, (char *)buffer, size,dev->curpos);
	dev->curpos += readsize;
	return readsize;
#endif
#endif
}

long long  exfat_write(struct exfat_dev* dev, const void* buffer, size_t size)
{
#ifdef USE_UBLIO
	ssize_t result = ublio_pwrite(dev->ufh, buffer, size, dev->pos);
	if (result >= 0)
		dev->pos += size;
	return result;
#else
#if USEXDISK==0
	return  write(dev->fd, buffer, size);
#else
	int writesize = pwrite(dev->fd,(char*)buffer,size,dev->curpos);
	dev->curpos += writesize;
	return writesize;
#endif
#endif
}

long long exfat_pread(struct exfat_dev* dev, void* buffer, size_t size,
		off_t offset)
{
#ifdef USE_UBLIO
	return ublio_pread(dev->ufh, buffer, size, offset);
#else
	return pread(dev->fd, (char *)buffer, size, offset);
#endif
}

long long exfat_pwrite(struct exfat_dev* dev, const void* buffer, size_t size,
		off_t offset)
{
#ifdef USE_UBLIO
	return ublio_pwrite(dev->ufh, buffer, size, offset);
#else
	return pwrite(dev->fd, (char *)buffer, size, offset);
#endif
}

long long exfat_generic_pread(const struct exfat* ef, struct exfat_node* node,
		void* buffer, size_t size, off_t offset)
{
	uint64_t newsize = offset;
	cluster_t cluster;
	char* bufp = (char *)buffer;
	off_t lsize, loffset, remainder;

	if (offset < 0)
		return -EINVAL;
	if (newsize >= node->size)
		return 0;
	if (size == 0)
		return 0;

	cluster = exfat_advance_cluster(ef, node, newsize / CLUSTER_SIZE(*ef->sb));
	if (CLUSTER_INVALID(*ef->sb, cluster))
	{
		exfat_error("invalid cluster 0x%x while reading", cluster);
		return -EIO;
	}

	loffset = newsize % CLUSTER_SIZE(*ef->sb);
	remainder = MIN(size, node->size - newsize);
	while (remainder > 0)
	{
		if (CLUSTER_INVALID(*ef->sb, cluster))
		{
			exfat_error("invalid cluster 0x%x while reading", cluster);
			return -EIO;
		}
		lsize = MIN(CLUSTER_SIZE(*ef->sb) - loffset, remainder);
		if (exfat_pread(ef->dev, bufp, lsize,
					exfat_c2o(ef, cluster) + loffset) < 0)
		{
			exfat_error("failed to read cluster %#x", cluster);
			return -EIO;
		}
		bufp += lsize;
		loffset = 0;
		remainder -= lsize;
		cluster = exfat_next_cluster(ef, node, cluster);
	}
	if (!(node->attrib & EXFAT_ATTRIB_DIR) && !ef->ro && !ef->noatime)
		exfat_update_atime(node);
	return MIN(size, node->size - newsize) - remainder;
}

long long exfat_generic_pwrite(struct exfat* ef, struct exfat_node* node,
		const void* buffer, size_t size, off_t offset)
{
	uint64_t newsize = offset;
	int rc;
	cluster_t cluster;
	const char* bufp = (const char *)buffer;
	off_t lsize, loffset, remainder;

	if (offset < 0)
		return -EINVAL;
	if (newsize > node->size)
	{
		rc = exfat_truncate(ef, node, newsize, true);
		if (rc != 0)
			return rc;
	}
	if (newsize + size > node->size)
	{
		rc = exfat_truncate(ef, node, newsize + size, false);
		if (rc != 0)
			return rc;
	}
	if (size == 0)
		return 0;

	cluster = exfat_advance_cluster(ef, node, newsize / CLUSTER_SIZE(*ef->sb));
	if (CLUSTER_INVALID(*ef->sb, cluster))
	{
		exfat_error("invalid cluster 0x%x while writing", cluster);
		return -EIO;
	}

	loffset = newsize % CLUSTER_SIZE(*ef->sb);
	remainder = size;
	while (remainder > 0)
	{
		if (CLUSTER_INVALID(*ef->sb, cluster))
		{
			exfat_error("invalid cluster 0x%x while writing", cluster);
			return -EIO;
		}
		lsize = MIN(CLUSTER_SIZE(*ef->sb) - loffset, remainder);
		if (exfat_pwrite(ef->dev, bufp, lsize,
				exfat_c2o(ef, cluster) + loffset) < 0)
		{
			exfat_error("failed to write cluster %#x", cluster);
			return -EIO;
		}
		bufp += lsize;
		loffset = 0;
		remainder -= lsize;
		cluster = exfat_next_cluster(ef, node, cluster);
	}
	if (!(node->attrib & EXFAT_ATTRIB_DIR))
		/* directory's mtime should be updated by the caller only when it
		   creates or removes something in this directory */
		exfat_update_mtime(node);
	return size - remainder;
}



