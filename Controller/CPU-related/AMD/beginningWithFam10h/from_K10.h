/*
 * from_K10.h
 *
 *  Created on: Aug 28, 2013
 *      Author: Stefan
 */

#ifndef FROM_K10_H_
#define FROM_K10_H_

#include <hardware/CPU/fastest_data_type.h> //typedef fastestUnsignedDataType
#include <preprocessor_macros/bitmasks.h>
#include <stdint.h> // uint32_t
#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
  #include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
  float g_fMaxMultiplier;
#endif
/** ReadMSR(...)*/
#include <Controller/AssignPointersToExportedExeFunctions/inline_register_access_functions.hpp>
//CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER, ...
#include "../configuration_space_addresses.h"
#include "voltage.hpp"
#include "CPUcoreMultiplier.hpp"

#include "MSR_registers.h"	

extern ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace;

extern float g_fMaxMultiplier;

namespace AMD
{
  /** function for AMD CPUs beginning with K10 (including 15hex. */
  namespace fromK10
  {
    #ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
    inline void GetSameReferenceClockForAllCPUcores(
      float * p_fReferenceClockInMHz)
    {
      //TODO 
			/**"41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG",
          "MSRC001_0015 Hardware Configuration Register (HWCR)":
          "TscFreqSel: TSC frequency select. Read-write. BIOS: 1.
           0= The TSC increments at the rate of the REFCLK frequency.
           1=The TSC increments at the rate of the core P-state 0 COF specified 
           by MSRC001_0064." */
			
			//"NCLK. The main Northbridge clock. The NCLK frequency is the NB COF."

      //TODO "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG",
      // "CPUID Fn8000_0007 Advanced Power Management Information" :
      // Register EDX; bit # 8, value = 1: "TscInvariant"
      //  : "The TSC rate is ensured to be invariant across all P-states, C-States,
      //  and stop-grant transitions (such as STPCLK Throttling)."

      //#ifdef _DEBUG
      //This call sets g_fReferenceClockInMHz to the current reference clock.
      //This update of the value would be senseful for setting the CPU core
      //via "frequency" as parameter value the next time.
      GetCurrentReferenceClock(
        //TODO what is the reference clock/ max multiplier?
        // http://en.wikipedia.org/wiki/List_of_AMD_Turion_microprocessors#.22Lion.22_.2865_nm.29
        // says: max multi = ( "Performance State 0's Frequency ID" + 8 ) / 2,
        //http://en.wikipedia.org/wiki/List_of_AMD_Turion_microprocessors#fn_1:
        //  "The multiplier here applies to the 200 MHz system clock frequency"

        // "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG", page 14:
        // "CLKIN. The reference clock provided to the processor, nominally 200Mhz."
        g_fMaxMultiplier , //divide TSC diff by this divisor
        * p_fReferenceClockInMHz ,
      //    fReferenceClockInMHz,
        1000 , //min. timespan in ms
        10000  //max. timespan in ms
        ) ;

      DEBUGN(/*FULL_FUNC_NAME <<*/ "calculated reference clock:"
        << * p_fReferenceClockInMHz)
      //  if( * p_fReferenceClockInMHz == 0.0f )
      //    * p_fReferenceClockInMHz =
      //      //Default reference clock in MHz for Griffin CPUs.
      //      100.0f;

      //Time span too high or too low.
      if( * p_fReferenceClockInMHz == 0.0f )
        * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
      else
        //Save the current reference clock as last retrieved reference clock.
        g_fReferenceClockInMHz = * p_fReferenceClockInMHz ;
    }
    #endif //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF

    inline void GetReferenceClock(float * p_fReferenceClockInMHz, unsigned coreID)
    {
      //TODO better use APIC Timer Operation?
      /** BIOS and Kernel Developer’s Guide (BKDG) For AMD Family 11h Processors:
      * "The pro-cessor bus clock is divided by the value in APIC3E0[Div] to
      * obtain a time base for the timer." */

    #ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
      //Only get ref. clock if 1st core (else may be too small because the time
      //diff between core 0 and 1 may be taken)
      if( coreID == 0)
			{
				//TODO ReadMSR(MSRC001_0015 Hardware Configuration Register)
				// if ( TscFreqSel == 1 )
          GetSameReferenceClockForAllCPUcores(p_fReferenceClockInMHz);
			}
      else
        * p_fReferenceClockInMHz = g_fReferenceClockInMHz;
    #else //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
      #ifdef MAX_MULTI_IS_MAIN_PLL_OP_FREQ_ID_MAX_PLUS_8
      * p_fReferenceClockInMHz = 100.0f ;
      #else
      * p_fReferenceClockInMHz = 200.0f ;
      #endif
    #endif //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
    }

