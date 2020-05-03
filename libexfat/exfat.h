/*
	exfat.h (29.08.09)
	Definitions of structures and constants used in exFAT file system
	implementation.

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

#ifndef EXFAT_H_INCLUDED
#define EXFAT_H_INCLUDED

#ifdef DLL_EXPORTS

#define EXFAT_EXPORT	__declspec(dllexport)

#else
#define EXFAT_EXPORT __declspec(dllimport)
#endif

#ifndef ANDROID
/* Android.bp is used instead of autotools when targeting Android */
#if  !defined(WIN32) && !defined(__WIN__)
#include "config.h"
#endif
#endif
#include "compiler.h"
#include "exfatfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if defined(WIN32) || defined(__WIN__)
#if _MSC_VER < 1900
#include "../win/libexfat/stdbool.h"
#ifndef _CRT_NO_TIME_T
struct timespec
{
	time_t tv_sec;
	long tv_nsec;
};
#endif
#else
#include <stdbool.h>
#endif
#else
#include <stdbool.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXFAT_NAME_MAX 255
/* UTF-16 encodes code points up to U+FFFF as single 16-bit code units.
   UTF-8 uses up to 3 bytes (i.e. 8-bit code units) to encode code points
   up to U+FFFF. One additional character is for null terminator. */
#define EXFAT_UTF8_NAME_BUFFER_MAX (EXFAT_NAME_MAX * 3 + 1)
#define EXFAT_UTF8_ENAME_BUFFER_MAX (EXFAT_ENAME_MAX * 3 + 1)

#define SECTOR_SIZE(sb) (1 << (sb).sector_bits)
#define CLUSTER_SIZE(sb) (SECTOR_SIZE(sb) << (sb).spc_bits)
#define CLUSTER_INVALID(sb, c) ((c) < EXFAT_FIRST_DATA_CLUSTER || \
	(c) - EXFAT_FIRST_DATA_CLUSTER >= le32_to_cpu((sb).cluster_count))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define DIV_ROUND_UP(x, d) (((x) + (d) - 1) / (d))
#define ROUND_UP(x, d) (DIV_ROUND_UP(x, d) * (d))

#define BMAP_SIZE(count) (ROUND_UP(count, sizeof(bitmap_t) * 8) / 8)
#define BMAP_BLOCK(index) ((index) / sizeof(bitmap_t) / 8)
#define BMAP_MASK(index) ((bitmap_t) 1 << ((index) % (sizeof(bitmap_t) * 8)))
#define BMAP_GET(bitmap, index) \
	((bitmap)[BMAP_BLOCK(index)] & BMAP_MASK(index))
#define BMAP_SET(bitmap, index) \
	((bitmap)[BMAP_BLOCK(index)] |= BMAP_MASK(index))
#define BMAP_CLR(bitmap, index) \
	((bitmap)[BMAP_BLOCK(index)] &= ~BMAP_MASK(index))

