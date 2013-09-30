/*
 * Core2.hpp
 *
 *  Created on: 23.12.2012
 *      Author: Stefan
 */

#ifndef CORE2_HPP_
#define CORE2_HPP_

#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_8BIT
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
//for ReadMSR(), WriteMSR(); should be the file for
// I_CPUcontroller-derived class (that calls I_CPUcontroller-derived::ReadMSR()
//or for the DLL (that calls the exe's exported function)
//1 include path must match the path where the header file is in
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <Controller/CPU-related/Intel/Core/Core_and_Core2_GetVoltage.hpp>
// Intel::CoreAndCore2::AllocateAndFillVoltageArray(...)
#include <Controller/CPU-related/Intel/Core/CoreAndCore2.hpp>

#include "Core2registers.h"
#include <fastest_data_type.h> //typedef fastestUnsignedDataType

extern float g_fReferenceClockInMHz;
extern float g_fReferenceClockMultiplier;

namespace Intel
{
  namespace Core2
  {
    unsigned GetVoltageAndMultiplier(
//      unsigned coreID,
      DWORD affMask,
      float * p_fVoltageInVolt, float * );
  }
}
//using namespace Intel::Core2;


/** @param coreID: unsigned is fastest data type */
inline float GetVoltageAndMultiplier(
//  unsigned coreID,
  DWORD affMask,
  float & r_fVoltageInVolt
  )
{
  return //multi;
    g_fReferenceClockMultiplier;
}

#ifdef TSC_DIFF_WHILE_BUSY_WAITING
//#include <Controller/CPU-related/GetCPUclockAndMultiplier.hpp>
#include <hardware/CPU/x86/GetCPUclockAndMultiplier.hpp>
#endif

namespace Intel
{
  namespace Core2
  {
    /** @brief whether to add 0.5 to the multiplier */
    inline bool HasHalfMultiplier(fastestUnsignedDataType byFrequencyIDentifier)
    {
      return (byFrequencyIDentifier & BITMASK_FOR_HALF_MULTIPLIER);
    }

    inline BYTE GetFreqID(fastestUnsignedDataType byFrequencyIDentifier)
    {
      return ( byFrequencyIDentifier & BITMASK_FOR_LOWMOST_5BIT );
    }

    /** @brief whether to half (divide by 2) the multiplier */
    inline bool IsHalfMultiplier(fastestUnsignedDataType byFrequencyIDentifier)
    {
      return byFrequencyIDentifier & BITMASK_FOR_SUPER_LOW_FREQUENCY_MODE;
    }

    inline float GetMultiplier(const fastestUnsignedDataType byFrequencyIDentifier)
    {
      //1000 0110 bin (134dec) was multiplier 3
      //      110 bin=6dec
      //If multiplier should be divided by "2".
      //                  73 (0100 1001 bin) was multi/ FID "9"
      //                      ++   1001 bin = 9dec
      //                     / |+-----+
      //                    /  | FreqID
      //SuperLowFrequencyMode add
      //(half multiplier)     half multi: 1=yes
      //-> only the lowmost 4 bits are used?!
      float fFreqID = (float) GetFreqID(byFrequencyIDentifier);
      if( IsHalfMultiplier(byFrequencyIDentifier) )
      {
        //Intel: "15:0 Current performance State Value"
        //   "63:16 Reserved"
        fFreqID /= 2.0 ;
      }
      else
      {
        if( HasHalfMultiplier(byFrequencyIDentifier) )
          fFreqID += 0.5 ;
//        else
//          //Neither bit 7 nor bit 8 is set.
//          return byFrequencyIDentifier ;
      }
      return fFreqID;
    }

    inline float GetMultiplierAsEncodedInMSR(DWORD dwValue )
    {
      static BYTE byFrequencyIDentifier = 0 ;
      //g_byValue1 =
      byFrequencyIDentifier =
        ( dwValue >> 8 ) & BITMASK_FOR_LOWMOST_8BIT ;
      return GetMultiplier(byFrequencyIDentifier);
    }

