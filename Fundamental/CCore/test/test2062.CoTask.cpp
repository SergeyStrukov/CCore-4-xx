/* test2062.CoTask.cpp */
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

#include <CCore/inc/CoTask.h>

namespace App {

namespace Private_2062 {

/* class Test1 */

class Test1 : NoCopy
 {
   int value = 0 ;

  private:

   CoTask<void> coGen()
    {
     for(int i=1; i<=25 ;i++)
       {
        value=i;

        co_await std::suspend_always{};
       }
    }

  public:

   Test1() {}

   void process()
    {
     CoTask<void> cogen=coGen();

     Printf(Con,"beg\n");

     for(;;)
       {
        auto result=cogen.push();

        if( result.ok )
          {
           break;
          }
        else
          {
           Printf(Con,"#;\n",value);
          }
       }

     Printf(Con,"end\n\n");
    }
 };

/* test1() */

void test1()
 {
  Test1 test;

  test.process();
 }

/* struct Result */

struct Result
 {
  int value;

  Result() : value(0) {}

  explicit Result(int value_) : value(value_) {}

  Result(const Result &obj) : value(obj.value) { Printf(Con,"copy\n"); }

  Result(Result &&obj) : value(obj.value) { Printf(Con,"move\n"); }

  Result & operator = (const Result &obj)
   {
    value=obj.value;

    Printf(Con,"copy =\n");

    return *this;
   }

  Result & operator = (Result &&obj)
   {
    value=obj.value;

    Printf(Con,"move =\n");

    return *this;
   }
 };

/* class Test2 */

class Test2 : NoCopy
 {
   int value = 0 ;

  private:

   CoTask<Result> coGen()
    {
     for(int i=1; i<=25 ;i++)
       {
        value=i;

        co_await std::suspend_always{};
       }

     co_return Result(12345);
    }

  public:

   Test2() {}

   void process()
    {
     CoTask<Result> cogen=coGen();

     Printf(Con,"beg\n");

     for(;;)
       {
        auto result=cogen.push();

        if( result.ok )
          {
           Printf(Con,"result = #;\n",result.obj.value);

           break;
          }
        else
          {
           Printf(Con,"#;\n",value);
          }
       }

     Printf(Con,"end\n\n");
    }
 };

/* test2() */

void test2()
 {
  Test2 test;

  test.process();
 }

/* class Test3 */

class Test3 : NoCopy
 {
   static constexpr ulen Len = 30 ;

   int buf[Len];
   ulen len = 0 ;

   CoTask<void> cofill;
   CoTask<void> cosum;

   // fill

   static constexpr int Last = 10'000 ;

   int next = 0 ;

   CoTask<void> coFillBuffer()
    {
     while( next<=Last )
       {
        while( len>=Len )
          {
           co_await cosum.getResumer();
          }

        buf[len++]=next++;
       }
    }

   // sum

   int sum = 0 ;

   CoTask<void> coSumBuffer()
    {
     for(;;)
       {
        while( len==0 )
          {
           if( cofill.done() ) co_return;

           co_await cofill.getResumer();
          }

        for(int val : Range(buf,len) ) sum+=val;

        len=0;
       }
    }

  public:

   Test3() {}

   void process()
    {
     next=0;
     sum=0;

     cofill=coFillBuffer();
     cosum=coSumBuffer();

     cosum.push();

     if( !cosum.push() )
       {
        Printf(Exception,"Test3::process() : push failed");
       }

     Printf(Con,"sum = #;\n",sum);
    }
 };

/* test3() */

void test3()
 {
  Test3 test;

  test.process();
 }

} // namespace Private_2062

using namespace Private_2062;

/* Testit<2062> */

template<>
const char *const Testit<2062>::Name="Test2062 CoTask";

template<>
bool Testit<2062>::Main()
 {
  test1();
  test2();
  test3();

  return true;
 }

} // namespace App

