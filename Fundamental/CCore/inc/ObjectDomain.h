/* ObjectDomain.h */
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

#ifndef CCore_inc_ObjectDomain_h
#define CCore_inc_ObjectDomain_h

#include <CCore/inc/Array.h>
#include <CCore/inc/NewDelete.h>
#include <CCore/inc/AnyPtr.h>

namespace CCore {

/* classes */

class ObjectDomain;

template <class T> struct KeepAliveAdapter;

template <class T> struct BreakWeakAdapter;

template <class T> class DeleteObjNode;

template <class T> class ExtDelObjPtr;

template <class T> class IntDelObjPtr;

/* class ObjectDomain */

class ObjectDomain : NoCopy
 {
   struct ObjBase;

  public:

   template <class T> class ExtPtr;

   template <class ... TT> class ExtAnyPtr;

   template <class T> class IntPtr;

   template <class ... TT> class IntAnyPtr;

   template <class T> class WeakPtr;

   template <class T> class OwnPtr;

   class Keeper
    {
      ObjectDomain *domain;

     private:

      friend class ObjectDomain;

      template <class T>
      friend class IntPtr;

      template <class ... TT>
      friend class IntAnyPtr;

      Keeper(ObjectDomain *domain_) : domain(domain_) {}

      void markAlive(ObjBase *obj) { domain->markAlive(obj); }

     public:

      template <class ... TT>
      void operator () (TT & ... tt);
    };

   class Breaker
    {
      ulen preserved;

     private:

      friend class ObjectDomain;

      template <class T>
      friend class WeakPtr;

      Breaker(ulen preserved_) : preserved(preserved_) {}

     public:

      template <class ... TT>
      void operator () (TT & ... tt);
    };

  private:

   template <class Ptr>
   static auto GetNode(Ptr &ptr) { return ptr.node; }

   template <class Ptr>
   static auto GetType(Ptr &ptr) { return ptr.type; }

   struct AllocInit
    {
     ObjectDomain *domain;

     AllocInit(ObjectDomain *domain_) : domain(domain_) {}

     using AllocType = AllocInit ;

     void * alloc(ulen len) { return domain->alloc(len); }

     void free(void *mem,ulen len) noexcept { domain->free(mem,len); }
    };

   struct ObjBase : NoCopy
    {
     ulen index;
     ulen ref_count;

     explicit ObjBase(ulen ref_count_) : ref_count(ref_count_) {}

     ~ObjBase() {}

     virtual void destroy(ObjectDomain *domain)=0;

     virtual void keepAlive(Keeper keeper)=0;

     virtual void breakWeak(Breaker breaker)=0;

     void incRef() { ++ref_count; }

     void decRef() { --ref_count; }
    };

   template <class T> struct ObjNode;

   struct ObjRec
    {
     ObjBase *obj;

     explicit ObjRec(ObjBase *obj_) : obj(obj_) {}
    };

  private:

   DynArray<ObjRec> list;
   ulen total_len;
   ulen max_total_len;
   bool cleanup_flag;
   bool addlock_flag;

   // collect state

   ulen preserved;

  private:

   void * alloc(ulen len);

   void free(void *mem,ulen len) noexcept;

   virtual void * try_alloc(ulen len);

   virtual void free(void *mem) noexcept;

  private:

   void add(ObjBase *obj);

   void markAlive(ObjBase *obj);

   void swap(ulen index1,ulen index2);

  protected:

   void cleanup(); // must be called in the destructor of a derived class

  public:

   // constructors

   explicit ObjectDomain(ulen max_total_len);

   ~ObjectDomain();

   void collect();

   // props

   ulen getObjectCount() const { return list.getLen(); }

   ulen getTotalLen() const { return total_len; }

   ulen getMaxTotalLen() const { return max_total_len; }

