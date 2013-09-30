/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * Griffin.hpp (".hpp" because of C++'s "inline" keyword)
 *
 *  Created on: Jul 11, 2010
 *      Author: Stefan
 *
 * functions that are used by I_CPUcontroller-derived class and by DLL
 */

#ifndef GRIFFIN_H_
#define GRIFFIN_H_

#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_3BIT
//#include <math.h> //log2(...), pow()

//for ReadMSR(), WriteMSR(); should be the file for
// I_CPUcontroller-derived class (that calls I_CPUcontroller-derived::ReadMSR()
//or for the DLL (that calls the exe's exported function)
// 1 include path must match the path where the header file is in.
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp>
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
#include <stdlib.h> //EXIT_SUCCESS, EXIT_FAILURE

#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
  //For "GetCurrentReferenceClock(...)" .
  #include <Controller/CPU-related/GetCurrentReferenceClock.hpp>
#endif //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
#ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
  #include "GetMaxMultiIfGreaterCurrentPstateLimitMulti.hpp"
#endif
#include <fastest_data_type.h> //fastestUnsignedDataType
#include "../from_K10.h"

extern BYTE g_byFreqID,g_byDivisorID ;
extern BYTE g_byValue1 , g_byValue2, g_byValue3 ;
extern BYTE g_byDivisor ;
extern uint32_t g_dwMSRhighmost, g_dwMSRlowmost ;
//MSRC001_0071 COFVID Status Register
//54:49 MainPllOpFreqIdMax: main PLL operating frequency ID maximum.
extern float g_fMainPllOpFreqIdMax ;
extern float g_fMaxMultiplier;
extern float g_fMainPllOpFreqId ;
extern float g_fMainPllOpFreqIdMaxDiv2 ;
extern float g_fMaxMultiDiv2 ;
extern float g_fValue1 ;
extern float g_fReferenceClockInMHz;

#ifdef FAMILY16
  #include "family10h.hpp"
  using namespace AMD::family16;
  #define FAMILY_NAMESPACE_NAME AMD::family16
#else
  #include "AMD_family17.h"
  using namespace AMD::family17;
  #define FAMILY_NAMESPACE_NAME AMD::family17
#endif

//#define COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT

#ifdef _DEBUG
#include <math.h> //pow(...)
#endif

//http://www.parashift.com/c++-faq-lite/inline-functions.html:
//"Note: It's imperative that the function's definition (the part between the
//{...}) be placed in a header file, unless the function is used only in a
//single .cpp file. In particular, if you put the inline function's definition
//into a .cpp file and you call it from some other .cpp file, you'll get an
//"unresolved external" error from the linker. "
#define INLINE inline

  namespace AMD
  {
    namespace K10_and_K11 {
      void GetMinAndMaxVoltageID(BYTE &,BYTE &) ;
  } }

  inline float * GetAvailableVoltagesAMDfamilyFh(WORD & r_wNum )
  {
    static BYTE maximumVoltageID, minimumVoltageID;

    AMD::K10_and_K11::GetMinAndMaxVoltageID(maximumVoltageID, minimumVoltageID) ;
    r_wNum = ( maximumVoltageID - minimumVoltageID + 1 ) ;

    DEBUGN("# VIDs: " << (WORD) r_wNum )
    float * ar_f = new float[ r_wNum ] ;
    DEBUGN("ar_f: " << ar_f )
    if( ar_f )
    {
      WORD wIndex = 0 ;
      for( BYTE wCurrentVoltageID =
        //for AMD Griffin: max. VID = min voltage
          maximumVoltageID ;
        wCurrentVoltageID >= minimumVoltageID ; -- wCurrentVoltageID )
      {
        g_fValue1 =
          //VID 28 = 1.2 = 1.55 - 28 * 0.0125 ;
          //VID 64 = 0.75 = 1.55 - 64 * 0.0125
          1.55f - wCurrentVoltageID * 0.0125f ;
        DEBUGN("adding voltage # " << wIndex << " : " << g_fValue1 << " Volt")
        ar_f[ wIndex ++ ] = g_fValue1 ;
      }
    }
    DEBUGN( /*FULL_FUNC_NAME <<*/ "returning " << ar_f)
    return ar_f ;
  }

namespace AMD
{
  namespace K10_and_K11
  {
    WORD GetMaximumVoltageID() ;
    WORD GetMinimumVoltageID() ;

    /** 41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG
     *   chapter "2.4.1.2 Serial VID Interface" :
     *  "The processor includes an interface, intended to control external
     *  voltage regulators, called the serial VID inter-face (SVI)."
     *
     *  see "31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG"
     *    chapter "2.4.1.6.3 Serial VID (SVI) Encodings"
     *
     *  see Intersil "ISL6265 FN6599.0 Data Sheet May 7, 2008",
     *     "TABLE 3. SERIAL VID CODES"
     *  */
    inline void GetVoltageFromVoltageID(const BYTE byVoltageID,
      float * p_fVoltageInVolt)
    {
      //Diff 1.0875 (VID 30) & 0.925 (VID=17) = 0.1625V
      //1 voltage ID step = 0.1625V / (30-17) = 0.1625V / 13 = 0.0125
      //0.925- 17 * 0.0125 = 0.7125 ;
      * p_fVoltageInVolt = //0.7125 + g_byValue3 * 0.0125 ;
        //The maximum possible voltage.
        1.55f - ( (float)( byVoltageID ) * 0.0125f ) ;
    }

    /** 42301  Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models
     *    00h-0Fh Processors:
     *  "Core current operating frequency in MHz. CoreCOF = 100 *
     *   (MSRC001_00[6B:64][CpuFid] + 10h) / (2^MSRC001_00[6B:64][CpuDid])."
     *
     *  31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG:
     *  "The CPU COF specified by CpuFid and CpuDid is: CPU COF = 100 MHz *
     *   (CpuFid + 10h) / (2^CpuDid)"
     *
     *  FID+16 / 2^DID
     * */
    inline void GetMultiplier(
      const BYTE byFrequencyID,
      const BYTE byDivisorID,
      float * p_fMultiplier)
    {
      //41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
      //" if MainPllOpFreqIdMax = 00h, then there is no frequency limit."
      // -> use the max possible number.

      //multi = Frequency IDentifier + 8 / 2 ^ "Divisor IDentifier"
    //  * p_fMultiplier = ( (float) ( byFrequencyID + 8 ) ) /
    //    (float) ( 1 << byDivisorID ) ;
      * p_fMultiplier = //GET_MULTIPLIER(byFrequencyID, byDivisorID) ;
        FAMILY_NAMESPACE_NAME::GetMultiplier(byFrequencyID, byDivisorID);
    }

//    inline float GetMultiplier(BYTE byFrequencyID, BYTE byDivisorID)
//    {
//      float fMultiplier;
//      GetMultiplier(byFrequencyID, byDivisorID, & fMultiplier);
//      return fMultiplier;
//    }

    inline void GetMultiplierfromCOFVIDstatusRegisterBits(
      DWORD dwMSRlowmost, float * p_fMultiplier)
    {
      static BYTE byFrequencyID, byDivisorID;
      byFrequencyID = AMD::fromK10::GetFrequencyID(dwMSRlowmost);
      DEBUGN( "current core frequency ID:" << (WORD) byFrequencyID)

      AMD::fromK10::GetDivisorID(dwMSRlowmost, byDivisorID);
      DEBUGN( "current core divisor ID:" << (WORD) byDivisorID)

      GetMultiplier(byFrequencyID, byDivisorID, p_fMultiplier);
      DEBUGN( "multiplier= (frequency_ID + 8) / 1 << divisor_ID ="
        << * p_fMultiplier)
    }

    #ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
    inline void GetSameReferenceClockForAllCPUcores(
      float * p_fReferenceClockInMHz)
    {
      //TODO "41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG",
      //  "MSRC001_0015 Hardware Configuration Register (HWCR)":
      //"TscFreqSel: TSC frequency select. Read-write. BIOS: 1.
      // 0= The TSC increments at the rate of the REFCLK frequency.
      // 1=The TSC increments at the rate of the core P-state 0 COF specified by
      //   MSRC001_0064."

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
      if( * p_fReferenceClockInMHz == 0.0 )
        * p_fReferenceClockInMHz = g_fReferenceClockInMHz ;
      else
        //Save the current reference clock as last retrieved reference clock.
        g_fReferenceClockInMHz = * p_fReferenceClockInMHz ;
    }
    #endif //#ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF

    inline void GetVoltageFromCOFVIDstatusRegisterBits(
      DWORD dwMSRlowmost, float * p_fVoltageInVolt)
    {
      static BYTE byVoltageID;
      AMD::fromK10::GetVoltageIDfromCOFVIDstatusRegisterBits(dwMSRlowmost, byVoltageID);

      GetVoltageFromVoltageID(byVoltageID, p_fVoltageInVolt);
    }

    inline void GetReferenceClock(float * p_fReferenceClockInMHz, unsigned coreID)
    {
      //TODO better use APIC Timer Operation?
      //BIOS and Kernel Developer’s Guide (BKDG) For AMD Family 11h Processors:
      // "The pro-cessor bus clock is divided by the value in APIC3E0[Div] to
      //obtain a time base for the timer."

    #ifdef GET_REFERENCE_CLOCK_VIA_TSC_DIFF
      //Only get ref. clock if 1st core (else may be too small because the time
      //diff between core 0 and 1 may be taken)
      if( coreID == 0)
        GetSameReferenceClockForAllCPUcores(p_fReferenceClockInMHz);
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

    inline BYTE GetCurrentVoltageAndFrequency(
      float * p_fVoltageInVolt,
      float * p_fMultiplier ,
      float * p_fReferenceClockInMHz ,
      WORD wCoreID
      )
    {
      DEBUGN( "begin core ID:" << wCoreID )
    //  static BYTE g_byValue1;
      static BYTE byReadMSRreturnValue;

      byReadMSRreturnValue =
    //    (*g_pfnreadmsr) (
        ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,    // MSR index
      //      P_STATE_STATUS_REGISTER
        & g_dwMSRlowmost,// bit  0-31 (register "EAX")
        & g_dwMSRhighmost,
        1 << wCoreID //m_dwAffinityMask
        ) ;

      GetMultiplierfromCOFVIDstatusRegisterBits(g_dwMSRlowmost, p_fMultiplier);
      GetVoltageFromCOFVIDstatusRegisterBits(g_dwMSRlowmost, p_fVoltageInVolt);

      GetReferenceClock(p_fReferenceClockInMHz, wCoreID);
      DEBUGN( "reference clock:" << * p_fReferenceClockInMHz )
      DEBUGN( "returning" << (WORD) byReadMSRreturnValue )
      return byReadMSRreturnValue ;
    }

    //inline
    //void GetMainPllOpFreqIdMax() ;
    //Gets the max. multiplier.
    inline void GetMainPllOpFreqIdMax()
    {
      DEBUGN(/*"GetMainPllOpFreqIdMax"*/ "begin")
      ReadMSR(
        COFVID_STATUS_REGISTER ,
        & g_dwMSRlowmost ,
        & g_dwMSRhighmost ,
        1 ) ;
      //"54:49 MainPllOpFreqIdMax"
      g_fMainPllOpFreqIdMax = (g_dwMSRhighmost >> (49-32) &
        BITMASK_FOR_LOWMOST_6BIT );
      DEBUGN(/*"MainPllOpFreqIdMax:" <<*/ g_fMainPllOpFreqIdMax)
    }

    //inline void GetMSRregisterValue(
    //    BYTE byVoltageID,
    //    //const DIDandFID & didandfid ,
    //    BYTE FID ,
    //    BYTE DID ,
    //    DWORD & dwMSRhighmost ,
    //    DWORD & dwMSRlowmost
    //    ) ;
    //inline WORD GetVoltageID(float fVoltageInVolt ) ;
    inline void GetMSRregisterValue(
      const BYTE byVoltageID,
    //  const DIDandFID & didandfid ,
      const BYTE byFrequencyID ,
      const BYTE byDivisorID ,
      uint32_t & dwHighmostMSRvalue ,
      uint32_t & dwLowmostMSRvalue
      )
    {
      //GetFIDandDID( wFreqInMHz, byFrequencyID, byDivisorID ) ;
      //didandfid
      dwHighmostMSRvalue = 0 ;
      SET_P_STATE_TO_VALID(dwHighmostMSRvalue) ;

      //See AMD Family 11h Processor BKDG (document # 41256),
      // "MSRC001_00[6B:64] P-state [7:0] Registers"
      //5:0 CpuFid
      dwLowmostMSRvalue = ( byFrequencyID & 63) ;
      //CpuDid: core divisor ID.
      dwLowmostMSRvalue |= ( ( (WORD) byDivisorID ) <<
        START_BIT_FOR_CPU_CORE_DIVISOR_ID ) ;
      //15:9 CpuVid: core VID.
      dwLowmostMSRvalue |= ( ( (WORD) byVoltageID ) << 9) ; //<=>bits 9-15 shifted
    }

    /** Uses table "Table 5: SVI and internal VID codes" (7 bit) because same
     *  bit width as "15:9 CurCpuVid: current CPU core VID." (7 bit) ? */
    inline fastestUnsignedDataType GetVoltageID(const float fVoltageInVolt )
    {
      //E.g. for "1.1" V the float value is 1.0999999
      // (because not all numbers are representable with a 8 byte value)
      // so the voltage ID as float value gets "36.000004".
      float fVoltageID = (fVoltageInVolt - 1.55f) / -0.0125f ;
      fastestUnsignedDataType voltageID =
        //without explicit cast: compiler warning
        //Avoid g++ warning "warning: converting to `WORD' from `float'"
        (fastestUnsignedDataType)
        //ceil( (fVoltageInVolt - 1.55f) * -1.0f / 0.0125f ) ;
        //ceil( //(fVoltageInVolt - 1.55f) / -0.0125f
          fVoltageID //)
        ;
      //Check to which integer voltage ID the float value is nearer.
      //E.g. for: "36.0000008" - "36" = "0.0000008". -> use "36"
      if( fVoltageID - (float) voltageID >= 0.5 )
        ++ voltageID ;
      return voltageID ;
    }

    //inline unsigned long GetMSRregisterForPstate(
    //  unsigned char byPstate ) ;
    /** @return data type must have at least 32 bit: C0010064 has 8 hex digits->
     *    8*4=32 bit; if compiled for at least 32 bit then "unsigned" data type
     *    is sufficient */
    INLINE fastestUnsignedDataType GetMSRregisterForPstate(
      const fastestUnsignedDataType performanceStateIndex )
    {
      //unsigned long index ;
      return //"AMD: MSR C001_0064 specifies P-state 0"
        0xC0010064 + performanceStateIndex ;
    }

    /** @brief AMD composes the multiplier from 2 operands: divisor ID and
     *   frequency ID.
     *   multiplier=freqID/2^divisorID
     */
    INLINE void GetFreqIDandDivisorIDfromMulti(
      float fMultiplier,
      BYTE & r_byFreqID,
      BYTE & r_byDivisorID
      )
    {
      DEBUGN(//"GetFreqIDandDivisorIDfromMulti("
        << fMultiplier )

      r_byDivisorID = 0 ;
      g_fMaxMultiDiv2 = ( //g_fMainPllOpFreqIdMax + 8
        g_fMaxMultiplier ) / 2.0 ;
      //e.g. MaxMultiplier = 11, multiplier = 2,5:
      // -> multiplierAboveMaxMultiplier = 2,5 * 2 * 2 * 2 = 2,5 * 8 = 20;
      // DivisorID = 3 = log2(8)
      while( fMultiplier <= //g_fMaxMultiplier
          g_fMaxMultiDiv2)
      {
        fMultiplier *= 2.0f ;
        ++ r_byDivisorID ;
      }
      //multiplier is in range ]1/2 max_multi...max_multi] now
      r_byFreqID =
        //"The CPU COF specified by MSRC001_00[6B:64][CpuFid,CpuDid] is
        //((100 MHz * (CpuFid + 08h)) / (2^CpuDid))."
        (BYTE) (fMultiplier * 2.0f) - 8 ;
      DEBUGN(//"GetFreqIDandDivisorIDfromMulti(...)"
        << "FID:" << (WORD) r_byFreqID
        << "DID:" << (WORD) r_byDivisorID
        //"The CPU COF specified by MSRC001_00[6B:64][CpuFid,CpuDid] is
        //((100 MHz * (CpuFid + 08h)) / (2^CpuDid))"
        << "test (FID,DID)->multi: multi="//"FID+8/divisor=" << (r_byFreqID + 8) << "/"
    //    << pow(2.0,r_byDivisorID)
    //    << "=" << (r_byFreqID + 8)/pow(2.0,r_byDivisorID)
        << GetMultiplier(r_byFreqID, r_byDivisorID)
        )
      //multi = FID / 2^DID  divisor=2^DID   multi = FID / divisor | * divisor
      // FID=multi* divisor
      //lowest multi: minFID/8 (e.g. 12/8)

      //TODO
      //"•The frequency specified by (100 MHz * (CpuFid + 08h)) must always be >50%
      //of and <= 100% of the frequency specified by
      //F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
      //ReadPCIconfig(F3xD4) ;
    //  r_byFreqID = fMultiplier * g_byDivisor ;
    //  "CpuDid: core divisor ID. Read-write. Specifies the CPU frequency divisor;
      //see CpuFid.
    //  0h=Divisor of 1     3h=Divisor of 8
    //  1h=Divisor of 2     4h - 7h=Reserved
    //  2h=Divisor of 4"
    //  r_byDivisorID = log2(g_byDivisor) ;
    }

    //inline void SetVoltageAndMultiplier(
    //  float fVoltageInVolt,
    //  float fMultiplier ,
    //  BYTE byCoreID ) ;

    //inline BYTE SetPstateViaPstateControlRegister(
    //  BYTE byNewPstate,DWORD dwCoreBitmask);
    //It seems: setting a p-state for more than 1 core at a time does NOT work.
    //so call this method "n" times if you want the same p-state for "n" cores.
    INLINE BYTE SetPstateViaPstateControlRegister(BYTE byNewPstate, DWORD dwCoreBitmask)
    {
      BYTE byReturn = EXIT_FAILURE ;

      //Safety check.
      if( byNewPstate < NUMBER_OF_PSTATES )
      {
        DWORD dwMSRlow = byNewPstate ;

          dwMSRlow = (BYTE) byNewPstate ;
          //DEBUG("For core bitmask %lu: setting to pstate %u\n", dwCoreBitmask, byNewPstate);
          LOGN_VERBOSE( "For core bitmask " << dwCoreBitmask
            << ": setting to pstate "
            //Cast BYTE to WORD to output as number.
            << (WORD) byNewPstate );
          //DEBUG("the low 32 bits: %s\n", getBinaryRepresentation(arch,dwMSRlow) );
          //printf("  would write:  %s to MSR %lx\n", getBinaryRepresentation(&msrvalue,arch), msr_register_number);
          //waitForEnter("um in MSR zu schreiben") ;
          //if ((msr_write(msrfile, msr_register_number, &msrvalue)) != OK)
          //  printf("MSR write failed\n");

          //DEBUG("Adress of mp_cpuaccess: %lx\n", mp_cpuaccess);
    #ifndef _EMULATE_TURION_X2_ULTRA_ZM82
          if(
            //CONTROLLER_PREFIX
    //          mp_cpuaccess->WrmsrEx(
            WriteMSR(
              P_STATE_CONTROL_REGISTER,
              dwMSRlow,
              0,
              dwCoreBitmask
              )
    //          1
            )
          {
            LOGN_VERBOSE("Setting p-state succeeded.");
            byReturn = EXIT_SUCCESS ;
            //Wait 1 millisecond (> maximum stabilization time).
    //          SLEEP_1_MILLI_SECOND
          }
          else
          {
            DEBUG("Setting p-state failed\n");
          }
          byReturn = EXIT_SUCCESS ;
    #endif //_EMULATE_TURION_X2_ULTRA_ZM82
    //#ifndef LINK_TO_WINRING0_STATICALLY
    //    }
    //#endif //#ifdef LINK_TO_WINRING0_STATICALLY
      }
      return byReturn ;
    }

    /** see "2.4.2.3 P-state Transition Behavior"
     * AMD K10 doc:
     * 2.4.1.9.2 Software-Initiated CPU Voltage Transitions
     * "NewCpuVid = the destination CPU VID.
      F3xA0[SlamVidMode]=1:
      1. Write NewCpuVid to MSRC001_0070[CpuVid].
      2. Wait the specified F3xD8[VSSlamTime].
      F3xA0[SlamVidMode]=0:
      Software must use the sequence for F3xA0[SlamVidMode]=0 defined in section 2.4.1.9.1 [Software-Initiated
      NB Voltage Transitions] on page 30 to control the single-plane through NbVid."

      @return use unsigned data type because it is the fastest data type.
     */
    INLINE unsigned SetVoltageAndMultiplier(
      float fVoltageInVolt,
      float fMultiplier ,
      BYTE byCoreID )
    {
      //    DWORD dwLowmostBits , dwHighmostBits = 0 ;
      //    dwLowmostBits =
      //      //Freq / "FSB in MHz" = multiplier
      //      (BYTE) fMultiplier ;
      ////    std::stringstream ss ;
      ////    ss << "multiplier to set: " << fMultiplier << "lowmost bits:"
      ////        << dwLowmostBits ;
      ////    MessageBox(NULL,ss.str().c_str(), "info" , MB_OK ) ;
      //    g_byValue1 = GetVoltageID( fVoltageInVolt ) ;

      //BYTE byFrequencyID , byDivisorID ;
      DEBUGN(//"SetVoltageAndMultiplier("
        << fVoltageInVolt
        << "," << fMultiplier
        << "," << (WORD) byCoreID )

    #ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
      if( g_fReferenceClockInMHz != 0.0f &&
          GetMaxMultiIfGreaterCurrentPstateLimitMulti(byCoreID, fVoltageInVolt,
        fMultiplier) &&
        //fVoltageInVolt <> 0 (is 0 if GetInterpolated...(...) failed)
        fVoltageInVolt
        )
    #endif
      {
        BYTE byVoltageID = GetVoltageID( fVoltageInVolt ) ;
        GetFreqIDandDivisorIDfromMulti(fMultiplier, g_byFreqID, g_byDivisorID) ;
        GetMSRregisterValue(
          byVoltageID,
          g_byFreqID ,
          g_byDivisorID
          , g_dwMSRhighmost ,
          g_dwMSRlowmost ) ;

    //#ifndef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
        static DWORD dwMSRregisterIndex;
        dwMSRregisterIndex = GetMSRregisterForPstate(
          g_byDivisorID
          ) ;
        DEBUGN(//"SetVoltageAndMultiplier(...)"
          "before WriteMSR" )
        if( WriteMSR(
            dwMSRregisterIndex
            , g_dwMSRlowmost
            , g_dwMSRhighmost
            , 1 << byCoreID
            )
          )
        {
          //see "2.4.1.9 Software-Initiated Voltage Transitions"
          //TODO "2. Wait the specified F3xD8[VSSlamTime]."
    //      if(
          SetPstateViaPstateControlRegister(
            g_byDivisorID ,
            1 << byCoreID ) ;
    //        )
    //        byRet = true ;
          return 0;
        }
    //#endif //#ifndef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
      }
      return 1;
    }
  }
}

#endif /* GRIFFIN_H_ */
