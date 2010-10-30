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

//extern //inline
//  BYTE ReadMSR(
//  DWORD dwRegisterIndex,   // MSR index
//  PDWORD p_dwEAX,     // bit  0-31
//  PDWORD p_dwEDX,     // bit 32-63
//       //1bin =core 0; 10bin=2dec= core 1
// DWORD_PTR affinityMask  // Thread Affinity Mask
//  ) ;
////extern
//  inline
//  BYTE WriteMSR(
//  DWORD dwRegisterIndex,   // MSR index
//  DWORD dwEAX,     // bit  0-31
//  DWORD dwEDX,     // bit 32-63
//       //1bin =core 0; 10bin=2dec= core 1
//  DWORD_PTR affinityMask  // Thread Affinity Mask
//  ) ;

extern BYTE g_byFreqID,g_byDivisorID ;
extern BYTE g_byValue1 , g_byValue2, g_byValue3 ;
extern BYTE g_byDivisor ;
extern DWORD g_dwMSRhighmost, g_dwMSRlowmost ;
//MSRC001_0071 COFVID Status Register
//54:49 MainPllOpFreqIdMax: main PLL operating frequency ID maximum.
extern float g_fMainPllOpFreqIdMax ;
extern float g_fMainPllOpFreqId ;
extern float g_fMainPllOpFreqIdMaxDiv2 ;
extern float g_fMaxMultiDiv2 ;
extern float g_fValue1 ;

//#define COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT

#ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
  //class I_CPUcontroller ;
  #include <Controller/CPU-related/I_CPUcontroller.hpp>

  extern I_CPUcontroller * g_p_cpucontroller ;
  extern std::set<VoltageAndFreq> * g_p_stdsetvoltageandfreqWanted ;
#endif

#ifdef _DEBUG
#include <math.h> //pow(...)
#endif

WORD GetMaximumVoltageID() ;
WORD GetMinimumVoltageID() ;
void GetMinAndMaxVoltageID(BYTE &,BYTE &) ;

