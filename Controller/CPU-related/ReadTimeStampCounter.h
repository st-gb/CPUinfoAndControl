/*
 * ReadTimeStampCounter.h
 *
 *  Created on: Jun 13, 2010
 *      Author: Stefan
 */

#ifndef READTIMESTAMPCOUNTER_H_
#define READTIMESTAMPCOUNTER_H_

//TODO use affinity mask for reading the TSC
//http://en.wikipedia.org/wiki/Time_Stamp_Counter:
//"[...]whether all cores (processors) have identical values in their
//time-keeping registers. There is no promise that the timestamp counters of
//multiple CPUs on a single motherboard will be synchronized.
//In such cases, programmers can
//only get reliable results by locking their code to a single CPU."
//->on a motherboard CPU/ CPU core 0 may have TSC freq of 1000 MHz,
//CPU/ CPU core 1 may have TSC freq of 1200 MHz.
//If not using affinity mask TSC may be read on CPU0 and afterwards on CPU1.

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
