/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany
 * ("Trilobyte SE") 2010-at least 2012.
 * You are allowed to modify and use the source code from Trilobyte SE for free
 * if you are not making profit directly or indirectly with it or its adaption.
 * Else you may contact Trilobyte SE. */
/*
 * AMD_NPT_family_0Fh.hpp
 *
 *  Created on: 04.05.2012
 *      Author: Stefan
 */

#ifndef AMD_NPT_FAMILY_0FH_HPP_
#define AMD_NPT_FAMILY_0FH_HPP_

//ReadMSR(...)
#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp>

#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_3BIT
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
//#define DEBUGN(to_ostream) ;

//#include <pantheios/pantheios.hpp>

//From "BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh Processors",
// 32559 Rev. 3.16 November 2009 :

//190 Memory System Configuration Chapter 4: Current Temperature (CurTmp)—Bits 23-14.

#include "AMD_NPT_family_0Fh_registers.h"
#include <math.h> //ceil(...)
//TODO
//10.6.2.1.1 Voltage Stabilization Time
//10.6.2.1.3 PLL Lock Time
//10.5.7.2.1 Changing the VID
//10.5.7.2.2 Changing the FID
//P-State Voltage Step Size (PstateStep)—Bit 56.
//Voltage Regulator Stabilization Time (VSTime)
//10.6.2.1.5 Isochronous Relief Time

#define BIT_31_SET 2147483648UL

extern ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace;

#if defined(COMPILE_WITH_LOG) && defined(USE_LOG4CPLUS)
  #include <log4cplus/logger.h>

  extern log4cplus::Logger log4cplus_logger;
#endif

#define GET_FREQUENCY_ID_FROM_FREQ_IN_MHZ(freqInMHz) ( \
  (BYTE)( \
    ( (float) freqInMHz / 200.0f  /*= multiplier) */ \
    - 4.0f ) / 0.5f ) /*= frequency ID) */ \
    )

class MinAndMaxFID
{
public:
  BYTE m_minFID;
  BYTE m_maxFID;
  MinAndMaxFID(BYTE minFID, BYTE maxFID)
    : m_minFID(minFID), m_maxFID(maxFID)
  {
  }
};

/**Array index == frequency ID. */
//TODO do these only apply to the MINIMUM FIDs for a CPU? i.e.
// array index == FID ; FID+4=multiplier multiplier*200MHz=CPU core freq.
/** BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh Processors
    32559 Rev. 3.16 November 2009
    "10.5.7.1 FID to VCO Frequency Relationship":
 *  "The processor supports direct transitions between the minimum core
 *  frequency in Table 69 and any of the core frequencies from Table 70 listed
 *  in the Portal Core Frequencies column associated with the minimum core
 *  frequency."
 */
static MinAndMaxFID s_ar_minandmaxfid[] = {
  //800 MHz -> 1600, 1700, 1800 MHz
  MinAndMaxFID( GET_FREQUENCY_ID_FROM_FREQ_IN_MHZ(1600),10),
  MinAndMaxFID(8,12), //900 MHz -> 1600, 1700, 1800, 1900, 2000 MHz
  MinAndMaxFID(10,14), //1000 MHz -> 1800, 1900, 2000, 2100, 2200 MHz
  MinAndMaxFID(12,16), //1100 MHz -> 2000, 2100, 2200, 2300, 2400 MHz
  MinAndMaxFID(14,18), //1200 MHz -> 2200, 2300, 2400, 2500, 2600 MHz
  MinAndMaxFID(16,20), //1300 MHz -> 2400, 2500, 2600, 2700, 2800 MHz
  MinAndMaxFID(18,22), //1400 MHz -> 2600, 2700, 2800, 2900, 3000 MHz
  MinAndMaxFID(20,24)  //1500 MHz -> 2800, 2900, 3000, 3100, 3200 MHz
  };

