/* test6010.Double.cpp */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/base/DoubleFormat.h>

namespace CCore {

/* struct PrintProxy<DoubleFormat> */

template <>
struct PrintProxy<DoubleFormat>
 {
  struct ProxyType
   {
    DoubleFormat obj;

    explicit ProxyType(const DoubleFormat &obj_) : obj(obj_) {}

    void print(PrinterType auto &out) const
     {
      Printf(out,"( #; , #; , #; )",obj.getSign(),obj.getExp(),obj.getFract());
     }
   };
 };

} // namespace CCore

namespace App {

namespace Private_6010 {

/* test1() */

void test1()
 {
  DoubleFormat obj(0b11'1111'1111);

  Printf(Con,"#;\n",obj);
 }


} // namespace Private_6010

using namespace Private_6010;

/* Testit<6010> */

template<>
const char *const Testit<6010>::Name="Test6010 Double";

template<>
bool Testit<6010>::Main()
 {
  test1();

  return true;
 }

} // namespace App

