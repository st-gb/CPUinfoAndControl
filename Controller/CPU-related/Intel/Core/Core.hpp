/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-at
 * least 2011. You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * Core.hpp (".hpp" -> C++ header file because "inline" is used)
 *
 *  Created on: Jul 21, 2010
 *      Author: Stefan
 */

#ifndef CORE_H_
#define CORE_H_

#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_3BIT
//for ReadMSR(), WriteMSR(); should be the file for
// I_CPUcontroller-derived class (that calls I_CPUcontroller-derived::ReadMSR()
//or for the DLL (that calls the exe's exported function)
//1 include path must match the path where the header file is in
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
#include <float.h> //FLT_MIN
#include <windef.h> //BYTE, DWORD
#include "CoreAndCore2.hpp"
#include "Core_and_Core2_GetVoltage.hpp"
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include <winuser.h> //MessageBox(...)
  #include <windows.h> //MessageBox(...)
#endif
//  extern BYTE g_byValue1 ;

//  extern float g_fReferenceClockInMHz ;
//  float g_fReferenceClockInMHz ;
//  DWORD g_dwValue1, g_dwValue2 ;
  extern uint32_t g_dwValue1, g_dwValue2 ;
  float g_fValue1 ;

namespace Intel
{
  namespace Core
  {
    inline float GetMultiplierAsEncodedInMSR(DWORD dwValue )
    {
      static BYTE byFrequencyIDentifier = 0 ;
      //g_byValue1 =
      byFrequencyIDentifier =
        ( dwValue >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ;
      return byFrequencyIDentifier;
    }

    inline float * GetAvailableVoltagesInVolt(WORD & r_byNumVoltages)
    {
      DEBUGN( FULL_FUNC_NAME << " begin")
  //    uint32_t lowmostBits, highmostBits;
      float * ar_f = NULL ;
      //For Intel Core T2400: MSR reg.# 0xCE
      //                    byte index:7  6  5
      //ex.: "value at MSR address 408:2C 13 0B "
      // 13: min. voltage: = 0.7125 + 13*0.0125V = 0.7125+0.1625V 0.95 V
      // 2C=max. voltage: 0.7125V + 42 * 0.0125V = 0.7125V + 0.525 = 1.2375V
      g_byValue1 =
  //      (*g_pfnreadmsr) (
        ReadMSR(
  //      IA32_PERF_STATUS,
        //MSR Address from image: http://img223.imageshack.us/img223/6914/msr.png
        //referred from http://www.techpowerup.com/forums/showthread.php?p=1572128
        0xCE,
        & g_dwValue1,// bit  0-31 (register "EAX")
        & g_dwValue2, //bytes 4-8
        1 //<< wCoreID //m_dwAffinityMask
        ) ;
      if( g_byValue1 ) //Reading from ModelSpecificRegister succeeded.
      {
  //      //Lowest voltage ID.
  //      g_byValue2 = ( ( g_dwValue2 >> 16 ) & BITMASK_FOR_LOWMOST_8BIT ) ;
#ifdef EMULATE_N270
        BYTE lowestVoltageID = 0xF;
        BYTE highestVoltageID = 0x1F;
#else
        BYTE lowestVoltageID = ( (g_dwValue2 >> 16 ) & BITMASK_FOR_LOWMOST_8BIT );
        DEBUGN( "lowest voltage ID:" << (WORD) lowestVoltageID)
        BYTE highestVoltageID = ( (g_dwValue2 >> 24 ) & BITMASK_FOR_LOWMOST_8BIT );
        DEBUGN( "highest voltage ID:" << (WORD) highestVoltageID)
#endif
        return Intel::CoreAndCore2::AllocateAndFillVoltageArray(
          lowestVoltageID, highestVoltageID, r_byNumVoltages);
      }
      return ar_f ;
    }

    inline BYTE GetCurrentVoltageAndFrequency(
      float * p_fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , float * p_fMultiplier
      , float * p_fReferenceClockInMHz
      , WORD wCoreID
      )
    {
  //    DWORD g_dwValue1 , g_dwValue2 ;
      g_byValue1 =
        ReadMSR(
        IA32_PERF_STATUS,
        & g_dwValue1,// bit  0-31 (register "EAX")
        & g_dwValue2,
        1 << wCoreID //m_dwAffinityMask
        ) ;
      if( g_byValue1 ) //success
      {
        g_byValue1 = (g_dwValue1 ) ;
        * p_fVoltageInVolt = Intel::CoreAndCore2::GetVoltage(
          g_byValue1 ) ;
        * p_fMultiplier = GetMultiplierAsEncodedInMSR(g_dwValue1) ;
        DEBUGN("FID:" << g_byValue1 << "multiplier: " << * p_fMultiplier )

        * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
        return 1 ;
      }
  //    else
        return 0 ;
    }

    inline BYTE SetCurrentVoltageAndMultiplier(
      float fVoltageInVolt ,
      float fMultiplier ,
      BYTE byCoreID
      )
    {
      DEBUGN( FULL_FUNC_NAME << "--begin")
      static uint32_t dwLowmostBits; //, dwHighmostBits = 0
      static BYTE byVoltageID;
      BYTE byFrequencyID;

      byVoltageID = Intel::CoreAndCore2::GetClosestVoltageID(fVoltageInVolt) ;
      dwLowmostBits = byVoltageID ;

      byFrequencyID = (BYTE)fMultiplier ;
      dwLowmostBits |= ( byFrequencyID << 8 ) ;

      DEBUGN( FULL_FUNC_NAME << " byVoltageID:" << (WORD) byVoltageID
        << "byFrequencyID:" << (WORD) byFrequencyID
        << " lowmost bits:" << dwLowmostBits)

  #ifdef _DEBUG
      bool bWrite = true ;
  //    byFrequencyID =
      std::stringstream stdstringstreamMessage ;
  //    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
  //        << dwLowmostBits ;
      stdstringstreamMessage << "written voltage would be:"
        << Intel::CoreAndCore2::GetVoltage( dwLowmostBits )
//        << "\nwritten multiplier would be:"
//        << GetMultiplierAsEncodedInMSRIntelCore(dwLowmostBits)
        ;
      std::string stdstrMessage =  stdstringstreamMessage.str() ;
      stdstrMessage += "\nshould be written?" ;
  #ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
      if( ::MessageBox( NULL, stdstrMessage.c_str(), "info" , //MB_OK
            MB_YESNO )
          == IDNO
        )
        bWrite = false ;
  #endif
      DEBUGN( stdstringstreamMessage.str() )
      if( bWrite )
  #endif
      g_byValue1 =
        WriteMSR(
        IA32_PERF_CTL,
        dwLowmostBits,// bit  0-31 (register "EAX")
        0 ,
        //m_dwAffinityMask
        1 << byCoreID
        ) ;
      return g_byValue1 ;
    }
  }
}

#endif /* CORE_H_ */