inline float * GetAvailableMultipliers(WORD * p_wNumberOfArrayElements )
{
  float fMaxMultiDiv2 ;
  //TODO should be MainPllOpFreqId from F3xD4 Clock Power/Timing Control 0 Register
  g_fMainPllOpFreqId = g_fMainPllOpFreqIdMax ;
  float fMaxMultiplier = g_fMainPllOpFreqId + 8 ;
  //41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
  //" if MainPllOpFreqIdMax = 00h, then there is no frequency limit."
  if( g_fMainPllOpFreqIdMax )
  {
    fMaxMultiDiv2 =
     //"The frequency specified by (100 MHz * (CpuFid + 08h)) must always be
     //>50% of and <= 100% of the frequency specified by
     //F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]"
     ( fMaxMultiplier ) / 2 ;
  }
  else
  {
    //41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG:
    //" if MainPllOpFreqIdMax = 00h, then there is no frequency limit."
    fMaxMultiDiv2 =
      //41256  Rev 3.00 - July 07, 2008  AMD Family 11h Processor BKDG p.236:
      //"5:0 CpuFid: core frequency ID."
      //highest value for 6 bits is 111111bin = 63dec
      ( 63 + 8 ) / 2 ;
  }
  DEBUGN("fMainPllOpFreqIdDiv2:" << fMaxMultiDiv2)
  BYTE byMaxMultiDiv2 = (BYTE) fMaxMultiDiv2 ;
  //"The frequency specified by (100 MHz * (CpuFid + 08h)) must always be
  //>50% of and <= 100% of the frequency specified by
  //F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
  while( byMaxMultiDiv2 <= fMaxMultiDiv2 )
    ++ byMaxMultiDiv2 ;
  BYTE byNumMultisPerDivisor = (BYTE)
      ( fMaxMultiplier - byMaxMultiDiv2 + 1 ) ;
  BYTE byNumMultis =
    //e.g. 22 - 12 + 1 = 11 multipliers (12,13,14,15,16,17,18,19,20,21,22)
    byNumMultisPerDivisor *
    //num divisor IDs exc. Divisor ID "o"
    3
    //for divisor ID 0 other FIDs than max. FID cause a crash.
    + 1 ;
  DEBUGN("byMaxMultiDiv2" << (WORD) byMaxMultiDiv2
    << " byNumMultisPerDivisor: " << (WORD) byNumMultisPerDivisor
    << " byNumMultis" << (WORD) byNumMultis )
  float * ar_f = new float[byNumMultis] ;
  //If allocating the array on the heap succeeded.
  if( ar_f )
  {
    BYTE byMultiplierIndex = 0 ;
    * p_wNumberOfArrayElements = byNumMultis ;
    float fDivisor ;
    float fMulti = //The minimum multiplier for Griffin
      (float) byMaxMultiDiv2 / 8.0 ;
//     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;
    for( BYTE byDivisorIDIndex = 3 ; byDivisorIDIndex > 0
      ; -- byDivisorIDIndex )
    {
      fDivisor = (float)
        //^= power of 2
        ( 1 << byDivisorIDIndex ) ;
      DEBUGN("fDivisor:" << fDivisor )
      for( BYTE byFrequencyID = byMaxMultiDiv2 ;
        byFrequencyID <= fMaxMultiplier ; ++ byFrequencyID )
      {
//      8:6 CpuDid: core divisor ID. Read-write.
//      Specifies the CPU frequency divisor; see CpuFid.
//      0h=Divisor of 1   3h=Divisor of 8
//      1h=Divisor of 2   4h - 7h=Reserved
//      2h=Divisor of 4
        fMulti = (float) byFrequencyID /
          fDivisor ;
        DEBUGN("adding multi" << fMulti )
        ar_f[ byMultiplierIndex ] = fMulti ;
//       stdstrstream << fMulti << " " ;
        ++ byMultiplierIndex ;
      }
    }
    DEBUGN("adding multi" << fMaxMultiplier )
    ar_f[ byMultiplierIndex ] = fMaxMultiplier ;
    return ar_f ;
  }
  else
    *p_wNumberOfArrayElements = 0 ;
  return NULL ;
}

inline float * GetAvailableVoltages(WORD & r_wNum )
{
//  g_byValue1 = GetMaximumVoltageID() ;
//  g_byValue2 = GetMinimumVoltageID() ;
  GetMinAndMaxVoltageID(g_byValue1,g_byValue2) ;
  r_wNum = ( g_byValue1 - g_byValue2 + 1 ) ;
  DEBUGN("# VIDs:" << (WORD) r_wNum )
  float * ar_f = new float[ r_wNum ] ;
  DEBUGN("ar_f:" << ar_f )
  if( ar_f )
  {
    WORD wIndex = 0 ;
    for( BYTE wCurrentVoltageID =
      //for AMD Griffin: max. VID = min voltage
        g_byValue1 ;
      wCurrentVoltageID >= g_byValue2 ; -- wCurrentVoltageID )
    {
      g_fValue1 =
        //VID 28 = 1.2 = 1.55 - 28 * 0.0125 ;
        //VID 64 = 0.75 = 1.55 - 64 * 0.0125
        1.55f - wCurrentVoltageID * 0.0125f ;
      DEBUGN("adding voltage " << g_fValue1 )
      ar_f[ wIndex ++ ] = g_fValue1 ;
    }
  }
  return ar_f ;
}

