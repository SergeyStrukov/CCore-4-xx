/* Deflate.cpp */
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

#include <CCore/inc/Deflate.h>

#include <CCore/inc/FeedBuf.h>
#include <CCore/inc/Sort.h>
#include <CCore/inc/algon/BestSearch.h>
#include <CCore/inc/algon/BinarySearch.h>

#include <CCore/inc/Exception.h>

namespace CCore {
namespace Deflate {

/* tables */

const USym LengthCodes[256]=
 {
  257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267, 268, 268,
  269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271, 272, 272, 272, 272,
  273, 273, 273, 273, 273, 273, 273, 273, 274, 274, 274, 274, 274, 274, 274, 274,
  275, 275, 275, 275, 275, 275, 275, 275, 276, 276, 276, 276, 276, 276, 276, 276,
  277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277,
  278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
  279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279,
  280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
  281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
  281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
  283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
  283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
  284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284,
  284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 285
 };

const unsigned LengthBases[29]=
 {
  3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258
 };

const unsigned DistanceBases[30]=
 {
  1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577
 };

const unsigned Order[19]=
 {
  16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
 };

const unsigned LengthExtraBits[29]=
 {
  0, 0, 0, 0,
  0, 0, 0, 0,
  1, 1, 1, 1,
  2, 2, 2, 2,
  3, 3, 3, 3,
  4, 4, 4, 4,
  5, 5, 5, 5,
  0
 };

const unsigned DistanceExtraBits[30]=
 {
  0, 0,
  0, 0,
  1, 1,
  2, 2,
  3, 3,
  4, 4,
  5, 5,
  6, 6,
  7, 7,
  8, 8,
  9, 9,
  10, 10,
  11, 11,
  12, 12,
  13, 13
 };

/* BitReverse() */

inline uint32 BitSwap(uint32 value,uint32 hi,uint32 lo,unsigned shift)
 {
  return uint32( (value&hi)>>shift )|uint32( (value&lo)<<shift );
 }

inline uint32 BitReverse(uint32 value)
 {
  value=BitSwap(value,0xAAAAAAAAu,0x55555555u,1);
  value=BitSwap(value,0xCCCCCCCCu,0x33333333u,2);
  value=BitSwap(value,0xF0F0F0F0u,0x0F0F0F0Fu,4);

  return Quick::ByteSwap32(value);
 }

/* Mismatch() */

template <class Ptr>
Ptr Mismatch(Ptr a,Ptr lim,Ptr b)
 {
  for(; a<lim ;++a,++b) if( *a!=*b ) return a;

  return lim;
 }

/* TrimNull() */

template <class Ptr>
ulen TrimNull(Ptr beg,Ptr lim)
 {
  while( beg<lim && lim[-1]==0 ) --lim;

  return Dist(beg,lim);
 }

/* functions */

BitLen MaxValue(PtrLen<const BitLen> bitlens)
 {
  BitLen ret=0;

  for(BitLen t : bitlens ) Replace_max(ret,t);

  return ret;
 }

/* class BitWriter */

BitWriter::BitWriter(OutFunc out_)
 : out(out_)
 {
 }

void BitWriter::putBits(Code code)
 {
  if( count_mode )
    {
     bit_count+=code.bitlen;
    }
  else
    {
     buffer|=(code.code<<bits) ;
     bits+=code.bitlen;

     while( bits>=8 )
       {
        outbuf[outlen++]=(uint8)buffer;

        if( outlen==BufLen )
          {
           put(outbuf,Replace_null(outlen));
          }

        buffer>>=8;
        bits-=8;
       }
    }
 }

void BitWriter::flushBitBuffer()
 {
  if( !count_mode )
    {
     if( outlen>0 )
       {
        put(outbuf,Replace_null(outlen));
       }

     if( bits>0 )
       {
        uint8 octet=(uint8)buffer;

        buffer=0;
        bits=0;

        put(octet);
       }
    }
 }

void BitWriter::clearBitBuffer()
 {
  buffer=0;
  bits=0;

  outlen=0;
 }

void BitWriter::startCounting()
 {
  count_mode=true;
  bit_count=0;
 }

unsigned BitWriter::finishCounting()
 {
  count_mode=false;

  return bit_count;
 }

/* class HuffmanEncoder */

void HuffmanEncoder::init(PtrLen<const BitLen> bitlens)
 {
  // count table

  BitLen max_code_bits=MaxValue(bitlens);

  if( max_code_bits>MaxCodeBits )
    {
     Printf(Exception,"CCore::Deflate::HuffmanEncoder::init() : code length exceeds maximum");
    }

  ulen len=max_code_bits+1;

  TempArray<ulen,MaxBitLens+1> counts(len);

  Range(counts).set_null();

  for(BitLen bitlen : bitlens ) counts[bitlen]++;

  // base codes

  TempArray<UCode,MaxBitLens+1> code(len);

  {
   UCode next=0;

   code[1]=next;

   for(ulen i=2; i<len ;i++)
     {
      next = (next+counts[i-1]) << 1 ;

      code[i]=next;
     }
  }

  // set codes

  table=SimpleArray<Code>(bitlens.len);

  for(ulen i=0; i<bitlens.len ;i++)
    {
     BitLen bitlen=bitlens[i];

     table[i].bitlen=bitlen;

     if( bitlen!=0 )
       table[i].code = BitReverse(code[bitlen]++) >> (MaxCodeBits-bitlen) ;
     else
       table[i].code = 0 ;
    }
 }

void HuffmanEncoder::Tree(BitLen bitlens[ /* counts.len */ ],BitLen maxbitlen,PtrLen<const ulen> counts)
 {
  struct Node
   {
    ulen sym;

    union
     {
      ulen count;
      ulen parent;
      ulen depth;
     };
   };

  // Huffman tree

  TempArray<Node,2*MaxSyms> tree(2*counts.len);

  for(ulen i=0; i<counts.len ;i++)
    {
     tree[i].sym=i;
     tree[i].count=counts[i];
    }

  auto initial=Range(tree.getPtr(),counts.len);

  IncrSort(initial, [] (const Node &a,const Node &b) { return a.count<b.count; } );

  Algon::BinarySearch_if(initial, [] (const Node &a) { return a.count>0; } );

  if( !initial )
    {
     Range(bitlens,counts.len).set_null();

     return;
    }

  if( initial.len==1 )
    {
     Range(bitlens,counts.len).set_null();

     bitlens[initial->sym]=1;

     return;
    }

  // build

  ulen beg=counts.len-initial.len;
  ulen lim=counts.len+initial.len-1;

  struct Builder
   {
    Node *tree;
    ulen leaf;
    ulen comb;

    ulen getLeast(ulen leaf_lim,ulen comb_lim)
     {
      return ( leaf>=leaf_lim || ( comb<comb_lim && tree[comb].count<tree[leaf].count ) ) ? comb++ : leaf++ ;
     }

    void setComb(ulen ind,ulen elem)
     {
      tree[ind].count=tree[elem].count;
      tree[elem].parent=ind;
     }

    void addComb(ulen ind,ulen elem)
     {
      tree[ind].count+=tree[elem].count;
      tree[elem].parent=ind;
     }
   };

  Builder builder{tree.getPtr(),beg,counts.len};

  for(ulen next=counts.len; next<lim ;next++)
    {
     builder.setComb(next, builder.getLeast(counts.len,next) );

     builder.addComb(next, builder.getLeast(counts.len,next) );
    }

  // combo depth

  tree[lim-1].depth=0;

  for(ulen i=lim-2; i>=counts.len ;i--) tree[i].depth=tree[tree[i].parent].depth+1;

  // bitlen counts

  TempArray<BitLen,MaxBitLens+1> blcounts(maxbitlen+1);

  Range(blcounts).set_null();

  unsigned sum=0;

  for(Node &node : initial )
    {
     ulen depth=Min<ulen>(maxbitlen,tree[node.parent].depth+1);

     blcounts[depth]++;

     sum += ulen(1)<<(maxbitlen-depth) ;
    }

  // correct bitlens

  unsigned cap=ulen(1)<<maxbitlen;

  if( sum>cap )
    {
     unsigned overflow=sum-cap;

     while( overflow-- )
       {
        BitLen bits=maxbitlen-1;

        while( blcounts[bits]==0 ) bits--;

        blcounts[bits]--;
        blcounts[bits+1]+=2;

        blcounts[maxbitlen]--;
       }
    }

  // set bitlens

  for(Node &node : Range(tree.getPtr(),beg) ) bitlens[node.sym]=0;

  BitLen bits=maxbitlen;

  for(Node &node : initial )
    {
     while( blcounts[bits]==0 ) bits--;

     bitlens[node.sym]=bits;

     blcounts[bits]--;
    }
 }

/* struct StaticLiteralBitlens */

StaticLiteralBitlens::StaticLiteralBitlens()
 {
  Range(bitlens,bitlens+144).set(8);
  Range(bitlens+144,bitlens+256).set(9);
  Range(bitlens+256,bitlens+280).set(7);
  Range(bitlens+280,bitlens+288).set(8);
 }

/* struct StaticDistanceBitlens */

StaticDistanceBitlens::StaticDistanceBitlens()
 {
  Range(bitlens).set(5);
 }

/* class SymWriter */

struct SymWriter::CodeLenEncoder
 {
  USym sym; // < 19
  Code extra;
  PtrLen<const BitLen> next;