    /** @param coreID: unsigned is fastest data type */
    inline unsigned GetVoltageAndMultiplier(
      //unsigned coreID,
      DWORD affMask,
      float * p_fVoltageInVolt,
      float * p_fMultiplier)
    {
      static uint32_t lowmostBits , highmostBits ;
      static unsigned byValue1;
      //  //Intel: 198H 408 IA32_PERF_STATUS
      byValue1 =
      //      (*g_pfnreadmsr) (
        ReadMSR(
        IA32_PERF_STATUS,
        & lowmostBits,// bit  0-31 (register "EAX")
        & highmostBits,
        //1 << coreID //m_dwAffinityMask
        affMask
        ) ;
      if( byValue1 ) //success
      {
//        byValue1 = (lowmostBits ) ;
        * p_fVoltageInVolt = Intel::CoreAndCore2::GetVoltage( lowmostBits &
          BITMASK_FOR_LOWMOST_8BIT ) ;
        * p_fMultiplier = GetMultiplierAsEncodedInMSR(lowmostBits) ;
        DEBUGN("FID:" << byValue1 << "multiplier: " << * p_fMultiplier )
      }
      return byValue1;
    }

    inline float * GetAvailableVoltagesInVolt(WORD & r_byNumVoltages)
    {
      uint32_t lowmostBits, highmostBits;
      float * ar_f = NULL ;
      //For Intel Core T2400: MSR reg.# 0xCE
      //                    byte index:4  0
      //ex.: "value at MSR address 408:2A 0D"
      //0D=13:min. voltage: = 0.7125 + 13*0.0125V = 0.7125+0.1625V 0.95 V
      //2C=42:max. voltage: 0.7125V + 42 * 0.0125V = 0.7125V + 0.525 = 1.2375V
      BYTE byValue1 =
  //      (*g_pfnreadmsr) (
        ReadMSR(
  //      IA32_PERF_STATUS,
        //MSR address from image: http://img223.imageshack.us/img223/6914/msr.png
        //referred from http://www.techpowerup.com/forums/showthread.php?p=1572128
        0xCE,
        & lowmostBits,// bit  0-31 (register "EAX")
        & highmostBits, //bytes 4-8
        1 //<< wCoreID //m_dwAffinityMask
        ) ;
      if( byValue1 ) //Reading from ModelSpecificRegister succeeded.
      {
  //      //Lowest voltage ID.
  //      g_byValue2 = ( ( highmostBits >> 16 ) & BITMASK_FOR_LOWMOST_8BIT ) ;
        BYTE lowestVoltageID = ( lowmostBits & BITMASK_FOR_LOWMOST_8BIT );
        DEBUGN( "lowest voltage ID:" << (WORD) lowestVoltageID)
        BYTE highestVoltageID = ( highmostBits & BITMASK_FOR_LOWMOST_8BIT );
        DEBUGN( "highest voltage ID:" << (WORD) highestVoltageID)
        return Intel::CoreAndCore2::AllocateAndFillVoltageArray(
          lowestVoltageID, highestVoltageID, r_byNumVoltages);
      }
      return ar_f ;
    }

    inline BYTE GetCurrentVoltageAndFrequency(
      float & r_fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , float & r_fMultiplier
      , float & r_fReferenceClockInMHz
      , WORD wCoreID
      )
    {
      static BYTE retVal;
#ifdef TSC_DIFF_WHILE_BUSY_WAITING
      static unsigned numTimerSteps;
      numTimerSteps = 100;
      static float waitTimeInSeconds = 0.00001;
      static double CPUclock;
//#ifdef SLEEP_ON_BUSY_WAITING
//      CPUclock = CPU::OneLogicalCore::GetClockWithLoad(
//        1 << wCoreID
//        , r_fMultiplier //float & multiplier
//        , numTimerSteps //unsigned & numMinimumSteps,
//        , waitTimeInSeconds //float & waitTimeInSeconds,
//        , r_fVoltageInVolt //float & r_fVoltageInVolt
//        );
//#else
      CPUclock = CPU::GetClockWithLoad(
        1 << wCoreID
        , r_fMultiplier //float & multiplier
        , numTimerSteps //unsigned & numMinimumSteps,
        , waitTimeInSeconds //float & waitTimeInSeconds,
        , r_fVoltageInVolt //float & r_fVoltageInVolt
        );
      if( r_fReferenceClockInMHz < 0.0f )
        r_fReferenceClockInMHz = 0.0f;
      else
      {
        r_fReferenceClockInMHz = CPUclock / r_fMultiplier;
      }
      return 1;
#else
      retVal = GetVoltageAndMultiplier(1 << wCoreID, & r_fVoltageInVolt,
        & r_fMultiplier);
      r_fReferenceClockInMHz = g_fReferenceClockInMHz ;
      return retVal;
#endif
//#else
//#endif

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
  //    else
//        return 0 ;
    }

