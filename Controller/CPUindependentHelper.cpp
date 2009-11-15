//#include <string>
//This file must have extension ".cpp" for including <string> (std::string )
//else error "C2059", "C2143" with MSVC in cstdio at "size_t" and below
#include "global.h" //for DEBUG etc.
#include "CPUindependentHelper.h"
#include <math.h> //for log10(...);

//Use an object class like std::string instead of "char *". Advantage: 
//So there is no need to care about releasing memory.
//char * getBinaryRepresentation(char arch [],unsigned long ulValue//,long size
std::string getBinaryRepresentation(unsigned long ulValue//,long size
  )
{
//	char arch[65] ;
  std::string str;
  DEBUG("getBinaryRepresentation--begin\n");
  //char arch[ 
  //  //returns size in bytes.
  //  sizeof(unsigned long) 
  //  //=size in BITS.
  //  * 8 
  //  //for terminating NULL char.
  //  + 1 ] ;
  BYTE i = MAX_BIT_INDEX_FOR_UL ;
  for(; i != 
    //Value after value wrap when "0-1" .
    255 ; i --)
  {
    //DEBUG("getBinaryRepresentation--index: %u\n",(WORD) i);
	  //arch[MAX_BIT_INDEX_FOR_UL-i] = (char) ( ((ulValue >> i )& 1) + '0' ) ;
    str+=(char) ( ((ulValue >> i )& 1) + '0' ) ;
    if(  i%8 == 0)
      str+=' ';
  }
  //arch[MAX_BIT_INDEX_FOR_UL+1] = '\0' ;
  //return arch ;

  //return std::string(arch) ;
  DEBUG("getBinaryRepresentation--end\n");
  return str;
}

// this function was created because there is no logarithm for base 2 in 
//the standard library
double log_dualis(double fValue)
{
  return 
    //MSVC++ has no log2() function (in <math.h>).
    //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
    log10
    (
      fValue
    )
    / log10(2.0f) ;

}

double log_x(double dBase, double dValue)
{
  return 
    //MSVC++ has no log2() function (in <math.h>).
    //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
    log10
    (
      dValue
    )
    / log10(dBase) ;
}