		/** The calculation for the multiplier applies to AMD family 10h and 15h */
    inline BYTE GetCurrentVoltageAndFrequency(
        float * p_fVoltageInVolt
        //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
        , float * p_fMultiplier
        , float * p_fReferenceClockInMHz
        , fastestUnsignedDataType wCoreID
        )
    {
      static uint32_t EAX, EDX;
      static fastestUnsignedDataType frequencyID, divisorID;
      static BYTE by ;
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin core ID:" << wCoreID)
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG: 
			 *     MSRC001_0071 COFVID Status Register :
			 *    "8:6 CurCpuDid: current core divisor ID"
       *    "5:0 CurCpuFid: current core frequency ID.". 
			 * 
			 *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 
			 *    00h-0Fh Processors" : 
			 *    MSRC001_0071 COFVID Status :
			 *    "8:6 CurCpuDid: current core divisor ID"  
			 *    "5:0 CurCpuFid" 
			 * 
			 *  => read register contents of MSRC001_0071 COFVID Status */
      by = ::ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & EAX,
        & EDX,
        1 << wCoreID
        );

      AMD::fromK10::GetDivisorID(EAX, divisorID);
      frequencyID = AMD::fromK10::GetFrequencyID(EAX);
      * p_fMultiplier = AMD::fromK10::GetMultiplier(frequencyID, divisorID);

      const fastestUnsignedDataType voltageID = AMD::fromK10::
        GetCurrentVoltageIDfromCOFVIDstatusRegisterBits(EAX);
      * p_fVoltageInVolt = AMD::fromK10::GetVoltageInVolt(voltageID);
//      * p_fReferenceClockInMHz = 0.0f;
      AMD::fromK10::GetReferenceClock(p_fReferenceClockInMHz, wCoreID);
      return by;
    }

    /** Applies to : 
     *  -AMD family 10h,
     *   -MSRC001_00[68:64] P-State [4:0] Registers
     *   -MSRC001_0070 COFVID Control Register
     *  -AMD family 11h, 
     *   -MSRC001_00[6B:64] P-state [7:0] Registers 
     *   -MSRC001_0070 COFVID Control Register  */
    inline void GetMSRregisterValue(
      const fastestUnsignedDataType voltageID,
      const fastestUnsignedDataType frequencyID ,
      const fastestUnsignedDataType divisorID ,
      uint32_t & dwLowmostMSRvalue
      )
    {
//      SET_P_STATE_TO_VALID(dwHighmostMSRvalue) ;

      /** See AMD Family 11h Processor BKDG (document # 41256),
			 *  31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG
       *  "MSRC001_00[6B:64] P-state [7:0] Registers"
       *  "5:0 CpuFid" */
      dwLowmostMSRvalue |= ( frequencyID & 63) ;
      /** "8:6 CpuDid: core divisor ID" */
      dwLowmostMSRvalue |= ( divisorID << START_BIT_FOR_CPU_CORE_DIVISOR_ID );
      /** "15:9 CpuVid: core VID" */
      dwLowmostMSRvalue |= ( voltageID << 9) ; //<=>bits 9-15 shifted
    }

