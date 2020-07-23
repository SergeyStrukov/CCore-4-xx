/* FileNameMatch.cpp */
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

#include <CCore/inc/FileNameMatch.h>

#include <CCore/inc/Cmp.h>
#include <CCore/inc/Tree.h>
#include <CCore/inc/NodeAllocator.h>
#include <CCore/inc/algon/SortUnique.h>
#include <CCore/inc/StrToChar.h>

#include <CCore/inc/Exception.h>

namespace CCore {

using namespace FileNameMatch;

/* class SlowFileNameFilter */

void SlowFileNameFilter::prepare(StrLen text)
 {
  filter.reserve(text.len);

  while( +text )
    {
     Char ch=CutChar_guarded(text);

     filter.append_copy(ch);
    }

  filter.shrink_extra();

  ulen len=LenAdd(filter.getLen(),1);

  flags.extend_default(len);
  list.extend_default(len);

  ok=true;
 }

void SlowFileNameFilter::suffixes(ulen count,FuncInitArgType<Filter> auto func_init) const
 {
  Suffixes(Range(list.getPtr(),count),Range(filter),func_init);
 }

void SlowFileNameFilter::add(Filter s,Char ch) const
 {
  Add(s,ch, [this] (ulen len) { flags[len]=true; } );
 }

ulen SlowFileNameFilter::complete() const
 {
  auto base=list.getPtr();

  ulen count=0;

  for(ulen i=0,lim=list.getLen(); i<lim ;i++) if( flags[i] ) base[count++]=i;

  return count;
 }

SlowFileNameFilter::SlowFileNameFilter() noexcept
 {
 }

SlowFileNameFilter::SlowFileNameFilter(StrLen filter)
 {
  prepare(filter);
 }

SlowFileNameFilter::~SlowFileNameFilter() {}

 // methods

void SlowFileNameFilter::reset()
 {
  filter.erase();
  flags.erase();
  list.erase();

  ok=false;
 }

void SlowFileNameFilter::reset(StrLen filter)
 {
  reset();
  prepare(filter);
 }

bool SlowFileNameFilter::operator () (StrLen file) const
 {
  if( !ok ) return false;

  ulen len=filter.getLen();

  // initial state

  list[0]=len;

  ulen count=1;

  // state machine

  while( +file )
    {
     Char ch=CutChar_guarded(file);

     for(bool &flag : flags ) flag=false;

     suffixes(count, [ch,this] (Filter s) { add(s,ch); } );

     count=complete();

     if( !count ) return false;
    }

  // final

  TestFinal test;

  suffixes(count,FunctorRef(test));

  return test;
 }

/* class FileNameFilter::State */

class FileNameFilter::State
 {
   Filter filter;
   DynArray<ulen> list;

  private:

   // all suffixes

   void suffixes(FuncInitArgType<Filter> auto func_init) const
    {
     Suffixes(Range(list),filter,func_init);
    }

   // follow states

   static ulen Normalize(PtrLen<ulen> list)
    {
     DecrSort(list);

     ulen *out=list.ptr;

     Algon::ApplyUniqueRange(list, [&out] (PtrLen<ulen> r) { *(out++)=(*r); } );

     return list.len-Dist(list.ptr,out);
    }

   void complete()
    {
     ulen delta=Normalize(Range(list));

     list.shrink(delta);

     list.shrink_extra();
    }

   void add(Filter s)
    {
     Add(s, [this] (ulen len) { list.append_copy(len); } );
    }

   explicit State(const State *obj)
    : filter(obj->filter),
      list(DoReserve,2*filter.len)
    {
     obj->suffixes( [this] (Filter s) { add(s); } );

     complete();
    }

   void add(Filter s,Char ch)
    {
     Add(s,ch, [this] (ulen len) { list.append_copy(len); } );
    }

   State(const State *obj,Char ch)
    : filter(obj->filter),
      list(DoReserve,2*filter.len)
    {
     obj->suffixes( [this,ch] (Filter s) { add(s,ch); } );

     complete();
    }

  public:

   explicit State(Filter filter_)
    : filter(filter_),
      list(DoReserve,1)
    {
     list.append_copy(filter_.len);
    }

   ~State()
    {
    }

   // std move

   State(State &&obj) = default ;

   State & operator = (State &&obj) = default ;

   // methods

   bool isFinal() const
    {
     TestFinal test;

     suffixes(FunctorRef(test));

     return test;
    }

   void follow(Char buf[],FuncArgType<Char,State &&> auto char_func,FuncArgType<State &&> auto other_func) const // buf.len == filter.len
    {
     bool other=false;

     Char *temp=buf;

     suffixes( [&other,&temp] (Filter s)
                              {
                               if( +s )
                                 {
                                  Char ch=*s;

                                  if( ch=='*' || ch=='?' )
                                    {
                                     if( ch=='*' )
                                       {
                                        for(++s; +s && *s=='*' ;++s);

                                        if( +s )
                                          {
                                           Char ch=*s;

                                           if( ch!='?' ) *(temp++)=ch;
                                          }
                                       }

                                     other=true;
                                    }
                                  else
                                    {
                                     *(temp++)=ch;
                                    }
                                 }

                              } );

     Algon::SortThenApplyUnique(Range(buf,temp), [this,char_func] (Char ch)
                                                                  {
                                                                   char_func(ch,State(this,ch));

                                                                  } );

     if( other )
       {
        other_func(State(this));
       }
    }

   // cmp objects

