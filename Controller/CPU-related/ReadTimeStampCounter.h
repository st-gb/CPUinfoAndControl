/*
 * ReadTimeStampCounter.h
 *
 *  Created on: Jun 13, 2010
 *      Author: Stefan
 */

#ifndef READTIMESTAMPCOUNTER_H_
#define READTIMESTAMPCOUNTER_H_

//for MSVC ReadTimeStampCounter() already defined in winnt.h
#ifndef _MSC_VER //Microsoft compiler
  inline //ULONGLONG
    void ReadTimeStampCounter(DWORD & a //r_dwLow
      , DWORD & d //r_dwHigh
      )
  {
//    unsigned a, d;
   // //Adapted from http://www.fftw.org/cycle.h:
   // ULONGLONG ull ;
   // __asm {
    //RDTSC
   // ret
    //mov ull, edx
    //mov ull, eax
   // }
   // return ull ;
//    return __rdtsc() ;
//#else //__MINGW__ etc.
    //from http://www.cs.wm.edu/~kearns/001lab.d/rdtsc.html:
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
    //return ((unsigned long long)a) | (((unsigned long long)d) << 32) ;
  }
  inline ULONGLONG ReadTimeStampCounter()
  {
    unsigned a, d;
   // //Adapted from http://www.fftw.org/cycle.h:
   // ULONGLONG ull ;
   // __asm {
    //RDTSC
   // ret
    //mov ull, edx
    //mov ull, eax
   // }
   // return ull ;
//    return __rdtsc() ;
//#else //__MINGW__ etc.
    //from http://www.cs.wm.edu/~kearns/001lab.d/rdtsc.html:
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long long)a) | (((unsigned long long)d) << 32) ;
  }
#endif

#endif /* READTIMESTAMPCOUNTER_H_ */
