/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <Controller/CPU-related/I_CPUcontroller.hpp>
//#include <windows.h> //ULONGLONG
//#include <Windows_compatible_typedefs.h> //for ULONGLONG

//#define IA32_APERF 0xE8 //dec. 232

//maybe see http://www.meisterkuehler.de/cms/cpu_verlustleistung.html

//some measurement values for trying to compute the Watts:
//measured with the "EKM" meas. device: 100% Load for every core :
//prime 95 8 cores at 1600 MHz: 76 W   P=V^2*1600MHz*100%
//prime 95 8 cores at 1330 MHz: 66 W
  // 66W=V^2*1330MHz*100%  | : 1330 MHz   V^2=66W / 1330 MHz
//  V^2 = 66W / (1 330 000 000/s) = 66 W * s / 1 300 000 000
//  V^2 = 0,0496240601503759 Ws/1000 000  = 0,0496240601503759
 //  | sqrt
//  V= (66W / 1330 MHz)^(0.5) = 0,0496240601503759 W/MHz
// V = 2,253202848596432e-4 Ws = 0.0002253202848596432 Ws
//prime 95 8 cores at 931 MHz: 51 W
//
//P=V^2*f*load

class NehalemController
  : public I_CPUcontroller
{
  //Model * mp_model ;
public:
  void AccuratelyStartPerformanceCounting(
    DWORD dwAffinityBitMask ,
    //Pentium M has 1 or 2 "Performance Event Select Register" from 
    //  MSR ... to MSR ...  for 
    // 1 or 2 "Performance Event Counter Registers" from 
    //  ... to ...
    //  that store the 48 bit counter value
    BYTE byPerformanceEventSelectRegisterNumber ,
    BYTE byEventSelect , //8 bit
    BYTE byUnitMask , // 8 bit
    bool bUserMode,
    bool bOSmode,
    bool bEdgeDetect,
    bool bPINcontrol,
    bool bEnableAPICinterrupt,
    bool bInvertCounterMask ,
    BYTE byCounterMask
    ) ;
  void DecreaseVoltageBy1Step(float & r_fVoltage) ;
  BYTE GetCurrentVoltageAndFrequency(
    //float because: the FSB*multiplier is not exactly an integer without
    //floating point digits
    float & r_fFreqInMHz
    , float & r_fVoltageInVolt
    , BYTE byCoreID
    ) ;
  BYTE GetCurrentPstate(WORD & wFreqInMHz, float & Volt, BYTE byCoreID) ;
  BYTE GetCurrentPstate(
    BYTE & r_byFreqID
    , BYTE & r_byVoltageID
    , BYTE byCoreID
    ) ;
  NehalemController(void);
  BYTE Init() ;
  void GetAllPossibleFrequencies(std::set<VoltageAndFreq> & 
    r_stdsetvoltageandfreq) ;
  WORD GetNumberOfPstates() ;
  //Intel soft dev man vol 3 A  8.6
  //Addressable IDs for Logical processors in the same Package
//(CPUID.1:EBX[23:16]) — Indicates the maximum number of addressable
//ID for logical processors in a physical package. Within a physical package,
//there may be addressable IDs that are not occupied by any logical
//processors. This parameter does not represents the hardware capability of
//the physical processor.2

  WORD GetNumberOfCPUcores() {
    //not always correct
    return 8 ; }
  WORD GetMaximumFrequencyInMHz() ;
  WORD GetMinimumFrequencyInMHz() ;
  WORD GetMaximumVoltageID() ;
  WORD GetMinimumVoltageID() ;
  inline float GetMinimumVoltageInVolt() ;
  //WORD GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld) ;
  BYTE GetPstate(WORD wPstateID, VoltageAndFreq & r_voltageandfreq) ;
  //bool GetPstateSafefy(
  //  WORD wFreqInMHz
  //  , float fVoltage 
  //  ) ;
  WORD GetVoltageID(float fVoltageInVolt ) ;
  float GetVoltageInVolt(WORD wVoltageID ) ;
  void IncreaseVoltageBy1Step(float & r_fVoltage) ;
  void IncreaseVoltageForCurrentPstate(BYTE byCoreID) ;
  void PerformanceEventSelectRegisterWrite(
    DWORD dwAffinityBitMask ,
    //Pentium M has 1 or 2 "Performance Event Select Register" from 
    //  MSR ... to MSR ...  for 
    // 1 or 2 "Performance Event Counter Registers" from 
    //  ... to ...
    //  that store the 48 bit counter value
    BYTE byPerformanceEventSelectRegisterNumber ,
    BYTE byEventSelect , //8 bit
    BYTE byUnitMask , // 8 bit
    bool bUserMode,
    bool bOSmode,
    bool bEdgeDetect,
    bool bPINcontrol,
    bool bEnableAPICinterrupt,
    bool bEnablePerformanceCounter,
    bool bInvertCounterMask ,
    BYTE byCounterMask
    ) ;
  void PrepareForNextPerformanceCounting(
    DWORD dwAffinityBitMask 
    , BYTE byPerformanceEventSelectRegisterNumber
    ) ;
  BYTE SetVoltageAndFrequency( 
    float fVolt , 
    WORD wFreqInMHz ,
    BYTE byCoreID 
    ) ;
  //BYTE SetFreqAndVoltageFromFreq(
  //  WORD wFreqInMHz 
  //  , BYTE byCoreID) ;
  BYTE SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , const std::set<VoltageAndFreq> & 
      cr_stdsetvoltageandfreqForInterpolation
    , BYTE byCoreID ) ;
  BYTE TooHot() ;
  BOOL // TRUE: success, FALSE: failure
    //WINAPI
    WrmsrEx(
      DWORD index,		// MSR index
      DWORD dwLow ,//eax,			// bit  0-31
      DWORD dwHigh, //edx,			// bit 32-63
      DWORD affinityMask	// Thread Affinity Mask
    ) ;
public:
  ~NehalemController(void);
};
