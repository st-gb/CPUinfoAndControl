/*
 * AMD_K7_GetAvailableVoltages.hpp
 *
 *  Created on: Sep 14, 2013
 *      Author: stefan
 */

#ifndef AMD_K7_GETAVAILABLEVOLTAGES_HPP_
#define AMD_K7_GETAVAILABLEVOLTAGES_HPP_

#include "AMD_K7.hpp"

namespace AMD
{
  namespace K7
  {
    inline float * CalculateAvailableVoltages(
        uint16_t & r_wNumberOfArrayElements)
    {
      static uint32_t lowmostMSRbits;
      static uint32_t highmostMSRbits;
      ReadMSR(
        FIDVID_STATUS_MSR_ADDRESS,
        & lowmostMSRbits,
        & highmostMSRbits,
        1
        );
      const fastestUnsignedDataType maximumVoltageVID = ( highmostMSRbits >>
        MAX_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
        BITMASK_FOR_LOWMOST_6BIT;
      const fastestUnsignedDataType VIDforLowestVoltage = AMD::K7::GetVoltageID(
        AMD_K7_MOBILE_MIN_VOLTAGE_IN_VOLT);
      r_wNumberOfArrayElements = //MAXIMUM_VOLTAGE_ID
        //32
        VIDforLowestVoltage
        - maximumVoltageVID + 1;
      DEBUGN( "# array elements:" << r_wNumberOfArrayElements)
      float * voltages = new float[r_wNumberOfArrayElements];
      if(voltages)
      {
        fastestUnsignedDataType currentVID = //MAXIMUM_VOLTAGE_ID;
          maximumVoltageVID;
        fastestUnsignedDataType arrayIndexForCurrentVoltage = 0;
    //    while( //currentVID >= maximumVoltageVID
    //        currentVID <= maximumVoltageVID + r_wNumberOfArrayElements )
        for( ; arrayIndexForCurrentVoltage < r_wNumberOfArrayElements;
            ++ arrayIndexForCurrentVoltage)
        {
          voltages[arrayIndexForCurrentVoltage] =
            AMD::K7::GetVoltageInVolt(currentVID);
          DEBUGN( "added #" << arrayIndexForCurrentVoltage
            << " : "
            << voltages[arrayIndexForCurrentVoltage]
            << " Volt for VID:" << currentVID)
    //      -- currentVID;
          ++ currentVID;
    //      ++ arrayIndexForCurrentVoltage;
        }
      }
      return voltages;
    }

    inline float * GetAvailableVoltages(
      uint16_t & r_wNumberOfArrayElements)
    {
      DEBUGN( "begin")
    //  s_maxVoltageInVolt = GetVoltageInVolt_AMD_NPT_family_0FH(maximumVoltageVID);
    //#ifdef _DEBUG
    //  const BYTE MaxRampVID = ( lowmostMSRbits >> MAX_RAMP_VID_START_ADDRESS_IN_BIT
    //    ) & BITMASK_FOR_LOWMOST_6BIT;
    //#endif
    //  DEBUGN(/*FULL_FUNC_NAME << "--"*/ "MaxRampVID:" << (WORD) MaxRampVID << "^=" <<
    //    GetVoltageInVolt_AMD_K7(MaxRampVID) << " Volt")

    //  BYTE startVID = ( highmostMSRbits >>
    //    START_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES ) &
    //    BITMASK_FOR_LOWMOST_6BIT;
      DEBUGN("highmost bits:" << getBinaryRepresentation(highmostMSRbits) )
      DEBUGN("VID for maximum voltage:" << maximumVoltageVID)
    //  DEBUGN(/*FULL_FUNC_NAME << "--"*/ "StartVID:" << (WORD) startVID)
    //  BYTE maximumVID = maximumVoltageVID > startVID ? maximumVoltageVID : startVID;
    //  float maxVoltage = GetVoltageInVolt_AMD_NPT_family_0FH(maximumVoltageVID);

      float * voltages = CalculateAvailableVoltages(r_wNumberOfArrayElements);
      DEBUGN( "return " << voltages)
      return voltages;
    }
  } /** namespace K7 */
} /** namespace AMD */


#endif /* AMD_K7_GETAVAILABLEVOLTAGES_HPP_ */
