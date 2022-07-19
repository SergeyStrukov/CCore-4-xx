# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: CCore 4.01
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003 
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2022 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

OPT = -j8

LIST = ExecTest Keywords MakeList MakeString Volume EllipticPi Spawnee \
       EchoServer PSecEchoServer PTPEchoServer PTPServerProc PTPServer PTPSecureServerProc PTPSecureServer PTPSecureClient \
       DDLShow DDLTypeSet \

       
#       CondLangLR1/Main CondLangLR1/Gen \
#       Regen \
#       vmake VMakeDep

include ../Target/Makefile.list

PTPServer : PTPServerProc

PTPSecureServer : PTPSecureServerProc
