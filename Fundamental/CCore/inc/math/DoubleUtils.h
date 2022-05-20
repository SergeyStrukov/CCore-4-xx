/* DoubleUtils.h */
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

#ifndef CCore_inc_math_DoubleUtils_h
#define CCore_inc_math_DoubleUtils_h

#include <CCore/inc/base/DoubleFormat.h>
#include <CCore/inc/Printf.h>

namespace CCore {

/* classes */

//enum DoubleAlign;

//enum DoubleFmt;

struct DoublePrintOpt;

/* enum DoubleAlign */

enum DoubleAlign
 {
  DoubleAlignLeft,
  DoubleAlignRight,

  DoubleAlignDefault = DoubleAlignRight
 };

template <CharPeekType Dev>
void Parse_empty(Dev &dev,DoubleAlign &ret)
 {
  typename Dev::Peek peek(dev);

  if( !peek )
    {
     ret=DoubleAlignDefault;
    }
  else
    {
     switch( *peek )
       {
        case 'l' : case 'L' : ret=DoubleAlignLeft;  ++dev; break;
        case 'r' : case 'R' : ret=DoubleAlignRight; ++dev; break;

        default: ret=DoubleAlignDefault;
       }
    }
 }

/* enum DoubleFmt */

enum DoubleFmt
 {
  DoubleFmtF,
  DoubleFmtE,
  DoubleFmtX,

  DoubleFmtDefault = DoubleFmtF
 };

template <CharPeekType Dev>
void Parse_empty(Dev &dev,DoubleFmt &ret)
 {
  typename Dev::Peek peek(dev);

  if( !peek )
    {
     ret=DoubleFmtDefault;
    }
  else
    {
     switch( *peek )
       {
        case 'e' : case 'E' : ret=DoubleFmtE; ++dev; break;
        case 'f' : case 'F' : ret=DoubleFmtF; ++dev; break;
        case 'x' : case 'X' : ret=DoubleFmtX; ++dev; break;

        default: ret=DoubleFmtDefault;
       }
    }
 }

/* struct DoublePrintOpt */

enum DoubleLenPrefix
 {
  DoubleLenNone,
  DoubleDigitLen,
  DoubleExpLen
 };

template <CharPeekType Dev>
DoubleLenPrefix Parse_DoubleLenPrefix(Dev &dev)
 {
  if( ParseChar_try(dev,'.') )
    {
     if( ParseChar_try(dev,'e') )
       {
        return DoubleExpLen;
       }
     else
       {
        return DoubleDigitLen;
       }
    }
  else
    {
     return DoubleLenNone;
    }
 }

struct DoublePrintOpt
 {
  static constexpr unsigned DefaultDigitLen =  6 ;
  static constexpr unsigned DefaultExpLen   = 10 ;

  ulen width;
  unsigned digit_len; // 3.14 -> 3
  unsigned exp_len;
  DoubleFmt fmt;
  DoubleAlign align;
  IntShowSign show_sign;

  void setDefault()
   {
    width=0;
    digit_len=DefaultDigitLen;
    exp_len=DefaultExpLen;
    fmt=DoubleFmtDefault;
    align=DoubleAlignDefault;
    show_sign=IntShowSignDefault;
   }

  DoublePrintOpt() { setDefault(); }

  DoublePrintOpt(const char *ptr,const char *lim);

  //
  // [+][width=0][e|E|f|F|x|X=f][.<digit_len>=.6][.e<exp_len=.e10>][l|L|r|R=R]
  //
 };

} // namespace CCore

#endif