   ulen getAvail() const { return max_total_len-total_len; }
 };

/* concept Has_keepAlive<T> */

template <class T>
concept Has_keepAlive = requires()
 {
  Ground<void (T::*)(ObjectDomain::Keeper)>( &T::keepAlive );
 } ;

/* concept No_keepAlive<T> */

template <class T>
concept No_keepAlive = !Has_keepAlive<T> ;

/* struct KeepAliveAdapter<T> */

template <Has_keepAlive T>
struct KeepAliveAdapter<T>
 {
  static void KeepAlive(T &obj,ObjectDomain::Keeper keeper) { obj.keepAlive(keeper); }
 };

template <No_keepAlive T>
struct KeepAliveAdapter<T>
 {
  static void KeepAlive(T &,ObjectDomain::Keeper) {}
 };

/* concept Has_breakWeak<T> */

template <class T>
concept Has_breakWeak = requires()
 {
  Ground<void (T::*)(ObjectDomain::Breaker)>( &T::breakWeak );
 } ;

/* concept No_breakWeak<T> */

template <class T>
concept No_breakWeak = !Has_breakWeak<T> ;

/* struct BreakWeakAdapter<T> */

template <Has_breakWeak T>
struct BreakWeakAdapter<T>
 {
  static void BreakWeak(T &obj,ObjectDomain::Breaker breaker) { obj.breakWeak(breaker); }
 };

template <No_breakWeak T>
struct BreakWeakAdapter<T>
 {
  static void BreakWeak(T &,ObjectDomain::Breaker) {}
 };

/* struct ObjectDomain::ObjNode<T> */

template <class T>
struct ObjectDomain::ObjNode : ObjBase
 {
  T obj;

  template <class ... SS> requires ( ConstructibleType<T,SS...> )
  ObjNode(ObjectDomain *domain,ulen ref_count,SS && ... ss)
   : ObjBase(ref_count),
     obj( std::forward<SS>(ss)... )
   {
    domain->add(this);
   }

  ~ObjNode() {}

  virtual void destroy(ObjectDomain *domain) { Delete<ObjNode<T>,AllocInit>(domain,this); }

  virtual void keepAlive(Keeper keeper) { KeepAliveAdapter<T>::KeepAlive(obj,keeper); }

  virtual void breakWeak(Breaker breaker) { BreakWeakAdapter<T>::BreakWeak(obj,breaker); }

  T * getPtr() { return &obj; }
 };

/* class ObjectDomain::ExtPtr<T> */

template <class T>
class ObjectDomain::ExtPtr
 {
   ObjNode<T> *node;

   template <class Ptr>
   friend auto ObjectDomain::GetNode(Ptr &obj);

  private:

   template <class ... TT>
   friend class ExtAnyPtr;

   explicit ExtPtr(ObjBase *node_) : node(static_cast<ObjNode<T> *>(node_)) { if( node ) node->incRef(); }

  public:

   // constructors

   ExtPtr() noexcept : node(0) {}

   ExtPtr(NothingType) : node(0) {}

   ExtPtr(const IntPtr<T> &obj) : node(GetNode(obj)) { if( node ) node->incRef(); }

   ExtPtr(const WeakPtr<T> &obj) : node(GetNode(obj)) { if( node ) node->incRef(); }

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit ExtPtr(ObjectDomain *domain,SS && ... ss)
    {
     node=New<ObjNode<T>,AllocInit>(domain, domain,1, std::forward<SS>(ss)... );
    }

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit ExtPtr(ObjectDomain &domain,SS && ... ss)
    {
     node=New<ObjNode<T>,AllocInit>(&domain, &domain,1, std::forward<SS>(ss)... );
    }

   ~ExtPtr() { if( node ) node->decRef(); }

   void nullify()
    {
     if( ObjNode<T> *temp=Replace_null(node) ) temp->decRef();
    }

   template <class ... SS>
   void create(SS && ... ss) { (*this)=ExtPtr( std::forward<SS>(ss)... ); }

   // copying

   ExtPtr(const ExtPtr<T> &obj) noexcept
    : node(obj.node)
    {
     if( node ) node->incRef();
    }

   ExtPtr<T> & operator = (const ExtPtr<T> &obj) noexcept
    {
     ObjNode<T> *new_node=obj.node;
     ObjNode<T> *old_node=Replace(node,new_node);

     if( new_node ) new_node->incRef();
     if( old_node ) old_node->decRef();

     return *this;
    }

   // object ptr

   void * operator + () const { return node; }

   bool operator ! () const { return !node; }

   T * getPtr() const { return node->getPtr(); }

   T & operator * () const { return *node->getPtr(); }

   T * operator -> () const { return node->getPtr(); }

   ulen getExtRefs() const { return node->ref_count; }

   // swap/move objects

   void objSwap(ExtPtr<T> &obj) noexcept
    {
     Swap(node,obj.node);
    }