    /** */
    fastestSignedDataType GetPstateAccordingToFrequency(const float multiplier)
    {
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG : 
       *  MSRC001_00[68:64] P-State [4:0] Registers */
      //TODO make usable for AMD family 11h
      fastestUnsignedDataType numberOfPstateRegisters = 5; //GetNumberOfPstateRegisters();
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG
          "2.4.2.1.2.2 Hardware P-state Numbering" : 
          "When referring to hardware P-state numbering, the following naming convention is used:
          • All P-states are referred to as P0, P1, etc.
          • P0 is the highest power, highest-performance P-state, regardless of whether it is a boosted P-state or a
          non-boosted P-state.
          • Each ascending P-state number represents a lower-power, lower-performance P-state, regardless of
          whether it is a boosted P-state or not." */
      //			float multiplierForDirectLowerPstate = 0.0f;
      for (fastestUnsignedDataType pStateIndex = 0;
          pStateIndex < numberOfPstateRegisters; ++pStateIndex)
      {
        uint32_t lowmostBits, highmostBits;
        if( ReadMSR(
          0xC0010064 + pStateIndex,
          & lowmostBits, // bit  0-31
          & highmostBits, // bit 32-63
          //1bin =core 0; 10bin=2dec= core 1
          1 //DWORD_PTR affinityMask  // Thread Affinity Mask
          ) )
        {
          fastestUnsignedDataType divisorID;
          AMD::fromK10::GetDivisorID(lowmostBits, divisorID);
          fastestUnsignedDataType frequencyID = AMD::fromK10::GetFrequencyID(
            lowmostBits);
          float multiplierForCurrentPstate = AMD::fromK10::GetMultiplier(frequencyID, divisorID);
          if (multiplier >= multiplierForCurrentPstate)
            return pStateIndex;
        }
      }
      return -1;
    }

    bool COFVIDcontrolRegisterSanityCheck(
      const uint32_t MSRlowmost,
      const float voltageInVoltToSet,
      const float multiplierToSet,
      const fastestUnsignedDataType northBridgeVoltageID,
      const fastestUnsignedDataType northBridgeDivisorID,
      const fastestUnsignedDataType performanceStateID
      )
    {
      fastestUnsignedDataType northBridgeVoltageIDbeforeWrite = MSRlowmost >> 25;
      fastestUnsignedDataType northBridgeDivisorIDbeforeWrite = (MSRlowmost >> 22) & 1;
      fastestUnsignedDataType performanceStateIDbeforeWrite = (MSRlowmost >> 16) & 7;
      fastestUnsignedDataType CPUcoreVoltageIDbeforeWrite = (MSRlowmost >> 9) & 127;
      fastestUnsignedDataType CPUdivisorIDbeforeWrite = (MSRlowmost >> 6) & 7;
      fastestUnsignedDataType CPUfrequencyIDbeforeWrite = MSRlowmost & 63;
      float multiplierBeforeWrite = GetMultiplier(CPUfrequencyIDbeforeWrite,
                                                  CPUdivisorIDbeforeWrite);
      float voltageBeforeWrite = GetVoltageInVolt(CPUcoreVoltageIDbeforeWrite);
      if (multiplierBeforeWrite == multiplierToSet &&
        voltageBeforeWrite == voltageInVoltToSet &&
        northBridgeVoltageIDbeforeWrite == northBridgeVoltageID &&
        northBridgeDivisorIDbeforeWrite == northBridgeDivisorID &&
        performanceStateIDbeforeWrite == performanceStateID
        )
        return true;
      return false;
    }
		
		bool PstateRegisterSanityCheck(
			const uint32_t MSRlowmost,
			const float voltageInVoltToSet,
			const float multiplierToSet,
			const fastestUnsignedDataType northBridgeVoltageID,
			const fastestUnsignedDataType northBridgeDivisorID
			) 
		{
			fastestUnsignedDataType northBridgeVoltageIDbeforeWrite = MSRlowmost >> 25;
			fastestUnsignedDataType northBridgeDivisorIDbeforeWrite = (MSRlowmost >> 22) & 1;
			fastestUnsignedDataType CPUcoreVoltageIDbeforeWrite = (MSRlowmost >> 9) & 127;
			fastestUnsignedDataType CPUdivisorIDbeforeWrite = (MSRlowmost >> 6) & 7;
			fastestUnsignedDataType CPUfrequencyIDbeforeWrite = MSRlowmost & 63;
			float multiplierBeforeWrite = GetMultiplier(CPUfrequencyIDbeforeWrite, 
				CPUdivisorIDbeforeWrite);
			float voltageBeforeWrite = GetVoltageInVolt(CPUcoreVoltageIDbeforeWrite);
			if( multiplierBeforeWrite == multiplierToSet && 
					voltageBeforeWrite == voltageInVoltToSet &&
					northBridgeVoltageIDbeforeWrite == northBridgeVoltageID &&
					northBridgeDivisorIDbeforeWrite == northBridgeDivisorID
					)
				return true;
			return false;
		}
        
