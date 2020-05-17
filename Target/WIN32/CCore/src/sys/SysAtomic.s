# SysAtomic.s
#-----------------------------------------------------------------------------------------
#
#  Project: CCore 4.01
#
#  Tag: Target/WIN32
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2020 Sergey Strukov. All rights reserved.
#
#-----------------------------------------------------------------------------------------

        .text

        .global __ZN5CCore3Sys6Atomic3SetEPVjj
        .global __ZN5CCore3Sys6Atomic3AddEPVjj
        .global __ZN5CCore3Sys6Atomic6TrySetEPVjjj

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Sys::Atomic::Set

__ZN5CCore3Sys6Atomic3SetEPVjj:

        pushl   %ebp
        movl    %esp, %ebp

        movl     8(%ebp), %ecx
        movl    12(%ebp), %eax

        xchgl   %eax, (%ecx)

        popl    %ebp
        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Sys::Atomic::Add

__ZN5CCore3Sys6Atomic3AddEPVjj:

        pushl   %ebp
        movl    %esp, %ebp

        movl     8(%ebp), %ecx
        movl    12(%ebp), %eax

   lock xaddl   %eax, (%ecx)

        popl    %ebp
        ret

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::Sys::Atomic::TrySet

__ZN5CCore3Sys6Atomic6TrySetEPVjjj:

        pushl   %ebp
        movl    %esp, %ebp

        movl     8(%ebp), %ecx
        movl    12(%ebp), %eax
        movl    16(%ebp), %edx

   lock cmpxchgl   %edx, (%ecx)

        popl    %ebp
        ret

#-----------------------------------------------------------------------------------------



