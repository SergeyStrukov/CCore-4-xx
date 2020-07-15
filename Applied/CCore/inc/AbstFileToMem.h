/* AbstFileToMem.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_AbstFileToMem_h
#define CCore_inc_AbstFileToMem_h

#include <CCore/inc/AbstFileToRead.h>
#include <CCore/inc/ToMemBase.h>

namespace CCore {

/* classes */

class AbstFileToMem;

/* class AbstFileToMem */

class AbstFileToMem : public ToMemBase
 {
  public:

   AbstFileToMem() noexcept {}

   AbstFileToMem(AbstFileToRead file,StrLen file_name,ulen max_len=MaxULen);

   ~AbstFileToMem() {}

   // std move

   AbstFileToMem(AbstFileToMem &&obj) noexcept = default ;

   AbstFileToMem & operator = (AbstFileToMem &&obj) noexcept = default ;

   // swap/move objects

   void objSwap(AbstFileToMem &obj) noexcept
    {
     Swap<ToMemBase>(*this,obj);
    }

   explicit AbstFileToMem(ToMoveCtor<AbstFileToMem> obj) noexcept
    : ToMemBase(obj.template cast<ToMemBase>())
    {}
 };

} // namespace CCore

#endif