#define EXFAT_REPAIR(hook, ef, ...) \
	(exfat_ask_to_fix(ef) && exfat_fix_ ## hook(ef, __VA_ARGS__))

/* The size of off_t type must be 64 bits. File systems larger than 2 GB will
   be corrupted with 32-bit off_t. */
STATIC_ASSERT(sizeof(off_t) == 8);

struct exfat_node
{
	struct exfat_node* parent;
	struct exfat_node* child;
	struct exfat_node* next;
	struct exfat_node* prev;

	int references;
	uint32_t fptr_index;
	cluster_t fptr_cluster;
	off_t entry_offset;
	cluster_t start_cluster;
	uint16_t attrib;
	uint8_t continuations;
	int is_contiguous : 1;
	int is_cached : 1;
	int is_dirty : 1;
	int is_unlinked : 1;
	uint64_t size;
	time_t mtime, atime;
	le16_t name[EXFAT_NAME_MAX + 1];
};

enum exfat_mode
{
	EXFAT_MODE_RO,
	EXFAT_MODE_RW,
	EXFAT_MODE_ANY,
};

struct exfat_dev;

struct exfat
{
	struct exfat_dev* dev;
	struct exfat_super_block* sb;
	uint16_t* upcase;
	struct exfat_node* root;
	struct
	{
		cluster_t start_cluster;
		uint32_t size;				/* in bits */
		bitmap_t* chunk;
		uint32_t chunk_size;		/* in bits */
		int dirty;
	}
	cmap;
	char label[EXFAT_UTF8_ENAME_BUFFER_MAX];
	void* zero_cluster;
	int dmask, fmask;
#ifndef WIN32
	uid_t uid;
	gid_t gid;
#endif
	int ro;
	int noatime;
	enum { EXFAT_REPAIR_NO, EXFAT_REPAIR_ASK, EXFAT_REPAIR_YES } repair;
};

/* in-core nodes iterator */
struct exfat_iterator
{
	struct exfat_node* parent;
	struct exfat_node* current;
};

struct exfat_human_bytes
{
	uint64_t value;
	const char* unit;
};

extern int exfat_errors;
extern int exfat_errors_fixed;

EXFAT_EXPORT void exfat_bug(const char* format, ...) PRINTF NORETURN;
EXFAT_EXPORT void exfat_error(const char* format, ...) PRINTF;
EXFAT_EXPORT void exfat_warn(const char* format, ...) PRINTF;
EXFAT_EXPORT void exfat_debug(const char* format, ...) PRINTF;

EXFAT_EXPORT struct exfat_dev* exfat_open(const char* spec, enum exfat_mode mode);
EXFAT_EXPORT int exfat_close(struct exfat_dev* dev);
EXFAT_EXPORT int exfat_fsync(struct exfat_dev* dev);
EXFAT_EXPORT enum exfat_mode exfat_get_mode(const struct exfat_dev* dev);
EXFAT_EXPORT off_t exfat_get_size(const struct exfat_dev* dev);
EXFAT_EXPORT off_t exfat_seek(struct exfat_dev* dev, off_t offset, int whence);
EXFAT_EXPORT long long exfat_read(struct exfat_dev* dev, void* buffer, size_t size);
EXFAT_EXPORT long long exfat_write(struct exfat_dev* dev, const void* buffer, size_t size);
EXFAT_EXPORT long long exfat_pread(struct exfat_dev* dev, void* buffer, size_t size,
		off_t offset);
EXFAT_EXPORT long long exfat_pwrite(struct exfat_dev* dev, const void* buffer, size_t size,
		off_t offset);
EXFAT_EXPORT long long exfat_generic_pread(const struct exfat* ef, struct exfat_node* node,
		void* buffer, size_t size, off_t offset);
EXFAT_EXPORT long long exfat_generic_pwrite(struct exfat* ef, struct exfat_node* node,
		const void* buffer, size_t size, off_t offset);

EXFAT_EXPORT int exfat_opendir(struct exfat* ef, struct exfat_node* dir,
		struct exfat_iterator* it);
EXFAT_EXPORT void exfat_closedir(struct exfat* ef, struct exfat_iterator* it);
EXFAT_EXPORT struct exfat_node* exfat_readdir(struct exfat_iterator* it);
EXFAT_EXPORT int exfat_lookup(struct exfat* ef, struct exfat_node** node,
		const char* path);
EXFAT_EXPORT int exfat_split(struct exfat* ef, struct exfat_node** parent,
		struct exfat_node** node, le16_t* name, const char* path);

EXFAT_EXPORT off_t exfat_c2o(const struct exfat* ef, cluster_t cluster);
EXFAT_EXPORT cluster_t exfat_next_cluster(const struct exfat* ef,
		const struct exfat_node* node, cluster_t cluster);
cluster_t exfat_advance_cluster(const struct exfat* ef,
		struct exfat_node* node, uint32_t count);
EXFAT_EXPORT int exfat_flush_nodes(struct exfat* ef);
EXFAT_EXPORT int exfat_flush(struct exfat* ef);
EXFAT_EXPORT int exfat_truncate(struct exfat* ef, struct exfat_node* node, uint64_t size,
		int erase);
EXFAT_EXPORT uint32_t exfat_count_free_clusters(const struct exfat* ef);
EXFAT_EXPORT int exfat_find_used_sectors(const struct exfat* ef, off_t* a, off_t* b);

EXFAT_EXPORT void exfat_stat(const struct exfat* ef, const struct exfat_node* node,
		struct stat* stbuf);
EXFAT_EXPORT void exfat_get_name(const struct exfat_node* node,
		char buffer[EXFAT_UTF8_NAME_BUFFER_MAX]);
EXFAT_EXPORT uint16_t exfat_start_checksum(const struct exfat_entry_meta1* entry);
EXFAT_EXPORT uint16_t exfat_add_checksum(const void* entry, uint16_t sum);
EXFAT_EXPORT le16_t exfat_calc_checksum(const struct exfat_entry* entries, int n);
EXFAT_EXPORT uint32_t exfat_vbr_start_checksum(const void* sector, size_t size);
EXFAT_EXPORT uint32_t exfat_vbr_add_checksum(const void* sector, size_t size, uint32_t sum);
EXFAT_EXPORT le16_t exfat_calc_name_hash(const struct exfat* ef, const le16_t* name,
		size_t length);
EXFAT_EXPORT void exfat_humanize_bytes(uint64_t value, struct exfat_human_bytes* hb);
EXFAT_EXPORT void exfat_print_info(const struct exfat_super_block* sb,
		uint32_t free_clusters);

EXFAT_EXPORT int exfat_utf16_to_utf8(char* output, const le16_t* input, size_t outsize,
		size_t insize);
EXFAT_EXPORT int exfat_utf8_to_utf16(le16_t* output, const char* input, size_t outsize,
		size_t insize);
EXFAT_EXPORT size_t exfat_utf16_length(const le16_t* str);

EXFAT_EXPORT struct exfat_node* exfat_get_node(struct exfat_node* node);
EXFAT_EXPORT void exfat_put_node(struct exfat* ef, struct exfat_node* node);
EXFAT_EXPORT int exfat_cleanup_node(struct exfat* ef, struct exfat_node* node);
EXFAT_EXPORT int exfat_cache_directory(struct exfat* ef, struct exfat_node* dir);
EXFAT_EXPORT void exfat_reset_cache(struct exfat* ef);
EXFAT_EXPORT int exfat_flush_node(struct exfat* ef, struct exfat_node* node);
EXFAT_EXPORT int exfat_unlink(struct exfat* ef, struct exfat_node* node);
EXFAT_EXPORT int exfat_rmdir(struct exfat* ef, struct exfat_node* node);
EXFAT_EXPORT int exfat_mknod(struct exfat* ef, const char* path);
EXFAT_EXPORT int exfat_mkdir(struct exfat* ef, const char* path);
EXFAT_EXPORT int exfat_rename(struct exfat* ef, const char* old_path, const char* new_path);
EXFAT_EXPORT void exfat_utimes(struct exfat_node* node, const struct timespec *tv);
EXFAT_EXPORT void exfat_update_atime(struct exfat_node* node);
EXFAT_EXPORT void exfat_update_mtime(struct exfat_node* node);
EXFAT_EXPORT const char* exfat_get_label(struct exfat* ef);
EXFAT_EXPORT int exfat_set_label(struct exfat* ef, const char* label);

EXFAT_EXPORT int exfat_soil_super_block(const struct exfat* ef);
EXFAT_EXPORT int exfat_mount(struct exfat* ef, const char* spec, const char* options);
EXFAT_EXPORT void exfat_unmount(struct exfat* ef);

EXFAT_EXPORT time_t exfat_exfat2unix(le16_t date, le16_t time, uint8_t centisec,
		uint8_t tzoffset);
EXFAT_EXPORT void exfat_unix2exfat(time_t unix_time, le16_t* date, le16_t* time,
		uint8_t* centisec, uint8_t* tzoffset);
EXFAT_EXPORT void exfat_tzset(void);

EXFAT_EXPORT int exfat_ask_to_fix(const struct exfat* ef);
EXFAT_EXPORT int exfat_fix_invalid_vbr_checksum(const struct exfat* ef, void* sector,
		uint32_t vbr_checksum);
EXFAT_EXPORT int exfat_fix_invalid_node_checksum(const struct exfat* ef,
		struct exfat_node* node);
EXFAT_EXPORT int exfat_fix_unknown_entry(struct exfat* ef, struct exfat_node* dir,
		const struct exfat_entry* entry, off_t offset);
#ifdef __cplusplus
}
#endif
#endif /* ifndef EXFAT_H_INCLUDED */
