
#ifndef _EXTRA_FS_DEFS
#define _EXTRA_FS_DEFS

#ifndef _int64
#define _int64 __int64
typedef signed _int64       int64_t;
typedef int64_t         RTFOFF;
typedef unsigned int        uint32_t;

typedef uint32_t        RTFMODE;

typedef struct EFTIMESPEC
{
    /** Nanoseconds since epoch.
     * The name is intentially too long to be comfortable to use because you should be
     * using inline helpers! */
    int64_t     i64NanosecondsRelativeToUnixEpoch;
} EFTIMESPEC;
#endif
#if 0
/**
 * Filesystem object attributes.
 */
typedef struct RTFSOBJATTR
{
    /** Mode flags (st_mode). RTFS_UNIX_*, RTFS_TYPE_*, and RTFS_DOS_*. */
    RTFMODE         fMode;

    /** The additional attributes available. */
    RTFSOBJATTRADD  enmAdditional;

    /**
     * Additional attributes.
     *
     * Unless explicitly specified to an API, the API can provide additional
     * data as it is provided by the underlying OS.
     */
    union RTFSOBJATTRUNION
    {
        /** Additional Unix Attributes - RTFSOBJATTRADD_UNIX. */
        RTFSOBJATTRUNIX         Unix;
        /** Additional Unix Owner Attributes - RTFSOBJATTRADD_UNIX_OWNER. */
        RTFSOBJATTRUNIXOWNER    UnixOwner;
        /** Additional Unix Group Attributes - RTFSOBJATTRADD_UNIX_GROUP. */
        RTFSOBJATTRUNIXGROUP    UnixGroup;

        /**
         * Extended attribute size is available when RTFS_DOS_HAVE_EA_SIZE is set.
         */
        struct RTFSOBJATTREASIZE
        {
            /** Size of EAs. */
            RTFOFF          cb;
        } EASize;
        /** Reserved space. */
        uint8_t         abReserveSpace[128];
    } u;
} RTFSOBJATTR;
/** Pointer to a filesystem object attributes structure. */
typedef RTFSOBJATTR *PRTFSOBJATTR;
/** Pointer to a const filesystem object attributes structure. */
typedef const RTFSOBJATTR *PCRTFSOBJATTR;
#endif

/**
 * Filesystem object information structure.
 *
 * This is returned by the RTPathQueryInfo(), RTFileQueryInfo() and RTDirRead() APIs.
 */
typedef struct EFFSOBJINFO
{
   /** Logical size (st_size).
    * For normal files this is the size of the file.
    * For symbolic links, this is the length of the path name contained
    * in the symbolic link.
    * For other objects this fields needs to be specified.
    */
   RTFOFF       cbObject;

   /** Disk allocation size (st_blocks * DEV_BSIZE). */
   RTFOFF       cbAllocated;

   /** Time of last access (st_atime). */
   EFTIMESPEC   AccessTime;

   /** Time of last data modification (st_mtime). */
   EFTIMESPEC   ModificationTime;

   /** Time of last status change (st_ctime).
    * If not available this is set to ModificationTime.
    */
   EFTIMESPEC   ChangeTime;

   /** Time of file birth (st_birthtime).
    * If not available this is set to ChangeTime.
    */
   EFTIMESPEC   BirthTime;

   /** Attributes. */
   //RTFSOBJATTR  Attr;

} EFFSOBJINFO;
/** Pointer to a filesystem object information structure. */
typedef EFFSOBJINFO *PEFFSOBJINFO;
/** Pointer to a const filesystem object information structure. */
typedef const EFFSOBJINFO *PCEFFSOBJINFO;

#define EFFILE_SEEK_BEGIN     0x00
/** Seek from the current file position. */
#define EFFILE_SEEK_CURRENT   0x01
/** Seek from the end of the file. */
#define EFFILE_SEEK_END       0x02
/** @internal */
#define EFFILE_SEEK_FIRST     EFFILE_SEEK_BEGIN
/** @internal */
#define EFFILE_SEEK_LAST      EFFILE_SEEK_END

#endif