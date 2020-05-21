/* test2999.Blank.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
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

namespace Private_2999 {

struct Test
 {
  int val;

  /*std::strong_ordering*/ int operator <=> (const Test &obj) const noexcept
   {
    Printf(Con,"<=>\n");

    return val <=> obj.val ;
   }

  bool operator == (const Test &obj) const noexcept
   {
    Printf(Con,"==\n");

    return val == obj.val ;
   }
 };

struct Test2
 {
  Test x;
  Test y;

  int operator <=> (const Test2 &obj) const noexcept = default ;
 };

} // namespace Private_2999

using namespace Private_2999;

/* Testit<2999> */

template<>
const char *const Testit<2999>::Name="Test2999 Blank";

template<>
bool Testit<2999>::Main()
 {
  Test a{0},b{1};

  Printf(Con,"a < b  #;\n",a < b);
  Printf(Con,"a <= b #;\n",a <= b);
  Printf(Con,"a > b  #;\n",a > b);
  Printf(Con,"a >= b #;\n",a >= b);

  Printf(Con,"a == b #;\n",a == b);
  Printf(Con,"a != b #;\n",a != b);

  Test2 A{1,0},B{1,1};

  Printf(Con,"A < B  #;\n",A < B);
  Printf(Con,"A <= B #;\n",A <= B);
  Printf(Con,"A > B  #;\n",A > B);
  Printf(Con,"A >= B #;\n",A >= B);

  return true;
 }

} // namespace App