   explicit ExtPtr(ToMoveCtor<ExtPtr<T> > obj) noexcept
    : node(Replace_null(obj->node))
    {
    }
 };

/* class ObjectDomain::ExtAnyPtr<TT> */

template <class ... TT>
class ObjectDomain::ExtAnyPtr
 {
   ObjBase *node;
   unsigned type;

   template <class Ptr>
   friend auto ObjectDomain::GetNode(Ptr &obj);

   template <class Ptr>
   friend auto ObjectDomain::GetType(Ptr &obj);

  private:

   template <class T>
   static AnyPtr<TT...> GetPtr(ObjBase *node)
    {
     T *ptr;

     if( node )
       ptr=static_cast<ObjNode<T> *>(node)->getPtr();
     else
       ptr=0;

     return ptr;
    }

   template <class T,class FuncInit,class ... SS>
   static void ApplyToNode(ObjBase *node,const FuncInit &func_init,SS && ... ss)
    {
     ExtPtr<T> ptr(node);

     FunctorTypeOf<FuncInit> func(func_init);

     func(ptr, std::forward<SS>(ss)... );
    }

  public:

   // constructors

   ExtAnyPtr() noexcept : node(0),type(0) {}

   ExtAnyPtr(NothingType) : node(0),type(0) {}

   template <class T>
   ExtAnyPtr(const IntPtr<T> &obj) : node(GetNode(obj)),type(Meta::IndexOf<T,TT...>) { if( node ) node->incRef(); }

   template <class T>
   ExtAnyPtr(const ExtPtr<T> &obj) : node(GetNode(obj)),type(Meta::IndexOf<T,TT...>) { if( node ) node->incRef(); }

   template <class T>
   ExtAnyPtr(const WeakPtr<T> &obj) : node(GetNode(obj)),type(Meta::IndexOf<T,TT...>) { if( node ) node->incRef(); }

   ExtAnyPtr(const IntAnyPtr<TT...> &obj) : node(GetNode(obj)),type(GetType(obj)) {}

   ~ExtAnyPtr() { if( node ) node->decRef(); }

   void nullify()
    {
     type=0;

     if( ObjBase *temp=Replace_null(node) ) temp->decRef();
    }

   template <class T,class ... SS>
   void create(SS && ... ss) { (*this)=ExtAnyPtr<TT...>( std::forward<SS>(ss)... ); }

   // copying

   ExtAnyPtr(const ExtAnyPtr<TT...> &obj) noexcept
    : node(obj.node),
      type(obj.type)
    {
     if( node ) node->incRef();
    }

   ExtAnyPtr<TT...> & operator = (const ExtAnyPtr<TT...> &obj) noexcept
    {
     type=obj.type;

     ObjBase *new_node=obj.node;
     ObjBase *old_node=Replace(node,new_node);

     if( new_node ) new_node->incRef();
     if( old_node ) old_node->decRef();

     return *this;
    }

   // methods

   void * operator + () const { return node; }

   bool operator ! () const { return !node; }

   AnyPtr<TT...> getPtr() const
    {
     if( type )
       {
        using GetFuncType = AnyPtr<TT...> (*)(ObjBase *node) ;

        static const GetFuncType Table[]={ GetPtr<TT>... };

        return Table[type-1](node);
       }

     return Null;
    }

   template <class FuncInit,class ... SS>
   void apply(const FuncInit &func_init,SS && ... ss) const requires ( ... && FuncInitArgType<FuncInit,ExtPtr<TT>,SS...> )
    {
     if( type )
       {
        using FuncType = void (*)(ObjBase *node,const FuncInit &func_init,SS && ...) ;

        static const FuncType Table[]={ ApplyToNode<TT,FuncInit,SS...>... };

        Table[type-1](node,func_init, std::forward<SS>(ss)... );
       }
    }

   ulen getExtRefs() const { return node->ref_count; }

   // swap/move objects

   void objSwap(ExtAnyPtr<TT...> &obj) noexcept
    {
     Swap(type,obj.type);
     Swap(node,obj.node);
    }

   explicit ExtAnyPtr(ToMoveCtor<ExtAnyPtr<TT...> > obj) noexcept
    : node(Replace_null(obj->node)),
      type(obj->type)
    {
    }
 };

/* class ObjectDomain::IntPtr<T> */

template <class T>
class ObjectDomain::IntPtr // default copying
 {
   ObjNode<T> *node;

