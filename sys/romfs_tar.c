#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>

#ifdef CONFIG_ROMFS_TAR_SUPPORT
#include "romfs_tar.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
#ifndef CONFIG_MAX_OPEN_FD
    #define CONFIG_MAX_OPEN_FD 10
#endif

#define MIN_FD_NUM      10
#define BLOCKSIZE       512

#define REGULAR          0
#define NORMAL          '0'
#define HARDLINK        '1'
#define SYMLINK         '2'
#define CHAR            '3'
#define BLOCK           '4'
#define DIRECTORY       '5'
#define FIFO            '6'
#define CONTIGUOUS      '7'

//-----------------------------------------------------------------
// Types: 
//-----------------------------------------------------------------

// TAR header structures
struct tar_header
{
    union {
        union {
            struct {
              char name[100];
              char mode[8];
              char uid[8];
              char gid[8];
              char size[12];
              char mtime[12];
              char chksum[8];
              char typeflag;
              char linkname[100];
              char magic[6];
              char version[2];
              char uname[32];
              char gname[32];
              char devmajor[8];
              char devminor[8];
              char prefix[155];
            };
        };

        char block[BLOCKSIZE];
    };
};

// Structure describing an open file
struct file_desc
{
    int         id;
    int         in_use;
    uint32_t    offset;
    uint32_t    length;
    void *      object;
};

//-----------------------------------------------------------------
// Locals: 
//-----------------------------------------------------------------
static struct file_desc m_files[CONFIG_MAX_OPEN_FD];
static uint8_t *        m_tar;
static uint32_t         m_tar_size;
static int              m_next_fd;

//-----------------------------------------------------------------
// romfs_alloc_fd: Allocate file_desc entry
//-----------------------------------------------------------------
static struct file_desc* romfs_alloc_fd(void)
{
    int i;

    for (i=0;i<CONFIG_MAX_OPEN_FD;i++)
        if (!m_files[i].in_use)
        {
            m_files[i].in_use = 1;
            break;
        }

    if (i < CONFIG_MAX_OPEN_FD)
        return &m_files[i];
    else
        return NULL;
}
//-----------------------------------------------------------------
// romfs_free_fd: Free file_desc entry
//-----------------------------------------------------------------
static void romfs_free_fd(struct file_desc* f)
{
    f->in_use = 0;
}
//-----------------------------------------------------------------
// romfs_lookup_fd: Lookup file structure from FD
//-----------------------------------------------------------------
static struct file_desc* romfs_lookup_fd(int fd)
{
    if (fd >= MIN_FD_NUM)
        return &m_files[fd - MIN_FD_NUM];