    /** It seems: setting a p-state for more than 1 core at a time does NOT work.
    * so call this method "n" times if you want the same p-state for "n" cores.*/
    inline BYTE SetPstateViaPstateControlRegister(
			fastestUnsignedDataType PstateToSet, DWORD dwCoreBitmask)
    {
      bool byReturn = false;
      /** Safety check. */
      //TODO re-enable?
//      if( PstateToSet < NUMBER_OF_PSTATES )
      {
        DWORD dwMSRlow = PstateToSet ;

          dwMSRlow = (BYTE) PstateToSet ;
          LOGN_VERBOSE( "For core bitmask " << dwCoreBitmask
            << ": setting to pstate " << PstateToSet );
          if( WriteMSR(
              P_STATE_CONTROL_REGISTER,
              dwMSRlow,
              0,
              dwCoreBitmask
              )
            )
          {
            LOGN_VERBOSE("Setting p-state succeeded.");
            byReturn = true;
            //Wait 1 millisecond (> maximum stabilization time).
    //          SLEEP_1_MILLI_SECOND
          }
          else
          {
            DEBUGN("Setting p-state failed");
          }
          byReturn = true;
      }
      return byReturn ;
    }
    
    inline fastestUnsignedDataType CopyPstateRegister(
      fastestUnsignedDataType pstateOrig, 
      fastestUnsignedDataType pstateDest,
      const fastestSignedDataType coreID)
    {
      uint32_t MSRhighmost, MSRlowmost;
      if( ReadMSR(
        0xC0010064 + pstateOrig,
        & MSRlowmost,
        & MSRhighmost,
        1 << coreID
        ) )
        if( WriteMSR(
          0xC0010064 + pstateDest,
          MSRlowmost,
          MSRhighmost,
          1 << coreID
          ) )
          return 0;
      return 1;
    }
    
    inline fastestUnsignedDataType GetNorthbridgeVoltageID(const uint32_t MSRlowmost)
    {
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
        *   MSRC001_0070 COFVID Control Register :
        *   "31:25 NbVid: Northbridge VID"  */
      return MSRlowmost >> 25;
    }
    
    inline fastestUnsignedDataType GetNorthbridgeDivisorID(const uint32_t MSRlowmost)
    {
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
        *   MSRC001_0070 COFVID Control Register :
        *   "22 NbDid: Northbridge divisor ID"  */
      return (MSRlowmost >> 22) & 1;
    }

    inline fastestUnsignedDataType EnsurePstateChanges(
      const fastestSignedDataType currentPstateID, 
      fastestSignedDataType & performanceStateIDAccordingToMultiplier,
      fastestSignedDataType coreID
      )
    {
      /** A multiplier change for a p-state is only applied if the 
       *   p-state changes. */
      if( currentPstateID == performanceStateIDAccordingToMultiplier)
        if( performanceStateIDAccordingToMultiplier > 0)
        {
          performanceStateIDAccordingToMultiplier --;
          return 0;
        }
        else
        {
          /** Because 0 is the highest p-state: copy to # 1 and set p-state 1
            *  as the current p-state. */
          if( ! CopyPstateRegister(0, 1, coreID) )
            return SetPstateViaPstateControlRegister(
              1 ,
              1 << coreID );
        }
      return 1;
    }
  
