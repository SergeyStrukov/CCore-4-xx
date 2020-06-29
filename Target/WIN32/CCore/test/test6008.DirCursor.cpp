/* test6008.DirCursor.cpp */
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

#include <CCore/inc/FileSystem.h>
#include <CCore/inc/PrintTitle.h>

namespace App {

namespace Private_6008 {

/* show() */

void show(StrLen dir_name)
 {
  Printf(Con,"#;\n\n",PrintTitle(dir_name));

  FileSystem fs;
  FileSystem::DirCursor cur(fs,dir_name);

  cur.apply( [] (StrLen file_name,FileType file_type)
                {
                 Printf(Con,"#; #;\n",file_name,file_type);

                } );

  Putobj(Con,"\n"_c);
 }

} // namespace Private_6008

using namespace Private_6008;

/* Testit<6008> */

template<>
const char *const Testit<6008>::Name="Test6008 DirCursor";

template<>
bool Testit<6008>::Main()
 {
  show(".");
  show("./");

  show("D:");
  show("D:/");

  return true;
 }

} // namespace App

