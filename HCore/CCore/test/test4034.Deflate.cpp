/* test4034.Deflate.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/Deflate.h>
#include <CCore/inc/PlatformRandom.h>

#include <CCore/inc/Timer.h>
#include <CCore/inc/PrintTime.h>

#include <CCore/inc/FileToMem.h>
#include <CCore/inc/FileSystem.h>
#include <CCore/inc/MakeFileName.h>

#include <CCore/inc/CoTask.h>

namespace App {

namespace Private_4034 {

namespace Alt {

using namespace Deflate;

/* class Inflator */

class Inflator : CoTaskStack<10>
 {
   WindowOut out;
   BitReader reader;
   bool eof = false ;

   // params

   bool repeat;
   Function<void (void)> trigger;

   // decoders

   HuffmanDecoder dynamic_literal_decoder;
   HuffmanDecoder dynamic_distance_decoder;

  private:

   bool noeof = false ;

   CoTask<void> waitBits();

   CoTask<void> cowaitbits;

   auto wait(bool noeof_) { noeof=noeof_; return cocall(cowaitbits); }

  private:

   BitLen bitlen = 0 ;

   CoTask<void> processBits();

   CoTask<void> cobits;

   auto decodeBits(unsigned bitlen_) { bitlen=bitlen_; return cocall(cobits); }

   auto waitMoreBits() { return decodeBits(reader.bitsBuffered()+1); }

  private:

   CoTask<void> processCode();

   CoTask<void> cocode;

  private:

   bool last_block = false ;
   UCode block_type = 0 ;
   ulen stored_len = 0 ;

   CoTask<void> processHeader();

   CoTask<void> coheader;

  private:

   CoTask<void> processBody();

   CoTask<void> cobody;

  private:

   CoTask<void> process();

   CoTask<void> coproc;

  public:

   explicit Inflator(OutFunc out,bool repeat=false);

   ~Inflator();

   void setTrigger(Function<void (void)> trigger_) { trigger=trigger_; }

   void put(const uint8 *ptr,ulen len) { put({ptr,len}); }

   void put(PtrLen<const uint8> data);

   void complete();
 };

CoTask<void> Inflator::waitBits()
 {
  for(;;)
    {
     while( !eof && reader.isEmpty() )
       {
        co_await std::suspend_always{};
       }

     if( eof && noeof )
       {
        Printf(Exception,"CCore::Deflate::Inflator::processBody() : unexpected EOF");
       }

     co_await coret();
    }
 }

CoTask<void> Inflator::processBits()
 {
  for(;;)
    {
     while( !reader.fillBuffer(bitlen) )
       {
        co_await std::suspend_always{};
       }

     co_await coret();
    }
 }

CoTask<void> Inflator::processCode()
 {
  for(;;)
    {
     BitLen bitlens[286+30];

     // 1

     co_await decodeBits(5+5+4);

     unsigned nlit=reader.getBits(5)+257;

     if( nlit>286 )
       {
        Printf(Exception,"CCore::Deflate::Inflator::decodeCode() : incorrect coder data");
       }

     unsigned ndist=reader.getBits(5)+1;

     if( ndist>30 )
       {
        Printf(Exception,"CCore::Deflate::Inflator::decodeCode() : incorrect coder data");
       }

     unsigned hclen=reader.getBits(4)+4;

     Range(bitlens,19).set_null();

     for(unsigned i=0; i<hclen ;i++)
       {
        co_await decodeBits(3);

        bitlens[Order[i]]=reader.getBits(3);
       }

     HuffmanDecoder decoder({bitlens,19});

     // 2

     unsigned len=nlit+ndist;

     for(unsigned i=0; i<len ;)
       {
        USym k;

        co_await decodeBits(decoder.getMaxCodeBits());

        decoder.decodePrepared(reader,k);

        if( k<=15 )
          {
           bitlens[i++]=k;
          }
        else
          {
           unsigned count=0;
           unsigned repeater=0;

           switch( k )
             {
              case 16:
               {
                co_await decodeBits(2);

                count=3+reader.getBits(2);

                if( i==0 )
                  {
                   Printf(Exception,"CCore::Deflate::Inflator::decodeCode() : incorrect coder data");
                  }

                repeater=bitlens[i-1];
               }
              break;

              case 17:
               {
                co_await decodeBits(3);

                count=3+reader.getBits(3);
               }
              break;

              case 18:
               {
                co_await decodeBits(7);

                count=11+reader.getBits(7);
               }
              break;
             }

           if( i+count>len )
             {
              Printf(Exception,"CCore::Deflate::Inflator::decodeCode() : incorrect coder data");
             }

           Range(bitlens+i,count).set(repeater);

           i+=count;
          }
       }

     // 3

     dynamic_literal_decoder.init({bitlens,nlit});

     if( ndist==1 && bitlens[nlit]==0 )
       {
        if( nlit!=257 )
          {
           Printf(Exception,"CCore::Deflate::Inflator::decodeCode() : incorrect coder data");
          }
       }
     else
       {
        dynamic_distance_decoder.init({bitlens+nlit,ndist});
       }

     co_await coret();
    }
 }