  CodeLenEncoder(const BitLen *beg,PtrLen<const BitLen> cur)
   {
    BitLen v=*cur;

    if( cur.len>=3 )
      {
       ulen prev=cur.len;

       if( v==0 && cur[1]==0 && cur[2]==0 )
         {
          for(cur+=3; +cur && *cur==0 && prev-cur.len<138u ;++cur);

          UCode repeat=(UCode)(prev-cur.len);

          if( repeat<=10 )
            {
             sym=17;
             extra.code=repeat-3;
             extra.bitlen=3;
            }
          else
            {
             sym=18;
             extra.code=repeat-11;
             extra.bitlen=7;
            }

          next=cur;

          return;
         }

       if( cur.ptr>beg && v==cur.ptr[-1] && v==cur[1] && v==cur[2] )
         {
          for(cur+=3; +cur && *cur==v && prev-cur.len<6u ;++cur);

          UCode repeat=(UCode)(prev-cur.len);

          sym=16;
          extra.code=repeat-3;
          extra.bitlen=2;

          next=cur;

          return;
         }
      }

    sym=v;
    extra.code=0;
    extra.bitlen=0;

    ++cur;

    next=cur;
   }
 };

void SymWriter::encodeCode(PtrLen<const BitLen> combined,unsigned hlit,unsigned hdist)
 {
  // create encoder

  ulen counts[19]={};

  for(PtrLen<const BitLen> cur=combined; +cur ;)
    {
     CodeLenEncoder lencode(combined.ptr,cur);

     counts[lencode.sym]++;

     cur=lencode.next;
    }

  BitLen bitlens[19];

  HuffmanEncoder::Tree(bitlens,7,Range(counts));

  HuffmanEncoder encoder(Range(bitlens));

  // put header

  unsigned hclen=19;

  while( hclen>4 && bitlens[Order[hclen-1]]==0 ) hclen--;

  writer.putBits({hlit,5});
  writer.putBits({hdist,5});
  writer.putBits({hclen-4,4});

  for(unsigned i=0; i<hclen ;i++) writer.putBits({bitlens[Order[i]],3});

  // put combined

  for(PtrLen<const BitLen> cur=combined; +cur ;)
    {
     CodeLenEncoder lencode(combined.ptr,cur);

     encoder.encode(writer,lencode.sym);

     writer.putBits(lencode.extra);

     cur=lencode.next;
    }
 }

void SymWriter::encodeBlock(bool eof,BlockType block_type,PtrLen<const uint8> block)
 {
  writer.putBits({eof,1});
  writer.putBits({block_type,2});

  if( block_type==Stored )
    {
     writer.flushBitBuffer();

     uint16 blen=uint16(block.len);

     writer.put16(blen);
     writer.put16(~blen);
     writer.put(block);
    }
  else
    {
     if( block_type==Dynamic )
       {
        BitLen literal_bitlens[286];
        BitLen distance_bitlens[30];

        literal_counts[256]=1;

        HuffmanEncoder::Tree(literal_bitlens,15,Range(literal_counts));

        dynamic_literal_encoder.init(Range(literal_bitlens));

        unsigned hlit=(unsigned)TrimNull(literal_bitlens+257,literal_bitlens+286);

        HuffmanEncoder::Tree(distance_bitlens,15,Range(distance_counts));

        dynamic_distance_encoder.init(Range(distance_bitlens));

        unsigned hdist=(unsigned)TrimNull(distance_bitlens+1,distance_bitlens+30);

        unsigned nlit=hlit+257;
        unsigned ndist=hdist+1;

        TempArray<BitLen,286+30> combined(nlit+ndist);

        Range(combined.getPtr(),nlit).copy(literal_bitlens);

        Range(combined.getPtr()+nlit,ndist).copy(distance_bitlens);

        encodeCode(Range(combined),hlit,hdist);
       }

     const HuffmanEncoder &literal_encoder = (block_type==Static)? StaticCoder<HuffmanEncoder,StaticLiteralBitlens>::Get()
                                                                 : dynamic_literal_encoder ;

     const HuffmanEncoder &distance_encoder = (block_type==Static)? StaticCoder<HuffmanEncoder,StaticDistanceBitlens>::Get()
                                                                  : dynamic_distance_encoder ;

     for(auto m : Range(buf.getPtr(),pos) )
       {
        USym literal_code=m.literal_code;

        literal_encoder.encode(writer,literal_code);

        if( literal_code>=257 )
          {
           writer.putBits({m.literal_extra,LengthExtraBits[literal_code-257]});

           USym distance_code=m.distance_code;

           distance_encoder.encode(writer,distance_code);

           writer.putBits({m.distance_extra,DistanceExtraBits[distance_code]});
          }
       }

     literal_encoder.encode(writer,256);
    }
 }

SymWriter::SymWriter(OutFunc out)
 : writer(out)
 {
 }

SymWriter::~SymWriter()
 {
 }

void SymWriter::init(Level level_,bool detect_uncompressible,unsigned buflen)
 {
  level=level_;
  compressible_level = detect_uncompressible? level_ : MinLevel ;

  buf=SimpleArray<EncodedMatch>(buflen);
 }

void SymWriter::reset()
 {
  pos=0;

  detect_skip=0;
  detect_count=1;

  Range(literal_counts).set_null();
  Range(distance_counts).set_null();

  writer.clearBitBuffer();
 }

bool SymWriter::testFull(PtrLen<const uint8> block)
 {
  if( pos==buf.getLen() )
    {
     endBlock(false,block);

     return true;
    }

  return false;
 }

void SymWriter::put(uint8 octet)
 {
  buf[pos++].literal_code=octet;

  literal_counts[octet]++;
 }

void SymWriter::put(unsigned distance,unsigned length)
 {
  EncodedMatch &m=buf[pos++];

  USym length_code=LengthCodes[length-3];

  m.literal_code=length_code;
  m.literal_extra=length-LengthBases[length_code-257];

  auto r=Range(DistanceBases);

  USym distance_code=(USym)Algon::BinarySearch_greater(r,distance).len-1;

  m.distance_code=distance_code;
  m.distance_extra=distance-DistanceBases[distance_code];

  literal_counts[length_code]++;
  distance_counts[distance_code]++;
 }

void SymWriter::endBlock(bool eof,PtrLen<const uint8> block)
 {
  if( block.len==0 && !eof ) return;

  if( level==MinLevel )
    {
     encodeBlock(eof,Stored,block);

     if( compressible_level>MinLevel && ++detect_count==detect_skip )
       {
        level=compressible_level;

        detect_count=1;
       }
    }
  else
    {
     unsigned stored_len=8*(block.len+4)+writer.extBits();

     writer.startCounting();

     encodeBlock(eof,Static,block);

     unsigned static_len=writer.finishCounting();

     unsigned dynamic_len;

     if( block.len<128 && level<EverdynamicLevel )
       {
        dynamic_len=MaxUInt<unsigned>;
       }
     else
       {
        writer.startCounting();

        encodeBlock(eof,Dynamic,block);

        dynamic_len=writer.finishCounting();
       }

     if( stored_len<=static_len && stored_len<=dynamic_len )
       {
        encodeBlock(eof,Stored,block);

        if( compressible_level>MinLevel )
          {
           if( detect_skip )
             {
              level=MinLevel;

              detect_skip=Min<unsigned>(2*detect_skip,128);
             }
           else
             {
              detect_skip=1;
             }
          }
       }
     else
       {
        if( static_len<=dynamic_len )
          {
           encodeBlock(eof,Static,block);
          }
        else
          {
           encodeBlock(eof,Dynamic,block);
          }

        if( compressible_level>MinLevel ) detect_skip=0;
       }
    }

  pos=0;

  Range(literal_counts).set_null();
  Range(distance_counts).set_null();

  if( eof ) writer.flushBitBuffer();
 }

/* class Deflator */

void Deflator::setLevel(Level level)
 {
  if( !( level>=MinLevel && level<=MaxLevel ) )
    {
     Printf(Exception,"CCore::Deflate::Deflator::setLevel(#;) : is an invalid deflate level",level);
    }

  struct Rec
   {
    unsigned good_match;
    unsigned min_lazy_len;
    unsigned max_chain_len;
   };

  static const Rec Table[10]=
   {
    /* 0 */ {0,    0,    0},  // store only
    /* 1 */ {4,    3,    4},  // maximum speed, no lazy matches
    /* 2 */ {4,    3,    8},
    /* 3 */ {4,    3,   32},
    /* 4 */ {4,    4,   16},  // lazy matches
    /* 5 */ {8,   16,   32},
    /* 6 */ {8,   16,  128},  // default
    /* 7 */ {8,   32,  256},
    /* 8 */ {32, 128, 1024},
    /* 9 */ {32, 258, 4096}  // maximum compression
   };

  min_lazy_len=Table[level].min_lazy_len;
  good_match=Table[level].good_match;
  max_chain_len=Table[level].max_chain_len;
 }

void Deflator::init(Param param)
 {
  if( !( MinLog2WindowLen<=param.log2_window_len && param.log2_window_len<=MaxLog2WindowLen ) )
    {
     Printf(Exception,"CCore::Deflate::Deflator::init(...) : #; is an invalid window length",param.log2_window_len);
    }

  log2_window_len=param.log2_window_len;

  wind_len = 1 << log2_window_len ; // > MaxMatch
  wind_mask = wind_len - 1 ;

  hash_len = 1 << log2_window_len ;
  hash_mask = hash_len - 1 ;

  cap_data_len=Min<unsigned>(2*wind_len,0xFFFFu); // >= wind_len+MaxMatch , >= 2*wind_len-1

  buf=SimpleArray<uint8>(2*wind_len);

  hashed_head=SimpleArray<uint16>(hash_len);

  prevpos=SimpleArray<uint16>(wind_len);

  setLevel(param.level);

  sym.init(param.level,param.detect_uncompressible,wind_len/2);

  reset();
 }

void Deflator::reset()
 {
  sym.reset();

  block.null();

  string.null();

  hashed_len=0;
  min_testlen=MaxMatch;

  has_match=false;

  Range(hashed_head).set_null();
 }

void Deflator::downWindow()
 {
  if( block.pos<wind_len ) endBlock(false);

  uint8 *base=buf.getPtr();

  Range(base,wind_len).copy(base+wind_len);

  block.pos-=wind_len;
  string.pos-=wind_len;

  match.pos-=wind_len;

  hashed_len=PosSub(hashed_len,wind_len);

  for(auto &start : hashed_head ) start=PosSub(start,wind_len);

  for(auto &prev : prevpos ) prev=PosSub(prev,wind_len);
 }

unsigned Deflator::fillWindow(PtrLen<const uint8> data)
 {
  if( string.pos>=cap_data_len-MaxMatch ) downWindow();

  unsigned off=string.getLim();

  unsigned delta=(unsigned)Min<ulen>(cap_data_len-off,data.len) ;

  Range(buf.getPtr()+off,delta).copy(data.ptr);

  string.len+=delta;

  return delta;
 }

unsigned Deflator::computeHash(const uint8 *str) const
 {
  unsigned s0=str[0];
  unsigned s1=str[1];
  unsigned s2=str[2];

  return ((s0<<10)^(s1<<5)^s2)&hash_mask;
 }

void Deflator::insertHash(unsigned start)
 {
  unsigned hash=computeHash(buf.getPtr()+start);

  prevpos[start&wind_mask]=hashed_head[hash];

  hashed_head[hash]=uint16(start);
 }

auto Deflator::bestMatch(unsigned prev_len) const -> Frame
 {
  unsigned best_pos=0;
  unsigned best_len=Max(prev_len,MinMatch-1);

  unsigned testlen=Min(MaxMatch,string.len);

  if( testlen<=best_len ) return {0,0};

  const uint8 *ptr=buf.getPtr()+string.pos;
  const uint8 *lim=ptr+testlen;

  unsigned pos_limit=PosSub(string.pos,wind_len-MaxMatch);

  unsigned pos=hashed_head[computeHash(ptr)];

  unsigned chain_len=max_chain_len;

  if( prev_len>=good_match ) chain_len>>=2;

  while( pos>pos_limit && --chain_len>0 )
    {
     const uint8 *match=buf.getPtr()+pos;

     if( ptr[best_len-1]==match[best_len-1] && ptr[best_len]==match[best_len] && ptr[0]==match[0] && ptr[1]==match[1] )
       {
        unsigned len=(unsigned)Dist(ptr,Mismatch(ptr+3,lim,match+3));

        if( len>best_len )
          {
           best_len=len;
           best_pos=pos;

           if( len==testlen ) break;
          }
       }

     pos=prevpos[pos&wind_mask];
    }

  if( best_pos ) return {best_pos,best_len};

  return {0,0};
 }

void Deflator::processBuffer()
 {
  if( getLevel()==MinLevel )
    {
     string.finish();

     block.len=string.pos-block.pos;

     has_match=false;

     return;
    }

  while( string.len>min_testlen )
    {
     while( hashed_len<string.pos && hashed_len+3<=string.getLim() ) insertHash(hashed_len++);

     if( has_match )
       {
        Frame next;

        bool push_current;

        if( match.len>=min_lazy_len )
          {
           push_current=true;
          }
        else
          {
           next=bestMatch(match.len);

           push_current=(next.len==0);
          }

        if( push_current )
          {
           matchFound(string.pos-1-match.pos,match.len);

           string+=match.len-1;

           has_match=false;
          }
        else
          {
           match=next;

           literalByte(buf[string.pos-1]);

           ++string;
          }
       }
     else
       {
        match=bestMatch(0);

        if( match.len )
          has_match=true;
        else
          literalByte(buf[string.pos]);

        ++string;
       }
    }

  if( min_testlen==0 && has_match )
    {
     literalByte(buf[string.pos-1]);

     has_match=false;
    }
 }

void Deflator::literalByte(uint8 octet)
 {
  if( sym.testFull(getBlock()) ) block.finish();

  sym.put(octet);

  block.len++;
 }

void Deflator::matchFound(unsigned distance,unsigned length)
 {
  if( sym.testFull(getBlock()) ) block.finish();

  sym.put(distance,length);

  block.len+=length;
 }

void Deflator::endBlock(bool eof)
 {
  sym.endBlock(eof,getBlock());

  block.finish();
 }

Deflator::Deflator(OutFunc out,Param param)
 : sym(out)
 {
  init(param);
 }

Deflator::~Deflator()
 {
 }

void Deflator::put(PtrLen<const uint8> data)
 {
  while( +data )
    {
     unsigned delta=fillWindow(data);

     data+=delta;

     processBuffer();
    }
 }

void Deflator::complete()
 {
  min_testlen=0;

  processBuffer();

  endBlock(true);

  reset();
 }

/* class WindowOut */

PtrLen<const uint8> WindowOut::output() const
 {
  return Range(buf.getPtr()+outpos,addpos-outpos);
 }

void WindowOut::commit()
 {
  if( addpos==WindowLen )
    {
     auto r=output();

     outpos=0;
     addpos=0;
     wrapped=true;

     out(r);
    }
 }

WindowOut::WindowOut(OutFunc out_)
 : out(out_),
   buf(WindowLen)
 {
 }

WindowOut::~WindowOut()
 {
 }

void WindowOut::flush()
 {
  auto r=output();

  outpos=addpos;

  out(r);
 }

void WindowOut::put(uint8 octet)
 {
  buf[addpos++]=octet;

  commit();
 }

void WindowOut::put(PtrLen<const uint8> data)
 {
  while( +data )
    {
     PtrLen<uint8> dst(buf.getPtr()+addpos,WindowLen-addpos);

     FeedBuf feed(dst,data);

     addpos+=feed.delta;
     data+=feed.delta;

     commit();
    }
 }

void WindowOut::put(unsigned distance,unsigned length)
 {
  unsigned start;

  if( distance<=addpos )
    {
     start=addpos-distance;
    }
  else if( wrapped && distance<=WindowLen )
    {
     start=addpos+WindowLen-distance;
    }
  else
    {
     Printf(Exception,"CCore::Deflate::WindowOut::put(#;,#;) : incorrect input",distance,length);

     start=0;
    }

  if( start+length>WindowLen )
    {
     for(; start<WindowLen ;start++,length--) put(buf[start]);

     start=0;
    }

  if( start+length>addpos || addpos+length>=WindowLen )
    {
     while( length-- ) put(buf[start++]);
    }
  else
    {
     Range(buf.getPtr()+addpos,length).copy(buf.getPtr()+start);

     addpos+=length;
    }
 }

/* class BitReader */

bool BitReader::next(uint8 &octet)
 {
  if( getpos<addpos )
    {
     octet=inpbuf[getpos++];

     return true;
    }

  if( !input ) return false;

  octet=*input;

  ++input;

  return true;
 }

void BitReader::copyDown()
 {
  if( getpos>=addpos )
    {
     getpos=0;
     addpos=0;
    }
  else
    {
     unsigned delta=getpos;

     if( delta==0 ) return;

     unsigned count=addpos-getpos;

     for(unsigned i=0; i<count ;i++) inpbuf[i]=inpbuf[i+delta];

     getpos=0;
     addpos=count;
    }
 }

bool BitReader::canRead(unsigned bitlen) const
 {
  if( bitlen<=bits ) return true;

  unsigned len=RoundUpCount(bitlen-bits,8u);
  unsigned count=addpos-getpos;

  return len<=count || len-count<=input.len ;
 }

void BitReader::extend(PtrLen<const uint8> data)
 {
  if( +input )
    {
     Printf(Exception,"CCore::Deflate::BitReader::extend(...) : dirty");
    }

  input=data;
 }

bool BitReader::bufferize()
 {
  if( !input ) return true;

  if( input.len>BufLen-addpos )
    {
     if( input.len>BufLen-addpos+getpos ) return false;

     copyDown();
    }

  input.copyTo(inpbuf+addpos);

  addpos+=(unsigned)input.len;

  input=Empty;

  return true;
 }

void BitReader::pumpTo(WindowOut &out)
 {
  if( bits%8 )
    {
     Printf(Exception,"CCore::Deflate::BitReader::pumpTo(...) : not aligned");
    }

  // 1

  while( bits>=8 ) out.put((uint8)getBits(8));

  // 2

  out.put(inpbuf+getpos,addpos-getpos);

  getpos=0;
  addpos=0;

  // 3

  out.put(input);

  input=Empty;
 }

ulen BitReader::pumpToCap(WindowOut &out,ulen cap)
 {
  if( bits%8 )
    {
     Printf(Exception,"CCore::Deflate::BitReader::pumpTo(...) : not aligned");
    }

  // 1

  for(; bits>=8 && cap ;cap--) out.put((uint8)getBits(8));

  if( !cap ) return 0;

  // 2

  unsigned len=addpos-getpos;

  if( cap<len )
    {
     out.put(inpbuf+getpos,cap);

     getpos+=(unsigned)cap;

     return 0;
    }
  else
    {
     out.put(inpbuf+getpos,len);

     cap-=len;

     getpos=0;
     addpos=0;

     if( !cap ) return 0;
    }

  // 3

  if( cap<input.len )
    {
     out.put(input+=cap);

     return 0;
    }
  else
    {
     out.put(input);

     cap-=input.len;

     input=Empty;

     return cap;
    }
 }

bool BitReader::fillBuffer(unsigned bitlen)
 {
  while( bits<bitlen )
    {
     uint8 octet;

     if( !next(octet) ) return false;

     buffer|=UCode(octet)<<bits;

     bits+=8;
    }

  return true;
 }

void BitReader::reqBuffer(unsigned bitlen)
 {
  if( !fillBuffer(bitlen) )
    {
     Printf(Exception,"CCore::Deflate::BitReader::reqBuffer(...) : underflow");
    }
 }

/* class HuffmanDecoder */

auto HuffmanDecoder::Find(PtrLen<const CodeInfo> r,UCode ncode) -> const CodeInfo &
 {
  auto prefix=Algon::BinarySearch_if(r, [ncode] (const CodeInfo &obj) { return obj.ncode>ncode; } );

  return prefix.back(1);
 }

UCode HuffmanDecoder::NormalizeCode(UCode code,BitLen bitlen)
 {
  return code<<(MaxCodeBits-bitlen);
 }

void HuffmanDecoder::FillCacheEntry(CacheEntry &entry,UCode ncode) const
 {
  ncode&=norm_cache_mask;

  const CodeInfo &base=Find(Range(table),ncode);

  if( base.bitlen<=cache_bits )
    {
     entry.type=1;
     entry.sym=base.sym;
     entry.bitlen=base.bitlen;
    }
  else
    {
     entry.beg=&base;

     const CodeInfo &last=Find(Range(table),ncode+ ~norm_cache_mask);

     if( base.bitlen==last.bitlen )
       {
        entry.type=2;
        entry.bitlen=base.bitlen;
       }
     else
       {
        entry.type=3;
        entry.lim=&last+1;
       }
    }
 }

void HuffmanDecoder::init(PtrLen<BitLen> bitlens)
 {
  // count table

  max_code_bits=MaxValue(bitlens);

  if( max_code_bits==0 )
    {
     Printf(Exception,"CCore::Deflate::HuffmanDecoder::init() : null code");
    }

  if( max_code_bits>MaxCodeBits-7 )
    {
     Printf(Exception,"CCore::Deflate::HuffmanDecoder::init() : code length exceeds maximum");
    }

  ulen len=max_code_bits+1;

  TempArray<ulen,MaxBitLens+1> counts(len);

  Range(counts).set_null();

  for(BitLen bitlen : bitlens ) counts[bitlen]++;

  // base codes

  TempArray<UCode,MaxBitLens+1> code(len);

  {
   UCode next=0;

   code[1]=next;

   for(ulen i=2; i<len ;i++)
     {
      auto t=counts[i-1];

      if( t>MaxUInt<UCode> || UIntAdd(next,(UCode)t) || UIntAdd(next,next) )
        {
         Printf(Exception,"CCore::Deflate::HuffmanDecoder::init() : code overflow");
        }

      code[i]=next;
     }

   uint64 cap=(uint64(1)<<max_code_bits);

   ulen cnt=counts[max_code_bits];

   if( cnt>cap )
     {
      Printf(Exception,"CCore::Deflate::HuffmanDecoder::init() : code overflow");
     }

   cap-=cnt;

   if( next>cap )
     {
      Printf(Exception,"CCore::Deflate::HuffmanDecoder::init() : code overflow");
     }

   if( max_code_bits>1 && next<cap )
     {
      Printf(Exception,"CCore::Deflate::HuffmanDecoder::init() : code incomplete");
     }
  }

  // table

  table=SimpleArray<CodeInfo>(bitlens.len-counts[0]);

  for(ulen i=0,j=0; i<bitlens.len ;i++)
    {
     BitLen bitlen=bitlens[i];

     if( bitlen!=0 )
       {
        table[j++].set(code[bitlen]++,bitlen,USym(i));
       }
    }

  Sort(Range(table));

  // initialize the decoding cache

  cache_bits=Min<BitLen>(9,max_code_bits);

  cache_mask=(UCode(1)<<cache_bits)-1;

  norm_cache_mask=NormalizeCode(cache_mask,cache_bits);

  ulen cache_len=(ulen(1)<<cache_bits);

  if( cache.getLen()!=cache_len ) cache=SimpleArray<CacheEntry>(cache_len);

  for(auto &m : cache ) m.type=0;
 }

BitLen HuffmanDecoder::decode(UCode code,USym &sym) const
 {
  CacheEntry &entry=cache[code&cache_mask];

  if( entry.type==1 )
    {
     sym=entry.sym;

     return entry.bitlen;
    }

  UCode ncode=BitReverse(code);

  if( entry.type==0 ) FillCacheEntry(entry,ncode);

  switch( entry.type )
    {
     case 2 :
      {
       const CodeInfo &info=entry.index(ncode,cache_bits);

       sym=info.sym;

       return info.bitlen;
      }

     case 3 :
      {
       const CodeInfo &info=entry.find(ncode);

       sym=info.sym;

       return info.bitlen;
      }

     default: // 1
      {
       sym=entry.sym;

       return entry.bitlen;
      }
    }
 }

bool HuffmanDecoder::decode(BitReader &reader,USym &sym) const
 {
  reader.fillBuffer(max_code_bits);

  BitLen bits=decode(reader.peekBuffer(),sym);

  if( bits>reader.bitsBuffered() ) return false;

  reader.skipBits(bits);

  return true;
 }

void HuffmanDecoder::decodePrepared(BitReader &reader,USym &sym) const
 {
  BitLen bits=decode(reader.peekBuffer(),sym);

  reader.skipBits(bits);
 }

void HuffmanDecoder::reqDecode(BitReader &reader,USym &sym) const
 {
  reader.reqBuffer(max_code_bits);

  BitLen bits=decode(reader.peekBuffer(),sym);

  reader.skipBits(bits);
 }

/* class Inflator */

void Inflator::decodeCode()
 {
  BitLen bitlens[286+30];

  // 1

  reader.reqBuffer(5+5+4);

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
     reader.reqBuffer(3);

     bitlens[Order[i]]=reader.getBits(3);
    }

