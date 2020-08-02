#ifndef __ROMFS_TAR_H__
#define __ROMFS_TAR_H__

#include <stdint.h>

//-----------------------------------------------------------------
// Prototypes:
//-----------------------------------------------------------------
void    romfs_tar_init(uint8_t *tar_file, uint32_t size);
int     romfs_tar_open(const char *name, int flags, int mode);
ssize_t romfs_tar_read(int file, void *ptr, size_t len);
int     romfs_tar_lseek(int fd, int offset, int whence);
int     romfs_tar_close(int fd);

#endif

