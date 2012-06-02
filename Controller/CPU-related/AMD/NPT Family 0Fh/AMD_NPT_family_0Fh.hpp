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

//From "BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh Processors",
// 32559 Rev. 3.16 November 2009 :

//190 Memory System Configuration Chapter 4: Current Temperature (CurTmp)—Bits 23-14.

//10.5 Processor Performance States:
//MSR C001_0041h (FIDVID_CTL) and MSR C001_0042h (FIDVID_STATUS) are used for
//initiating P-state transitions and verifying that they are complete.

// "EventSelect 07Fh [L2 Fill/Writeback]"
#define EVENT_SELECT 07Fh //[L2 Fill/Writeback],
// "CPUID Fn8000_0008 Address Size And Physical Core Count Information"

#define FIDVID_STATUS_REGISTER_MSR 0xC0010042
// "61 IntPstateSup Intermediate P-state support"
#define INTERMEDIATE_P_STATE_SUPPORT_BIT_ADDRESS 61
// "56 PstateStep P-state Voltage Step"
// "37–32 CurrVID Current VID"
#define CURRENT_VID_START_ADDRESS_IN_BIT 32
// 31 FidVidPending FID/VID Change Pending
// "5–0 CurrFID Current FID"
#define CURRENT_FID_START_BIT 0
#define CURRENT_FID_END_BIT 5
//21–16 MaxFID Max FID
#define MAX_FID_START_ADDRESS_IN_BIT 16

//53–48 MaxVID Max VID
#define MAX_VID_START_ADDRESS_IN_BIT 48
#define MAX_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES 16
#define START_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES 8

//Thermtrip Status Register Function 3: Offset E4h
#define CPU_TEMPERATURE_OFFSET 0xE4

//#define CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER 3
//Device number 24, Function 3 is "CPU Miscellaneous Control"
// ( 24 << 3 ) | 3 = 192 | 3 = 192 + 3 = 195
#define CPU_TEMPERATURE_DEVICE_AND_FUNCTION_NUMBER 195

//"23-14 CurTmp Current Temperature"
// 4.6 Function 3—Miscellaneous Control
#define MISCELLANEOUS_CONTROL_FUNCTION_NUMBER 3
//4.1 Configuration Space Accesses : "[...]Configuration accesses to Bus 0[...]"
#define CONFIGURATION_ACCESS_BUS_NUMBER 0
//see Table 74. VID Code Voltages
#define MAXIMUM_VOLTAGE_ID 63

//TODO
//10.6.2.1.1 Voltage Stabilization Time
//10.6.2.1.3 PLL Lock Time
//10.5.7.2.1 Changing the VID
//10.5.7.2.2 Changing the FID
//P-State Voltage Step Size (PstateStep)—Bit 56.
//Voltage Regulator Stabilization Time (VSTime)
//10.6.2.1.5 Isochronous Relief Time


extern ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace;

inline float GetMultiplier(BYTE frequencyID)
{
  //BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
  // Processors 32559 Rev. 3.16 November 2009
  // "Table 83. FID Code Translations" :
  // FID 0 -> muiltiplier = 4 = 4 + 0 = 4 + 0 * 0.5
  //FID 1 -> muiltiplier = 4.5 = FID + 4 * 0.5
  //FID 2 -> muiltiplier = 5 multi = FID + 4 * 0.5
  //FID 42 -> muiltiplier = 25 multi = FID + 4 * 0.5
  return 4 + (float) frequencyID * 0.5;
}

inline float GetVoltageInVolt(BYTE voltageID)
{
  //BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh
  // Processors 32559 Rev. 3.16 November 2009
  //Table 74. VID Code Voltages
  // VID 0 -> 1.550 V = 1.550 V - VID * 0.25 V = 4 + 0 * 0.5
  //VID 1 -> 1.525 V = 4.5 = FID + 4 * 0.5
  //VID 01_1111b -> 0.775 V = 5 multi = FID + 4 * 0.5
  //VID 31 -> 0.775 V = 1.550 V - VID(31) * 0.025 = 1.550 V - 0,775

  // 10_0101b 0.7000 V
  // 11_1111b (=63 dec) -> 0.3750 V

  //voltage_in_Volt = 1.550 V - VID * 0.025 V | VID <= 01_1111b
  return voltageID < 32 ?
    (1.550f - (float) voltageID * 0.025) :
    //voltage_in_Volt = 0.7625 V - (VID - 32) * 0.0125 V | VID > 01_1111b
    0.7625f - (voltageID - 32) * 0.0125f ;
}