   template <class Ptr>
   friend auto ObjectDomain::GetNode(Ptr &obj);

  private:

   template <class ... TT>
   friend class IntAnyPtr;

   explicit IntPtr(ObjBase *node_) : node(static_cast<ObjNode<T> *>(node_)) {}

  public:

   // constructors

   IntPtr() noexcept : node(0) {}

   IntPtr(NothingType) : node(0) {}

   IntPtr(const ExtPtr<T> &obj) : node(GetNode(obj)) {}

   IntPtr(const WeakPtr<T> &obj) : node(GetNode(obj)) {}

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit IntPtr(ObjectDomain *domain,SS && ... ss)
    {
     node=New<ObjNode<T>,AllocInit>(domain, domain,0, std::forward<SS>(ss)... );
    }

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit IntPtr(ObjectDomain &domain,SS && ... ss)
    {
     node=New<ObjNode<T>,AllocInit>(&domain, &domain,0, std::forward<SS>(ss)... );
    }

   void nullify() { node=0; }

   template <class ... SS>
   void create(SS && ... ss) { (*this)=IntPtr( std::forward<SS>(ss)... ); }

   // object ptr

   void * operator + () const { return node; }

   bool operator ! () const { return !node; }

   T * getPtr() const { return node->getPtr(); }

   T & operator * () const { return *node->getPtr(); }

   T * operator -> () const { return node->getPtr(); }

   ulen getExtRefs() const { return node->ref_count; }

   void keepAlive(Keeper keeper)
    {
     keeper.markAlive(node);
    }
 };

/* class ObjectDomain::IntAnyPtr<TT> */

template <class ... TT>
class ObjectDomain::IntAnyPtr // default copying
 {
   ObjBase *node;
   unsigned type;

   template <class Ptr>
   friend auto ObjectDomain::GetNode(Ptr &obj);

   template <class Ptr>
   friend auto ObjectDomain::GetType(Ptr &obj);

  private:

   template <class T>
   static AnyPtr<TT...> GetPtr(ObjBase *node)
    {
     T *ptr;

     if( node )
       ptr=static_cast<ObjNode<T> *>(node)->getPtr();
     else
       ptr=0;

     return ptr;
    }

   template <class T,class FuncInit,class ... SS>
   static void ApplyToNode(ObjBase *node,const FuncInit &func_init,SS && ... ss)
    {
     IntPtr<T> ptr(node);

     FunctorTypeOf<FuncInit> func(func_init);

     func(ptr, std::forward<SS>(ss)... );
    }

  public:

   // constructors

   IntAnyPtr() noexcept : node(0),type(0) {}

   IntAnyPtr(NothingType) : node(0),type(0) {}

   template <class T>
   IntAnyPtr(const IntPtr<T> &obj) : node(GetNode(obj)),type(Meta::IndexOf<T,TT...>) {}

   template <class T>
   IntAnyPtr(const ExtPtr<T> &obj) : node(GetNode(obj)),type(Meta::IndexOf<T,TT...>) {}

   template <class T>
   IntAnyPtr(const WeakPtr<T> &obj) : node(GetNode(obj)),type(Meta::IndexOf<T,TT...>) {}

   IntAnyPtr(const ExtAnyPtr<TT...> &obj) : node(GetNode(obj)),type(GetType(obj)) {}

   void nullify() { node=0; type=0; }

   template <class T,class ... SS>
   void create(SS && ... ss) { (*this)=IntPtr<T>( std::forward<SS>(ss)... ); }

   // methods

   void * operator + () const { return node; }

   bool operator ! () const { return !node; }

   AnyPtr<TT...> getPtr() const
    {
     if( type )
       {
        using GetFuncType = AnyPtr<TT...> (*)(ObjBase *node) ;

        static const GetFuncType Table[]={ GetPtr<TT>... };

        return Table[type-1](node);
       }

     return Null;
    }

   template <class FuncInit,class ... SS>
   void apply(FuncInit func_init,SS && ... ss) const requires ( ... && FuncInitArgType<FuncInit,IntPtr<TT>,SS...> )
    {
     if( type )
       {
        using FuncType = void (*)(ObjBase *node,const FuncInit &func_init,SS && ...) ;

        static const FuncType Table[]={ ApplyToNode<TT,FuncInit,SS...>... };

        Table[type-1](node,func_init, std::forward<SS>(ss)... );
       }
    }

