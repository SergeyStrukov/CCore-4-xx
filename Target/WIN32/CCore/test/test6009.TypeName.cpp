/* test6009.TypeName.cpp */
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

#include <CCore/test/test.h>

#include "TypeName.h"

namespace App {

namespace Private_6009 {


} // namespace Private_6009

using namespace Private_6009;

/* Testit<6009> */

template<>
const char *const Testit<6009>::Name="Test6009 TypeName";

template<>
bool Testit<6009>::Main()
 {
  Printf(Con,"#;\n",TypeName<int>());

  Printf(Con,"#;\n",TypeName<NoCopy>());

  Printf(Con,"#;\n",TypeName<StrLen>());

  Printf(Con,"#;\n",TypeName< NoCopyBase<StrLen> >());

  return true;
 }

} // namespace App

