
#=========================================================================
# crt0.S : Entry point for RISC-V user programs
#=========================================================================
  .text
  .global _start
  .type   _start, @function
_start:
  # Initialize global pointer
.option push
.option norelax
1:auipc gp, %pcrel_hi(__global_pointer$)
  addi  gp, gp, %pcrel_lo(1b)
.option pop

  # Setup stack pointer
  lui sp, %hi(_sp_top)
  add sp, sp, %lo(_sp_top)

  # Setup IRQ vector
  lui t0, %hi(isr_vector)
  add t0, t0, %lo(isr_vector)    
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
  csrw stvec, t0
#else
  csrw mtvec, t0
#endif

  # Clear the bss segment
  la      a0, _edata
  la      a2, _end
  sub     a2, a2, a0
  li      a1, 0
#ifndef CONFIG_ARCH_RISCV_SKIP_BSS_INIT
  call    memset
#endif

  # Jump to init
  jal init

  la      a0, __libc_fini_array   # Register global termination functions
  call    atexit                  #  to be called upon exit
  call    __libc_init_array       # Run global initialization functions

  lw      a0, 0(sp)                  # a0 = argc
  addi    a1, sp, __SIZEOF_POINTER__ # a1 = argv
  li      a2, 0                      # a2 = envp = NULL
  call    main
  tail    exit
  .size  _start, .-_start

  .global _init
  .type   _init, @function
  .global _fini
  .type   _fini, @function
_init:
_fini:
  # These don't have to do anything since we use init_array/fini_array.
  ret
  .size  _init, .-_init
  .size  _fini, .-_fini

#####################################################
# Actual ISR vector
#####################################################
int_vector:
#include "exception_asm.h"

.section .data
.global _sp_base
.global _sp_top

#ifndef NEWLIB_STACK_SIZE
#define NEWLIB_STACK_SIZE 4096
#endif

_sp_base:
    .align 4
    .space  NEWLIB_STACK_SIZE,0
_sp_top:

#####################################################
# Exit
#####################################################
.global _exit
_exit:
    andi a0, a0, 0xFF
    csrw dscratch, a0
_exit_loop:
    jal _exit_loop

#####################################################
# Embedded TAR filesystem
#####################################################
#ifdef CONFIG_ROMFS_TAR_SUPPORT
.global _tarfs_start, _tarfs_end
_tarfs_start:
.incbin CONFIG_ROMFS_TAR_BIN
_tarfs_end:
#endif