   ulen getExtRefs() const { return node->ref_count; }

   void keepAlive(Keeper keeper)
    {
     keeper.markAlive(node);
    }
 };

/* class ObjectDomain::WeakPtr<T> */

template <class T>
class ObjectDomain::WeakPtr // default copying
 {
   ObjNode<T> *node;

   template <class Ptr>
   friend auto ObjectDomain::GetNode(Ptr &obj);

   friend class Breaker;

  private:

   void breakWeak(ulen preserved)
    {
     if( node )
       {
        if( node->index>=preserved )
          {
           node=0;
          }
       }
    }

  public:

   // constructors

   WeakPtr() noexcept : node(0) {}

   WeakPtr(NothingType) : node(0) {}

   WeakPtr(const ExtPtr<T> &obj) : node(GetNode(obj)) {}

   WeakPtr(const IntPtr<T> &obj) : node(GetNode(obj)) {}

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit WeakPtr(ObjectDomain *domain,SS && ... ss)
    {
     node=New<ObjNode<T>,AllocInit>(domain, domain,0, std::forward<SS>(ss)... );
    }

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit WeakPtr(ObjectDomain &domain,SS && ... ss)
    {
     node=New<ObjNode<T>,AllocInit>(&domain, &domain,0, std::forward<SS>(ss)... );
    }

   void nullify() { node=0; }

   template <class ... SS>
   void create(SS && ... ss) { (*this)=WeakPtr( std::forward<SS>(ss)... ); }

   // object ptr

   void * operator + () const { return node; }

   bool operator ! () const { return !node; }

   T * getPtr() const { return node->getPtr(); }

   T & operator * () const { return *node->getPtr(); }

   T * operator -> () const { return node->getPtr(); }

   ulen getExtRefs() const { return node->ref_count; }

   void breakWeak(Breaker breaker)
    {
     breakWeak(breaker.preserved);
    }
 };

/* class ObjectDomain::OwnPtr<T> */

template <class T>
class ObjectDomain::OwnPtr : NoCopy
 {
   struct Node : NoCopy
    {
     ObjectDomain *domain;
     T obj;

     template <class ... SS>
     explicit Node(ObjectDomain *domain_,SS && ... ss) : domain(domain_),obj( std::forward<SS>(ss)... ) {}

     ~Node() {}

     T * getPtr() { return &obj; }
    };

   Node *node;

  private:

   static void Destroy(Node *node)
    {
     if( node )
       {
        ObjectDomain *domain=node->domain;

        Delete<Node,AllocInit>(domain,node);
       }
    }

  public:

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit OwnPtr(ObjectDomain *domain,SS && ... ss)
    {
     node=New<Node,AllocInit>(domain, domain, std::forward<SS>(ss)... );
    }

   ~OwnPtr()
    {
     Destroy(node);
    }

   // object ptr

   void * operator + () const { return node; }

   bool operator ! () const { return !node; }

   T * getPtr() const { return node->getPtr(); }

   T & operator * () const { return *node->getPtr(); }

   T * operator -> () const { return node->getPtr(); }

   // destroy

   void destroy()
    {
     Destroy(Replace_null(node));
    }
 };

/* types */

template <class T>
using ExtObjPtr = ObjectDomain::ExtPtr<T> ;

template <class ... TT>
using ExtAnyObjPtr = ObjectDomain::ExtAnyPtr<TT...> ;

template <class T>
using IntObjPtr = ObjectDomain::IntPtr<T> ;

template <class ... TT>
using IntAnyObjPtr = ObjectDomain::IntAnyPtr<TT...> ;

template <class T>
using WeakObjPtr = ObjectDomain::WeakPtr<T> ;

/* class DeleteObjNode<T> */

template <class T>
class DeleteObjNode : NoCopy
 {
   ObjectDomain::OwnPtr<T> ptr;

  public:

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   explicit DeleteObjNode(ObjectDomain *domain,SS && ... ss)
    : ptr(domain, std::forward<SS>(ss)... )
    {
    }

   ~DeleteObjNode()
    {
    }

   void destroy()
    {
     ptr.destroy();
    }

   void * hasPtr() const { return +ptr; }

   T * getPtr() const { return ptr.getPtr(); }