CoTask<void> Inflator::processHeader()
 {
  for(;;)
    {
     co_await decodeBits(3);

     last_block=( reader.getBits(1)!=0 );

     block_type=reader.getBits(2);

     switch( block_type )
       {
        case Stored :
         {
          reader.align8();

          co_await decodeBits(32);

          uint16 len=(uint16)reader.getBits(16);
          uint16 nlen=(uint16)reader.getBits(16);

          stored_len=len;

          if( nlen!=(uint16)~len )
            {
             Printf(Exception,"CCore::Deflate::Inflator::decodeHeader() : incorrect stored length");
            }
         }
        break;

        case Static :
         {
         }
        break;

        case Dynamic :
         {
          co_await cocall(cocode);
         }
        break;

        default:
         {
          Printf(Exception,"CCore::Deflate::Inflator::decodeHeader() : incorrect block type");
         }
       }

     co_await coret();
    }
 }

CoTask<void> Inflator::processBody()
 {
  for(;;)
    {
     if( block_type==Stored )
       {
        while( stored_len )
          {
           co_await wait(true);

           stored_len=reader.pumpToCap(out,stored_len);
          }
       }
     else
       {
        const HuffmanDecoder &literal_decoder = (block_type==Static)? StaticCoder<HuffmanDecoder,StaticLiteralBitlens>::Get()
                                                                    : dynamic_literal_decoder ;

        const HuffmanDecoder &distance_decoder = (block_type==Static)? StaticCoder<HuffmanDecoder,StaticDistanceBitlens>::Get()
                                                                     : dynamic_distance_decoder ;

        for(;;)
          {
           USym literal;

           while( !literal_decoder.decode(reader,literal) ) co_await waitMoreBits();

           if( literal<256 )
             {
              out.put((uint8)literal);
             }
           else if( literal==256 )
             {
              break;
             }
           else
             {
              if( literal>=286 )
                {
                 Printf(Exception,"CCore::Deflate::Inflator::processBody() : incorrect literal");
                }

              {
               unsigned bits=LengthExtraBits[literal-257];

               co_await decodeBits(bits);

               literal=reader.getBits(bits)+LengthBases[literal-257];
              }

              USym distance;

              while( !distance_decoder.decode(reader,distance) ) co_await waitMoreBits();

              if( distance>=30 )
                {
                 Printf(Exception,"CCore::Deflate::Inflator::processBody() : incorrect distance");
                }

              {
               unsigned bits=DistanceExtraBits[distance];

               co_await decodeBits(bits);

               distance=reader.getBits(bits)+DistanceBases[distance];
              }

             out.put(distance,literal);
            }
         }
      }

     co_await coret();
    }
 }

CoTask<void> Inflator::process()
 {
  for(;;)
    {
     co_await wait(false);

     if( eof && reader.isEmpty() ) co_return;

     co_await cocall(coheader);

     co_await cocall(cobody);

     if( last_block )
       {
        out.flush();

        reader.align8();

        trigger();

        if( repeat )
          {
           out.reset();
          }
        else
          {
           reader.pumpTo(out);

           co_return;
          }
       }
    }
 }

