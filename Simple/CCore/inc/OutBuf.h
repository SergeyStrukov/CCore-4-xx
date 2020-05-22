/* OutBuf.h */
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

#ifndef CCore_inc_OutBuf_h
#define CCore_inc_OutBuf_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* classes */

template <class T> class OutBuf;

/* class OutBuf<T> */

template <class T>
class OutBuf
 {
   T *ptr;

  public:

   explicit OutBuf(T *ptr_) : ptr(ptr_) {}

   operator T * () const { return ptr; }

   void operator () (const T &obj) { *(ptr++)=obj; }

   void operator () (PtrLen<const T> r) { r.copyTo(ptr); ptr+=r.len; }

   template <class ... TT> requires ( sizeof ... (TT) > 1 )
   void operator () (TT ... tt)
    {
     ( (*this)(tt) , ... );
    }
 };

} // namespace CCore

#endif

