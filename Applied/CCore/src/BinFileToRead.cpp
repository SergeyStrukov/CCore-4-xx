/* BinFileToRead.cpp */
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

#include <CCore/inc/BinFileToRead.h>

#include <CCore/inc/FeedBuf.h>

#include <CCore/inc/Exception.h>

namespace CCore {

namespace Private_BinFileToRead {

class NullBinFileToRead : public BinFileToReadBase
 {
  public:

   NullBinFileToRead() {}

   ~NullBinFileToRead() {}

   virtual void open(StrLen,FileOpenFlags)
    {
     Printf(Exception,"CCore::NullBinFileToRead::open(...) : no such file");
    }

   virtual void close()
    {
     Printf(NoException,"CCore::NullBinFileToRead::close() : file is not opened");
    }

   virtual ulen read(FilePosType,uint8 *,ulen)
    {
     Printf(Exception,"CCore::NullBinFileToRead::read(...) : file is not opened");

     return 0;
    }

   virtual void read_all(FilePosType,uint8 *,ulen)
    {
     Printf(Exception,"CCore::NullBinFileToRead::read_all(...) : file is not opened");
    }
 };

NullBinFileToRead Object CCORE_INITPRI_3 ;

} // namespace Private_BinFileToRead

using namespace Private_BinFileToRead;

/* GetNullBinFileToReadPtr() */

BinFileToReadBase * GetNullBinFileToReadPtr() { return &Object; }

/* GuardBinFileTooLong() */

void GuardBinFileTooLong(StrLen file_name,ulen max_len,FilePosType file_len)
 {
  Printf(Exception,"CCore::BinFileToMem::BinFileToMem(...,#.q;,max_len=#;) : file is too long #;",file_name,max_len,file_len);
 }

/* class BinFileToMem */

BinFileToMem::BinFileToMem(BinFileToRead file,StrLen file_name,ulen max_len)
 {
  file->open(file_name);

  try
    {
     auto file_len=file->getLen();

     if( file_len>max_len ) GuardBinFileTooLong(file_name,max_len,file_len);

     ulen len=(ulen)file_len;

     file->read_all(0,alloc(len),len);
    }
  catch(...)
    {
     file->close();

     throw;
    }

  file->close();
 }

/* class DecodeBinFile */

void DecodeBinFile::underflow()
 {
  uint8 *ptr=getBase();

  ulen len=file->read(off,ptr,buf.getLen());

  if( !len )
    {
     Printf(Exception,"CCore::DecodeBinFile::underflow() : EOF");
    }
  else
    {
     cur=Range_const(ptr,len);

     off+=len;
    }
 }

bool DecodeBinFile::underflow_eof()
 {
  uint8 *ptr=getBase();

  ulen len=file->read(off,ptr,buf.getLen());

  if( !len )
    {
     return false;
    }
  else
    {
     cur=Range_const(ptr,len);

     off+=len;

     return true;
    }
 }

 // constructors

DecodeBinFile::DecodeBinFile(const BinFileToRead &file_,StrLen file_name)
 : file(file_),
   buf(BufLen)
 {
  file->open(file_name);
 }

DecodeBinFile::~DecodeBinFile()
 {
  file->close();
 }

 // get

void DecodeBinFile::do_get(uint8 *ptr,ulen len)
 {
  auto dst=Range(ptr,len);

  while( +dst )
    {
     if( !cur ) underflow();

     Pumpup(dst,cur);
    }
 }

} // namespace CCore