  HuffmanDecoder decoder({bitlens,19});

  // 2

  unsigned len=nlit+ndist;

  for(unsigned i=0; i<len ;)
    {
     USym k;

     decoder.reqDecode(reader,k);

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
             reader.reqBuffer(2);

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
             reader.reqBuffer(3);

             count=3+reader.getBits(3);
            }
           break;

           case 18:
            {
             reader.reqBuffer(7);

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
 }

void Inflator::decodeHeader()
 {
  reader.reqBuffer(3);

  last_block=( reader.getBits(1)!=0 );

  block_type=reader.getBits(2);

  switch( block_type )
    {
     case Stored :
      {
       reader.align8();

       reader.reqBuffer(32);

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
       decode_state=Literal;
      }
     break;

     case Dynamic :
      {
       decodeCode();

       decode_state=Literal;
      }
     break;

     default:
      {
       Printf(Exception,"CCore::Deflate::Inflator::decodeHeader() : incorrect block type");
      }
    }
 }

bool Inflator::decodeBody()
 {
  if( block_type==Stored )
    {
     stored_len=reader.pumpToCap(out,stored_len);

     return !stored_len;
    }
  else
    {
     const HuffmanDecoder &literal_decoder = (block_type==Static)? StaticCoder<HuffmanDecoder,StaticLiteralBitlens>::Get()
                                                                 : dynamic_literal_decoder ;

     const HuffmanDecoder &distance_decoder = (block_type==Static)? StaticCoder<HuffmanDecoder,StaticDistanceBitlens>::Get()
                                                                  : dynamic_distance_decoder ;

     switch( decode_state )
       {
        case Literal:

        for(;;)
          {
           if( !literal_decoder.decode(reader,literal) )
             {
              decode_state=Literal;

              return false;
             }

           if( literal<256 )
             {
              out.put((uint8)literal);
             }
           else if( literal==256 )
             {
              return true;
             }
           else
             {
              if( literal>=286 )
                {
                 Printf(Exception,"CCore::Deflate::Inflator::decodeBody() : incorrect literal");
                }

              case LengthBits :
               {
                unsigned bits=LengthExtraBits[literal-257];

                if( !reader.fillBuffer(bits) )
                  {
                   decode_state=LengthBits;

                   return false;
                  }

                literal=reader.getBits(bits)+LengthBases[literal-257];
               }

              case Distance :
               {
                if( !distance_decoder.decode(reader,distance) )
                  {
                   decode_state=Distance;

                   return false;
                  }
               }

              if( distance>=30 )
                {
                 Printf(Exception,"CCore::Deflate::Inflator::decodeBody() : incorrect distance");
                }

              case DistanceBits :
               {
                unsigned bits=DistanceExtraBits[distance];

                if( !reader.fillBuffer(bits) )
                  {
                   decode_state=DistanceBits;

                   return false;
                  }

                distance=reader.getBits(bits)+DistanceBases[distance];

                out.put(distance,literal);
               }
             }
          }
       }

     return false;
    }
 }

void Inflator::processInput(bool eof)
 {
  for(;;)
    {
     switch( state )
       {
        case PreStream :
         {
          out.reset();

          state=WaitHeader;
         }
        break;

        case WaitHeader :
         {
          if( eof )
            {
             if( reader.isEmpty() )
               {
                state=AfterEnd;

                return;
               }
            }
          else
            {
             if( !reader.canRead(MaxHeaderBitlen) ) return;
            }

          // can read MaxHeaderBitlen OR eof and reader is not empty

          decodeHeader();

          state=DecodingBody;
         }
        break;

        case DecodingBody :
         {
          if( !decodeBody() ) return;

          if( last_block )
            {
             out.flush();

             reader.align8();

             state=PostStream;
            }
          else
            {
             state=WaitHeader;
            }
         }
        break;

        case PostStream :
         {
          trigger();

          state = repeat? PreStream : AfterEnd ;
         }
        break;

        case AfterEnd :
         {
          reader.pumpTo(out);

          return;
         }
       }
    }
 }

Inflator::Inflator(OutFunc out_,bool repeat_)
 : out(out_),
   repeat(repeat_),
   state(PreStream)
 {
 }

Inflator::~Inflator()
 {
 }

void Inflator::put(PtrLen<const uint8> data)
 {
  out.flush();

  reader.extend(data);

  processInput(false);

  reader.bufferize();
 }

void Inflator::complete()
 {
  processInput(true);

  if( state!=AfterEnd )
    {
     Printf(Exception,"CCore::Deflate::Inflator::complete() : unexpected EOF");
    }
 }

} // namespace Deflate
} // namespace CCore

