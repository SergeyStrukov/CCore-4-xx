# IntelRandom.s
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

        .global __ZN5CCore7Crypton11IntelRandom4nextEv

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # uint32 CCore::Crypton::IntelRandom::next()

__ZN5CCore7Crypton11IntelRandom4nextEv:

1:
        rdrand  %eax
        jnc     1b

        ret

#-----------------------------------------------------------------------------------------
