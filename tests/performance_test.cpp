/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include <windef.h> //BYTE
#include "../Controller/CPU-related/Intel/Core/Core.hpp"
#include <iostream>
#include <stdio.h>

#ifndef DO_MOT_COMPILE_INLINE
  inline 
#endif //#ifndef DO_MOT_COMPILE_INLINE
void test(int i)
//see http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Inline.html:
#ifndef DO_MOT_COMPILE_INLINE
//http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Inline.html:
//"GCC does not inline any functions when not optimizing unless you specify the `always_inline' attribute for the function"
__attribute__((always_inline))
#endif //#ifndef DO_MOT_COMPILE_INLINE
;

#ifndef DO_MOT_COMPILE_INLINE
  inline
#endif //#ifndef DO_MOT_COMPILE_INLINE
void test(int i)
#ifndef DO_MOT_COMPILE_INLINE
//http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Inline.html:
//"GCC does not inline any functions when not optimizing unless you specify the `always_inline' attribute for the function"
 //__attribute__((always_inline))
#endif //#ifndef DO_MOT_COMPILE_INLINE
{
  i ++ ;
}

int main(int argc, char * argv[] )
{
  double dTimeDiff ; 
  time_t time_tStart, time_tEnd ;
  time_tStart = time (NULL);
  DWORD dwNumTimes = 1000000000 ;
  if( argc > 1 )
  {
    std::stringstream std_strstream ;
    std_strstream << argv[1] ;
    std_strstream >> dwNumTimes;
    if( std_strstream.bad() )
    {
      std::cout <<  "could not convert \"" << argv[1] << "\" to a number\n" ;
    }
  }
  for( DWORD dw = 0 ; dw < dwNumTimes ; ++ dw )
    GetTemperatureInDegCelsiusIntel(1) ;
    //test(1) ;
  time_tEnd = time (NULL);
  //http://www.cplusplus.com/reference/clibrary/ctime/difftime/
  dTimeDiff = difftime (time_tEnd,time_tStart);
  std::cout <<  "iterating took " << dTimeDiff << "seconds\n" ;
}
