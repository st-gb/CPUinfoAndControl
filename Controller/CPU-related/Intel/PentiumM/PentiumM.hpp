/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * PentiumM.hpp
 *  For usage ( "inline" for no performance loss)
 *  by both I_CPUcontroller-derived class and by DLL code.
 *  Created on: Aug 3, 2010
 *      Author: Stefan
 */

#ifndef PENTIUMM_HPP_
#define PENTIUMM_HPP_

//BITMASK_FOR_LOWMOST_3BIT
#include <preprocessor_macros/bitmasks.h>
//for ReadMSR(), WriteMSR(); should be the file for
// I_CPUcontroller-derived class (that calls I_CPUcontroller-derived::ReadMSR()
//or for the DLL (that calls the exe's exported function)
//1 include path must match the path where the header file is in
//#include <inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
#include <preprocessor_macros/value_difference.h> //ULONGLONG_VALUE_DIFF
//ReadMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/inline_register_access_functions.hpp>
//for CPU_TEMP_IS_BELOW_CRITICAL, CPU_TEMP_IS_CRITICAL
#include <Controller/CPU-related/CPU_core_temperature_defs.h>
//PERFORMANCE_COUNTER_VALUE_DIFF(...)
#include <Controller/CPU-related/PerformanceCounterValueDiff.h>
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
//Intel::Pentium_M::GetVoltage(...)
#include <Controller/CPU-related/Intel/PentiumM/PentiumM_GetVoltage.hpp>
#include <winnt.h> //ULONGLONG
#include "PentiumM_registers.h" //MAXIMUM_PERFORMANCE_COUNTER_VALUE
#include <fastest_data_type.h> //typedef fastestUnsignedDataType
//Intel::PerformanceEventSelectRegisterWrite(...)
#include <Controller/CPU-related/Intel/PerformanceEventSelectRegisterWrite.hpp>

//These variables should be defined in a source (.cpp file) to avoid multiple
// definitions if this header file is more than once.
extern uint32_t g_ui32LowmostBits ;
extern uint32_t g_ui32HighmostBits ;