//"Table 69 additionally defines Portal Core Frequencies. The “Portal Core Frequencies in Table 70”
//column of Table 69 defines:
//• The core frequencies in Table 70 to which direct transitions can be made from the minimum core
//frequency in Table 69.
//• The core frequencies in Table 70 that support transitions to the minimum core frequency in
//Table 69."
//array index = "FID -8", so the 1st ("0") array index represents FID 8/1600 MHz
//static BYTE s_ar_byVCOfreqToLowerFID[] = {
static MinAndMaxFID s_ar_minandmaxfidVCO_FIDtoLowerFID[] = {
//  8,9,10 -> 0  //1600, 1700, 1800 MHz -> 800 MHz
    MinAndMaxFID(0,1), //1600 MHz -> 800, 900 MHz
  MinAndMaxFID(0,1), //1700 MHz -> 800, 900 MHz
  MinAndMaxFID(0,2), //1800 MHz -> 800, 900, 1000 MHz
  MinAndMaxFID(1,2), //1900 MHz -> 900, 1000 MHz
  MinAndMaxFID(1,3), //2000 MHz -> 900, 1000, 1100 MHz
  MinAndMaxFID(2,3), //2100 MHz -> 1000, 1100 MHz
  MinAndMaxFID(2,4), //2200 MHz -> 1000 - 1200 MHz
  MinAndMaxFID(3,4), //2300 MHz -> 1100 - 1200 MHz
  MinAndMaxFID(3,5), //2400 MHz -> 1100 - 1300 MHz
  MinAndMaxFID(4,5), //2500 MHz -> 1200 - 1300 MHz
  MinAndMaxFID(4,6), //2600 MHz -> 1200 - 1400 MHz
  MinAndMaxFID(5,6), //2700 MHz -> 1300 - 1400 MHz
  MinAndMaxFID(5,7), //2800 MHz -> 1300 - 1500 MHz
  MinAndMaxFID(6,7), //2900 MHz -> 1400 - 1500 MHz
  MinAndMaxFID(6,6), //3000 MHz -> 1400 MHz
  MinAndMaxFID(7,7), //3100 MHz -> 1500 MHz
  MinAndMaxFID(7,7), //3200 MHz -> 1500 MHz
  }; //17 entries

static BYTE g_s_stepping; //= GetStepping();
static float s_maxVoltageInVolt;

inline float GetMultiplier_AMD_NPT_family_0FH(BYTE frequencyID)
{
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin--FID:" << (WORD) frequencyID)
  //BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
  // Processors 32559 Rev. 3.16 November 2009
  // "Table 83. FID Code Translations" :
  // FID 0 -> multiplier = 4 = 4 + 0 = 4 + 0 * 0.5
  //FID 1 -> multiplier = 4.5 = FID + 4 * 0.5
  //FID 2 -> multiplier = 5 multi = FID + 4 * 0.5
  //FID 42 -> multiplier = 25 multi = FID + 4 * 0.5
  return 4 + (float) frequencyID * 0.5;
}

inline BYTE GetFrequencyID_AMD_NPT_family_0FH(float fMultiplier)
{
  BYTE frequencyID = 0;
  if( fMultiplier > 4.0f )
    //BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
    // Processors 32559 Rev. 3.16 November 2009
    // "Table 83. FID Code Translations" :

    // multiplier 4 -> FID 0  = 4 + 0 = 4 + 0 * 0.5
    //FID 1 -> multiplier = 4.5 = FID + 4 * 0.5
    //FID 2 -> multiplier = 5 multi = FID + 4 * 0.5
    //multiplier = 25 -> FID 42   FID = 25-4/0.5 = 21/0.5 = 42
    frequencyID = (BYTE) ( ( fMultiplier - 4.0f ) / 0.5f );
  return frequencyID;
}

/** see "BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
* Processors 32559 Rev. 3.16 November 2009"
* "Table 74. VID Code Voltages" :
* VID 0 -> 1.550 V = 1.550 V - VID * 0.25 V = 4 + 0 * 0.5
* VID 1 -> 1.525 V = 4.5 = FID + 4 * 0.5
* VID 01_1111b -> 0.775 V = 5 multi = FID + 4 * 0.5
* VID 31 -> 0.775 V = 1.550 V - VID(31) * 0.025 = 1.550 V - 0,775

* 10_0101b 0.7000 V
* 11_1111b (=63 dec) -> 0.3750 V */
inline float GetVoltageInVolt_AMD_NPT_family_0FH(BYTE voltageID)
{
  static float fVoltageInVolt;

  if( voltageID < 32) //voltage >= 0.775 V
  {
    //voltage_in_Volt = 1.550 V - VID * 0.025 V | VID <= 01_1111b
    fVoltageInVolt = (1.550f - (float) voltageID * 0.025);
    DEBUGN( /*FULL_FUNC_NAME <<*/ "voltage=1.550f-VID("
      << (WORD) voltageID << ")*0.025=" << fVoltageInVolt)
  }
  else
  {
    //voltage_in_Volt = 0.7625 V - (VID - 32) * 0.0125 V | VID > 01_1111b
    fVoltageInVolt = 0.7625f - (voltageID - 32) * 0.0125f;
    DEBUGN( /*FULL_FUNC_NAME <<*/ "voltage=0.7625-(VID(" << (WORD) voltageID
      << ")-32) * 0.0125=" << fVoltageInVolt)
  }
  return fVoltageInVolt;
}

