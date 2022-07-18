/* InternalUtils.h */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_InternalUtils_h
#define CCore_inc_video_InternalUtils_h

#include <CCore/inc/video/FrameWindow.h>

#include <CCore/inc/sys/SysTypes.h>
#include <CCore/inc/sys/SysError.h>
#include <CCore/inc/sys/SysUtf8.h>

#include <CCore/inc/task/TaskEvent.h>

#include <CCore/inc/PrintError.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/win32/Win32gui.h>

namespace CCore {
namespace Video {
namespace Internal {

/* namespace */

namespace WinNN = Sys::WinNN ;

/* SysGuard() */

void SysGuardFailed(const char *format,Sys::ErrorType error=Sys::NonNullError());

inline void SysGuard(const char *format,bool ok)
 {
  if( !ok ) SysGuardFailed(format);
 }

/* ToPane() */

inline Pane ToPane(WinNN::Rectangle r)
 {
  return Pane((Coord)r.left,(Coord)r.top,(Coord)(r.right-r.left),(Coord)(r.bottom-r.top));
 }

/* GetWorkPane() */

Pane GetWorkPane(Pane pane={});

/* classes */

struct MsgEvent;

struct TickEvent;

class WindowBitmap;

class WindowBuf;

class Clipboard;

class PutToClipboard;

class GetFromClipboard;

class TextToClipboard;

/* struct MsgEvent */

struct MsgEvent
 {
  EventTimeType time;
  EventIdType id;

  uint32 code;
  uint8 flag;

  enum FlagType
   {
    Entry,
    Leave,
    Stop
   };

  void init(EventTimeType time_,EventIdType id_,WinNN::MsgCode code_,FlagType flag_)
   {
    time=time_;
    id=id_;

    code=code_;
    flag=flag_;
   }

  static void * Offset_time(void *ptr) { return &(static_cast<MsgEvent *>(ptr)->time); }

  static void * Offset_id(void *ptr) { return &(static_cast<MsgEvent *>(ptr)->id); }

  static void * Offset_code(void *ptr) { return &(static_cast<MsgEvent *>(ptr)->code); }

  static void * Offset_flag(void *ptr) { return &(static_cast<MsgEvent *>(ptr)->flag); }

  static void Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc);
 };

/* struct TickEvent */

struct TickEvent
 {
  EventTimeType time;
  EventIdType id;

  uint8 flag;

  enum FlagType
   {
    Entry,
    Leave
   };

  void init(EventTimeType time_,EventIdType id_,FlagType flag_)
   {
    time=time_;
    id=id_;

    flag=flag_;
   }

  static void * Offset_time(void *ptr) { return &(static_cast<TickEvent *>(ptr)->time); }

  static void * Offset_id(void *ptr) { return &(static_cast<TickEvent *>(ptr)->id); }

  static void * Offset_flag(void *ptr) { return &(static_cast<TickEvent *>(ptr)->flag); }

  static void Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc);
 };

/* class WindowBitmap */

class WindowBitmap : NoCopy
 {
   WinNN::HBitmap hBitmap;
   void *mem = 0 ;
   Point size;

  public:

   explicit WindowBitmap(Point size);

   ~WindowBitmap();

   WinNN::HBitmap getHandle() const { return hBitmap; }

   Point getSize() const { return size; }

   void * getMem() const { return mem; }

   ColorPlane getPlane() const { return ColorPlane(mem,size.x,size.y,size.x*4u); }
 };

/* class WindowBuf */

class WindowBuf : NoCopy
 {
   WinNN::HGDevice hGD;
   WinNN::HGDObject old_bmp;

   WinNN::HGDObject bmp;
   void *mem = 0 ;
   Point size;

  private:

   void setSize(Point new_size,bool first_time);

  public:

   WindowBuf();

   ~WindowBuf();

   Point getSize() const { return size; }

   void * getMem() const { return mem; }

   ColorPlane getPlane() const { return ColorPlane(mem,size.x,size.y,size.x*4u); }

   void setSize(Point new_size);

   void draw(WinNN::HGDevice dstGD,Pane pane);
 };

/* class Clipboard */

class Clipboard : NoCopy
 {
  public:

   explicit Clipboard(WinNN::HWindow hWnd);

   ~Clipboard();
 };

/* class PutToClipboard */

class PutToClipboard : NoCopy
 {
   WinNN::handle_t h_mem;
   void *mem;

  public:

   explicit PutToClipboard(ulen len);

   ~PutToClipboard();

   void * getMem() const { return mem; }

   void commit(unsigned cbd_format);
 };

/* class GetFromClipboard */

class GetFromClipboard : NoCopy
 {
   WinNN::handle_t h_mem;
   void *mem;
   ulen len;

  public:

   explicit GetFromClipboard(unsigned cbd_format);

   ~GetFromClipboard();

   void * getMem() const { return mem; }

   ulen getLen() const { return len; }
 };

/* class TextToClipboard */

class TextToClipboard : NoCopy
 {
   PtrLen<const Char> text;

  private:

   static void Feed(PtrLen<const Char> text,FuncArgType<Sys::WChar> auto func);

  public:

   explicit TextToClipboard(PtrLen<const Char> text_) : text(text_) {}

   ulen getLen() const;

   void fill(void *mem) const;
 };

} // namespace Internal
} // namespace Video
} // namespace CCore

#endif
