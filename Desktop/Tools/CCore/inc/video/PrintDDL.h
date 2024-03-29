/* PrintDDL.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_PrintDDL_h
#define CCore_inc_video_PrintDDL_h

#include <CCore/inc/CharProp.h>

#include <CCore/inc/video/Point.h>

namespace CCore {
namespace Video {

/* DDLBool() */

inline StrLen DDLBool(bool val) { return val?"True"_c:"False"_c; }

/* classes */

struct DDLPoint;

struct DDLPane;

struct DDLString;

struct DDLPrintableString;

/* struct DDLPoint */

struct DDLPoint
 {
  Point point;

  explicit DDLPoint(const Point &point_) : point(point_) {}

  // print object

  void print(PrinterType auto &out) const
   {
    Printf(out,"{ #; , #; }",point.x,point.y);
   }
 };

/* struct DDLPane */

struct DDLPane
 {
  Pane pane;

  explicit DDLPane(const Pane &pane_) : pane(pane_) {}

  // print object

  void print(PrinterType auto &out) const
   {
    Printf(out,"{ #; , #; , #; , #; }",pane.x,pane.y,pane.dx,pane.dy);
   }
 };

/* struct DDLString */

struct DDLString
 {
  StrLen str;

  explicit DDLString(StrLen str_) : str(str_) {}

  explicit DDLString(const ConstTypeRangeableType<char> auto &obj) : str(Range_const(obj)) {}

  // print object

  static void PrintChar(PrinterType auto &out,char ch)
   {
    switch( ch )
      {
       case '\b' : out.put('\\'); out.put('b'); break;

       case '\t' : out.put('\\'); out.put('t'); break;

       case '\n' : out.put('\\'); out.put('n'); break;

       case '\v' : out.put('\\'); out.put('v'); break;

       case '\f' : out.put('\\'); out.put('f'); break;

       case '\r' : out.put('\\'); out.put('r'); break;

       case '"' : out.put('\\'); out.put('"'); break;

       case '\\' : out.put('\\'); out.put('\\'); break;

       default:
        {
         if( charIsPrintable(ch) ) out.put(ch); else out.put(' ');
        }
      }
   }

  void print(PrinterType auto &out) const
   {
    out.put('"');

    for(char ch : str ) PrintChar(out,ch);

    out.put('"');
   }
 };

/* struct DDLPrintableString */

struct DDLPrintableString
 {
  StrLen str;

  explicit DDLPrintableString(StrLen str_) : str(str_) {}

  explicit DDLPrintableString(const ConstTypeRangeableType<char> auto &obj) : str(Range_const(obj)) {}

  // print object

  static void GuardNotPrintable();

  static void PrintChar(PrinterType auto &out,char ch)
   {
    switch( ch )
      {
       case '"' : out.put('\\'); out.put('"'); break;

       case '\\' : out.put('\\'); out.put('\\'); break;

       default:
        {
         if( charIsPrintable(ch) ) out.put(ch); else GuardNotPrintable();
        }
      }
   }

  void print(PrinterType auto &out) const
   {
    out.put('"');

    for(char ch : str ) PrintChar(out,ch);

    out.put('"');
   }
 };

} // namespace Video
} // namespace CCore

#endif