inline BYTE GetVoltageID_AMD_NPT_family_0FH(float fVoltageInVoltToSet)
{
  //BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
  // Processors 32559 Rev. 3.16 November 2009
  //Table 74. VID Code Voltages
  // 1.550 V -> VID 0 = 1.550 V - VID * 0.25 V = 4 + 0 * 0.5
  //VID 1 -> 1.525 V = 4.5 = FID + 4 * 0.5
  //VID 01_1111b -> 0.775 V = 5 multi = FID + 4 * 0.5
  //VID 31 -> 0.775 V = 1.550 V - VID(31) * 0.025 = 1.550 V - 0,775

  // 10_0101b 0.7000 V
  // 11_1111b (=63 dec) -> 0.3750 V

  // voltage_in_Volt / 0.025 V = 1.550 V - VID | where voltage >= 0.775 V
  // voltage_in_Volt / 0.025 V - 1.550 V = - VID | where voltage >= 0.775 V
  // (voltage_in_Volt / 0.025 V - 1.550 V ) * -1 = VID | where voltage >= 0.775 V

#ifdef _DEBUG
  float fSubtrahent, fDivisor;
  if( fVoltageInVoltToSet >= 0.775f )
  {
    fSubtrahent = 1.550f;
    fDivisor = 0.025f;
  }
  else
  {
    fSubtrahent = 0.7625f;
    fDivisor = 0.0125f;
  }
  static float fMaxVoltageMinusVoltageInVoltToSet, fQuotient;
  fMaxVoltageMinusVoltageInVoltToSet = fSubtrahent -
    fVoltageInVoltToSet;
  fQuotient = fMaxVoltageMinusVoltageInVoltToSet / fDivisor;
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "difference:" <<
    fMaxVoltageMinusVoltageInVoltToSet << " Quotient:" << fQuotient)
#endif
  float fVoltageID = fVoltageInVoltToSet >= 0.775f ?
    //where VID <= 01_1111b
    //voltage_in_Volt = 1.550 V - VID * 0.025 V | -1.550 V
    //voltage_in_Volt - 1.550 V = - VID * 0.025 V | / 0.025 V
    //(voltage_in_Volt - 1.550 V ) /0.025 V = - VID / -1
    //-(voltage_in_Volt - 1.550 V ) /0.025 V = VID
    //(-voltage_in_Volt -(- 1.550 V ) ) / 0.025 V = VID
    //(-voltage_in_Volt + 1.550 V ) / 0.025 V = VID
    //( 1.550 V - voltage_in_Volt ) / 0.025 V = VID
    //(voltage_in_Volt - 1.550 V ) /-0.025 V = VID
    (1.550f - fVoltageInVoltToSet) / 0.025f :
    //voltage_in_Volt = 0.7625 V - (VID - 32) * 0.0125 V
    // voltage_in_Volt - 0.7625f = -(voltageID - 32) * 0.0125f | / 0.0125f
    //(voltage_in_Volt-0.7625f) / 0.0125f = -(voltageID - 32)
    //(voltage_in_Volt-0.7625f) / 0.0125f = -voltageID + 32 | -32
    //(voltage_in_Volt-0.7625f) / 0.0125f - 32 = -voltageID | / -1
    //-( (voltage_in_Volt-0.7625f) / 0.0125f - 32) = voltageID
    //-(voltage_in_Volt-0.7625f) / 0.0125f -(- 32) = voltageID
    //(-voltage_in_Volt-(-0.7625f)) / 0.0125f + 32 = voltageID
    //-voltage_in_Volt + 0.7625) / 0.0125 + 32 = voltageID
    //(-fVoltageInVoltToSet + 0.7625f) / 0.0125f + 32;
    (0.7625f - fVoltageInVoltToSet) / 0.0125f + 32;
  // fVoltageID may be < real result due to miscalculations.
  BYTE voltageID = //ceil(fVoltageID);
    (BYTE) fVoltageID;
  if( fVoltageID - voltageID > 0.5 )
  {
    DEBUGN( /*FULL_FUNC_NAME << "--"*/ "float voltage ID - integer voltage ID > 0.5")
    ++ voltageID;
  }
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "voltage ID for " << fVoltageInVoltToSet
    << " Volt=" << (WORD) voltageID)
  return voltageID;
}