inline BYTE GetCurrentVoltageAndFrequencyAMDGriffin(
  float * p_fVoltageInVolt,
  float * p_fMultiplier ,
  float * p_fReferenceClockInMHz ,
  WORD wCoreID
  )
{
  g_byValue3 =
//    (*g_pfnreadmsr) (
    ReadMSR(
    COFVID_STATUS_REGISTER ,    // MSR index
  //      P_STATE_STATUS_REGISTER
    & g_dwMSRlowmost,// bit  0-31 (register "EAX")
    & g_dwMSRhighmost,
    1 << wCoreID //m_dwAffinityMask
    ) ;
  //Frequency ID. "5:0 CurCpuFid: current core frequency ID. Read-only."
  g_byValue1 = (BYTE)( g_dwMSRlowmost & BITMASK_FOR_LOWMOST_6BIT ) ;
  //Divisor ID. "8:6 CurCpuDid: current core divisor ID. Read-only."
  g_byValue2 = //(BYTE)(
  //      (g_dwLowmostBits & 448//=111000000bin
  //      ) >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) ; //<=>bits 6-8 shifted to     }
    ( g_dwMSRlowmost >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) &
    BITMASK_FOR_LOWMOST_3BIT ;
  //multi = Frequency IDentifier + 8 / 2 ^ "Divisor IDentifier"
  * p_fMultiplier = ( (float) ( g_byValue1 + 8 ) ) /
      (float) ( 1 << g_byValue2 ) ;
  //voltage ID "15:9 CurCpuVid: current core VID. Read-only."
  g_byValue1 = //(BYTE) (
  //      (g_dwLowmostBits & BITMASK_FOR_CPU_CORE_VOLTAGE_ID//=1111111000000000bin
  //    ) >> 9 ) ; //<=>bits 9-15 shifted   }
    ( g_dwMSRlowmost >> 9 ) & BITMASK_FOR_LOWMOST_7BIT ;
  //Diff 1.0875 (VID 30) & 0.925 (VID=17) = 0.1625V
  //1 voltage ID step = 0.1625V / (30-17) = 0.1625V / 13 = 0.0125
  //0.925- 17 * 0.0125 = 0.7125 ;
  * p_fVoltageInVolt = //0.7125 + g_byValue3 * 0.0125 ;
    //The maximum possible voltage.
    1.55f - ( (float)( g_byValue1 ) * 0.0125f ) ;

  //TODO better use APIC Timer Operation?
  //BIOS and Kernel Developer’s Guide (BKDG) For AMD Family 11h Processors:
  // "The pro-cessor bus clock is divided by the value in APIC3E0[Div] to
  //obtain a time base for the timer."
  //This call sets g_fReferenceClockInMHz to the current reference clock.
  //This update of the value would be senseful for setting the CPU core
  //via "frequency" as parameter value the next time.
  //    GetCurrentReferenceClock(12.0 ,
  //      1 , //min. timespan in ms
  //      10000 ) ;
  * p_fReferenceClockInMHz = 100.0 ;
  return g_byValue3 ;
}

//inline
//void GetMainPllOpFreqIdMax() ;
//Gets the max. multiplier.
inline void GetMainPllOpFreqIdMax()
{
  DEBUGN("GetMainPllOpFreqIdMax")
  ReadMSR(
    COFVID_STATUS_REGISTER ,
    & g_dwMSRlowmost ,
    & g_dwMSRhighmost ,
    1 ) ;
  //"54:49 MainPllOpFreqIdMax"
  g_fMainPllOpFreqIdMax = (g_dwMSRhighmost >> (49-32) &
    BITMASK_FOR_LOWMOST_6BIT );
}

inline void GetMinAndMaxVoltageID(BYTE & byMaxVID,BYTE & byMinVID)
{
  ReadMSR(
    COFVID_STATUS_REGISTER,
    & g_dwMSRlowmost,
    & g_dwMSRhighmost,
    1 ) ;
  byMaxVID = ( g_dwMSRhighmost >>
    //"48:42 [...]: minimum voltage. Read-only. Specifies the VID code
    //corresponding to the minimum voltage (highest VID code) that the processor
    //drives. 00h indicates that no minimum VID code is specified.
    //See section 2.4.1 [Processor Power Planes And Voltage Control]."
    ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VID - 32 )
    ) & BITMASK_FOR_LOWMOST_7BIT;
  DEBUGN("highest VID:" << (WORD) byMaxVID )
  byMinVID = ( g_dwMSRhighmost >>
  //"41:35 [...]: maximum voltage. Read-only. Specifies the VID code
  //corresponding to the maximum voltage (lowest VID code) that the processor
  //drives. 00h indicates that no maximum VID code is specified.
  //See section 2.4.1 [Processor Power Planes And Voltage Control].
    ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
    ) & BITMASK_FOR_LOWMOST_7BIT;
  DEBUGN("lowest VID:" << (WORD) byMinVID )
}

