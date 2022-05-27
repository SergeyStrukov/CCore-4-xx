/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: Labs/Double
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Print.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/math/IntegerFastAlgo.h>
#include <CCore/inc/math/Integer.h>

#include <CCore/inc/PrintStem.h>

#include <CCore/inc/Array.h>

#include <cmath>

#include <CCore/inc/math/DoubleUtils.h>

namespace App {

using namespace CCore;

/* type Int */

using Int = Math::Integer<Math::IntegerFastAlgo> ;

/* consts */

static constexpr int DExp = DoubleFormat::ExpBias+DoubleFormat::FractBits ;
static constexpr int MinBinExp = 1-DExp ;
static constexpr int MaxBinExp = DoubleFormat::MaxExp-1-DoubleFormat::ExpBias ;

/* struct Rec */

struct Rec
 {
  int bin_exp;
  int dec_exp;
  Int pow2;  // 2^abs(bin_exp)
  Int pow10; // 10^abs(dec_exp)

  void print(PrinterType auto &out) const
   {
    if( bin_exp>=0 )
      Printf(out,"#; #; #; #;",bin_exp,dec_exp,pow2,pow10);
    else
      Printf(out,"#; #; 1/#; 1/#;",bin_exp,dec_exp,pow2,pow10);
   }
 };

/* class RecTable */

class RecTable : NoCopy
 {
   Rec table[MaxBinExp-MinBinExp+1];

  public:

   RecTable() {}

   ~RecTable() {}

   Rec & operator [] (int bin_exp) { return table[bin_exp-MinBinExp]; }
 };

static RecTable Table;

/* Pow10 */

static int MinDecExp=0;
static int MaxDecExp=0;

static DynArray<double> Pow10hi;

/* functions */

double ToDoubleHi(Int x)
 {
  unsigned bits=x.bitsOf().total();

  if( bits>DoubleFormat::FractBits )
    {
     unsigned extra=bits-DoubleFormat::FractBits-1;

     Int y=x>>extra; // DoubleFormat::FractBits+1 bits

     if( x>(y<<extra) ) y+=1;

     return std::ldexp(y.cast<DoubleFormat::BodyType>(),extra);
    }
  else
    {
     return x.cast<DoubleFormat::BodyType>();
    }
 }

double InvToDoubleHi(Int x) // not pow2
 {
  unsigned bits=x.bitsOf().total();
  unsigned extra=DoubleFormat::FractBits+1;
  unsigned t=bits+extra;

  DoubleFormat::BodyType y=(Int(2).pow(t)/x).cast<DoubleFormat::BodyType>();

  return ldexp(y+1,-(int)t);
 }

/* Main1() */

void Main1()
 {
  Printf(Con,"DExp = #; MinBinExp = #; MaxBinExp = #;\n",DExp,MinBinExp,MaxBinExp);

  Int x(1);
  Int y(10);
  int dec_exp=1;

  Table[0]={0,dec_exp,x,y};

  for(int bin_exp=1; bin_exp<=MaxBinExp ;bin_exp++)
    {
     x*=2;

     if( x>=y )
       {
        dec_exp++;
        y*=10;
       }

     Table[bin_exp]={bin_exp,dec_exp,x,y};
    }

  x=2;
  y=1;
  dec_exp=0;

  Table[-1]={-1,dec_exp,x,y};

  for(int bin_exp=-2; bin_exp>=MinBinExp ;bin_exp--)
    {
     x*=2;

     Int y1=10*y;

     if( x>10*y )
       {
        dec_exp--;
        y=y1;
       }

     Table[bin_exp]={bin_exp,dec_exp,x,y};
    }

  MinDecExp=Table[MinBinExp].dec_exp;
  MaxDecExp=Table[MaxBinExp].dec_exp;

  Pow10hi.extend_raw(MaxDecExp-MinDecExp+1);

  y=1;

  Pow10hi[0-MinDecExp]=1;

  for(int dec_exp=1; dec_exp<=MaxDecExp ;dec_exp++)
    {
     y*=10;

     Pow10hi[dec_exp-MinDecExp]=ToDoubleHi(y);
    }

  y=1;

  for(int dec_exp=-1; dec_exp>=MinDecExp ;dec_exp--)
    {
     y*=10;

     Pow10hi[dec_exp-MinDecExp]=InvToDoubleHi(y);
    }

  {
   PrintFile out("table.txt");

   for(int bin_exp=MinBinExp; bin_exp<=MaxBinExp ;bin_exp++)
     {
      Printf(out,"#;\n",Table[bin_exp]);
     }
  }

  {
   PrintFile out("data.txt");

   // 1

   Printf(out,"MinDecExp = #; MaxDecExp = #;\n\n",MinDecExp,MaxDecExp);

   Printf(out,"#;\n\n",TextDivider());

   // 2
   {
    PrintPeriod stem(20,"    "_c," , "_c," ,\n    "_c);

    for(int bin_exp=MinBinExp; bin_exp<=MaxBinExp ;bin_exp++)
      {
       Printf(out,"#;#4;",*(stem++),Table[bin_exp].dec_exp);
      }

    Printf(out,"\n\n#;\n\n",TextDivider());
   }

   // 3
   {
    PrintPeriod stem(8,"    "_c," , "_c," ,\n    "_c);

    for(double val : Pow10hi )
      {
       Printf(out,"#;#22x;",*(stem++),val);
      }

    Printf(out,"\n\n#;\n\n",TextDivider());
   }
  }
 }

} // namespace App

/* main() */

using namespace App;

int main()
 {
  try
    {
     ReportException report;

     Main1();

     report.guard();

     return 0;
    }
  catch(CatchType)
    {
     return 1;
    }
 }