    inline BYTE EncodeMultiplierAsInMSR(float fMultiplier)
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

    inline BYTE SetCurrentVoltageAndMultiplier(
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
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin")
      DWORD dwLowmostBits //, dwHighmostBits = 0
        ;
  //    dwLowmostBits =
  //      //Freq / "FSB in MHz" = multiplier
  //      (BYTE) fMultiplier ;
  //
  //    BYTE byVoltageID = (dwLowmostBits ) ;

      BYTE byVoltageID = Intel::CoreAndCore2::GetClosestVoltageID(fVoltageInVolt) ;

      dwLowmostBits = byVoltageID ;

      BYTE byFrequencyID = Intel::Core2::EncodeMultiplierAsInMSR(fMultiplier) ;
      dwLowmostBits |= ( byFrequencyID << 8 ) ;

  #ifdef _DEBUG
      bool bWrite = true ;
  //    byFrequencyID =
      std::stringstream stdstringstreamMessage ;
  //    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
  //        << dwLowmostBits ;
      stdstringstreamMessage << "written voltage would be:"
        << Intel::CoreAndCore2::GetVoltage( dwLowmostBits )
        << "\nwritten multiplier would be:"
        << GetMultiplierAsEncodedInMSR(dwLowmostBits) ;
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

    /** Intel Core has default voltages for min and max multiplier stored in
     * MSRegister? */
    inline BYTE GetDefaultPstates(
      float & fVoltageForLowerMulti,
      float & fLowerMulti,
      float & fVoltageForHighestMulti,
      float & fHighestMulti
      )
    {
      static uint32_t ui32LowmostBits;
      static uint32_t ui32HighmostBits;

      do
      {
        //bytes 4 and 5 (bits 32-39, 40-47) from MSR 0x198 (STATUS) change
        //  during operation byte 4:32<->42, byte 5:9<->73)
        // VID 32 : 0.7125V + 32 * 0.0125V = 1.1125 V
        // VID 42 : 0.7125V + 42 * 0.0125V = 1.2375 V
        // FID 9 : multiplier 9, FID 73 = multiplier 9.5?
        // 23: 0.7125V + 23 * 0.0125V = 1V
        // 0 6 32 9 23 6
        if( ReadMSR(
            //MIN_AND_MAX_FID ,
            //According to the MSR walker of CrystalCPUID:
            //for Pentium M reg. addr. 0x198:
            //Bit 24-32 showed hex "0E" for a max. multipl. "14" for 1.86 133 MHz FSB.
            //Bit 24-32 showed hex "0C" for a max. multipl. "12" for 1.6 133 MHz FSB.
            IA32_PERF_STATUS ,
            & ui32LowmostBits, // bits 0-31 (register "EAX")
            & ui32HighmostBits,
            //m_dwAffinityMask
        //    1 << wCoreID
            1
            )
          )
        {
          //CrystalCPUID's MSR walker for Intel mobile_Core2_Duo_P8600:
          //00000198 : 06170920 0600860D
          //           06 : min multi for High Frequency Mode (=HFM) = 6
          //           0x17: default voltage for min HFMode multi = 1.0V
          //               09 : max multi = 9
          //               0x20: default voltage for max multi = 1.1125 V
          fVoltageForHighestMulti = Intel::CoreAndCore2::GetVoltage(
            ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
          DEBUGN("fVoltageForHighestMulti: " << fVoltageForHighestMulti )

          ui32HighmostBits >>= 8 ;
          //"fHighestMulti" may be either the IDA (turbo) multiplier or the non-
          //turbo multiplier.
          fHighestMulti = ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ;
          DEBUGN("fHighestMulti: " << fHighestMulti )
        }
        else
          return 0 ;
      }while( //Ensure we don't have the IDA multiplier
          (float) ((BYTE) fHighestMulti) != fHighestMulti );

      ui32HighmostBits >>= 8 ;
      fVoltageForLowerMulti = Intel::CoreAndCore2::GetVoltage(
        ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
      DEBUGN("fVoltageForLowerMulti: " << fVoltageForLowerMulti )

      ui32HighmostBits >>= 8 ;
      fLowerMulti = ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ;
      DEBUGN("fLowerMulti: " << fLowerMulti )
      return 1 ;
    }
  }
}

#endif /* CORE2_HPP_ */
