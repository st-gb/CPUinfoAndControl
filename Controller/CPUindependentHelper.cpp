//#include <string>
//This file must have extension ".cpp" for including <string> (std::string )
//else error "C2059", "C2143" with MSVC in cstdio at "size_t" and below
//#include "global.h" //for DEBUG etc.
#include <windef.h> //for WORD, BYTE
#include "CPUindependentHelper.h"

BYTE GetCoreIDFromAffinityMask(DWORD dwAffinityMask )
{
  //core ID "0" is the 1st core.
  BYTE byCoreID = 0 ;
  //Examples:
  //Affinity mash for core 0 is 00000000 00000000 00000000 00000001
  //Affinity mash for core 1 is 00000000 00000000 00000000 00000010
  //Affinity mash for core 4 is 00000000 00000000 00000000 00001000
  while( dwAffinityMask >>= 1 )
    ++ byCoreID ;
  return byCoreID ;
}
