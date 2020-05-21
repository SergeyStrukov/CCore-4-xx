/* StrKey.h */
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

#ifndef CCore_inc_StrKey_h
#define CCore_inc_StrKey_h

#include <CCore/inc/Cmp.h>
#include <CCore/inc/String.h>

namespace CCore {

/* classes */

struct StrKey;

struct StringKey;

/* struct StrKey */

struct StrKey
 {
  using HashType = uint32 ;

  static HashType Hash(StrLen str);

  // data

  HashType hash;
  StrLen str;

  // constructors

  StrKey() noexcept { hash=Hash(Empty); }

  explicit StrKey(StrLen str_) : hash(Hash(str_)),str(str_) {}

  // cmp objects

  CmpResult operator <=> (const StrKey &obj) const noexcept
   {
    if( CmpResult ret=Cmp(hash,obj.hash) ) return ret;

    return StrCmp(str,obj.str);
   }

  bool operator == (const StrKey &obj) const noexcept = default ;
 };

/* struct StringKey */

struct StringKey
 {
  StrKey::HashType hash;
  String str;

  StringKey() noexcept { hash=StrKey::Hash(Empty); }

  explicit StringKey(StrKey obj) : hash(obj.hash),str(obj.str) {}

  // cmp objects

  CmpResult operator <=> (const StringKey &obj) const noexcept
   {
    if( CmpResult ret=Cmp(hash,obj.hash) ) return ret;

    return StrCmpOf(str,obj.str);
   }

  bool operator == (const StringKey &obj) const noexcept = default ;
 };

inline CmpResult Cmp(StrKey key,const StringKey &obj)
 {
  if( CmpResult ret=Cmp(key.hash,obj.hash) ) return ret;

  return StrCmp(key.str,Range(obj.str));
 }

} // namespace CCore

#endif


