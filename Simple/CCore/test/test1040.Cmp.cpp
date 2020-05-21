/* test1040.Cmp.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

namespace App {

namespace Private_1040 {

inline constexpr int CmpLess = std::strong_ordering::less ;

inline constexpr int CmpGreater = std::strong_ordering::greater ;

inline constexpr int CmpEqual = std::strong_ordering::equal ;

} // namespace Private_1040

using namespace Private_1040;

/* Testit<1040> */

template<>
const char *const Testit<1040>::Name="Test1040 Cmp";

template<>
bool Testit<1040>::Main()
 {
  int a=100;
  int b=200;

  auto cmp = ( a <=> b ) ;

  if( cmp ) // error
    {
    }
  else
    {
     Printf(Con,"equal\n");
    }

  switch( cmp ) // error
    {
     case CmpLess : break; // error

     case CmpGreater : break; // error

     case CmpEqual : break; // error
    }

  if( cmp<0 )
    {
    }

  return true;
 }

} // namespace App

