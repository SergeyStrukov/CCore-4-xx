/* JobMachine.h */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_JobMachine_h
#define CCore_inc_JobMachine_h

#include <CCore/inc/MemBase.h>
#include <CCore/inc/List.h>

#include <CCore/inc/Task.h>

namespace CCore {

/* classes */

class JobObject;

class JobList;

class JobMachine;

class JobMachineInit;

template <FuncInitArgType<> FuncInit> class FuncJobObject;

/* class JobObject */

class JobObject : public MemBase_nocopy
 {
   DLink<JobObject> link;

   friend class JobList;

  public:

   JobObject() {}

   virtual ~JobObject() {}

   virtual void job(bool stop_flag)=0;
 };

/* class JobList */

class JobList : NoCopy
 {
   using Algo = DLink<JobObject>::LinearAlgo<&JobObject::link>;

   Algo::FirstLast normal;
   Algo::FirstLast pri;

  public:

   JobList();

   ~JobList();

   void add(JobObject *job);

   void addPri(JobObject *job);

   JobObject * get();
 };

/* class JobMachine */

class JobMachine : NoCopy
 {
  private:

   Sem sem;
   Mutex mutex;
   Atomic stop_flag;
   Atomic ex_flag;

   JobList list;

   AntiSem asem;

  private:

   JobObject * getJob();

   void work();

  public:

   JobMachine();

   ~JobMachine();

   void start(unsigned exec_count);

   void start();

   void stop() noexcept;

   void add(JobObject *job);

   void addPri(JobObject *job);
 };

/* class JobMachineInit */

class JobMachineInit : NoCopy
 {
   JobMachine obj;

   static JobMachine * Object;

  public:

   JobMachineInit();

   explicit JobMachineInit(unsigned exec_count);

   ~JobMachineInit();

   static void Add(JobObject *job);

   static void AddPri(JobObject *job);
 };

/* class FuncJobObject<FuncInit> */

template <FuncInitArgType<> FuncInit>
class FuncJobObject : public JobObject
 {
   FunctorTypeOf<FuncInit> func;

  public:

   explicit FuncJobObject(const FuncInit &func_init) : func(func_init) {}

   virtual ~FuncJobObject() {}

   virtual void job(bool stop_flag)
    {
     if( !stop_flag ) func();

     delete this;
    }
 };

/* functions */

inline void AddJob(JobObject *job) { JobMachineInit::Add(job); }

inline void AddJobPri(JobObject *job) { JobMachineInit::AddPri(job); }

JobObject * CreateFuncJob(FuncInitArgType<> auto func_init)
 {
  return new FuncJobObject(func_init);
 }

void AddFuncJob(FuncInitArgType<> auto func_init)
 {
  AddJob(CreateFuncJob(func_init));
 }

} // namespace CCore

#endif

