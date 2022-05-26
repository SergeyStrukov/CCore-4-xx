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

#include <cmath>

#include <CCore/inc/math/DoubleUtils.h>
#include <CCore/inc/Print.h>

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
  Printf(Con,"#;\n",1.23456E+308);
  Printf(Con,"#;\n",1.23456E-308);
  Printf(Con,"#.20;\n",1.234567890123456789E+100);
  Printf(Con,"#.20;\n",1.234567890123456789E-100);
  Printf(Con,"| #30xL; |\n",1.);
  Printf(Con,"| #30xR; |\n",1000000000.);

  for(int e=-50; e<=50 ;e++)
    Printf(Con,"| #30.20L; |\n",1.234567890123456789*std::pow(10.,e));

  for(int e=-50; e<=50 ;e++)
    Printf(Con,"| #30R; |\n",1.234567890123456789*std::pow(10.,e));
 }

/* test3() */

void test3()
 {
  DoubleFormat obj;

  obj.setExp(DoubleFormat::MaxExp);

  Printf(Con,"#;\n",obj.get());

  obj.setFract(1);

  Printf(Con,"#;\n",obj.get());

  Printf(Con,"#;\n",0.);
 }

/* test4() */

void test4()
 {
  Printf(Con,"#x;\n",std::ldexp(1.,1023));         // max pow2
  Printf(Con,"#x;\n",0xF'FFFF'FFFF'FFFFP972);      // max
  Printf(Con,"#x;\n",std::ldexp(2.-0x1P-51,1023)); // max
  Printf(Con,"#x;\n",std::ldexp(1.,1024));         // inf
  Printf(Con,"#x;\n",std::ldexp(1.,-1074));        // min
  Printf(Con,"#x;\n",std::ldexp(1.,-1075));        // 0
 }

/* test5() */

void test5()
 {
  static constexpr int DExp = DoubleFormat::ExpBias+DoubleFormat::FractBits ;
  static constexpr int MinBinExp = 1-DExp ;
  static constexpr int MaxBinExp = DoubleFormat::MaxExp-1-DoubleFormat::ExpBias ;

  Printf(Con,"DExp = #; MinBinExp = #; MaxBinExp = #;\n",DExp,MinBinExp,MaxBinExp);

  const double M=std::log10(2);

  PrintFile out("test.txt");

  int ind=0;
  int line=20;

  for(int bin_exp=MinBinExp; bin_exp<=MaxBinExp ;bin_exp++)
    {
     double mul=M*bin_exp;
     int dec_exp=(int)std::ceil(mul);

     if( dec_exp-mul>1-0.0001 ) Printf(Con,"#; ???\n",bin_exp);

     if( ind==0 ) Putobj(out,"    ");

     Printf(out,"#4; ,",dec_exp);

     if( ++ind>=line ) ind=0;

     if( ind==0 )
       Putch(out,'\n');
     else
       Putch(out,' ');
    }
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
  //test2();
  //test3();
  //test4();
  test5();

  return true;
 }

} // namespace App

