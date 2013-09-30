/*
 * from_K10.h
 *
 *  Created on: Aug 28, 2013
 *      Author: Stefan
 */

#ifndef FROM_K10_H_
#define FROM_K10_H_

#include <fastest_data_type.h> //

  //Use prefix "COFVID_STATUS_REGISTER" to be eindeutig (because there may
  //also be a field with the same meaning in e.g. another register, but at
  //another start address)
  #define COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VOLTAGE 42
  #define COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VID \
      COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VOLTAGE
  #define COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VOLTAGE 35
  #define COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID \
      COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VOLTAGE

  //"MSRC001_0070 COFVID Control Register"
  #define COFVID_CONTROL_REGISTER_MSR_ADDRESS 0xC0010070
  /** see "31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG"
   * "MSRC001_0071 COFVID Status Register" */
  #define COFVID_STATUS_REGISTER_MSR_ADDRESS 0xC0010071

namespace AMD
{
  /** function for AMD CPUs beginning with K10 (including 15hex. */
  namespace fromK10
  {
    inline void GetDivisorID(DWORD dwMSRlowmost, BYTE & byDivisorID)
    {
      //Divisor ID. "8:6 CurCpuDid: current core divisor ID. Read-only."
      byDivisorID = //(BYTE)(
      //      (g_dwLowmostBits & 448//=111000000bin
      //      ) >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) ; //<=>bits 6-8 shifted to     }
        ( dwMSRlowmost >> START_BIT_FOR_CPU_CORE_DIVISOR_ID ) &
        BITMASK_FOR_LOWMOST_3BIT ;
    }

    inline fastestUnsignedDataType GetFrequencyID(const DWORD dwMSRlowmost)
    {
      //Frequency ID. "5:0 CurCpuFid: current core frequency ID. Read-only."
      return ( dwMSRlowmost & BITMASK_FOR_LOWMOST_6BIT ) ;
    }

    inline fastestUnsignedDataType GetMaximumVoltageID()
    {
    //    return 64 ;
      uint32_t dwEAXlowMostBits, dwEDXhighMostBits ;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & dwEAXlowMostBits,
        & dwEDXhighMostBits,
        1 ) ;
      fastestUnsignedDataType highestVID = ( dwEDXhighMostBits >>
          ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VID - 32 )
          ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("highest VID:" << highestVID)
      return highestVID ;
    }

    inline fastestUnsignedDataType GetMinimumVoltageID()
    {
    //    return 36 ;
      uint32_t dwEAXlowMostBits, dwEDXhighMostBits ;
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & dwEAXlowMostBits,
        & dwEDXhighMostBits,
        1
        ) ;
      const fastestUnsignedDataType lowestVID = ( dwEDXhighMostBits >>
          ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
          ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("lowest VID:" << lowestVID)
      return lowestVID ;
    }

    inline void GetMinAndMaxVoltageID(
      fastestUnsignedDataType & maxVID,
      fastestUnsignedDataType & minVID)
    {
      ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & g_dwMSRlowmost,
        & g_dwMSRhighmost,
        1 ) ;
      maxVID = ( g_dwMSRhighmost >>
        //"48:42 [...]: minimum voltage. Read-only. Specifies the VID code
        //corresponding to the minimum voltage (highest VID code) that the processor
        //drives. 00h indicates that no minimum VID code is specified.
        //See section 2.4.1 [Processor Power Planes And Voltage Control]."
        ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VID - 32 )
        ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("highest VID:" << maxVID )
      minVID = ( g_dwMSRhighmost >>
      //"41:35 [...]: maximum voltage. Read-only. Specifies the VID code
      //corresponding to the maximum voltage (lowest VID code) that the processor
      //drives. 00h indicates that no maximum VID code is specified.
      //See section 2.4.1 [Processor Power Planes And Voltage Control].
        ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
        ) & BITMASK_FOR_LOWMOST_7BIT;
      DEBUGN("lowest VID:" << minVID )
    }

    inline void GetVoltageIDfromCOFVIDstatusRegisterBits(DWORD dwMSRlowmost,
      BYTE & byVoltageID)
    {
      //voltage ID "15:9 CurCpuVid: current core VID. Read-only."
      byVoltageID = //(BYTE) (
      //      (g_dwLowmostBits & BITMASK_FOR_CPU_CORE_VOLTAGE_ID//=1111111000000000bin
      //    ) >> 9 ) ; //<=>bits 9-15 shifted   }
        ( dwMSRlowmost >> 9 ) & BITMASK_FOR_LOWMOST_7BIT ;
    }
  }
}

#endif /* FROM_K10_H_ */
