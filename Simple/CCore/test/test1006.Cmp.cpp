/* test1006.Cmp.cpp */
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

#include <CCore/inc/Cmp.h>

namespace App {

namespace Private_1006 {

/* struct Test */

struct Test
 {
  int a;
  int b;

  Test(int a_=0,int b_=0) : a(a_),b(b_) {}

  CmpResult operator <=> (const Test &obj) const CCORE_NOINLINE;

  bool operator == (const Test &obj) const = default ;

  void print(PrinterType auto &out) const
   {
    Printf(out,"(#;,#;)",a,b);
   }
 };

CmpResult Test::operator <=> (const Test &obj) const { return AlphaCmp(a,obj.a,b,obj.b); }

/* functions */

void Show(Test t1,Test t2)
 {
  t1<t2;
  t1<=t2;
  t1>t2;
  t1>=t2;
  t1==t2;
  t1!=t2;

  Printf(Con,"#; #; #; , < ? #;\n",t1,Cmp(t1,t2),t2,(t1<t2));
 }

void Show(StrLen a,StrLen b)
 {
  Printf(Con,"#; #; #;\n",a,RangeCmp<const char>(a,b),b);
 }

} // namespace Private_1006

using namespace Private_1006;

/* Testit<1006> */

template<>
const char *const Testit<1006>::Name="Test1006 Cmp";

template<>
bool Testit<1006>::Main()
 {
  Show(Test(1,2),Test(3,4));
  Show(Test(3,4),Test(1,2));

  Show(Test(1,2),Test(1,3));
  Show(Test(1,3),Test(1,2));

  Show(Test(1,2),Test(1,2));

  Show("a","a");

  Show("a","ab");

  Show("ab","a");

  Show("abc1fff","abc2eee");

  return true;
 }

} // namespace App

