/* DoubleUtils.cpp */
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

#if __has_include(<CCore/inc/base/DoubleFormat.h>)

#include <cmath>

#include <CCore/inc/math/DoubleUtils.h>

namespace CCore {

/* struct DoubleTo2Based */

DoubleTo2Based::DoubleTo2Based(double value)
 {
  if( std::isnan(value) )
    {
     kind=DoubleIsNan;
    }
  else if( std::isinf(value) )
    {
     kind=DoubleIsInf;
    }
  else if( value==0 )
    {
     kind=DoubleIsNull;
    }
  else
    {
     if( value<0 )
       {
        kind=DoubleIsNeg;
        value=-value;
       }
     else
       {
        kind=DoubleIsPos;
       }

     // TODO

     value=std::frexp(value,&bin_exp);

     int delta=53;

     value=std::ldexp(value,delta);
     bin_exp-=delta;

     base=static_cast<BaseType>(value);

     while( !(base&1u) )
       {
        base>>=1;
        bin_exp++;
       }
    }
 }

/* struct DoubleTo10Based */

int DoubleTo10Based::DecExp(double value) // value > 0
 {
  // TODO

  double exp = std::ceil(std::log10(value));

  return static_cast<int>(exp);

  // 10^exp > value
 }

DoubleTo10Based::DoubleTo10Based(double value,unsigned digit_len_)
 {
  if( std::isnan(value) )
    {
     kind=DoubleIsNan;
    }
  else if( std::isinf(value) )
    {
     kind=DoubleIsInf;
    }
  else if( value==0 )
    {
     kind=DoubleIsNull;
    }
  else
    {
     if( value<0 )
       {
        kind=DoubleIsNeg;
        value=-value;
       }
     else
       {
        kind=DoubleIsPos;
       }

     // TODO

     digit_len=Cap<unsigned>(6,digit_len_,16);

     dec_exp=DecExp(value)-(digit_len+1);

     static constexpr int MaxExp = 308 ;

     if( dec_exp > -MaxExp )
       {
        value=std::round(value*std::pow(10.,-dec_exp));
       }
     else
       {
        value=std::round(value*std::pow(10.,MaxExp)*std::pow(10.,-dec_exp-MaxExp));
       }

     double cap=std::pow(10.,digit_len);

     while( value>=cap )
       {
        value=std::round(value/10);
        dec_exp++;
       }

     base=static_cast<BaseType>(value);
    }
 }

/* class DoubleToDec */

DoubleToDec::DoubleToDec(double value,unsigned digit_len)
 {
  DoubleTo10Based obj(value,digit_len);

  kind=obj.kind;

  if( kind==DoubleIsPos || kind==DoubleIsNeg )
    {
     digit_len=obj.digit_len;
     dec_exp=obj.dec_exp;

     base.do_uint(obj.base);
    }
 }

ulen DoubleToDec::getDecLen(IntShowSign show_sign) const
 {
  PrintCount<void> out;

  printDec(out,show_sign);

  return out.getCount();
 }

ulen DoubleToDec::getExpLen(IntShowSign show_sign) const
 {
  PrintCount<void> out;

  printExp(out,show_sign);

  return out.getCount();
 }

/* class DoubleToHex */

DoubleToHex::DoubleToHex(double value)
 : base(16)
 {
  DoubleTo2Based obj(value);

  kind=obj.kind;

  if( kind==DoubleIsPos || kind==DoubleIsNeg )
    {
     bin_exp=obj.bin_exp;

     base.do_uint(obj.base);
    }
 }

ulen DoubleToHex::getLen() const
 {
  PrintCount<void> out;

  print(out);

  return out.getCount();
 }

/* struct DoublePrintOpt */

DoublePrintOpt::DoublePrintOpt(const char *ptr,const char *lim)
 {
  StrParse dev(ptr,lim);

  Parse_empty(dev,show_sign);

  ParseUInt_empty(dev,width,0u);

  Parse_empty(dev,fmt);

  switch( Parse_DoubleLenPrefix(dev) )
    {
     case DoubleDigitLen :
      {
       ParseUInt(dev,digit_len);

       switch( Parse_DoubleLenPrefix(dev) )
         {
          case DoubleDigitLen :
           {
            dev.fail();
           }
          break;

          case DoubleExpLen :
           {
            ParseUInt(dev,exp_len);
           }
          break;

          default:
           {
            exp_len=DefaultExpLen;
           }
         }
      }
     break;

     case DoubleExpLen :
      {
       digit_len=DefaultDigitLen;

       ParseUInt(dev,exp_len);
      }
     break;

     default:
      {
       digit_len=DefaultDigitLen;
       exp_len=DefaultExpLen;
      }
    }

  Parse_empty(dev,align);

  if( !dev.finish() ) setDefault();
 }

} // namespace CCore

#endif