/** Needed for CPU::GetClockWithLoad(...) */
    inline float GetMultiplier(DWORD affMask)
    {
      if( ReadMSR(
          //Intel: 198H 408 IA32_PERF_STATUS
          IA32_PERF_STATUS
          , & g_ui32LowmostBits
          , & g_ui32HighmostBits
          , affMask
          )
        )
      {
        //Intel: "15:0 Current performance State Value"
        //   "63:16 Reserved"
//        static BYTE byFID;
        return (float) ( ( g_ui32LowmostBits >> 8 ) & BITMASK_FOR_LOWMOST_8BIT);
      }
      return 0.0f;
    }

  inline float GetVoltageAndMultiplier(DWORD affMask, float & r_fVoltageInVolt)
  {
    if( ReadMSR(
        //Intel: 198H 408 IA32_PERF_STATUS
        IA32_PERF_STATUS
        , & g_ui32LowmostBits
        , & g_ui32HighmostBits
        , affMask
        )
      )
    {
      //Intel: "15:0 Current performance State Value"
      //   "63:16 Reserved"
//        static BYTE byFID;
      r_fVoltageInVolt = Intel::Pentium_M::GetVoltage( //byVoltageID
        ( g_ui32LowmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
      return (float) ( ( g_ui32LowmostBits >> 8 ) & BITMASK_FOR_LOWMOST_8BIT);
    }
    return 0.0f;
  }

#include <Controller/CPU-related/GetCPUclockAndMultiplier.hpp>


//These variables may be defined in file "PentiumM.cpp" .
extern DWORD g_dwValue1 ;
extern float g_fReferenceClockInMHz ;
extern bool gs_b2ndTimeOrLaterReadTSCandFIDchange ;

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
//  #define PERFORMANCE_COUNTER_FOR_FID_CHANGE IA32_PMC0
  inline void SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent() ;
  #include "GetReferenceClockViaTSCdiffDivMultiIfNoFiDchange.hpp"
#endif //GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
  //For GetCurrentReferenceClock(...)
  #include <Controller/CPU-related/GetCurrentReferenceClock.hpp>

//Write to 2nd performance event because the 1st is often used by other
// programs like RMclock etc.
//When PMC 0 is not enabled then PMC1 does not count
#define PMC_COUNTER_INDEX /*1*/0

namespace Intel
{
  namespace Pentium_M
  {
    inline unsigned GetPerfEvent(
      fastestDataType PMCcounterNumber, uint32_t & eventSelectValue)
    {
      static uint32_t val;
      unsigned by = ::ReadMSR(
        IA32_PERFEVTSEL0 + PMCcounterNumber
        , & eventSelectValue
        , & val
        , 1
        );
      return by;
    }

    inline fastestUnsignedDataType PerformanceCounter0Enabled(bool & b)
    {
      static fastestUnsignedDataType retVal;
      retVal = ReadMSR(
        IA32_PERFEVTSEL0
        , (uint32_t *) & PMCvalue
        , ( (uint32_t *) & PMCvalue ) + 1
        , 1
        );
      if( retVal )
      { // <=> PMC not enabled
        b = PMCvalue >> PERFORMANCE_COUNTING_ENABLED_BIT_INDEX;
      }
      return retVal;
    }

    /** @brief event number 58 hex for counting Enhanced Intel SpeedStep
      technology transitions is specific for Intel Pentium M */
    inline unsigned SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent(
      fastestUnsignedDataType PMCcounterNumber)
    {
      if(PMCcounterNumber == 1 )
      {
        bool bPerformanceCounter0Enabled;
        if( PerformanceCounter0Enabled(bPerformanceCounter0Enabled)
            && ! bPerformanceCounter0Enabled)
          return PMCcounter0NotEnabled;
      }
      unsigned by = ::WriteMSR(
        IA32_PERFEVTSEL0 + PMCcounterNumber
        , MONITOR_FID_TRANSITIONS_PERF_SELECT_VALUE
        , 0
        , 1
        );
      return by;
    }

    enum GetPerformanceCounterValueRetCodes
    {
      succeeded,
      PMCcounter0NotEnabled
    };

    /** @brief gets Performance Monitoring value for Event selected in MSR
     *   register index "IA32_PERFEVTSEL0" + PMCcounterNumber if
     * -enable bit is set in "IA32_PERFEVTSEL0" + PMCcounterNumber
     * -
     *  Ensure PMC0 is active if using PMC1 else value is not incremented.
     *  @param PMCvalue must be 64 bit because an MSR has 64 bit*/
    inline /*unsigned*/ enum GetPerformanceCounterValueRetCodes
      GetPerformanceCounterValue(
      fastestDataType PMCcounterNumber,
      uint64_t & PMCvalue
      )
    {
      static fastestDataType retVal;
      if(PMCcounterNumber == 1 )
      {
        bool bPerformanceCounter0Enabled;
        if( PerformanceCounter0Enabled(bPerformanceCounter0Enabled) &&
            ! bPerformanceCounter0Enabled)
          return PMCcounter0NotEnabled;
      }
      retVal = ::ReadMSR(
        //IA32_PERFEVTSEL0
        //Intel vol. 3B:
        //"IA32_PMCx MSRs start at address 0C1H and occupy a contiguous block of MSR
        //address space; the number of MSRs per logical processor is reported using
        //CPUID.0AH:EAX[15:8]."
        //"30.2.1.1 Architectural Performance Monitoring Version 1 Facilities":
        //The bit width of an IA32_PMCx MSR is reported using the
        //CPUID.0AH:EAX[23:16]
        //
        IA32_PMC0 + PMCcounterNumber
//        PERFORMANCE_COUNTER_FOR_FID_CHANGE
        , (uint32_t *) & PMCvalue
        , ( (uint32_t *) & PMCvalue ) + 1
        , 1 );
       return retVal;
    }

    //Forward declarations of functions.
    //inline float GetVoltage(BYTE byVoltageID) ;
    inline float GetVoltageID_asFloat(float fVoltage ) ;

    //Purpose of this function: converting from float to integer causes rounding
    // errors:
    // Analogy: for an AMD Griffin at 1.18 V the voltage ID in float is 29.999998
    //  and would get 29 (because automatically rounded down 1.164 V) when
    //  converting to an integer.
    // For 1.164 voltage ID in float is 29.000004
    inline BYTE GetClosestVoltageID(float fVoltageInVolt )
    {
      //WORD wVoltageID = (fVoltageInVolt - 0.7 ) / 0.016 ;
    //  float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
      float fVoltageID = //fVoltageMinusLowestVoltage / 0.016 ;
        GetVoltageID_asFloat(fVoltageInVolt) ;
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
      DEBUGN("GetClosestVoltageID--voltage in Volt" << fVoltageInVolt << "-> voltage ID:"
        << wVoltageID )
      return wVoltageID ;
    }

    inline BYTE GetCurrentVoltageAndFrequency(
      float & r_fVoltageInVolt
      , float & r_fMultiplier
      , float & r_fReferenceClockInMHz
      )
    {
      //  //This call sets g_fReferenceClockInMHz to the current reference clock.
      //  //This update of the value would be senseful for setting the CPU core
      //  //via "frequency" as parameter value the next time.
      //  GetCurrentReferenceClock(
      //    //For Pentium M the TimeStampCounter has the same frequency as the CPU core
      //    //clock-> must divide by the _current_ multiplier.
      //    * p_fMultiplier
      //    , 200 //min timespan in ms for diffs
      //    //If set too high, this may happen (example):
      //    //  1st TSC value reading when TSC freq was 600 MHz: read "10.000.000"
      //    //  shortly before 2nd reading the TSC value the TSC freq is 1800 MHz
      //    //  2nd time value reading after 10 ms is "16.500.000"
      //    // then taking the diff: 16.500.000 - 10.000.000 =
      //    //  6.500.000 -> ~ the TSC for multiplier "6", but the current multiplier
      //    // is 18, so 6.500.000 / 18 = 361,111.1 , 361,111.1 * 100 = 36,111,111.1
      //    //, what would be only 36.1 MHz !!
      //    , 1500 //max. time diff
      //    ) ;
      DEBUGN( FULL_FUNC_NAME << "--begin")
#ifdef MONITOR_FID_CHANGE
      static uint64_t PMCvalueBeforeTSCdiff;
      SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent(
        //Write to 2nd performance event because the 1st is often used by other
        // programs like RMclock etc.
        PMC_COUNTER_INDEX);

      GetPerformanceCounterValue(
        PMC_COUNTER_INDEX, PMCvalueBeforeTSCdiff);
#endif
#ifdef BUSY_TSC_DIFF
      //voltage and multi are got between start and end TSC reading.
      static unsigned numTimerSteps = 50;
      static float waitTimeInSeconds = 0.00001;
      r_fReferenceClockInMHz = CPU::GetClockWithLoad(1, r_fMultiplier,
        numTimerSteps, waitTimeInSeconds, r_fVoltageInVolt);
      DEBUGN( FULL_FUNC_NAME << " Clock from busy TSC diff func: " <<
        r_fReferenceClockInMHz << "multi:" << r_fMultiplier)
      if( r_fReferenceClockInMHz < 0.0f )
        r_fReferenceClockInMHz = 0.0f;
      else
        r_fReferenceClockInMHz /= r_fMultiplier;
#else //#ifdef BUSY_TSC_DIFF
//    r_fReferenceClockInMHz = g_fReferenceClockInMHz ;
  //GetCurrentReferenceClock(r_fMultiplier, r_fReferenceClockInMHz, 500, 10000);
      r_fMultiplier = GetVoltageAndMultiplier(1, r_fVoltageInVolt);
#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
    r_fReferenceClockInMHz = GetBaseClockViaTSCdiffdivMultiplierIfNoFIDchange(byFID) ;
#else
#ifdef MONITOR_FID_CHANGE
    static uint64_t PMCvalueAfterTSCdiff;
    static uint32_t PerfEvent;
    GetPerfEvent(PMC_COUNTER_INDEX, PerfEvent);
    if( PerfEvent == MONITOR_FID_TRANSITIONS_PERF_SELECT_VALUE )
    {
      GetPerformanceCounterValue(
        PMC_COUNTER_INDEX, PMCvalueAfterTSCdiff);
      if( PMCvalueBeforeTSCdiff != PMCvalueAfterTSCdiff)
      {
        DEBUGN( FULL_FUNC_NAME << " FID changed between taking TSC diff")
        r_fReferenceClockInMHz = 0.0f;
      }
    }
#endif
#endif //#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
#endif //#ifdef BUSY_TSC_DIFF
    //  BYTE bySuccess = 0 ;
    //    bySuccess = 1 ;
        DEBUGN( FULL_FUNC_NAME << "--return 1")
        return 1 ;
//    #ifdef _DEBUG
//      else
//      {
//    //    bySuccess = 0 ;
//        return 0 ;
//      }
//    #endif
//      return 0 ;
    //  return bySuccess ;
    }

    inline void SelectClocksNotHaltedPerformanceCounterEvent(
      //BYTE byCoreID
      ) ;

    /** @brief Getting the reference clock the
     *  -stepping 8: 133 MHz FSB: 1.6 GHz, 1.86 GHz
     *  -stepping ?: 100 MHz FSB:(e.g. Pentium M 745 1.8 GHz)
     *   Calculating the reference clock if the FID is changed in high
     *  frequency is nearly impossible for Pentium M:
     *  -the TimeStampCounter runs with the core clock. -> If the frequency ID
     *    (CPU core multiplier/ core to bus ratio) changes then the TSC freq changes.
     *    Dividing the TSC difference by the current multiplier then does not
     *     represent the FSB.
     *  -FID transitions may be counted, by the count event may have been changed
     *   in between by another program
     *   */
    inline void GetReferenceClockAccordingToStepping()
    {
      DWORD dwEAX, dwEBX, dwECX , dwEDX ;
      bool bRet = CPUID(
        //http://en.wikipedia.org/wiki/CPUID#EAX.3D1:_Processor_Info_and_Feature_Bits:
        //"EAX=1: Processor Info and Feature Bits"
        0x00000001
        , & dwEAX
        , & dwEBX
        , & dwECX
        , & dwEDX
        , 1
        ) ;
      if( bRet )
      {
        //http://en.wikipedia.org/wiki/CPUID#EAX.3D1:_Processor_Info_and_Feature_Bits:
        //"The format of the information in EAX is as follows:"
        //"3:0 - Stepping"
        BYTE byStepping = dwEAX & BITMASK_FOR_LOWMOST_4BIT ;
        DEBUGN("stepping via CPUID:" << (WORD) byStepping )
        if( byStepping == 8) //e.g. 1.86 GHz
          g_fReferenceClockInMHz = 133.3 ;
        else //e.g. 1.8 GHz 745
          g_fReferenceClockInMHz = 100.0 ;
      }
    }

    //TODO change to
    /** @brief Reads whether the "Thermal Status Log bit" is set
     *  Note: this bit may have been set via another process/ program and not by
     *  the CPU. Also this bit may have been set to 0 via another process.*/
    inline float GetTemperatureInDegCelsius()
    {
      static uint32_t dwEAX ;
      static uint32_t dwEDX ;
      ReadMSR(
        IA32_THERM_STATUS ,
        & dwEAX,     // bit  0-31
        & dwEDX,     // bit 32-63
        //1bin =core 0; 10bin=2dec= core 1
        1  // Thread Affinity Mask
        ) ;
      //See Intel Vol. 3A doc # 253668 14-19, chapter
      // "14.5.5.2 Reading the Digital Sensor" :
      //"Critical temperature conditions are detected
    //  using the “Critical Temperature Status” bit. When this bit is set, the processor is
    //  operating at a critical temperature and immediate shutdown of the system should
    //  occur. Once the “Critical Temperature Status” bit is set, reliable operation is not guar-
    //  anteed."

      //"Thermal Threshold #1 Status (bit 6, RO) — Indicates whether the actual
    //  temperature is currently higher than or equal to the value set in Thermal
    //  Threshold #1. If bit 6 = 0, the actual temperature is lower. If bit 6 = 1, the
    //  actual temperature is greater than or equal to TT#1. Quantitative information of
    //  actual temperature can be inferred from Digital Readout, bits 22:16.""

    //  "Critical Temperature Status (bit 4, RO) — Indicates whether the critical
    //  temperature detector output signal is currently active. If bit 4 = 1, the critical
    //  temperature detector output signal is currently active."
      //bit 2 PROCHOT # or FORCEPR# event (RO)
      return (dwEAX &
        //4dec = 100 bin
        //
        CRITICAL_TEMPERATURE_STATUS_BIT_MASK) ||
        (dwEAX & THERMAL_THRESHOLD_1_STATUS_BIT_MASK)
        ? //99.0
        CPU_TEMP_IS_CRITICAL
        : //30.0
        CPU_TEMP_IS_BELOW_CRITICAL
        ;
    }

    inline float GetVoltageID_asFloat(float fVoltageInVolt)
    {
      //Pentium M datasheet: Table 3-1: 0.016 Volt difference = 1 Voltage ID step
      //Examined following values :
      //Voltage ID 0 = 0.7V 1=0.716 V,...27=1.132V,...,40=1.340 Volt
      DEBUGN("GetVoltageID_PentiumM--voltage in Volt:" << fVoltageInVolt
        << "-> voltage ID:" << (fVoltageInVolt - 0.7 ) / 0.016
        << "-> as integer:" << (WORD) ( (fVoltageInVolt - 0.7 ) / 0.016 )
        )
      return (fVoltageInVolt - 0.7 ) / 0.016 ;
    }

    inline BYTE GetVoltageID(float fVoltageInVolt )
    {
      return (BYTE) GetVoltageID_asFloat( fVoltageInVolt) ;
    }

    inline void SelectClocksNotHaltedPerformanceCounterEvent(
      //BYTE byCoreID
      )
    {
      PerformanceEventSelectRegisterWrite(
    //    1 << byCoreID ,
        1 ,
        //Pentium M has 1 or 2 "Performance Event Select Register" from
        //  MSR ... to MSR ...  for
        // 1 or 2 "Performance Event Counter Registers" from
        //  ... to ...
        //  that store the 48 bit counter value
        0 ,
        //CPU_CLOCKS_NOT_HALTED , //event select, 8 bit
        INTEL_ARCHITECTURAL_CPU_CLOCKS_NOT_HALTED ,
        0 , // 8 bit unit mask
        1, //User Mode
        1, //OS mode
        0, //edge
        0, //pin control
        0, //APIC
        1, //enable counters
        0 , //invert counter mask
        0 //counter mask
        ) ;
    }

    /** @param coreID: although Pentium M has only 1 core: if testing under
     *   a another CPU that has multi-core this param is needed. */
    inline BYTE SetCurrentVoltageAndMultiplier(
      const float fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , const float fMultiplier
      , fastestUnsignedDataType coreID = 0
      )
    {
      //    std::stringstream ss ;
      //    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
      //        << dwLowmostBits ;
      //    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;

      //  MessageBox
      return ::WriteMSR (
        //Intel: "199H 409 IA32_PERF_CTL  (R/W)"
        // "15:0  Target performance State Value"
        //  "31:16  Reserved"
        //  "63:33 Reserved"
        IA32_PERF_CTL,
    //    g_ui32LowmostBits,// bit  0-31 (register "EAX")
        ( (BYTE) fMultiplier << 8 ) |
          GetClosestVoltageID(fVoltageInVolt) ,
    //      GetVoltageID_PentiumM(fVoltageInVolt) ,
        0,
        //m_dwAffinityMask
        1 << coreID
        ) ;
    }
  }//namespace Pentium_M
}//namespace Intel

#endif /* PENTIUMM_HPP_ */
