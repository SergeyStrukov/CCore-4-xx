/* test2052.ObjectDomain.cpp */
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
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/ObjectDomain.h>
#include <CCore/inc/MemSpaceHeap.h>
#include <CCore/inc/PlatformRandom.h>

namespace App {

namespace Private_2052 {

/* class TestDomain */

class TestDomain : public ObjectDomain
 {
   SpaceHeap heap;

  private:

   virtual void * try_alloc(ulen len) { return heap.alloc(len); }

   virtual void free(void *mem) noexcept { heap.free(mem); }

  public:

   explicit TestDomain(ulen max_total_mem) : ObjectDomain(max_total_mem),heap(2*max_total_mem) {}

   ~TestDomain() { cleanup(); }
 };

/* global Domain */

static ObjectDomain * Domain=0;

/* class Test */

class Test
 {
   DynArray< IntObjPtr<Test> > subs;

  public:

   explicit Test(ulen count)
    : subs(count)
    {
    }

   ~Test()
    {
    }

   void add(PlatformRandom &random)
    {
     ulen count=random.select(10);

     ExtObjPtr<Test> temp(Domain,count);

     subs.append_fill(temp);

     ulen len=subs.getLen();

     for(IntObjPtr<Test> &obj : temp->subs ) obj=subs[random.select(len)];
    }

   void del(PlatformRandom &random)
    {
     if( ulen len=subs.getLen() )
       {
        ulen index=random.select(len);

        IntObjPtr<Test> temp=subs[index];

        if( +temp )
          {
           temp->subs.shrink_one();
          }

        if( index<len-1 )
          {
           Swap(subs[index],subs[len-1]);
          }

        subs.shrink_one();
       }
    }

   void delAll()
    {
     subs.erase();
    }

   template <class Keeper>
   void keepAlive(Keeper keeper)
    {
     subs.apply(keeper);
    }
 };

static_assert( Has_keepAlive<Test> ,"Test : keepAlive is missing");

//static_assert( Has_breakWeak<Test> ,"Test : breakWeak is missing");

/* test1() */

void test1()
 {
  ExtObjPtr<Test> ptr1(Domain,10);

  ExtObjPtr<Test> ptr2;

  ptr2=ptr1;
 }

/* test2() */

void test2()
 {
  ExtObjPtr<Test> ptr(Domain,0);
  PlatformRandom random;

  ulen break_count=0;

  for(ulen count=1'000'000; count ;count--)
    {
     ulen collect_count=0;

     while( Domain->getObjectCount()>10'000 )
       {
        for(ulen count=1000; count ;count--) ptr->del(random);

        Domain->collect();

        collect_count++;
       }

     if( collect_count>1 ) Printf(Con,"#;\n",collect_count);

     if( collect_count ) break_count++;

     ptr->add(random);
    }

  Printf(Con,"breaks #;\n",break_count);
 }

/* test3() */

void test3()
 {
  ExtObjPtr<Test> ptr(Domain,0);
  PlatformRandom random;

  ulen break_count=0;

  for(ulen count=1'000'000; count ;count--)
    {
     if( Domain->getObjectCount()>=10'000 )
       {
        ptr->delAll();

        Domain->collect();

        break_count++;
       }

     ptr->add(random);
    }

  Printf(Con,"breaks #;\n",break_count);
 }

/* class Test1 */

class Test1 : NoCopy
 {
   int num;

  public:

   IntObjPtr<Test1> ptr;
   WeakObjPtr<Test1> weak_ptr;
   IntDelObjPtr<Test1> del_ptr;

   IntAnyObjPtr<Test1> any_ptr;

   explicit Test1(int num_)
    : num(num_)
    {
     Printf(Con,"Test1(#;)\n",num);
    }

   ~Test1()
    {
     Printf(Con,"~Test1(#;)\n",num);
    }

   template <class Keeper>
   void keepAlive(Keeper keeper)
    {
     keeper(ptr,del_ptr,any_ptr);
    }

   template <class Breaker>
   void breakWeak(Breaker breaker)
    {
     breaker(weak_ptr);
    }
 };

static_assert( Has_keepAlive<Test1> ,"Test1 : keepAlive is missing");

static_assert( Has_breakWeak<Test1> ,"Test1 : breakWeak is missing");

/* test4() */

void collect(int num)
 {
  Printf(Con,"\nbegin collect #;\n",num);

  Domain->collect();

  Printf(Con,"end collect\n");
 }

void test4()
 {
  ExtObjPtr<Test1> obj1(Domain,1);

  obj1->ptr=IntObjPtr<Test1>(Domain,2);

  obj1->any_ptr=IntObjPtr<Test1>(Domain,50);

  obj1->any_ptr.getPtr();

  collect(1);

  obj1->ptr.nullify();

  collect(2);

  obj1->ptr=obj1->weak_ptr=WeakObjPtr<Test1>(Domain,3);

  collect(3);

  obj1->ptr.nullify();

  collect(4);

  Printf(Con,"weak #;\n\n",!obj1->weak_ptr);

  ExtDelObjPtr<Test1> obj2(Domain,4);

  obj1->del_ptr=obj2;

  Printf(Con,"destroy1 #;\n",obj1->del_ptr.destroy());

  Printf(Con,"destroy2 #;\n",obj2.destroy());

  Printf(Con,"del #;\n",!obj1->del_ptr);

  Printf(Con,"objects = #;\n",Domain->getObjectCount());

  collect(5);

  Printf(Con,"objects = #;\n",Domain->getObjectCount());

  Printf(Con,"\nend test\n\n");
 }

} // namespace Private_2052

using namespace Private_2052;

/* Testit<2052> */

template<>
const char *const Testit<2052>::Name="Test2052 ObjectDomain";

template<>
bool Testit<2052>::Main()
 {
  TestDomain domain(10_MByte);

  Domain=&domain;

  //test1();
  //test2();
  //test3();
  test4();

  return true;
 }

} // namespace App

