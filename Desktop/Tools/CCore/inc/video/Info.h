/* Info.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_Info_h
#define CCore_inc_video_Info_h

#include <CCore/inc/RefObjectBase.h>

#include <CCore/inc/String.h>
#include <CCore/inc/ElementPool.h>
#include <CCore/inc/Array.h>
#include <CCore/inc/Sort.h>

#include <CCore/inc/video/RefVal.h>

namespace CCore {
namespace Video {

/* consts */

enum ComboInfoType
 {
  ComboInfoText,
  ComboInfoSeparator,
  ComboInfoTitle
 };

/* classes */

struct AbstractInfo;

class InfoFromString;

class InfoBuilder;

struct ComboInfoItem;

struct AbstractComboInfo;

class ComboInfoBuilder;

/* struct AbstractInfo */

struct AbstractInfo
 {
  virtual ulen getLineCount() const =0;

  virtual StrLen getLine(ulen index) const =0;
 };

/* type InfoBase */

using InfoBase = RefObjectBase<AbstractInfo> ;

/* GetNullInfoPtr() */

InfoBase * GetNullInfoPtr() noexcept;

/* type Info */

using Info = RefObjectHook<InfoBase,AbstractInfo,GetNullInfoPtr> ;

/* class InfoFromString */

class InfoFromString : public Info
 {
   class StringSet;

  public:

   explicit InfoFromString(const String &str);

   ~InfoFromString();
 };

/* class InfoBuilder */

class InfoBuilder : NoCopy
 {
   ElementPool pool;
   DynArray<StrLen> list;

   class PoolInfo;

  public:

   InfoBuilder();

   ~InfoBuilder();

   void add(StrLen line);

   void sort(ObjLessFuncType<StrLen> auto less) { IncrSort(Range(list),less); }

   Info complete();
 };

/* struct ComboInfoItem */

struct ComboInfoItem
 {
  ComboInfoType type;
  StrLen text;

  ComboInfoItem() noexcept : type(ComboInfoText) {}

  ComboInfoItem(NothingType) : ComboInfoItem() {}

  ComboInfoItem(ComboInfoType type_,StrLen text_) : type(type_),text(text_) {}
 };

/* struct AbstractComboInfo */

struct AbstractComboInfo
 {
  virtual ulen getLineCount() const =0;

  virtual ComboInfoItem getLine(ulen index) const =0;
 };

/* type ComboInfoBase */

using ComboInfoBase = RefObjectBase<AbstractComboInfo> ;

/* GetNullComboInfoPtr() */

ComboInfoBase * GetNullComboInfoPtr();

/* type ComboInfo */

using ComboInfo = RefObjectHook<ComboInfoBase,AbstractComboInfo,GetNullComboInfoPtr> ;

/* class ComboInfoBuilder */

class ComboInfoBuilder : NoCopy
 {
   ElementPool pool;
   DynArray<ComboInfoItem> list;

   class PoolInfo;

  private:

   static void SortGroups(PtrLen<ComboInfoItem> r,ObjLessFuncType<StrLen> auto less)
    {
     while( +r )
       {
        for(; +r && r->type!=ComboInfoText ;++r);

        if( !r ) break;

        auto temp=r;

        for(++r; +r && r->type==ComboInfoText ;++r);

        IncrSort(temp.prefix(r), [less] (ComboInfoItem a,ComboInfoItem b) { return less(a.text,b.text); } );
       }
    }

  public:

   ComboInfoBuilder();

   ~ComboInfoBuilder();

   // add...()

   void add(ComboInfoType type,StrLen line);

   void add(StrLen line) { add(ComboInfoText,line); }

   void addTitle(StrLen line) { add(ComboInfoTitle,line); }

   void addSeparator() { add(ComboInfoSeparator,Empty); }

   // sort...()

   void sortGroups(ObjLessFuncType<StrLen> auto less)
    {
     SortGroups(Range(list),less);
    }

   // complete()

   ComboInfo complete();
 };

} // namespace Video
} // namespace CCore

#endif