Inflator::Inflator(OutFunc out_,bool repeat_)
 : out(out_),
   repeat(repeat_)
 {
  cowaitbits=waitBits();
  cobits=processBits();
  cocode=processCode();
  coheader=processHeader();
  cobody=processBody();
  coproc=process();

  prepare(coproc);
 }

Inflator::~Inflator()
 {
 }

void Inflator::put(PtrLen<const uint8> data)
 {
  out.flush();

  if( !data ) return;

  reader.extend(data);

  push();

  if( !reader.bufferize() )
    {
     Printf(Exception,"CCore::Deflate::Inflator::put() : extra data");
    }
 }

void Inflator::complete()
 {
  eof=true;

  if( !push() )
    {
     Printf(Exception,"CCore::Deflate::Inflator::complete() : unexpected EOF");
    }
 }

} // namespace Alt

/* Rat() */

unsigned Rat(ulen a,ulen b)
 {
  if( !b ) return 100;

  return (a*100)/b;
 }

/* class Transform<T> */

template <class T>
class Transform : public Funchor_nocopy
 {
   PtrLen<const uint8> data;

   T work;

   Collector<uint8> result;

  private:

   void out(PtrLen<const uint8> r)
    {
     result.extend_copy(r);
    }

   Function<void (PtrLen<const uint8>)> function_out() { return FunctionOf(this,&Transform::out); }

  public:

   template <class ... SS>
   Transform(PtrLen<const uint8> data_,SS && ... ss)
    : data(data_),
      work(function_out(), std::forward<SS>(ss)... )
    {
    }

   ~Transform()
    {
    }

   PtrLen<const uint8> getResult() { return result.flat(); }

   void run(PlatformRandom &random)
    {
     ulen off=0;

     while( off<data.len )
       {
        ulen len=Min<ulen>(data.len-off,random.select(1000));

        work.put(data.part(off,len));

        off+=len;
       }

     work.complete();
    }
 };

/* class Engine */

class Engine : NoCopy
 {
   PlatformRandom random;
   StrLen file_name;
   PtrLen<const uint8> data;
   Deflate::Param param;

  private:

   void test()
    {
     // 1

     Transform<Deflate::Deflator> zip(data,param);

     zip.run(random);

     auto zipped=zip.getResult();

     // 2

     Transform<Alt::Inflator> unzip(zipped);

     unzip.run(random);

     auto unzipped=unzip.getResult();

     // 3

     if( data.equal(unzipped) )
       {
        Printf(Con,"#; : #; -> #; (#; %)\n",file_name,data.len,zipped.len,Rat(zipped.len,data.len));
       }
     else
       {
        Printf(Exception,"#; : failed",file_name);
       }
    }

   void test(Deflate::Level level,Deflate::Log2WindowLen log2_window_len)
    {
     param.log2_window_len=log2_window_len;

     Printf(Con,"#;.#;) ",level,log2_window_len);

     test();
    }

   void test(Deflate::Level level)
    {
     param.level=level;

     test(level,Deflate::MinLog2WindowLen);
     test(level,Deflate::MaxLog2WindowLen);
    }

  public:

   Engine() {}

   ~Engine() {}

   void test(StrLen dir,StrLen file_name)
    {
     this->file_name=file_name;

     MakeFileName path(dir,file_name);
     FileToMem file(path.get());

     data=Range(file);
     param={};

     test(Deflate::MinLevel);
     test(Deflate::DefaultLevel);
     test(Deflate::EverdynamicLevel);
     test(Deflate::MaxLevel);

     Putch(Con,'\n');
    }
 };

/* Test() */

void Test(StrLen dir)
 {
  MSecTimer timer;

  FileSystem fs;
  FileSystem::DirCursor cur(fs,dir);

  Engine engine;

  cur.apply( [&] (StrLen file_name,FileType file_type)
                 {
                  if( file_type==FileType_file ) engine.test(dir,file_name);

                 } );

  Printf(Con,"time = #; msec\n",timer.get());
 }

} // namespace Private_4034

using namespace Private_4034;

/* Testit<4034> */

template<>
const char *const Testit<4034>::Name="Test4034 Deflate";

template<>
bool Testit<4034>::Main()
 {
  //Test("../../../html");
  Test("../.test-obj");

  return true;
 }

} // namespace App

