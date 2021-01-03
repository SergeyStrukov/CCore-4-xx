/* TypeName.cpp */
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
//  Copyright (c) 2021 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include "TypeName.h"

#include <cxxabi.h>
#include <stdlib.h>

namespace App {

/* Demangle() */

String Demangle(const char *name)
 {
  int status;

  char *result=__cxxabiv1::__cxa_demangle(name,0,0,&status);

  if( result )
    {
     String ret(result);

     free(result);

     return ret;
    }

  return String("<error>");
 }

} // namespace App