    inline fastestUnsignedDataType ChangeVoltageAndFrequencyViaCOFVIDcontrolRegister(
      const float fVoltageInVolt, 
      const float fMultiplier,
      const fastestSignedDataType coreID,
      const fastestUnsignedDataType currentPstateID
      )
    {
      fastestSignedDataType performanceStateIDAccordingToMultiplier = 
        GetPstateAccordingToFrequency(fMultiplier);
      if( performanceStateIDAccordingToMultiplier == -1 )
        return 1;
      EnsurePstateChanges(currentPstateID, 
        performanceStateIDAccordingToMultiplier, coreID);
      fastestUnsignedDataType CPUcorefrequencyID, divisorID;
      GetFreqIDandDivisorIDfromMulti(fMultiplier, CPUcorefrequencyID, divisorID);
      uint32_t MSRhighmost, MSRlowmost;
      /** Must get the register content for MSRC001_0070 "COFVID Control 
       *   Register" in advance because of "31:25 NbVid: Northbridge VID." */
      if( ! ReadMSR(
        COFVID_CONTROL_REGISTER_MSR_ADDRESS,
        & MSRlowmost,
        & MSRhighmost,
        1
        ) )
        return 2;
//      MSRhighmost = 0;
//      MSRlowmost &= ~(127 << 25); /** mask bits 25-31 **/
//      MSRlowmost &= ~(1 << 22); /** mask bit 22 **/

      MSRlowmost &= ~(255); /** mask bits 0-7 **/
      MSRlowmost &= ~(255 << 8); /** mask bits 8-15 **/

      const fastestUnsignedDataType CPUcoreVoltageID = GetVoltageID( fVoltageInVolt ) ;
      GetMSRregisterValue(
        CPUcoreVoltageID,
        CPUcorefrequencyID ,
        divisorID,
//          , MSRhighmost ,
        MSRlowmost ) ;
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG
       *   MSRC001_0070 COFVID Control Register
       *   "18:16 PstateId: P-state identifier." */
		  uint32_t bitmask = ~(7 << 16);
      MSRlowmost &= bitmask; /** mask bits 16-18 **/
      MSRlowmost |= (performanceStateIDAccordingToMultiplier << 16);
#ifdef DEBUG
      fastestUnsignedDataType northBridgeVoltageID = GetNorthbridgeVoltageID(
        MSRlowmost);;
      fastestUnsignedDataType northBridgeDivisorID = GetNorthbridgeDivisorID(
        MSRlowmost);
      if( COFVIDcontrolRegisterSanityCheck(
            MSRlowmost, 
            fVoltageInVolt, 
            fMultiplier,
            northBridgeVoltageID,
            northBridgeDivisorID,
            performanceStateIDAccordingToMultiplier
          ) 
        )
#endif
        /** As it seems voltage and frequency are only changed if the 
         *   performance state id in MSRC001_0070 COFVID Control Register 
         *   differs from the one in MSRC001_0063 P-State Status Register. 
         *  If the voltage is higher than other CPU core voltages then this
         *   voltage is applied to other CPU cores. */
        if( WriteMSR(
            COFVID_CONTROL_REGISTER_MSR_ADDRESS
            , MSRlowmost
            , MSRhighmost
            , 1 << coreID
            )
          )
        {
					return 0;
				}
      return 1;
    }

    inline fastestSignedDataType ChangeVoltageAndFrequencyViaPstatecontrolRegister(
      const float fVoltageInVolt, 
      const float fMultiplier,
      const fastestSignedDataType coreID)
    {
      const fastestSignedDataType performanceStateID = 
        GetPstateAccordingToFrequency(fMultiplier);
      uint32_t MSRhighmost, MSRlowmost;
      fastestUnsignedDataType CPUcorefrequencyID, divisorID;
      GetFreqIDandDivisorIDfromMulti(fMultiplier, CPUcorefrequencyID, divisorID);
      if( performanceStateID < 0 )
        return 2;
      /** Must get the register content for MSRC001_0070 "COFVID Control 
       *   Register" in advance because of "31:25 NbVid: Northbridge VID." */
      ReadMSR(
        0xC0010064 + performanceStateID,
        & MSRlowmost,
        & MSRhighmost,
        1
        );
//      MSRlowmost &= ~(127 << 25); /** mask bits 25-31 **/
//      MSRlowmost &= ~(1 << 22); /** mask bit 22 **/

      MSRlowmost &= ~(255); /** mask bits 0-7 **/
      MSRlowmost &= ~(255 << 8); /** mask bits 8-15 **/

      const fastestUnsignedDataType CPUcoreVoltageID = GetVoltageID( fVoltageInVolt ) ;
      GetMSRregisterValue(
        CPUcoreVoltageID,
        CPUcorefrequencyID ,
        divisorID,
//          , MSRhighmost ,
        MSRlowmost ) ;
//      uint32_t bitmask = ~(7 << 16);
#ifdef DEBUG
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
        *   MSRC001_0070 COFVID Control Register :
        *   "31:25 NbVid: Northbridge VID"  */
      fastestUnsignedDataType northBridgeVoltageID = MSRlowmost >> 25;
      /** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG :
        *   MSRC001_0070 COFVID Control Register :
        *   "22 NbDid: Northbridge divisor ID"  */
      fastestUnsignedDataType northBridgeDivisorID = (MSRlowmost >> 22) & 1;
      if( PstateRegisterSanityCheck(
            MSRlowmost, 
            fVoltageInVolt, 
            fMultiplier,
            northBridgeVoltageID,
            northBridgeDivisorID
          ) 
        )
#endif
        if( WriteMSR(
            0xC0010064 + performanceStateID
            , MSRlowmost
            , MSRhighmost
            , 1 << coreID
            )
          )
        {
          //see "2.4.1.9 Software-Initiated Voltage Transitions"
          //TODO "2. Wait the specified F3xD8[VSSlamTime]."
          if( SetPstateViaPstateControlRegister(
              performanceStateID ,
              1 << coreID )
            )
            return 0;
        }
      return 1;
    }
		
