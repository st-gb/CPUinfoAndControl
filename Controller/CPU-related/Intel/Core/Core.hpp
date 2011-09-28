/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
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
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include <winuser.h> //MessageBox(...)
  #include <windows.h> //MessageBox(...)
#endif
//  extern BYTE g_byValue1 ;

//Define global variables here, so that I_CPUcontroller-derived class and
// DLL sourcecode both do not need to define them (or better in a file named
// "Core.cpp" ? ). These variables have a generic name ( "...Value">>#<<)
// because they are used for different purposes, else more global variables
// would be needed.
  BYTE g_byValue1, g_byValue2 ;
//  extern float g_fReferenceClockInMHz ;
  float g_fReferenceClockInMHz ;
//  DWORD g_dwValue1, g_dwValue2 ;
  extern uint32_t g_dwValue1, g_dwValue2 ;
  float g_fValue1 ;

  inline float GetMultiplierAsEncodedInMSRIntelCore(DWORD dwValue ) ;

  inline float * GetAvailableMultipliersIntelCore(WORD & r_byNumMultis)
  {
    float * ar_f = NULL ;
    //                        byte index: 7  6  5  4  3  2  1  0
    //example: "value at MSR address 408:06 23 73 42 06 00 73 42 "
    // 6: min. multiplier (if SuperLowFrequencyMode= min. Multi= 6/2=3)
    // 73: max. multiplier: 73dec = 1001001bin
    //                                 1001bin = 9dec
    //  -> max. Multi ~= 9 -> 9 or 9.5
    g_byValue1 =
//      (*g_pfnreadmsr) (
      ReadMSR(
      IA32_PERF_STATUS,
      & g_dwValue1,// bits 0-31 (register "EAX")
      & g_dwValue2,
      1 //<< wCoreID //m_dwAffinityMask
      ) ;
    if( g_byValue1 ) //successfully read from MSR.
    {
      //LowestFrequencyMode (LFM) multiplier.
      g_byValue2 = ( ( g_dwValue2 >> 24 ) & BITMASK_FOR_LOWMOST_8BIT ) ;
      DEBUGN("LowestFrequencyMode (LFM) multiplier:" << (WORD) g_byValue2 )
      //Max multiplier.
      g_fValue1 = GetMultiplierAsEncodedInMSRIntelCore( g_dwValue2 ) ;
      DEBUGN("Max multiplier:" << g_fValue1 )
      //ex. # multi if LowFrequencyMode (LFM=6-> SuperLowFrequencyMode=3), max.
      // multi = 9.5: {3,4,5,  6;6.5;7;7.5;8,8.5,9,9.5} = 3 + 8 = 11 multipliers
      //Number of different multipliers = highest multiplier-lowest multiplier + 1
//      g_byValue1 =
      r_byNumMultis = (WORD) (
        //# SuperLowFrequencyMode multis
//        g_byValue2 / 2 +
        //e.g. ( 9.5 - 6 )*2+1 = (3.5)*2 +1 = 7.0+1 = 8
        //Maximum multiplier.
        //( ( g_dwValue1 >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ) -
        ( g_fValue1 -
        //SuperLowFrequencyMode: LowFrequencyMode multiplier / 2
        g_byValue2 / 2
        // // lowest LowestFrequencyMode multi
        // g_byValue2
        ) * 2 + 1 ) ;
      DEBUGN("# multis:" << (WORD) r_byNumMultis )
      ar_f = new float [ //g_byValue1
        r_byNumMultis ] ;
      if( ar_f ) //Allocating memory on heap succeeded.
      {
        //loop "Number of different multipliers" times.
        //for( -- g_byValue1 ; g_byValue1 > 255 ; -- g_byValue1 )
        for( g_byValue1 = 0 ; g_byValue1 < r_byNumMultis ; ++ g_byValue1 )
        {
          g_fValue1 = (float) g_byValue2 / 2.0 ;
          ar_f[ g_byValue1 ] =
            //Minimum multi + Index
//            ( g_byValue2 + g_byValue1 )
            g_fValue1 + (float) g_byValue1 * 0.5 ;
          DEBUGN("adding multiplier " << g_fValue1 )
        }
      }
    }
    return ar_f ;
  }

  inline float * GetAvailableVoltagesInVoltIntelCore(WORD & r_byNumVoltages)
  {
    float * ar_f = NULL ;
    //                    byte index:7  6  5  4 3 2  1  0
    //ex.: "value at MSR address 408:6 23 73 42 6 0 73 42 "
    // 23: min. voltage: = 0.7125 + 23*0.0125V = 1 V
    // 42=max. Spannung: 0.7125V + 42 * 0.0125V = 0.7125V + 0.525 = 1.2375V
    g_byValue1 =
//      (*g_pfnreadmsr) (
      ReadMSR(
      IA32_PERF_STATUS,
      & g_dwValue1,// bit  0-31 (register "EAX")
      & g_dwValue2, //bytes 4-8
      1 //<< wCoreID //m_dwAffinityMask
      ) ;
    if( g_byValue1 ) //Reading from ModelSpecificRegister succeeded.
    {
//      //Lowest voltage ID.
//      g_byValue2 = ( ( g_dwValue2 >> 16 ) & BITMASK_FOR_LOWMOST_8BIT ) ;
      //Number of different voltages = highest voltage ID-lowest voltage ID + 1
//      g_byValue1 =
      r_byNumVoltages =
        //Highest voltage ID.
        ( g_dwValue2 & BITMASK_FOR_LOWMOST_8BIT )
//        //lowest voltage ID
//        - g_byValue2
        + 1 ;
      ar_f = new float [ //g_byValue1
        r_byNumVoltages ] ;
      //g_byValue1 = r_byNumVoltages ;
      if( ar_f ) //Allocating memory on heap succeeded.
      {
        //for( -- g_byValue1 ; g_byValue1 > 255 ; -- g_byValue1 )
        for( g_byValue1 = 0 ; g_byValue1 < r_byNumVoltages ; ++ g_byValue1 )
        {
          ar_f[g_byValue1] = 0.7125 +
//            //Minimum voltage ID + Index
//            ( g_byValue2 + g_byValue1 )
            g_byValue1
            * 0.0125 ;
        }
      }
    }
    return ar_f ;
  }

  inline float GetVoltageAsEncodedInMSRIntelCore(BYTE byValue)
  {
    //Diff 1.0875 (VID 30) & 0.925 (VID=17) = 0.1625V
    //1 voltage ID step = 0.1625V / (30-17) = 0.1625V / 13 = 0.0125
    //0.925- 17 * 0.0125 = 0.7125 ;
    return 0.7125 + byValue * 0.0125 ;
  }

  inline float GetMultiplierAsEncodedInMSRIntelCore(DWORD dwValue )
  {
    static BYTE byFrequencyIDentifier = 0 ;
    //g_byValue1 =
    byFrequencyIDentifier =
      ( dwValue >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ;
    //1000 0110 bin (134dec) was multiplier 3
    //      110 bin=6dec
    //If multiplier should be divided by "2".
    //73 (0100 1001 bin) was multi/ FID "9"
    //         1001 bin = 9dec
    //-> only the lowmost 4 bits are used?!
    //g_byValue2 = g_byValue1 & BITMASK_FOR_LOWMOST_5BIT ;
//    byFrequencyIDentifier &= BITMASK_FOR_LOWMOST_5BIT ;
    if( //g_byValue1 & 128
        byFrequencyIDentifier & 128 //128dec = 1000 0000bin
      )
    {
      //Intel: "15:0 Current performance State Value"
      //   "63:16 Reserved"
      return (float) //g_byValue2 / 2.0 ;
        ( byFrequencyIDentifier & BITMASK_FOR_LOWMOST_5BIT ) / 2.0 ;
    }
    else
    {
      if( //Half multi
//        g_byValue1 & 64
        byFrequencyIDentifier & 64 //64dec = 0100 0000bin
        )
        return (float) //g_byValue2 + 0.5 ;
          ( byFrequencyIDentifier & BITMASK_FOR_LOWMOST_5BIT ) + 0.5 ;
      else
        //Neither bit 7 nor bit 8 is set.
        return //g_byValue2 ;
          byFrequencyIDentifier ;
    }
  }

  float GetVoltageID_IntelCore_asFloat(float fVoltageInVolt)
  {
    //see "GetCurrentVoltageAndFrequency(...)":
    //VoltageInVolt = 0.7125 + VoltageID * 0.0125 | -0.7125
    //VoltageInVolt - 0.7125 = VoltageID * 0.0125 | :0.0125
    //( VoltageInVolt - 0.7125 ) / 0.0125 = VoltageID
    //<=> VoltageID = ( VoltageInVolt - 0.7125 ) / 0.0125
    return (fVoltageInVolt - 0.7125f ) / 0.0125f ;
  }

  //Purpose of this function: converting from float to integer causes rounding
  // errors:
  // Analogy: for an AMD Griffin at 1.18 V the voltage ID in float is 29.999998
  //  and would get 29 (because automatically rounded down 1.164 V) when
  //  converting to an integer.
  // For 1.164 voltage ID in float is 29.000004
  inline BYTE GetClosestVoltageID_IntelCore2(float fVoltageInVolt )
  {
    //WORD wVoltageID = (fVoltageInVolt - 0.7 ) / 0.016 ;
  //  float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
    float fVoltageID = //fVoltageMinusLowestVoltage / 0.016 ;
      GetVoltageID_IntelCore_asFloat(fVoltageInVolt) ;
    DEBUGN("GetClosestVoltageID--fVoltageID" << fVoltageID )
    //ceil( fVoltageID );
    WORD wVoltageID = //(fVolt - 0.7 ) / 0.016 ;
      //Avoid g++ warning "warning: converting to `WORD' from `float'"
      (WORD) fVoltageID ;
    DEBUGN("GetClosestVoltageID--wVoltageID" << wVoltageID )
    if( fVoltageID - (float) wVoltageID >=
        //e.g. if desired voltage is 7.014: 7.014 - 7 = 0.014; 0.014/0.016=0.875
        0.5 )
      ++ wVoltageID ;
    DEBUGN("GetClosestVoltageID--wVoltageID" << wVoltageID )
    DEBUGN("GetClosestVoltageID--voltage in Volt" << fVoltageInVolt
      << "-> voltage ID:" << wVoltageID )
    return wVoltageID ;
  }

  inline BYTE GetCurrentVoltageAndFrequencyIntelCore(
    float * p_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float * p_fMultiplier
    , float * p_fReferenceClockInMHz
    , WORD wCoreID
    )
  {
//    DWORD g_dwValue1 , g_dwValue2 ;
  //  //Intel: 198H 408 IA32_PERF_STATUS
    g_byValue1 =
//      (*g_pfnreadmsr) (
      ReadMSR(
      IA32_PERF_STATUS,
      & g_dwValue1,// bit  0-31 (register "EAX")
      & g_dwValue2,
      1 << wCoreID //m_dwAffinityMask
      ) ;
    if( g_byValue1 ) //success
    {
      g_byValue1 = (g_dwValue1 ) ;
      * p_fVoltageInVolt = GetVoltageAsEncodedInMSRIntelCore( g_byValue1 ) ;
      * p_fMultiplier = GetMultiplierAsEncodedInMSRIntelCore(g_dwValue1) ;
      DEBUGN("FID:" << g_byValue1 << "multiplier: " << * p_fMultiplier )

      //Although (see http://en.wikipedia.org/wiki/Time_Stamp_Counter:
      // paragraph "Implementation in various processors")
      //Core 2 should have a _constant_ Time Stamp Counter:
      //See "Intel® Core™2 Duo Mobile Processor, Intel® Core™2 Solo Mobile
      //Processor and Intel® Core™2 Extreme Mobile Processor on 45-nm Process
      //Datasheet Doc#320120-004.32012001
      //For platforms based on Mobile Intel® 4 Series Express Chipset Family
      //March 2009",
      //chapter "2.4.1Dynamic FSB Frequency Switching":
      //"Dynamic FSB frequency switching effectively reduces the internal bus
      //clock frequency in half to further decrease the minimum processor
      //operating frequency from the Enhanced Intel SpeedStep Technology
      //performance states and achieve the Super Low Frequency Mode (Super LFM).
      //This feature is supported at FSB frequencies of 1066MHz, 800 MHz and
      //667 MHz and does not entail a change in the external bus signal (BCLK)
      //frequency.
      //Instead, both the processor and GMCH internally lower their BCLK
      //reference frequency to 50% of the externally visible frequency. Both
      //the processor and GMCH maintain a virtual BCLK signal (VBCLK) that is
      //aligned to the external BCLK but at half the frequency.
      //After a downward shift, it would appear externally as if the bus is
      //running with a 133-MHz base clock in all aspects, except that the
      //actual external BCLK remains at 266 MHz. See Figure3 for details.
      //The transition into Super LFM, a “down-shift,” is done following a
      //handshake between the processor and GMCH. A similar handshake is used
      //to indicate an “up-shift,” a change back to normal operating mode.
      //Please ensure this feature is enabled and supported in the BIOS."
      //"2.4.2Enhanced Intel® Dynamic Acceleration Technology
      //The processor supports Intel Dynamic Acceleration Technology mode.
      //The Intel Dynamic Acceleration Technology feature allows one core of
      //the processor to operate at a higher frequency point when the other
      //core is inactive and the operating system requests increased
      //performance.
      //This higher frequency is called the opportunistic frequency and the
      //maximum rated operating frequency is the ensured frequency.
      //The processor includes a hysteresis mechanism that improves overall
      //Intel Dynamic Acceleration Technology performance by decreasing
      //unnecessary transitions of the cores in and out of Intel Dynamic
      //Acceleration Technology mode. Normally, the processor would exit
      //Intel Dynamic Acceleration Technology as soon as two cores are active.
      //This can become an issue if the idle core is frequently awakened for
      //a short periods (i.e., high timer tick rates).
      //The hysteresis mechanism allows two cores to be active for a limited
      //time before it transitions out of Intel Dynamic Acceleration
      //Technology mode."

  //    //This call sets g_fReferenceClockInMHz to the current reference clock.
  //    //This update of the value would be senseful for setting the CPU core
  //    //via "frequency" as parameter value the next time.
  //    GetCurrentReferenceClock(12.0 ,
  //      1 , //min. timespan in ms
  //      10000 ) ;
      * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
      return 1 ;
    }
//    else
      return 0 ;
  }

  inline void GetReferenceClockFromMSR_FSB_FREQ()
  {
    //"CDH 205 MSR_FSB_FREQ Shared [between cores?] Scaleable Bus Speed(RO)".
//      This field indicates the intended scaleable bus
//      clock speed for processors based on Intel Core
//      microarchitecture:
//    "2:0"
    if( ReadMSR(
        MSR_FSB_FREQ ,
        & g_dwValue1 , //lowmost bits 0-31
        & g_dwValue2 ,
        1
        )
      )
    {
//      DEBUGN("Scaleable Bus Speed(RO):" <<
//        g_dwValue1 & BITMASK_FOR_LOWMOST_3BIT )
      //bits: "2:0"
      switch( g_dwValue1 & BITMASK_FOR_LOWMOST_3BIT )
      {
//      •101B: 100 MHz (FSB 400)
//      •001B: 133 MHz (FSB 533)
//      •011B: 167 MHz (FSB 667)
//      •010B: 200 MHz (FSB 800)
//      •000B: 267 MHz (FSB 1067)
//      •100B: 333 MHz (FSB 1333)
      case 0:
        g_fReferenceClockInMHz = 267 ;
        break ;
      case 1:
        g_fReferenceClockInMHz = 133 ;
        break ;
      case 2:
        g_fReferenceClockInMHz = 200 ;
        break ;
      case 3:
        g_fReferenceClockInMHz = 167 ;
        break ;
      case 4:
        g_fReferenceClockInMHz = 333 ;
        break ;
      case 5: //101bin
        g_fReferenceClockInMHz = 100 ;
        break ;
      }
    }
  }

#ifdef DO_MOT_COMPILE_INLINE
#else
  inline
#endif
  float GetTemperatureInDegCelsiusIntel(
    WORD wCoreID )
  {
    //"23:16 Temperature Target. (R) "
//    byTempTarget = ( dwLowmostBits >> 16 ) & 255 ;
//    (*g_pfnreadmsr) (
    g_byValue1 = ReadMSR(
       IA32_THERM_STATUS ,
       & g_dwValue1 , // bit  0-31 (register "EAX")
       & g_dwValue2 ,
       //m_dwAffinityMask
       1 << wCoreID
       ) ;
    if( g_byValue1 ) //if successfull
    {
      //Intel: "22:16 Digital Readout (RO)"
      g_byValue1 = ( g_dwValue1 >> 16 ) & BITMASK_FOR_LOWMOST_7BIT ;

//      //Intel: "30:27 Resolution in Degrees Celsius (RO)"
//      byResolutionInDegreesCelsius = ( dwLowmostBits >> 27 ) &
//        BITMASK_FOR_LOWMOST_5BIT ;

      g_byValue1 = //-56 -
        105 - g_byValue1 ;
      return g_byValue1 ;
    }
    return FLT_MIN ;
  }

  inline BYTE EncodeMultiplierAsInMSRIntelCore(float fMultiplier)
  {
    BYTE byFrequencyID ;
    //Intel "B-10Vol. 3" "Table B-2.  IA-32 Architectural MSRs (Contd.)" :
    // "199H 409 IA32_PERF_CTL  (R/W)"
    // "15:0  Target performance State Value"
    //  "31:16  Reserved"
    //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA
    //   since: 06_0FH (Mobile)
    //  "63:33 Reserved"
    if( //multi 3 -> multi 6 / 2
      fMultiplier < 6.0
      )
    {
      //If multiplier should be divided by "2".
      byFrequencyID = 128 ;
      //if e.g multi = 3: set Frequency IDentifier to 6, divisor 2
      byFrequencyID |= (BYTE) ( fMultiplier * 2 ) ; // :2 * 2 = 1
    }
    else
      byFrequencyID = (BYTE) fMultiplier ;
    if( //half multipliers, e.g. multi 9.5 -> //multi 19 / 2
        //73dec= 0100 1001
        //            1001=9 dec
      fMultiplier - (BYTE) fMultiplier > 0.0
      )
    {
      byFrequencyID |= 64 ; //flag for half multiplier.
    }
  //    BYTE byFrequencyID = ( dwLowmostBits >> 8 ) & 255 ;
    return byFrequencyID ;
  }

  inline BYTE SetCurrentVoltageAndMultiplierIntelCore(
    float fVoltageInVolt ,
    //Intel® Core™2 Duo Mobile Processor, Intel® Core™2 Solo Mobile Processor
    //and Intel® Core™2 Extreme Mobile Processor on 45-nm Process
//      Datasheet
//      For platforms based on Mobile Intel® 4 Series Express Chipset Family
//      March 2009
    // "2.2Enhanced Intel SpeedStep® Technology"
    //  page 19:
    //"Each core in the dual-core processor implements an independent MSR
    //for controlling Enhanced Intel SpeedStep Technology, but both cores
    //must operate at the same frequency and voltage.
    //The processor has performance state coordination logic to resolve
    //frequency and voltage requests from the two cores into a single
    //frequency and voltage request for the package as a whole.
    //If both cores request the same frequency and voltage, then the
    //processor will transition to the requested common frequency and
    //voltage.
    //If the two cores have different frequency and voltage requests, then
    //the processor will take the highest of the two frequencies and
    //voltages as the resolved request and transition to that frequency and
    //voltage.
    //The processor also supports Dynamic FSB Frequency Switching and
    //Intel Dynamic Acceleration Technology mode on select SKUs.
    //The operating system can take advantage of these features and request
    //a lower operating point called SuperLFM (due to Dynamic FSB
    //Frequency Switching) and a higher operating point Intel Dynamic
    //Acceleration Technology mode."
    float fMultiplier ,
    BYTE byCoreID
    )
  {
    DWORD dwLowmostBits //, dwHighmostBits = 0
      ;
//    dwLowmostBits =
//      //Freq / "FSB in MHz" = multiplier
//      (BYTE) fMultiplier ;
//
//    BYTE byVoltageID = (dwLowmostBits ) ;

    BYTE byVoltageID = GetClosestVoltageID_IntelCore2(fVoltageInVolt) ;

    dwLowmostBits = byVoltageID ;

    BYTE byFrequencyID = EncodeMultiplierAsInMSRIntelCore(fMultiplier) ;
    dwLowmostBits |= ( byFrequencyID << 8 ) ;

#ifdef _DEBUG
    bool bWrite = true ;
//    byFrequencyID =
    std::stringstream stdstringstreamMessage ;
//    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
//        << dwLowmostBits ;
    stdstringstreamMessage << "written voltage would be:"
      << GetVoltageAsEncodedInMSRIntelCore( dwLowmostBits )
      << "\nwritten multiplier would be:"
      << GetMultiplierAsEncodedInMSRIntelCore(dwLowmostBits) ;
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
      //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA
      //   since: 06_0FH (Mobile)
      //  "63:33 Reserved"
      0 ,
      //m_dwAffinityMask
      1 << byCoreID
      ) ;
    return g_byValue1 ;
  }

#endif /* CORE_H_ */
