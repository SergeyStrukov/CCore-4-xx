/* PriorityBoost.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN32
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/PriorityBoost.h>

#include <CCore/inc/Print.h>

#include <CCore/inc/sys/SysTypes.h>

#include <CCore/inc/win32/Win32.h>

namespace App {

/* using */

using namespace CCore;
using namespace CCore::Sys;

/* PriorityBoost() */

void PriorityBoost()
 {
  if( !WinNN::SetThreadPriority(WinNN::GetCurrentThread(),WinNN::ThreadPriorityTimeCritical) )
    {
     Printf(Con,"No priority boost!\n");
    }
 }

} // namespace App