inline BYTE GetCurrentVoltageIDAndFrequencyID_AMD_NPT_family_0Fh(
  BYTE & r_byCurrentVoltageID,
  BYTE & r_byCurrentFrequencyID
  )
{
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  static BYTE readMSRreturnValue;

  readMSRreturnValue = ReadMSR(
    FIDVID_STATUS_MSR_ADDRESS,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  r_byCurrentVoltageID = highmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
  r_byCurrentFrequencyID = lowmostMSRbits & BITMASK_FOR_LOWMOST_6BIT;
  return readMSRreturnValue;
}

inline BYTE GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(
  float * p_fVoltageInVolt,
  float * p_fMultiplier ,
  float * p_fReferenceClockInMHz ,
  WORD wCoreID,
  BYTE & r_byCurrentFrequencyID,
  BYTE & r_byCurrentVoltageID
  )
{
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin")
//  DEBUGN_LOGGER_NAME( g_windows_api_logger, /*FULL_FUNC_NAME << "--"*/ "begin")
  static BYTE readMSRreturnValue;

  readMSRreturnValue = GetCurrentVoltageIDAndFrequencyID_AMD_NPT_family_0Fh(
    r_byCurrentVoltageID, r_byCurrentFrequencyID);

  * p_fVoltageInVolt =  GetVoltageInVolt_AMD_NPT_family_0FH(
    r_byCurrentVoltageID);
  * p_fMultiplier = GetMultiplier_AMD_NPT_family_0FH( r_byCurrentFrequencyID);

  * p_fReferenceClockInMHz = 200.0f;
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "return " << (WORD) readMSRreturnValue
    << " ref.clock:" << * p_fReferenceClockInMHz
    << " multi: " << * p_fMultiplier
    << " " << * p_fVoltageInVolt << "V"
    )
//  pantheios::log(pantheios::notice, FULL_FUNC_NAME);
#if defined(COMPILE_WITH_LOG) && defined(USE_LOG4CPLUS)
  LOG4CPLUS_WARN(log4cplus_logger, LOG4CPLUS_TEXT( FULL_FUNC_NAME ));
#endif
  return readMSRreturnValue;
}

inline BYTE GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(
  float * p_fVoltageInVolt,
  float * p_fMultiplier ,
  float * p_fReferenceClockInMHz ,
  WORD wCoreID
  )
{
  static BYTE byCurrentFrequencyID;
  static BYTE byCurrentVoltageID;

  BYTE ret =
    GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(
      p_fVoltageInVolt,
      p_fMultiplier ,
      p_fReferenceClockInMHz ,
      wCoreID,
      byCurrentFrequencyID,
      byCurrentVoltageID
    );
//  GetCurrentVoltageIDAndFrequencyID_AMD_NPT_family_0Fh(
//    byCurrentVoltageID, byCurrentFrequencyID);

  return
    ret;
}

