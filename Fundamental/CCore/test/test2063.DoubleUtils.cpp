/* test2063.DoubleUtils.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/math/DoubleUtils.h>

namespace App {

namespace Private_2063 {

/* toText() */

const char * toText(IntShowSign val)
 {
  switch( val )
    {
     case IntShowMinus : return  "";
     case IntShowPlus  : return "+";
     default: return "???";
    }
 }

const char * toText(DoubleFmt val)
 {
  switch( val )
    {
     case DoubleFmtF : return "F";
     case DoubleFmtE : return "E";
     case DoubleFmtX : return "X";
     default: return "???";
    }
 }

const char * toText(DoubleAlign val)
 {
  switch( val )
    {
     case DoubleAlignLeft  : return "L";
     case DoubleAlignRight : return "R";
     default: return "???";
    }
 }

/* ShowOpt() */

void ShowOpt(StrLen opt)
 {
  DoublePrintOpt obj(opt.ptr,opt.ptr+opt.len);

  Printf(Con,"#; #; #; .#; .e#; #;\n",toText(obj.show_sign),obj.width,toText(obj.fmt),
                                      obj.digit_len,obj.exp_len,toText(obj.align));
 }

/* test1() */

void test1()
 {
  ShowOpt("20");
  ShowOpt("+20.8");
  ShowOpt("+20.8L");
  ShowOpt("+20.8R");
  ShowOpt("+20x.8.e15R");
 }

/* test2() */

void test2()
 {
  Printf(Con,"#;\n",3.14159);
  Printf(Con,"#;\n",1.23456E+100);
  Printf(Con,"#;\n",1.23456E-100);
  Printf(Con,"#x;\n",1.);
  Printf(Con,"#x;\n",1000000000.);
 }

} // namespace Private_2063

using namespace Private_2063;

/* Testit<2063> */

template<>
const char *const Testit<2063>::Name="Test2063 DoubleUtils";

template<>
bool Testit<2063>::Main()
 {
  //test1();
  test2();

  return true;
 }

} // namespace App

