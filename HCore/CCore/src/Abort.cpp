/* Abort.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Abort.h>

#include <CCore/inc/Printf.h>

#include <CCore/inc/sys/SysCon.h>
#include <CCore/inc/sys/SysAbort.h>

namespace CCore {

/* functions */

void Abort(StrLen str)
 {
  if( AbortMsgFunc )
    {
     AbortMsgFunc(str);
    }
  else
    {
     char buf[TextBufLen];
     PrintBuf out(Range(buf));

     Printf(out,"\n#;\n\n",str);

     Sys::ConWrite(out.close());
    }

  Sys::AbortProcess();
 }

void Abort(const char *zstr)
 {
  StrLen str(zstr);

  Abort(str);
 }

} // namespace CCore


