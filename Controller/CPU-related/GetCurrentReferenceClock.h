/*
 * GetCurrentReferenceClock.hpp
 *
 *  Created on: Jun 4, 2010
 *      Author: Stefan
 */
//This file is intellectual property of Trilobyte SE GmbH, Berlin, Germany.
//Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany.
//It must not be used commercially without the permission of Trilobyte
//SE GmbH, Berlin, Germany.
//It may be used for educational / academic purposes for free.
//If you use (parts) of this sourcecode then this license text must be contained.
#ifndef GETCURRENTREFERENCECLOCK_HPP_
#define GETCURRENTREFERENCECLOCK_HPP_

#include <windows.h> // GetTickCount()
#include <Controller/value_difference.h> //ULONG_VALUE_DIFF
#include "ReadTimeStampCounter.h"
#include <global.h>

//inline void GetCurrentReferenceClock(float fDivisor) ;

//Declare and use global variables to be faster (using local variables creates
//them on stack each time).
float g_fReferenceClockInHertz ;
float g_fReferenceClockInMHz ;
unsigned long long int g_ullPreviousTimeStampCounter ;
unsigned long long int g_ullCurrentTimeStampCounter ;
unsigned long long int g_ullTimeStampCounterDiff = _UI64_MAX ;
DWORD g_dwPreviousTickCountInMilliseconds ;
DWORD g_dwTickCountInMilliseconds ;
DWORD g_dwTickCountDiffInMilliseconds = //ULONG_MAX ;
    0 ;

//inline: do not compile as function, but expand code for every call (->faster)
//Define the function in this _header_ file. It cannot be inline if declared in
// a header file and implemented in a source file?!
inline
void GetCurrentReferenceClock(float fDivisor //, float & fReferenceClockInMHz
  , DWORD dwMinimumTimeDiffInMilliseconds
  , DWORD dwMaximumTimeDiffInMilliseconds
  )
{
  //Use global var., so it does not need to be created on stack for every time.
  g_dwTickCountInMilliseconds = ::GetTickCount() ;
  g_dwTickCountDiffInMilliseconds =
    //Use this macro in order to take a value wrap into account.
    ULONG_VALUE_DIFF( g_dwTickCountInMilliseconds,
    g_dwPreviousTickCountInMilliseconds) ;
  DEBUGN("tick count diff in ms: " << g_dwTickCountDiffInMilliseconds )
  if( //g_ullTimeStampCounterDiff == _UI64_MAX // ^= "= 0"
      //If at the beginning / for the first time.
//      g_dwTickCountInMilliseconds == ULONG_MAX
//      g_dwTickCountDiffInMilliseconds //== 0
//      < dwMinimumTimeDiffInMilliseconds
//      ||
      g_dwTickCountDiffInMilliseconds > dwMaximumTimeDiffInMilliseconds
    )
  {
    g_dwPreviousTickCountInMilliseconds = g_dwTickCountInMilliseconds ;
    //Time diff too large: take a TSC measurement (again).
    g_ullPreviousTimeStampCounter = ReadTimeStampCounter() ;

//    //Wait some milliseconds to get a time difference.
//    ::Sleep(//500
//      dwMinimumTimeDiffInMilliseconds ) ;
//    g_dwTickCountInMilliseconds = ::GetTickCount();
//    g_dwTickCountDiffInMilliseconds =
////      //Use this macro in order to take a value wrap into account.
////      ULONG_VALUE_DIFF( g_dwTickCountInMilliseconds,
////      g_dwPreviousTickCountInMilliseconds) ;#
//        dwMinimumTimeDiffInMilliseconds ;
//    //overflows the same way as for GetTickCount() ;
//    g_dwTickCountInMilliseconds += dwMinimumTimeDiffInMilliseconds ;
    g_dwTickCountDiffInMilliseconds = 0 ;
  }
//  //Use global var., so it does not need to be created on stack for every time.
//  g_dwTickCountInMilliseconds = ::GetTickCount() ;
//  g_dwTickCountDiffInMilliseconds =
//    //Use this macro in order to take a value wrap into account.
//    ULONG_VALUE_DIFF( g_dwTickCountInMilliseconds,
//    g_dwPreviousTickCountInMilliseconds) ;
  //If getcurrentpstate for core 0 and directly afterwards for core 1,
  //g_dwTickCountDiffInMilliseconds may be too small->too inexact,
  //if =0: division by zero.
  if( g_dwTickCountDiffInMilliseconds >= //1000
    dwMinimumTimeDiffInMilliseconds )
  {
    DEBUGN("GetCurrentReferenceClock(...) tick count diff ("
      << g_dwTickCountDiffInMilliseconds << ") > min time span("
      << dwMinimumTimeDiffInMilliseconds << ")" )
    g_dwPreviousTickCountInMilliseconds = g_dwTickCountInMilliseconds ;
//      std::stringstream stdstrstream ;
  //  stdstrstream << g_dwTickCountDiffInMilliseconds ;
  //  MessageBox(NULL, stdstrstream.str().c_str() , "info" , MB_OK) ;

    g_ullCurrentTimeStampCounter = ReadTimeStampCounter() ;
    g_ullTimeStampCounterDiff = ULONGLONG_VALUE_DIFF(
      g_ullCurrentTimeStampCounter , g_ullPreviousTimeStampCounter ) ;
    DEBUGN("TSC diff: " << g_ullTimeStampCounterDiff )
    g_ullPreviousTimeStampCounter = g_ullCurrentTimeStampCounter ;
  //  stdstrstream << g_ullTimeStampCounterDiff ;
  //  MessageBox(NULL, stdstrstream.str().c_str() , "info" , MB_OK) ;

//    g_fReferenceClockInHertz = 1000.0 / (float) g_dwTickCountDiffInMilliseconds *
//        //720qm has 1,600 M TSC clockticks/s for multiplier 12 -> ": 12"
//        (double) g_ullTimeStampCounterDiff / //12.0 ;
//        fDivisor ;
    g_fReferenceClockInHertz = 1000.0 *
        //720qm has 1,600 M TSC clockticks/s for multiplier 12 -> ": 12"
        (double) g_ullTimeStampCounterDiff / //12.0 ;
        fDivisor / (float) g_dwTickCountDiffInMilliseconds ;
    g_fReferenceClockInMHz = g_fReferenceClockInHertz / 1000000.0 ;
//    fReferenceClockInMHz = g_fReferenceClockInHertz / 1000000.0 ;
  //  stdstrstream << fReferenceClockInMHz ;
  //  MessageBox(NULL, stdstrstream.str().c_str() , "info" , MB_OK) ;
  }
}

#endif /* GETCURRENTREFERENCECLOCK_HPP_ */