   CmpResult operator <=> (const State &obj) const
    {
     return RangeCmpOf(list,obj.list);
    }
 };

/* struct FileNameFilter::StateArrow */

struct FileNameFilter::StateArrow
 {
  Char ch;
  FullState *state;

  StateArrow(Char ch_,FullState *state_) : ch(ch_),state(state_) {}
 };

/* class FileNameFilter::FullState */

class FileNameFilter::FullState : NoCopy
 {
  private:

   State state;

   // ext

   ulen index = 0 ;
   DynArray<StateArrow> arrows;
   FullState *other = 0 ;

  public:

   explicit FullState(State &&state_) : state(std::move(state_)) {}

   ~FullState() {}

   // get

   const State & getState() const { return state; }

   ulen getIndex() const { return index; }

   const FullState * getOther() const { return other; }

   PtrLen<const StateArrow> getArrows() const { return Range(arrows); }

   // set

   void setIndex(ulen index_) { index=index_; }

   void setOther(FullState *other_) { other=other_; }

   void addArrow(Char ch,FullState *state) { arrows.append_fill(ch,state); }

   // Cmp()

   friend CmpResult Cmp(const State *a,const FullState &b) { return Cmp(*a,b.getState()); }

   friend CmpResult Cmp(const FullState &a,const FullState &b) { return Cmp(a.getState(),b.getState()); }

   // cmp objects

   CmpResult operator <=> (const FullState &obj) const { return Cmp(*this,obj); }
 };

/* class FileNameFilter::StateMap */

class FileNameFilter::StateMap : NoCopy
 {
   struct Node : MemBase_nocopy
    {
     RBTreeLink<Node,FullState> link;

     explicit Node(State &&state) : link(std::move(state)) {}
    };

   using Algo = RBTreeLink<Node,FullState>::Algo<&Node::link,const State *,NoCopyKey> ;

   ulen max_states;

   Algo::Root root;

   NodePoolAllocator<Node> allocator;

  private:

   void destroy(Node *node)
    {
     if( node )
       {
        destroy(Algo::Link(node).lo);
        destroy(Algo::Link(node).hi);

        allocator.free_nonnull(node);
       }
    }

  public:

   explicit StateMap(ulen max_states_)
    : max_states(max_states_)
    {
    }

   ~StateMap()
    {
     destroy(root.root);
    }

   struct Result
    {
     FullState *obj;
     bool new_flag;

     Result(FullState *obj_,bool new_flag_) : obj(obj_),new_flag(new_flag_) {}

     operator FullState * () const { return obj; }
    };

   Result find_or_add(State &&obj)
    {
     typename Algo::PrepareIns prepare(root,&obj);

     if( Node *node=prepare.found ) return Result(&Algo::Link(node).key,false);

     if( allocator.getCount()>=max_states )
       {
        Printf(Exception,"CCore::FileNameFilter::StateMap::find_or_add(...) : too many states");
       }

     Node *node=allocator.alloc(std::move(obj));

     prepare.complete(node);

     return Result(&Algo::Link(node).key,true);
    }
 };

/* class FileNameFilter::IndState::Arrow */

FileNameFilter::IndState::Arrow::Arrow(const StateArrow &obj)
 {
  ch=obj.ch;
  index=obj.state->getIndex();
 }

/* class FileNameFilter::IndState */

FileNameFilter::IndState::IndState(const FullState *state)
 {
  if( const FullState *other=state->getOther() ) other_index=other->getIndex();

  arrows.extend_cast(state->getArrows());

  is_final=state->getState().isFinal();
 }

 // methods

ulen FileNameFilter::IndState::next(Char ch) const
 {
  for(Arrow arr : arrows ) if( arr.ch==ch ) return arr.index;

  return other_index;
 }

/* class FileNameFilter */

void FileNameFilter::build(StrLen filter_,ulen max_states)
 {
  StateMap map(max_states);
  DynArray<FullState *> list;

  StrToChar filter(filter_);

  {
   State first(filter);

   list.append_copy(map.find_or_add(std::move(first)));
  }

  TempArray<Char,1024> temp((+filter).len);

  for(ulen index=0; index<list.getLen() ;index++)
    {
     FullState *state=list[index];

     state->setIndex(index);

     auto char_func = [&map,&list,state] (Char ch,State &&next_state)
                                         {
                                          auto result=map.find_or_add(std::move(next_state));

                                          if( result.new_flag ) list.append_copy(result);

                                          state->addArrow(ch,result);
                                         } ;

     auto other_func = [&map,&list,state] (State &&next_state)
                                          {
                                           auto result=map.find_or_add(std::move(next_state));

                                           if( result.new_flag ) list.append_copy(result);

                                           state->setOther(result);
                                          } ;

     state->getState().follow(temp.getPtr(),char_func,other_func);
    }

  states.extend_cast(Range_const(list));
 }

FileNameFilter::FileNameFilter() noexcept
 {
 }

FileNameFilter::FileNameFilter(StrLen filter,ulen max_states)
 {
  build(filter,max_states);
 }

FileNameFilter::~FileNameFilter()
 {
 }

void FileNameFilter::reset()
 {
  states.erase();
 }

void FileNameFilter::reset(StrLen filter,ulen max_states)
 {
  states.erase();

  build(filter,max_states);
 }

bool FileNameFilter::operator () (StrLen file) const
 {
  if( !states.getLen() ) return false;

  auto *ptr=states.getPtr();

  ulen index=0;

  while( +file )
    {
     Char ch=CutChar(file);

     ulen next=ptr[index].next(ch);

     if( next==MaxULen ) return false;

     index=next;
    }

  return ptr[index].isFinal();
 }

} // namespace CCore

