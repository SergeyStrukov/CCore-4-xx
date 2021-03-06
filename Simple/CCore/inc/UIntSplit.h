/* UIntSplit.h */
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

#ifndef CCore_inc_UIntSplit_h
#define CCore_inc_UIntSplit_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* concept UIntSplitEnable<UIntBig,UIntSmall> */

template <class UIntBig,class UIntSmall>
concept UIntSplitEnable = UIntType<UIntBig> && UIntType<UIntSmall> &&
                          ( Meta::UIntBits<UIntBig> % Meta::UIntBits<UIntSmall> == 0 ) &&
                          ( Meta::UIntBits<UIntBig> > Meta::UIntBits<UIntSmall> ) ;

/* classes */

template <UIntType UIntBig,UIntType UIntSmall> requires UIntSplitEnable<UIntBig,UIntSmall> class UIntSplit;

/* class UIntSplit<UIntBig,UIntSmall> */

template <UIntType UIntBig,UIntType UIntSmall> requires UIntSplitEnable<UIntBig,UIntSmall>
class UIntSplit
 {
   static constexpr unsigned BitLen    = Meta::UIntBits<UIntSmall> ;
   static constexpr unsigned BigBitLen = Meta::UIntBits<UIntBig> ;

  public:

   static constexpr unsigned Len       = BigBitLen/BitLen ;

  private:

   UIntSmall buf[Len]; // big-endian order

  public:

   // access

   UIntSmall operator [] (unsigned index) const { return buf[index]; }

   UIntSmall & operator [] (unsigned index) { return buf[index]; }

   PtrLen<const UIntSmall> ref() const { return Range(buf); }

   PtrLen<UIntSmall> ref() { return Range(buf); }

   PtrLen<const UIntSmall> ref_const() const { return Range(buf); }

   // get

   template <unsigned Ind,unsigned Off> struct Get_loop;

   template <unsigned Ind,unsigned Off> struct Get_last;

   template <unsigned Ind,unsigned Off>
   using Get = Meta::Select<( Ind>0 ), Get_loop<Ind,Off> , Get_last<Ind,Off> > ;

   UIntBig get() const
    {
     UIntBig ret=buf[Len-1];

     Get<Len-2,BitLen>::Do(ret,buf);

     return ret;
    }

   // set

   template <unsigned Ind,unsigned Off> struct Set_loop;

   template <unsigned Ind,unsigned Off> struct Set_last;

   template <unsigned Ind,unsigned Off>
   using Set = Meta::Select<( Ind>0 ), Set_loop<Ind,Off> , Set_last<Ind,Off> > ;

   void set(UIntBig value)
    {
     buf[Len-1]=UIntSmall( value );

     Set<Len-2,BitLen>::Do(value,buf);
    }
 };

template <UIntType UIntBig,UIntType UIntSmall> requires UIntSplitEnable<UIntBig,UIntSmall>
template <unsigned Ind,unsigned Off>
struct UIntSplit<UIntBig,UIntSmall>::Get_loop
 {
  static void Do(UIntBig &ret,const UIntSmall buf[])
   {
    ret|=( UIntBig(buf[Ind])<<Off );

    Get<Ind-1,Off+BitLen>::Do(ret,buf);
   }
 };

template <UIntType UIntBig,UIntType UIntSmall> requires UIntSplitEnable<UIntBig,UIntSmall>
template <unsigned Ind,unsigned Off>
struct UIntSplit<UIntBig,UIntSmall>::Get_last
 {
  static void Do(UIntBig &ret,const UIntSmall buf[])
   {
    ret|=( UIntBig(buf[Ind])<<Off );
   }
 };

template <UIntType UIntBig,UIntType UIntSmall> requires UIntSplitEnable<UIntBig,UIntSmall>
template <unsigned Ind,unsigned Off>
struct UIntSplit<UIntBig,UIntSmall>::Set_loop
 {
  static void Do(UIntBig value,UIntSmall buf[])
   {
    buf[Ind]=UIntSmall( value>>Off );

    Set<Ind-1,Off+BitLen>::Do(value,buf);
   }
 };

template <UIntType UIntBig,UIntType UIntSmall> requires UIntSplitEnable<UIntBig,UIntSmall>
template <unsigned Ind,unsigned Off>
struct UIntSplit<UIntBig,UIntSmall>::Set_last
 {
  static void Do(UIntBig value,UIntSmall buf[])
   {
    buf[Ind]=UIntSmall( value>>Off );
   }
 };

} // namespace CCore

#endif