inline WORD GetMaximumVoltageID()
{
//    return 64 ;
  DWORD dwEAXlowMostBits, dwEDXhighMostBits ;
  ReadMSR(
    COFVID_STATUS_REGISTER,
    & dwEAXlowMostBits,
    & dwEDXhighMostBits,
    1 ) ;
  BYTE byHighestVID = ( dwEDXhighMostBits >>
      ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VID - 32 )
      ) & BITMASK_FOR_LOWMOST_7BIT;
  DEBUGN("highest VID:" << (WORD)byHighestVID)
  return byHighestVID ;
}

inline WORD GetMinimumVoltageID()
{
//    return 36 ;
  DWORD dwEAXlowMostBits, dwEDXhighMostBits ;
  ReadMSR(
    COFVID_STATUS_REGISTER,
    & dwEAXlowMostBits,
    & dwEDXhighMostBits,
    1
    ) ;
  BYTE byLowestVID = ( dwEDXhighMostBits >>
      ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
      ) & BITMASK_FOR_LOWMOST_7BIT;
  DEBUGN("lowest VID:" << (WORD)byLowestVID)
  return byLowestVID ;
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
  BYTE byVoltageID,
//  const DIDandFID & didandfid ,
  BYTE byFrequencyID ,
  BYTE byDivisorID ,
  DWORD & dwHighmostMSRvalue ,
  DWORD & dwLowmostMSRvalue
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

inline WORD GetVoltageID(float fVoltageInVolt )
{
  //E.g. for "1.1" V the float value is 1.0999999
  // (because not all numbers are representable with a 8 byte value)
  // so the voltage ID as float value gets "36.000004".
  float fVoltageID = (fVoltageInVolt - 1.55f) / -0.0125f ;
  WORD wVoltageID =
    //without explicit cast: compiler warning
    //Avoid g++ warning "warning: converting to `WORD' from `float'"
    (WORD)
    //ceil( (fVoltageInVolt - 1.55f) * -1.0f / 0.0125f ) ;
    //ceil( //(fVoltageInVolt - 1.55f) / -0.0125f
      fVoltageID //)
    ;
  //Check to which integer voltage ID the float value is nearer.
  //E.g. for: "36.0000008" - "36" = "0.0000008". -> use "36"
  if( fVoltageID - (float) wVoltageID >= 0.5 )
    ++ wVoltageID ;
  return wVoltageID ;
}

//inline unsigned long GetMSRregisterForPstate(
//  unsigned char byPstate ) ;
inline unsigned long GetMSRregisterForPstate(
  unsigned char byPstate )
{
  //unsigned long index ;
  return //"AMD: MSR C001_0064 specifies P-state 0"
    0xC0010064 + byPstate ;
}

inline void GetFreqIDandDivisorIDfromMulti(
  float fMultiplier,
  BYTE & r_byFreqID,
  BYTE & r_byDivisorID
  )
{
  DEBUGN("GetFreqIDandDivisorIDfromMulti("
    << fMultiplier )
//  float fDivisor = g_fMainPllOpFreqIdMax / fMultiplier ;
//  g_byDivisor =
//    //Must be stored as integer: if e.g.
//    //max. multi: 22.0
//    //22.0 / 3.75 = 5.8666666666666666666666666666667 -> rounded down to "5"
//    //-> freqID-> 3.75*5=
//    (BYTE) g_fMainPllOpFreqIdMax / fMultiplier ;

//  g_byDivisor = 0 ;
  r_byDivisorID = 0 ;
//  g_fMainPllOpFreqIdMaxDiv2 = g_fMainPllOpFreqIdMax / 2.0 ;
  g_fMaxMultiDiv2 = ( g_fMainPllOpFreqIdMax + 8 ) / 2.0 ;
  while( fMultiplier <= //g_fMainPllOpFreqIdMaxDiv2
    g_fMaxMultiDiv2 )
  {
    fMultiplier *= 2.0 ;
//    ++ g_byDivisor ;
    ++ r_byDivisorID ;
  }
  r_byFreqID =
    //"The CPU COF specified by MSRC001_00[6B:64][CpuFid,CpuDid] is
    //((100 MHz * (CpuFid + 08h)) / (2^CpuDid))."
    (BYTE) fMultiplier - 8 ;
  DEBUGN("GetFreqIDandDivisorIDfromMulti(...)"
    << "FID:" << (WORD) r_byFreqID
    << "DID:" << (WORD) r_byDivisorID
    //"The CPU COF specified by MSRC001_00[6B:64][CpuFid,CpuDid] is
    //((100 MHz * (CpuFid + 08h)) / (2^CpuDid))"
    << "test: multi=FID+8/divisor=" << (r_byFreqID + 8) << "/"
    << pow(2.0,r_byDivisorID)
    << "=" << (r_byFreqID + 8)/pow(2.0,r_byDivisorID)
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
inline BYTE SetPstateViaPstateControlRegister(BYTE byNewPstate, DWORD dwCoreBitmask)
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

#ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
inline BYTE GetMaxMultiIfGreaterCurrentPstateLimitMulti(
  BYTE byCoreID , float & fVoltageInVolt , float & fMultiplier )
{
  static BYTE bySuccess = false ;
  //At my laptop the current limit was set to "0" on AC power, "1" on DC power.
  //If 100% CPU load and above freq calculated by the current p-state limit,
  //the previous freq persists. To avoid this, retrieve the current limit.
  bySuccess = ::ReadMSR(
    P_STATE_CURRENT_LIMIT_REGISTER,
    & g_dwMSRlowmost,
    & g_dwMSRhighmost,
    1 << byCoreID ) ;
  //TODO to use this variable we would have to get the "desired voltages"
  static BYTE byCurPstateLimit ;
  byCurPstateLimit = g_dwMSRlowmost & BITMASK_FOR_LOWMOST_3BIT ;
  //TODO to use this variable we would have to get the "desired voltages"
  // for the performance state (from the executable) the limit applies to
  static float fMaxMultiplierAccordingCurrentLimit ;
  fMaxMultiplierAccordingCurrentLimit =
    //g_fMaxMultiplier
    ( g_fMainPllOpFreqIdMax + 8 ) / ( 1 << byCurPstateLimit ) ;
  DEBUGN("p-state current limit:" << //dwMSRlowmost
    (WORD) byCurPstateLimit
    << " -> current max. Frequency ID:" << fMaxMultiplierAccordingCurrentLimit )
  if( fMultiplier > fMaxMultiplierAccordingCurrentLimit )
  {
    //Use the max multiplier according to the limit because the multiplier
    //wouldn't be applied.
    fMultiplier = fMaxMultiplierAccordingCurrentLimit ;

    fVoltageInVolt = 0.0 ;
    //TODO because the voltage changed (is too high):
    //get voltage for new multiplier.
    bySuccess = g_p_cpucontroller->GetInterpolatedVoltageFromFreq(
      (WORD) ( fMultiplier *
      //Reference clock for AMD Griffin usually is 100 MHz.
      100.0 ) ,
      fVoltageInVolt ,
      * g_p_stdsetvoltageandfreqWanted
      ) ;
    DEBUGN("multiplier > max. multplier according to current P state limit"
      "current pstate multi:" << fMultiplier
      << "voltage:" << fVoltageInVolt )
    return bySuccess ;
//    //This indirectly calls _this_ function.
//      SetFreqAndVoltageFromFreq
//      ( wMaxFreqAccordingCurrentLimit, byCoreID) ;
  }
#ifdef _DEBUG
  else
  {
//    float fVoltageInVoltTest ;
//    g_p_cpucontroller->GetInterpolatedVoltageFromFreq(
//      (WORD) ( fMultiplier * 100.0 ) ,
//      fVoltageInVoltTest ,
//      * g_p_stdsetvoltageandfreqWanted
//        ) ;
    DEBUGN("multiplier <= max. multplier according to current P state limit"
      "current pstate multi:" << fMultiplier
      << "interpolated voltage (in Volt) would be:" << fVoltageInVoltTest )
  }
#endif
//  else
  return bySuccess ;
}
#endif //#ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT

//http://www.parashift.com/c++-faq-lite/inline-functions.html:
//"Note: It's imperative that the function's definition (the part between the
//{...}) be placed in a header file, unless the function is used only in a
//single .cpp file. In particular, if you put the inline function's definition
//into a .cpp file and you call it from some other .cpp file, you'll get an
//"unresolved external" error from the linker. "
//inline
inline void SetVoltageAndMultiplier(
  float fVoltageInVolt,
  float fMultiplier ,
  BYTE byCoreID )
{
//  BYTE byRet = false ;
  //BYTE byFrequencyID , byDivisorID ;
  //DWORD dwIndex ;
  DEBUGN("SetVoltageAndMultiplier("
    << fVoltageInVolt
    << "," << fMultiplier
    << "," << (WORD) byCoreID )
  //GetVIDmFIDnDID(dwLow, pstateMergedFromUserAndMSR ) ;

#ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
  if( GetMaxMultiIfGreaterCurrentPstateLimitMulti(byCoreID,fVoltageInVolt,
    fMultiplier) &&
    //fVoltageInVolt <> 0 (is 0 if GetInterpolated...(...) failed)
    fVoltageInVolt
    )
#endif
  {
    BYTE byVoltageID = GetVoltageID( fVoltageInVolt ) ;
    GetFreqIDandDivisorIDfromMulti(fMultiplier,g_byFreqID,g_byDivisorID) ;
    GetMSRregisterValue(
      byVoltageID, //didandfid
      g_byFreqID ,
      g_byDivisorID
      , g_dwMSRhighmost ,
      g_dwMSRlowmost ) ;

//#ifndef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
    DWORD dwMSRregisterIndex = GetMSRregisterForPstate( //didandfid.m_byDivisorID
      g_byDivisorID
      ) ;
    //bool_ = WriteToPstateOrCOFVIDcontrolRegister (
     // dwIndex ,
    // // unsigned char byVID, //unsigned short wFreqInMHz
     // //unsigned char byFreqID,
    // // unsigned char byDivID,
    //  //const PState & rpstateFromMSR,

    //  //Used to check against overvolting.
    //  pstateMergedFromUserAndMSR,
    //  //Used to write to MSR.
    //  pstateFromUser,
    //  //PState & r_pstateMergedFromUserAndMSR,
    //  dwMSRhighmost,
    //  dwMSRlowmost,
    //  //DWORD_PTR w64ulAffinityMask
    //  //DWORD dwCoreID
    //  //CPU core number, beginning from number "0"
    //  byCoreID
    //  ) ;
    DEBUGN("SetVoltageAndMultiplier(...)"
      "before WriteMSR" )
    if( WriteMSR(
      dwMSRregisterIndex
      , g_dwMSRlowmost
      , g_dwMSRhighmost
      , 1 << byCoreID
      )
      )
    {
//      if(
        SetPstateViaPstateControlRegister( //didandfid.m_byDivisorID
          g_byDivisorID ,
          1 << byCoreID ) ;
//        )
//        byRet = true ;
    }
//#endif //#ifndef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
  }
}

#endif /* GRIFFIN_H_ */
