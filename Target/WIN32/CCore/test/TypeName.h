/* TypeName.h */
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

#ifndef CCore_test_TypeName_h
#define CCore_test_TypeName_h

#include <CCore/inc/String.h>

#include <typeinfo>

namespace App {

/* using */

using namespace CCore;

/* Demangle() */

String Demangle(const char *name);

/* TypeName<T>() */

template <class T>
String TypeName()
 {
  return Demangle(typeid (T).name());
 }

} // namespace App

#endif
