/* test6005.IntegerMulAlgo.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN32
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>
#include <CCore/test/testIntSpeed.h>

#include <CCore/test/PriorityBoost.h>

#include <CCore/inc/math/IntegerMulAlgo.h>
#include <CCore/inc/math/IntegerFastAlgo.h>

#include <gmp.h>

namespace App {

namespace Private_6005 {

/* struct GMPAlgo */

struct GMPAlgo
 {
  // types and consts

  using Unit = mp_limb_t ;

  // multiplicative operators

  static void UMul(Unit *restrict c,const Unit *a,ulen na,const Unit *b,ulen nb) // nc==na+nb
   {
    if( na<nb )
      {
       Swap(a,b);
       Swap(na,nb);
      }

    if( nb==0 )
      {
       return;
      }

    mpn_mul(c,a,na,b,nb);
   }
 };

/* struct Base */

struct Base : Math::IntegerFastAlgo
 {
#if 0

  class Int
   {
    public:

     Int() {}

     explicit Int(PtrLen<const Unit>) {}

     Int(const Unit *data,ulen len) : Int(Range(data,len)) {}

     friend Int operator + (Int,Int) { return Int(); }

     friend Int operator * (Int,Int) { return Int(); }

     friend Int operator - (Int,Int) { return Int(); }

     friend Int operator << (Int,ulen) { return Int(); }
   };

  template <class I>
  static void GuardNull(I,const char *)
   {
   }

#endif

  static constexpr ulen Toom22Min =     30 ; // 30
  static constexpr ulen Toom33Min =    110 ; // 100
  static constexpr ulen Toom44Min =    170 ; // 170

  static constexpr ulen TopMin    =  4'000 ;

  static constexpr ulen Toom55Min = TopMin ;
  static constexpr ulen Toom66Min = TopMin ;
  static constexpr ulen Toom77Min = TopMin ;
  static constexpr ulen Toom88Min = TopMin ;

#if 0

  static Unit/* c */ UNeg(Unit *a,ulen na)
   {
    if( na==0 ) return 0;

    return mpn_neg((mp_limb_t *)a,(mp_limb_t *)a,na);
   }

  static Unit/* c */ UAddUnit(Unit *a,ulen na,Unit b)
   {
    if( na==0 ) return b;

    return mpn_add_1((mp_limb_t *)a,(mp_limb_t *)a,na,b);
   }

  static Unit/* c */ USubUnit(Unit *a,ulen na,Unit b)
   {
    if( na==0 ) return b;

    return mpn_sub_1((mp_limb_t *)a,(mp_limb_t *)a,na,b);
   }

  static Unit/* c */ UAdd(Unit *restrict b,const Unit *a,ulen nab)
   {
    if( nab==0 ) return 0;

    return mpn_add_n((mp_limb_t *)b,(mp_limb_t *)b,(const mp_limb_t *)a,nab);
   }

  static Unit/* c */ USub(Unit *restrict b,const Unit *a,ulen nab)
   {
    if( nab==0 ) return 0;

    return mpn_sub_n((mp_limb_t *)b,(mp_limb_t *)b,(const mp_limb_t *)a,nab);
   }

  static Unit/* c */ URevSub(Unit *restrict b,const Unit *a,ulen nab)
   {
    if( nab==0 ) return 0;

    return mpn_sub_n((mp_limb_t *)b,(const mp_limb_t *)a,(mp_limb_t *)b,nab);
   }

  static Unit/* msu */ ULShift(Unit *a,ulen na,unsigned shift) noexcept // 0<shift<UnitBits
   {
    if( na==0 ) return 0;

    return mpn_lshift((mp_limb_t *)a,(mp_limb_t *)a,na,shift);
   }

  static void URShift(Unit *a,ulen na,unsigned shift) noexcept // 0<shift<UnitBits
   {
    if( na==0 ) return;

    mpn_rshift((mp_limb_t *)a,(mp_limb_t *)a,na,shift);
   }

  static Unit UAdd(Unit *restrict c,const Unit *a,const Unit *b,ulen nabc)
   {
    if( nabc==0 ) return 0;

    return mpn_add_n((mp_limb_t *)c,(const mp_limb_t *)a,(const mp_limb_t *)b,nabc);
   }

  static Unit USub(Unit *restrict c,const Unit *a,const Unit *b,ulen nabc)
   {
    if( nabc==0 ) return 0;

    return mpn_sub_n((mp_limb_t *)c,(const mp_limb_t *)a,(const mp_limb_t *)b,nabc);
   }

  static Unit UAdd(Unit *restrict c,const Unit *a,ulen nac,const Unit *b,ulen nb) // nac>=nb
   {
    Unit carry=UAdd(c,a,b,nb);

    if( nb<nac )
      {
       ulen top=nac-nb;

       Copy(c+nb,a+nb,top);

       return UAddUnit(c+nb,top,carry);
      }
    else
      {
       return carry;
      }
   }

  static Unit ULShift(Unit *restrict b,const Unit *a,ulen nab,unsigned shift)
   {
    if( nab==0 ) return 0;

    return mpn_lshift((mp_limb_t *)b,(const mp_limb_t *)a,nab,shift);
   }

#endif

#if 1

  static void RawUMul(Unit *restrict c,const Unit *a,const Unit *b,ulen nab)
   {
    if( nab==0 ) return;

    mpn_mul((mp_limb_t *)c,(const mp_limb_t *)a,nab,(const mp_limb_t *)b,nab);
   }

#endif
 };

using FastAlgo = Math::FastMulAlgo<Base> ;

} // namespace Private_6005

using namespace Private_6005;

/* Testit<6005> */

template<>
const char *const Testit<6005>::Name="Test6005 IntegerMulAlgo";

template<>
bool Testit<6005>::Main()
 {
  TaskMemStack tms(64_KByte);

  PrintFile out("test6005.txt");

  PriorityBoost();

  TestIntMulSpeed<GMPAlgo>::Run(out,"GMPAlgo");

  TestIntMulSpeed<FastAlgo>::Run(out,"FastAlgo");

#if 0

  for(ulen n=20; n<=200 ;n++)
    {
     ulen len=FastAlgo::UMulTempLen(n);

     Printf(Con,"n = #; len = #; mul = #;\n",n,len,RoundUpCount(len,n));
    }

  using A = Math::FFTMul<FastAlgo::FFTAlgo> ;

  for(unsigned d=A::LogUnitBits+1,dmax=A::FindD(MaxULen); d<=dmax ;d++)
    {
     ulen N=ulen(1)<<d;
     ulen K=(N-d)/2;
     ulen n=K*(ulen(1)<<(d-A::LogUnitBits));

     unsigned d1=A::FindD(n);

     Printf(out,"d = #; n = #; d1 = #;\n",d,n,d1);
    }

#endif

  return true;
 }

} // namespace App