    /** 31116 Rev 3.62 - January 11, 2013 2.4.2.5 AMD Family 10h Processor BKDG:
     * see "2.4.2.5 P-state Transition Behavior"
     * see "2.4.1.9.2 Software-Initiated CPU Voltage Transitions"
     *  "NewCpuVid = the destination CPU VID.
        F3xA0[SlamVidMode]=1:
        1. Write NewCpuVid to MSRC001_0070[CpuVid].
        2. Wait the specified F3xD8[VSSlamTime].
        F3xA0[SlamVidMode]=0:
        Software must use the sequence for F3xA0[SlamVidMode]=0 defined in section 2.4.1.9.1 [Software-Initiated
        NB Voltage Transitions] on page 30 to control the single-plane through NbVid."

      @return use unsigned data type because it is the fastest data type. */
    inline fastestUnsignedDataType SetVoltageAndMultiplier(
      float fVoltageInVolt,
      float fMultiplier ,
      fastestUnsignedDataType coreID )
    {
      DEBUGN( fVoltageInVolt << "," << fMultiplier << "," << coreID )

    #ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
      if( g_fReferenceClockInMHz != 0.0f &&
          GetMaxMultiIfGreaterCurrentPstateLimitMulti(byCoreID, fVoltageInVolt,
        fMultiplier) &&
        //fVoltageInVolt <> 0 (is 0 if GetInterpolated...(...) failed)
        fVoltageInVolt
        )
    #endif
      {
#ifdef DEBUG
        uint32_t MSRhighmost, MSRlowmost;
        ReadMSR(
          P_STATE_STATUS_REGISTER,
          & MSRlowmost,
          & MSRhighmost,
          1 << coreID
          );
        const fastestUnsignedDataType pStateID = MSRlowmost & 
          BITMASK_FOR_LOWMOST_3BIT;
#endif
#define CHANGE_VOLT_N_FREQ_VIA_COFVID_CONTROL_REGISTER
#ifdef CHANGE_VOLT_N_FREQ_VIA_COFVID_CONTROL_REGISTER
        ChangeVoltageAndFrequencyViaCOFVIDcontrolRegister(fVoltageInVolt, 
          fMultiplier, coreID, pStateID);
#else
        ChangeVoltageAndFrequencyViaPstatecontrolRegister(fVoltageInVolt, 
          fMultiplier, coreID);
#endif
#ifdef CHANGE_VOLT_N_FREQ_VIA_COFVID_CONTROL_REGISTER
#else
    //#ifndef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
//        static DWORD dwMSRregisterIndex;
//        dwMSRregisterIndex = GetMSRregisterForPstate(
//          g_byDivisorID
//          ) ;
//        DEBUGN(//"SetVoltageAndMultiplier(...)"
    //#endif //#ifndef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT				
#endif
      }
      return 1;
    }
  }
}

#endif /* FROM_K10_H_ */