inline float * GetAvailableMultipliersAMD_NPT_family0F(
  uint16_t & r_wNumberOfArrayElements)
{
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin")
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  ReadMSR(
    FIDVID_STATUS_MSR_ADDRESS,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  BYTE maxFID = ( lowmostMSRbits >> MAX_FID_START_ADDRESS_IN_BIT ) &
    BITMASK_FOR_LOWMOST_6BIT;
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "maximum FID:" << (WORD) maxFID)
//  float maxMultiplier = GetMultiplier_AMD_NPT_family_0FH(maxFID);
  BYTE FIDstep = 1;
  if( g_s_stepping < 3 )
  {
#ifdef ALLOW_ODD_FIDS_FOR_STEPPING_NUMBER_BELOW_3
    r_wNumberOfArrayElements = maxFID + 1;
#else
    r_wNumberOfArrayElements = (maxFID + 2) / 2;
    FIDstep = 2;
//    r_wNumberOfArrayElements = maxFID + 1;
#endif
  }
  else
    r_wNumberOfArrayElements = maxFID + 1;
  DEBUGN( /*FULL_FUNC_NAME << */ "# array elements:" << r_wNumberOfArrayElements)
  float * multipliers = new float[ r_wNumberOfArrayElements];
  if( multipliers)
  {
    BYTE currentFID = maxFID + //1;
      FIDstep;
    BYTE arrayIndex;
    while( currentFID //--
       )
    {
      currentFID -= FIDstep;
      arrayIndex = currentFID / 2;
      multipliers[arrayIndex] = GetMultiplier_AMD_NPT_family_0FH(currentFID);
#ifndef ALLOW_ODD_FIDS_FOR_STEPPING_NUMBER_BELOW_3
//      multipliers[currentFID + 1] = multipliers[currentFID];
#endif
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "added multiplier " << multipliers[arrayIndex]
        << " for FID:" << (WORD) currentFID << " at array index "
        << (WORD) arrayIndex)
    }
  }
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "return " << multipliers)
  return multipliers;
}

inline float * GetAvailableVoltagesAMD_NPT_family0F(
  uint16_t & r_wNumberOfArrayElements)
{
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin")
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  ReadMSR(
    FIDVID_STATUS_MSR_ADDRESS,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  const BYTE maximumVoltageVID = ( highmostMSRbits >>
    MAX_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
    BITMASK_FOR_LOWMOST_6BIT;
  s_maxVoltageInVolt = GetVoltageInVolt_AMD_NPT_family_0FH(maximumVoltageVID);
#ifdef _DEBUG
  const BYTE MaxRampVID = ( lowmostMSRbits >> MAX_RAMP_VID_START_ADDRESS_IN_BIT
    ) & BITMASK_FOR_LOWMOST_6BIT;
#endif
  DEBUGN(/*FULL_FUNC_NAME << "--"*/ "MaxRampVID:" << (WORD) MaxRampVID << "^=" <<
    GetVoltageInVolt_AMD_NPT_family_0FH(MaxRampVID) << " Volt")

//  BYTE startVID = ( highmostMSRbits >>
//    START_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
//    BITMASK_FOR_LOWMOST_6BIT;
  DEBUGN(/*FULL_FUNC_NAME << "--"*/ "highmost bits:"
    << getBinaryRepresentation(highmostMSRbits) )
  DEBUGN(/*FULL_FUNC_NAME << "--"*/ "VID for maximum voltage:" << (WORD) maximumVoltageVID)
//  DEBUGN(/*FULL_FUNC_NAME << "--"*/ "StartVID:" << (WORD) startVID)
//  BYTE maximumVID = maximumVoltageVID > startVID ? maximumVoltageVID : startVID;
//  float maxVoltage = GetVoltageInVolt_AMD_NPT_family_0FH(maximumVoltageVID);

  const BYTE VIDforLowestVoltage = GetVoltageID_AMD_NPT_family_0FH(0.6f);
  r_wNumberOfArrayElements = //MAXIMUM_VOLTAGE_ID
    //32
    VIDforLowestVoltage
    - maximumVoltageVID + 1;
  DEBUGN(/*FULL_FUNC_NAME << "--"*/ "# array elements:" << r_wNumberOfArrayElements)
  float * voltages = new float[r_wNumberOfArrayElements];
  if(voltages)
  {
    BYTE currentVID = //MAXIMUM_VOLTAGE_ID;
      maximumVoltageVID;
    BYTE arrayIndexForCurrentVoltage = 0;
//    while( //currentVID >= maximumVoltageVID
//        currentVID <= maximumVoltageVID + r_wNumberOfArrayElements )
    for( ; arrayIndexForCurrentVoltage < r_wNumberOfArrayElements;
        ++ arrayIndexForCurrentVoltage)
    {
      voltages[arrayIndexForCurrentVoltage] =
        GetVoltageInVolt_AMD_NPT_family_0FH(currentVID);
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "added #"
        << (WORD) arrayIndexForCurrentVoltage
        << " : "
        << voltages[arrayIndexForCurrentVoltage]
        << " Volt for VID:" << (WORD) currentVID)
//      -- currentVID;
      ++ currentVID;
//      ++ arrayIndexForCurrentVoltage;
    }
  }
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "return " << voltages)
  return voltages;
}

