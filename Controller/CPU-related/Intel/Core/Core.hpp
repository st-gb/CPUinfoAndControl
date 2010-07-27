/*
 * Core.hpp (".hpp" -> C++ header file because "inline" is used)
 *
 *  Created on: Jul 21, 2010
 *      Author: Stefan
 */

#ifndef CORE_H_
#define CORE_H_

#include <preprocessor_helper_macros.h> //BITMASK_FOR_LOWMOST_3BIT
//for ReadMSR(), WriteMSR(); should be the file for
// I_CPUcontroller-derived class (that calls I_CPUcontroller-derived::ReadMSR()
//or for the DLL (that calls the exe's exported function)
//1 include path must match the path where the header file is in
#include <inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
#include <float.h> //FLT_MIN

  extern BYTE g_byValue1 ;
  extern float g_fReferenceClockInMHz ;
  DWORD g_dwValue1, g_dwValue2 ;

  BYTE GetCurrentVoltageAndFrequencyIntelCore(
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
      //Diff 1.0875 (VID 30) & 0.925 (VID=17) = 0.1625V
      //1 voltage ID step = 0.1625V / (30-17) = 0.1625V / 13 = 0.0125
      //0.925- 17 * 0.0125 = 0.7125 ;
      * p_fVoltageInVolt = 0.7125 + g_byValue1 * 0.0125 ;

      g_byValue1 = ( g_dwValue1 >> 8 ) & 255 ;
      //1000 0110 bin (134dec) was multiplier 3
      //     110bin=6dec
      //If multiplier should be divided by "2".
      //73 (0100 1001 bin) was multi/ FID "9"
      //         1001 bin = 9dec
      //-> only the lowmost 4 bits are used?!
      if( g_byValue1 & 128 )
      {
        g_byValue1 = g_byValue1 & BITMASK_FOR_LOWMOST_5BIT ;
        //Intel: "15:0 Current performance State Value"
        //   "63:16 Reserved"
        * p_fMultiplier = (float) g_byValue1 / 2.0 ;
      }
      else
        * p_fMultiplier = g_byValue1 ;
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

  inline float GetTemperatureInDegCelsiusIntel(
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
        100 - g_byValue1 ;
      return 1 ;
    }
    return FLT_MIN ;
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
    DWORD dwLowmostBits , dwHighmostBits = 0 ;
    //Intel "B-10Vol. 3" "Table B-2.  IA-32 Architectural MSRs (Contd.)" :
    // "199H 409 IA32_PERF_CTL  (R/W)"
    // "15:0  Target performance State Value"
    //  "31:16  Reserved"
    //  "32  IDA Engage. (R/W)   When set to 1: disengages IDA
    //   since: 06_0FH (Mobile)
    //  "63:33 Reserved"
    dwLowmostBits =
      //Freq / "FSB in MHz" = multiplier
      (BYTE) fMultiplier ;

//    BYTE byRet =
//      (*g_pfnreadmsr) (
//      IA32_PERF_STATUS,
//      & dwLowmostBits,// bit  0-31 (register "EAX")
//      & dwHighmostBits,
//      1 << wCoreID //m_dwAffinityMask
//      ) ;

    BYTE byVoltageID = (dwLowmostBits ) ;
    //see "GetCurrentVoltageAndFrequency(...)":
    //VoltageInVolt = 0.7125 + VoltageID * 0.0125 | -0.7125
    //VoltageInVolt - 0.7125 = VoltageID * 0.0125 | :0.0125
    //( VoltageInVolt - 0.7125 ) / 0.0125 = VoltageID
    //<=> VoltageID = ( VoltageInVolt - 0.7125 ) / 0.0125
    byVoltageID = (BYTE) ( (fVoltageInVolt - 0.7125f ) / 0.0125f ) ;
    dwLowmostBits = byVoltageID ;

    BYTE byFrequencyID ;
    if( //multi 3 -> multi 6 / 2
      fMultiplier < 6.0
      //half multipliers, e.g. multi 9.5 -> multi 19 / 2
      || ( fMultiplier - (BYTE) fMultiplier > 0.0 )
      )
    {
      //If multiplier should be divided by "2".
      byFrequencyID = 128 ;
      //if e.g multi = 3: set Frequency IDentifier to 6, divisor 2
      byFrequencyID |= (BYTE) ( fMultiplier * 2 ) ; // :2 * 2 = 1
    }
    else
      byFrequencyID = (BYTE) fMultiplier ;
//    BYTE byFrequencyID = ( dwLowmostBits >> 8 ) & 255 ;
    dwLowmostBits |= ( byFrequencyID << 8 ) ;

    //10000110bin (134dec) was multiplier 3
    //     110bin=6dec
    //If multiplier should be divided by "2".
//    if( byFrequencyID & 128 )
//    {
//      byFrequencyID = byFrequencyID & BITMASK_FOR_LOWMOST_5BIT ;
//      //Intel: "15:0 Current performance State Value"
//      //   "63:16 Reserved"
//      * p_fMultiplier = (float) byFrequencyID / 2.0 ;
//    }
//    else
//      * p_fMultiplier = byFrequencyID ;

//    byFrequencyID =
//    std::stringstream ss ;
//    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
//        << dwLowmostBits ;
//    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
    g_byValue1 =
      //g_pi_cpuaccess->WrmsrEx(
//      (*g_pfn_write_msr) (
      WriteMSR(
      IA32_PERF_CTL,
      dwLowmostBits,// bit  0-31 (register "EAX")
      dwHighmostBits, //"63:33 Reserved"
      //m_dwAffinityMask
      1 << byCoreID
      ) ;
    return g_byValue1 ;
  }

#endif /* CORE_H_ */
