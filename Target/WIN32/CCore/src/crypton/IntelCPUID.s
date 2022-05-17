# IntelCPUID.s
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

#-----------------------------------------------------------------------------------------

        .global __ZN5CCore7Crypton11IntelCPUID1C1Ev

        .p2align 4,,15

  # CCore::Crypton::IntelCPUID1::IntelCPUID1() noexcept

__ZN5CCore7Crypton11IntelCPUID1C1Ev:

        pushl   %ebp
        movl    %esp, %ebp

        pushl   %ebx
        pushl   %edi

        movl    8(%ebp), %edi  # this

        movl    $1, %eax
        cpuid

        movl    %eax,   (%edi)
        movl    %ebx,  4(%edi)
        movl    %ecx,  8(%edi)
        movl    %edx, 12(%edi)

        popl    %edi
        popl    %ebx

        popl    %ebp
        ret

#-----------------------------------------------------------------------------------------