inline BYTE GetStepping()
{
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin")
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
    DEBUGN(/*FULL_FUNC_NAME << "--"*/ "stepping via CPUID:" << (WORD) byStepping )
    return byStepping;
  }
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "return 0" )
  return 0;
}

void GetAdvancedPowerManagementInformationViaCPUID()
{
  DWORD dwEAX, dwEBX, dwECX , dwEDX ;
  bool bRet = CPUID(
    //CPUID Fn8000_0007 Advanced Power Management Information
    0x80000007
    , & dwEAX
    , & dwEBX
    , & dwECX
    , & dwEDX
    , 1
    ) ;
  if( bRet )
  {
    //EDX 6 100MhzSteps
    DEBUGN( "100MhzSteps ?:" << ((dwEDX >> 6 ) & 1) )
    //EDX 2 VID: Voltage ID control is supported.
    DEBUGN( "Voltage ID control is supported ?:" << ((dwEDX >> 2 ) & 1) )
    //EDX 1 FID: Frequency ID control is supported.
    DEBUGN( "Frequency ID control is supported ?:" << ((dwEDX >> 1 ) & 1) )
    //EDX 0 TS: Temperature sensor = 1.
    DEBUGN( "Temperature sensor ?:" << (dwEDX & 1) )

    //http://en.wikipedia.org/wiki/CPUID#EAX.3D1:_Processor_Info_and_Feature_Bits:
    //"The format of the information in EAX is as follows:"
    //"3:0 - Stepping"
#ifdef _DEBUG
    BYTE byStepping = dwEAX & BITMASK_FOR_LOWMOST_4BIT ;
#endif
    DEBUGN(/*FULL_FUNC_NAME << "--"*/ "stepping via CPUID:" << (WORD) byStepping )
//    return byStepping;
  }
}

float GetTemperatureAccordingToStepping(BYTE stepping, DWORD dwValue)
{
//  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin")
  float fTempInDegCelsius;
  if( //revision == 'F'
      stepping == 2 )
  {
    // "01h = -48C"
    // "ffh = 206C"
    fTempInDegCelsius = (float) ( ( dwValue >>
      //page 190: "Revision F encodings bits 23-16 (ignore bits 15-14)"
      16 ) & BITMASK_FOR_LOWMOST_8BIT )
      // "00h = -49C"
      - 49.0f ;
  }
//  if( //revision == 'G' )
  else
  {
    fTempInDegCelsius = (float) ( ( dwValue >>
      //page 190: "Revision G encodings bits 23-14"
      14 ) & BITMASK_FOR_LOWMOST_10BIT )
      // "000h = -49.00C"
      // "0C4h = 0.00C"  0C4h*0.25 - 49 = 196*0.25 - 49 = 49-49 =0
      // "3ffh = 206.75C"
      * 0.25f - 49.0f
      ;
    // "1.4 Register Differences in Revisions of AMD NPT Family 0Fh Processors"
  }
//  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "return " << fTempInDegCelsius)
  return fTempInDegCelsius;
}

inline float GetTemperatureInDegCelsiusAMD_NPT_family0F()
{
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin")
//  DEBUGN_LOGGER_NAME( g_windows_api_logger, /*FULL_FUNC_NAME << "--"*/ "begin")
  DWORD dwValue ;
  ( * g_pfnReadPCIconfigSpace) (
    CONFIGURATION_ACCESS_BUS_NUMBER, //8-bit PCI bus number
    //Bus 0, Device number 24, Function 3 is "CPU Miscellaneous Control"
//      g_byValue1
    //Bits 0- 2 : Function Number
    //Bits 3- 7 : Device Number
    //"5-bit device, and 3-bit function"
    CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER
    ,//) ((Bus&0xFF)<<8) | ((Dev&0x1F)<<3) | (Func&7)
    CPU_TEMPERATURE_OFFSET , // Register Address
    & dwValue //      PDWORD p_dwValue ) ;
    ) ;

//  static BYTE stepping = GetStepping();

  float fTempInDegCelsius = GetTemperatureAccordingToStepping(//stepping,
    g_s_stepping, dwValue);
  DEBUGN( /*FULL_FUNC_NAME << "--"*/ "return")
  return fTempInDegCelsius;
}

#endif /* AMD_NPT_FAMILY_0FH_HPP_ */
