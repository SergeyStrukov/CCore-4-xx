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

#include <CCore/inc/math/DoubleUtils.h>

namespace CCore {

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
