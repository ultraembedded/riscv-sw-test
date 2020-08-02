#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <machine/syscall.h>

#include "syscalls.h"
#include "assert.h"
#include "console.h"

//-------------------------------------------------------------
// External
//-------------------------------------------------------------
extern int     sys_open(const char *name, int flags, int mode);
extern ssize_t sys_read(int file, void *ptr, size_t len);
extern ssize_t sys_write(int file, const void *ptr, size_t len);
extern int     sys_close(int file);
extern off_t   sys_lseek(int file, off_t ptr, int dir);
extern int     sys_gettimeofday(struct timeval *tv, struct timezone *tz);

//-----------------------------------------------------------------
// syscall_handler:
//-----------------------------------------------------------------
struct irq_context *syscall_handler(struct irq_context *ctx)
{
    uint64_t a0 = ctx->reg[REG_ARG0 + 0];
    uint64_t a1 = ctx->reg[REG_ARG0 + 1];
    uint64_t a2 = ctx->reg[REG_ARG0 + 2];
    uint64_t a3 = ctx->reg[REG_ARG0 + 3];
    uint64_t a4 = ctx->reg[REG_ARG0 + 4];
    uint64_t a5 = ctx->reg[REG_ARG0 + 5];
    uint64_t n  = ctx->reg[REG_ARG0 + 7];

    int      fd = (int)a0;

    // putchar()
    if (n == SYS_write && (fd == STDOUT_FILENO || fd == STDERR_FILENO))
    {
        uint8_t *p   = (uint8_t *)a1;
        int      len = (int)a2;

        for (int i=0;i<len;i++)
            console_putchar(p[i]);

        ctx->reg[REG_RET] = len;
    }
    // getchar()
    else if (n == SYS_read && fd == STDIN_FILENO)
    {
        uint8_t *p   = (uint8_t *)a1;
        int      len = (int)a2;
        int      ch  = -1;

        if (len && ((ch = console_getchar()) != -1))
        {
            *p = ch;
            ctx->reg[REG_RET] = 1;
        }
        else
            ctx->reg[REG_RET] = -1;
    }
    // Not handled
    else if (n == SYS_fstat && (fd == STDOUT_FILENO || fd == STDERR_FILENO || fd == STDIN_FILENO))
    {
        ctx->reg[REG_RET] = -1;
    }
    else if (n == SYS_close && (fd == STDOUT_FILENO || fd == STDERR_FILENO || fd == STDIN_FILENO))
    {
        ctx->reg[REG_RET] = -1;
    }    
    else if (n == SYS_exit)
    {
        _exit(a0);
    }
    // System I/O
    else if (n == SYS_read)
    {
        ctx->reg[REG_RET] = sys_read(a0, (void*)a1, a2);
    }
    else if (n == SYS_write)
    {
        ctx->reg[REG_RET] = sys_write(a0, (void*)a1, a2);
    }
    else if (n == SYS_open)
    {
        ctx->reg[REG_RET] = sys_open((const char *)a0, a1, a2);
    }
    // else if (n == SYS_openat)
    else if (n == SYS_close)
    {
        ctx->reg[REG_RET] = sys_close(a0);
    }
    else if (n == SYS_lseek)
    {
        ctx->reg[REG_RET] = sys_lseek(a0, a1, a2);
    }
    else if (n == SYS_link)
    {

    }
    else if (n == SYS_unlink)
    {

    }
    else if (n == SYS_mkdir)
    {

    }
    else if (n == SYS_chdir)
    {

    }
    else if (n == SYS_getcwd)
    {

    }
    else if (n == SYS_stat)
    {

    }
    else if (n == SYS_fstat)
    {

    }
    else if (n == SYS_lstat)
    {

    }
    else if (n == SYS_fstatat)
    {

    }
    else if (n == SYS_access)
    {

    }
    else if (n == SYS_faccessat)
    {

    }
    //else if (n == SYS_pread)
    //else if (n == SYS_pwrite)
    //else if (n == SYS_uname)
    //else if (n == SYS_getuid)
    //else if (n == SYS_geteuid)
    //else if (n == SYS_getgid)
    //else if (n == SYS_getegid)
    //else if (n == SYS_mmap)
    //else if (n == SYS_munmap)
    //else if (n == SYS_mremap)
    //else if (n == SYS_getmainvars)
    //else if (n == SYS_rt_sigaction)
    //else if (n == SYS_writev)    
    else if (n == SYS_time)
    {
        ctx->reg[REG_RET] = -1;
    }
    else if (n == SYS_gettimeofday)
    {
        ctx->reg[REG_RET] = sys_gettimeofday((struct timeval *)a0, (struct timezone *)a1);
    }
    else if (n == SYS_times)
    {
        ctx->reg[REG_RET] = -1;
    }
    else if (n == SYS_fcntl)
    {

    }
    else if (n == SYS_getdents)
    {

    }
    else if (n == SYS_dup)
    {

    }
    else
    {
        console_putstr("Unhandled system call...\n");
        assert(!"Bad system call ID");
    }

    return ctx;
}
//-----------------------------------------------------------------
// syscall_init:
//-----------------------------------------------------------------
void syscall_init(void)
{
    exception_set_syscall_handler(syscall_handler);
}