inline BYTE GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(
  float * p_fVoltageInVolt,
  float * p_fMultiplier ,
  float * p_fReferenceClockInMHz ,
  WORD wCoreID
  )
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  static BYTE readMSRreturnValue;

  readMSRreturnValue = ReadMSR(
    FIDVID_STATUS_REGISTER_MSR,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  * p_fVoltageInVolt =  GetVoltageInVolt(highmostMSRbits &
    BITMASK_FOR_LOWMOST_6BIT);
  * p_fMultiplier = GetMultiplier( lowmostMSRbits & BITMASK_FOR_LOWMOST_6BIT);
  * p_fReferenceClockInMHz = 200.0f;
  DEBUGN( FULL_FUNC_NAME << "--return " << (WORD) readMSRreturnValue)
  return readMSRreturnValue;
}

inline float * GetAvailableMultipliersAMD_NPT_family0F(
  uint16_t & r_wNumberOfArrayElements)
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  ReadMSR(
    FIDVID_STATUS_REGISTER_MSR,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  BYTE maxFID = ( lowmostMSRbits >> MAX_FID_START_ADDRESS_IN_BIT ) &
    BITMASK_FOR_LOWMOST_6BIT;
  DEBUGN( FULL_FUNC_NAME << "--maximum FID:" << (WORD) maxFID)
//  float maxMultiplier = GetMultiplier(maxFID);
  r_wNumberOfArrayElements = maxFID + 1;
  float * multipliers = new float[ r_wNumberOfArrayElements];
  if( multipliers)
  {
    BYTE currentFID = maxFID + 1;
    while( currentFID --)
    {
      multipliers[currentFID] = GetMultiplier(currentFID);
      DEBUGN( FULL_FUNC_NAME << "--added multiplier " << multipliers[currentFID]
        << " for FID:" << (WORD) currentFID)
    }
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << multipliers)
  return multipliers;
}

inline float * GetAvailableVoltagesAMD_NPT_family0F(
  uint16_t & r_wNumberOfArrayElements)
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
  static uint32_t lowmostMSRbits;
  static uint32_t highmostMSRbits;
  ReadMSR(
    FIDVID_STATUS_REGISTER_MSR,
    & lowmostMSRbits,
    & highmostMSRbits,
    1
    );
  BYTE maximumVoltageVID = ( highmostMSRbits >>
    MAX_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
    BITMASK_FOR_LOWMOST_6BIT;
//  BYTE startVID = ( highmostMSRbits >>
//    START_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
//    BITMASK_FOR_LOWMOST_6BIT;
  DEBUGN(FULL_FUNC_NAME << "--highmost bits:"
    << getBinaryRepresentation(highmostMSRbits) )
  DEBUGN(FULL_FUNC_NAME << "--VID for maximum voltage:" << (WORD) maximumVoltageVID)
//  DEBUGN(FULL_FUNC_NAME << "--StartVID:" << (WORD) startVID)
//  BYTE maximumVID = maximumVoltageVID > startVID ? maximumVoltageVID : startVID;
//  float maxVoltage = GetVoltageInVolt(maximumVoltageVID);

  r_wNumberOfArrayElements = //MAXIMUM_VOLTAGE_ID
    32 - maximumVoltageVID + 1;
  DEBUGN(FULL_FUNC_NAME << "--# array elements:" << r_wNumberOfArrayElements)
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
      voltages[arrayIndexForCurrentVoltage] = GetVoltageInVolt(currentVID);
      DEBUGN( FULL_FUNC_NAME << "--added #"
        << (WORD) arrayIndexForCurrentVoltage
        << " : "
        << voltages[arrayIndexForCurrentVoltage]
        << " Volt for VID:" << (WORD) currentVID)
//      -- currentVID;
      ++ currentVID;
//      ++ arrayIndexForCurrentVoltage;
    }
  }
  DEBUGN( FULL_FUNC_NAME << "--return " << voltages)
  return voltages;
}

inline BYTE GetStepping()
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
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
    DEBUGN(FULL_FUNC_NAME << "--stepping via CPUID:" << (WORD) byStepping )
    return byStepping;
  }
  DEBUGN( FULL_FUNC_NAME << "--return 0" )
  return 0;
}

float GetTemperatureAccordingToStepping(BYTE stepping, DWORD dwValue)
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
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
  DEBUGN( FULL_FUNC_NAME << "--return " << fTempInDegCelsius)
  return fTempInDegCelsius;
}

inline float GetTemperatureInDegCelsiusAMD_NPT_family0F()
{
  DEBUGN( FULL_FUNC_NAME << "--begin")
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

  static BYTE stepping = GetStepping();

  float fTempInDegCelsius = GetTemperatureAccordingToStepping(stepping, dwValue);
  DEBUGN( FULL_FUNC_NAME << "--return")
  return fTempInDegCelsius;
}

#endif /* AMD_NPT_FAMILY_0FH_HPP_ */
