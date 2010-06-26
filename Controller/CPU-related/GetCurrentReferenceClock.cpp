/*
 * GetCurrentReferenceClock.cpp
 *
 *  Created on: Jun 4, 2010
 *      Author: Stefan
 */

//  //Declare and use global variables to be faster (using local variables creates
//  //them on stack each time).
//  float g_fReferenceClockInHertz ;
//  float g_fReferenceClockInMHz ;
//  unsigned long long int g_ullPreviousTimeStampCounter ;
//  unsigned long long int g_ullCurrentTimeStampCounter ;
//  unsigned long long int g_ullTimeStampCounterDiff = 0 ;
//  DWORD g_dwPreviousTickCountInMilliseconds ;
//  DWORD g_dwTickCountInMilliseconds ;
//  DWORD g_dwTickCountDiffInMilliseconds ;
//
//  //inline: do not compile as function, but expand code for every call (->faster)
//  //inline
//  void GetCurrentReferenceClock(float fDivisor )
//  {
//    if( ! g_ullTimeStampCounterDiff ) // ^= "= 0"
//    {
//      g_ullPreviousTimeStampCounter = ReadTimeStampCounter() ;
//      g_dwPreviousTickCountInMilliseconds = ::GetTickCount();
//      //Wait some milliseconds to get a time difference.
//      ::Sleep(500) ;
//    }
//    //Use global var., so it does not need to be created on stack for every time.
//    g_dwTickCountInMilliseconds = ::GetTickCount() ;
//    g_dwTickCountDiffInMilliseconds =
//      //Use this macro in order to take a value wrap into account.
//      ULONG_VALUE_DIFF( g_dwTickCountInMilliseconds,
//      g_dwPreviousTickCountInMilliseconds) ;
//    //If getcurrentpstate for core 0 and directly afterwards for core 1,
//    //g_dwTickCountDiffInMilliseconds may be too small->too inexact,
//    //if =0: division by zero.
//    if( g_dwTickCountDiffInMilliseconds > 1000 )
//    {
//      g_dwPreviousTickCountInMilliseconds = g_dwTickCountInMilliseconds ;
//
////      std::stringstream stdstrstream ;
//    //  stdstrstream << g_dwTickCountDiffInMilliseconds ;
//    //  MessageBox(NULL, stdstrstream.str().c_str() , "info" , MB_OK) ;
//
//      g_ullCurrentTimeStampCounter = ReadTimeStampCounter() ;
//      g_ullTimeStampCounterDiff = ULONGLONG_VALUE_DIFF(
//        g_ullCurrentTimeStampCounter , g_ullPreviousTimeStampCounter ) ;
//      g_ullPreviousTimeStampCounter = g_ullCurrentTimeStampCounter ;
//    //  stdstrstream << g_ullTimeStampCounterDiff ;
//    //  MessageBox(NULL, stdstrstream.str().c_str() , "info" , MB_OK) ;
//
//      g_fReferenceClockInHertz = 1000.0 / (float) g_dwTickCountDiffInMilliseconds *
//          //720qm has 1,600 M TSC clockticks/s for multiplier 12 -> ": 12"
//          (double) g_ullTimeStampCounterDiff / //12.0 ;
//          fDivisor ;
//      g_fReferenceClockInMHz = g_fReferenceClockInHertz / 1000000.0 ;
//    //  stdstrstream << fReferenceClockInMHz ;
//    //  MessageBox(NULL, stdstrstream.str().c_str() , "info" , MB_OK) ;
//    }
//  }
