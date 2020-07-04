/* TaskEvent.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_task_TaskEvent_h
#define CCore_inc_task_TaskEvent_h

#include <CCore/inc/EventRecorder.h>
#include <CCore/inc/Task.h>
#include <CCore/inc/sys/SysTime.h>

namespace CCore {

/* consts */

inline constexpr bool TaskEventEnable = false ;

inline constexpr bool TaskEventEnableSync  = true ;
inline constexpr bool TaskEventEnableProto = true ;
inline constexpr bool TaskEventEnableDev   = true ;

/* classes */

struct TaskEventAlgo;

template <bool Enable=TaskEventEnable> class TaskEventHostType;

class TickTask;

/* struct TaskEventAlgo */

struct TaskEventAlgo
 {
  static constexpr ulen RecordAlign = 4 ;

  static constexpr uint64 TimeFreq = 0 ; // meaningless value

  class AllocPos : FastMutexBase
   {
     ulen off;

    public:

     AllocPos() : off(0) {}

     ~AllocPos() {}

     operator ulen() const { return off; }

     EventRecordPos alloc(ulen len)
      {
       Lock lock(*this);

       EventRecordPos ret;

       ret.pos=off;
       ret.time=(EventTimeType)Sys::GetClockTime();

       off+=len;

       return ret;
      }

     void back(ulen len)
      {
       Lock lock(*this);

       off-=len;
      }
   };
 };

/* type TaskEventRecorder */

using TaskEventRecorder = EventRecorder<TaskEventAlgo> ;

/* class TaskEventHostType<bool Enable> */

template <>
class TaskEventHostType<true> : public EventRecorderHost<TaskEventRecorder>
 {
  public:

   template <class T,class ... SS>
   void addSync(SS && ... ss) requires ( TaskEventEnableSync )
    {
     add<T>( std::forward<SS>(ss)... );
    }

   template <class T,class ... SS>
   void addSync(SS && ...) requires ( !TaskEventEnableSync )
    {
     // do nothing
    }

   template <class T,class ... SS>
   void addProto(SS && ... ss) requires ( TaskEventEnableProto )
    {
     add<T>( std::forward<SS>(ss)... );
    }

   template <class T,class ... SS>
   void addProto(SS && ...) requires ( !TaskEventEnableProto )
    {
     // do nothing
    }

   template <class T,class ... SS>
   void addDev(SS && ... ss) requires ( TaskEventEnableDev )
    {
     add<T>( std::forward<SS>(ss)... );
    }

   template <class T,class ... SS>
   void addDev(SS && ...) requires ( !TaskEventEnableDev )
    {
     // do nothing
    }
 };

template <>
class TaskEventHostType<false>
 {
  public:

   class StartStop : NoCopy
    {
     public:

      StartStop(TaskEventHostType &,TaskEventRecorder *obj)
       {
        EventTypeIdNode::Register(*obj);

        EventIdNode::Register(*obj,TaskEventRecorder::RecordAlign);
       }
    };

   template <class T,class ... SS>
   void add(SS && ...)
    {
     // do nothing
    }

   template <class T,class ... SS>
   void addSync(SS && ...)
    {
     // do nothing
    }

   template <class T,class ... SS>
   void addProto(SS && ...)
    {
     // do nothing
    }

   template <class T,class ... SS>
   void addDev(SS && ...)
    {
     // do nothing
    }

   void tick()
    {
     // do nothing
    }
 };

/* global TaskEventHost */

extern TaskEventHostType<> TaskEventHost;

/* class TickTask */

class TickTask : public Funchor_nocopy
 {
   Event stop;
   Event wait_stop;

  private:

   void task();

   Function<void (void)> function_task() { return FunctionOf(this,&TickTask::task); }

  public:

   TickTask();

   ~TickTask();
 };

} // namespace CCore

#endif


