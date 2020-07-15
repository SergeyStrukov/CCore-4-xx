/* AbstFileToMem.cpp */
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

#include <CCore/inc/AbstFileToMem.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class AbstFileToMem */

AbstFileToMem::AbstFileToMem(AbstFileToRead file,StrLen file_name,ulen max_len)
 {
  file->open(file_name);

  ScopeGuard guard( [&] () { file->close(); } );

  auto file_len=file->getLen();

  if( file_len>max_len )
    {
     Printf(Exception,"CCore::AbstFileToMem::AbstFileToMem(...,#.q;,max_len=#;) : file is too long #;",file_name,max_len,file_len);
    }

  ulen len=(ulen)file_len;

  file->read_all(0,alloc(len),len);
 }

} // namespace CCore