   template <class Keeper>
   void keepAlive(Keeper keeper)
    {
     if( +ptr ) KeepAliveAdapter<T>::KeepAlive(*ptr,keeper);
    }

   template <class Breaker>
   void breakWeak(Breaker breaker)
    {
     if( +ptr ) BreakWeakAdapter<T>::BreakWeak(*ptr,breaker);
    }
 };

/* class ExtDelObjPtr<T> */

template <class T>
class ExtDelObjPtr
 {
   ExtObjPtr< DeleteObjNode<T> > ptr;

   friend class IntDelObjPtr<T>;

  public:

   // constructors

   ExtDelObjPtr() noexcept {}

   ExtDelObjPtr(NothingType) {}

   ExtDelObjPtr(const IntDelObjPtr<T> &obj) : ptr(obj.ptr) {}

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   ExtDelObjPtr(ObjectDomain *domain,SS && ... ss)
    : ptr(domain, domain, std::forward<SS>(ss)... )
    {
    }

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   ExtDelObjPtr(ObjectDomain &domain,SS && ... ss)
    : ptr(&domain, &domain, std::forward<SS>(ss)... )
    {
    }

   ~ExtDelObjPtr() {}

   void nullify() { ptr.nullify(); }

   template <class ... SS>
   void create(SS && ... ss) { (*this)=ExtDelObjPtr( std::forward<SS>(ss)... ); }

   // object ptr

   void * operator + () const { return +ptr; }

   bool operator ! () const { return !ptr; }

   T * getPtr() const { return ptr->getPtr(); }

   T & operator * () const { return *getPtr(); }

   T * operator -> () const { return getPtr(); }

   // destroy

   bool destroy()
    {
     if( +ptr )
       {
        if( ptr.getExtRefs()>1 ) return false;

        ptr->destroy();

        ptr.nullify();
       }

     return true;
    }
 };

/* class IntDelObjPtr<T> */

template <class T>
class IntDelObjPtr
 {
   IntObjPtr< DeleteObjNode<T> > ptr;

   friend class ExtDelObjPtr<T>;
   friend class ObjectDomain::Keeper;

  private:

   IntObjPtr< DeleteObjNode<T> > getInnerPtr()
    {
     if( +ptr && !ptr->hasPtr() ) ptr.nullify();

     return ptr;
    }

  public:

   // constructors

   IntDelObjPtr() noexcept {}

   IntDelObjPtr(NothingType) {}

   IntDelObjPtr(const ExtDelObjPtr<T> &obj) : ptr(obj.ptr) {}

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   IntDelObjPtr(ObjectDomain *domain,SS && ... ss)
    : ptr(domain, domain, std::forward<SS>(ss)... )
    {
    }

   template <class ... SS> requires ( ConstructibleType<T,SS...> )
   IntDelObjPtr(ObjectDomain &domain,SS && ... ss)
    : ptr(&domain, &domain, std::forward<SS>(ss)... )
    {
    }

   void nullify() { ptr.nullify(); }

   template <class ... SS>
   void create(SS && ... ss) { (*this)=IntDelObjPtr( std::forward<SS>(ss)... ); }

   // object ptr

   void * operator + () const { return +ptr && ptr->hasPtr() ; }

   bool operator ! () const { return !ptr || !ptr->hasPtr() ; }

   T * getPtr() const { return ptr->getPtr(); }

   T & operator * () const { return *getPtr(); }

   T * operator -> () const { return getPtr(); }

   // destroy

   bool destroy()
    {
     if( +ptr )
       {
        if( ptr.getExtRefs() ) return false;

        ptr->destroy();

        ptr.nullify();
       }

     return true;
    }

   void keepAlive(ObjectDomain::Keeper keeper)
    {
     getInnerPtr().keepAlive(keeper);
    }
 };

/* class ObjectDomain::Keeper */

template <class ... TT>
void ObjectDomain::Keeper::operator () (TT & ... tt)
 {
  ( KeepAliveAdapter<TT>::KeepAlive(tt,*this) , ... );
 }

/* class ObjectDomain::Breaker */

template <class ... TT>
void ObjectDomain::Breaker::operator () (TT & ... tt)
 {
  ( BreakWeakAdapter<TT>::BreakWeak(tt,*this) , ... );
 }

} // namespace CCore

#endif