    return NULL;
}
//-----------------------------------------------------------------
// romfs_oct2uint: String to uint
//-----------------------------------------------------------------
static uint32_t romfs_oct2uint(char * s, uint32_t size)
{
    int      i   = 0;
    uint32_t out = 0;
    while ((i < size) && s[i])
        out = (out << 3) | (uint32_t) (s[i++] - '0');
    
    return out;
}
//-----------------------------------------------------------------
// romfs_tar_init: Register a TAR file as a ROM filesystem
//-----------------------------------------------------------------
void romfs_tar_init(uint8_t *tar_file, uint32_t size)
{
    printf("TAR: %p size %d\n", tar_file, size);
    m_tar      = tar_file;
    m_tar_size = size;

    int i;

    for (i=0;i<CONFIG_MAX_OPEN_FD;i++)
        m_files[i].in_use = 0;

    m_next_fd    = MIN_FD_NUM;    
}
//-----------------------------------------------------------------
// romfs_tar_open: open() for TAR filesystem
//-----------------------------------------------------------------
int romfs_tar_open(const char *name, int flags, int mode)
{
    const char *fname = name;

    if (!name || !m_tar || m_tar_size == 0)
        return -1;

    // Skip initial prefix to path
    if (!strncmp(name, "./", 2))
        fname += 2;
    else if (!strncmp(name, "/", 1))
        fname += 1;

    char *long_name = NULL;
    int   long_name_len = 0;

    uint32_t offset = 0;
    while (offset < m_tar_size)
    {
        int remain = m_tar_size - offset;
        if (remain <= sizeof(struct tar_header))
            break;

        struct tar_header * hdr_block = (struct tar_header *)&m_tar[offset];
        offset += sizeof(struct tar_header);

        uint32_t begin = offset;
        uint32_t size  = romfs_oct2uint(hdr_block->size, sizeof(hdr_block->size));

        char * tar_fname     = long_name ? long_name     : hdr_block->name;
        int    tar_fname_len = long_name ? long_name_len : sizeof(hdr_block->name);

        long_name = NULL;

        // Skip initial prefix to path
        if (!strncmp(tar_fname, "./", 2))
        {
            tar_fname     += 2;
            tar_fname_len -= 2;
        }
        else if (!strncmp(tar_fname, "/", 1))
        {
            tar_fname     += 1;
            tar_fname_len -= 1;
        }

        // File and path matches
        if (((hdr_block->typeflag == REGULAR) || (hdr_block->typeflag == NORMAL) || (hdr_block->typeflag == CONTIGUOUS)) && size != 0)
            if (!strncmp(tar_fname, fname, tar_fname_len))
            {
                struct file_desc* f = romfs_alloc_fd();
                if (!f)
                    return -1;

                f->object = &m_tar[begin];
                f->offset = 0;
                f->length = size;
                f->id     = m_next_fd++;
                return f->id;
            }

        // Long filename entry - assumed to be associated with next entry...
        if (hdr_block->typeflag == 'L')
        {
            long_name     = &m_tar[begin];
            long_name_len = size;
        }

        offset += (((size + BLOCKSIZE-1) / BLOCKSIZE) * BLOCKSIZE);
    }

    return -1;
}
//-----------------------------------------------------------------
// romfs_tar_read: read() for TAR filesystem
//-----------------------------------------------------------------
ssize_t romfs_tar_read(int file, void *ptr, size_t len)
{
    struct file_desc* f = romfs_lookup_fd(file);
    int count       = -1;

    if (f)
    {
        if ((f->offset + len) > f->length)
            count = f->length - f->offset;
        else
            count = len;

        if (count)
        {
            memcpy(ptr, ((uint8_t*)f->object) + f->offset, count);
            f->offset += count;
        }
    }

    return count;
}
//-----------------------------------------------------------------
// romfs_tar_lseek: lseek() for TAR filesystem
//-----------------------------------------------------------------
int romfs_tar_lseek(int fd, int offset, int whence)
{
    struct file_desc* file = romfs_lookup_fd(fd);
    int res = -1;

    if (file)
    {
        if (whence == SEEK_SET)
        {
            file->offset = (uint32_t)offset;

            if (file->offset > file->length)
                file->offset = file->length;

            res = file->offset;
        }
        else if (whence == SEEK_CUR)
        {
            // Positive shift
            if (offset >= 0)
            {
                file->offset += offset;

                if (file->offset > file->length)
                    file->offset = file->length;
            }
            // Negative shift
            else
            {
                // Make shift positive
                offset = -offset;

                // Limit to negative shift to start of file
                if ((uint32_t)offset > file->offset)
                    file->offset = 0;
                else
                    file->offset-= offset;
            }

            res = file->offset;
        }
        else if (whence == SEEK_END)
        {
            file->offset = file->length;
            res = file->offset;
        }
        else
            res = -1;
    }

    return res;
}
//-----------------------------------------------------------------
// romfs_tar_close: close() for TAR filesystem
//-----------------------------------------------------------------
int romfs_tar_close(int fd)
{
    struct file_desc* f = romfs_lookup_fd(fd);
    if (f)
    {
        romfs_free_fd(f);
        return 0;
    }
    else
        return -1;
}

//-----------------------------------------------------------------
// System call overloads:
//-----------------------------------------------------------------
#ifdef CONFIG_ROMFS_TAR_SYSCALLS
int sys_open(const char *name, int flags, int mode)
{
    return romfs_tar_open(name, flags, mode);
}
ssize_t sys_read(int file, void *ptr, size_t len)
{
    return romfs_tar_read(file, ptr, len);
}
off_t sys_lseek(int file, off_t ptr, int dir)
{
    return romfs_tar_lseek(file, ptr, dir);
}
int sys_close(int file)
{
    return romfs_tar_close(file);
}
#endif
#endif
