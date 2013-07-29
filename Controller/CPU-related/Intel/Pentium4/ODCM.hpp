/*
 * ODCM.hpp
 *
 *  Created on: 05.02.2013
 *      Author: Stefan
 */

#ifndef ODCM_HPP_
#define ODCM_HPP_

#include <Controller/AssignPointersToExportedExeFunctions/\
inline_register_access_functions.hpp> //ReadMSR(...), WriteMSR(...)
#include <preprocessor_macros/bitmasks.h> //BITMASK_FOR_LOWMOST_3BIT

#define IA32_CLOCK_MODULATION_REG_ADDR 0x19A
#define ON_DEMAND_CLOCK_MODULATION_DUTY_CYCLE_START_BIT 1
#define ON_DEMAND_CLOCK_MODULATION_DUTY_CYCLE_ENABLE_START_BIT 4
#define ENABLE_ON_DEMAND_CLOCK_MODULATION_BIT_MASK 16
#define NUMBER_OF_ODCM_VALUES 8

/** @param ar_f must have at least 8 elements
 *  @param baseMultiplier: e.g. "7" -> [0.875...6.125,7.0]*/
inline void AddODCMdutyCyclesAsMultipliers(float ar_f[], float baseMultiplier)
{
  unsigned num = 0;
  const float OneEighthOfBaseMulti = 0.125f * baseMultiplier;
  do
  {
    ar_f[num] = (float) (num+1) * OneEighthOfBaseMulti; //e.g. 23/ 8 = 3,3525
  }
  while( num ++ < 7);
}

/** @param coreID: unsigned-> same bit size as CPU arch. -> fast*/
inline BYTE DisableODCM(unsigned coreID)
{
  return WriteMSR(IA32_CLOCK_MODULATION_REG_ADDR, 0, 0, 1 << coreID);
}

inline float GetODCMdutyCyclePercentage(WORD wCoreID)
{
  static uint32_t s_ui32_t1, s_ui32_t2;
  ReadMSR(IA32_CLOCK_MODULATION_REG_ADDR, & s_ui32_t1, & s_ui32_t2, 1 << wCoreID);
  s_ui32_t2 = ( (s_ui32_t1 >> ON_DEMAND_CLOCK_MODULATION_DUTY_CYCLE_START_BIT)
    & BITMASK_FOR_LOWMOST_3BIT);
  DEBUGN("On-Demand Clock Modulation Duty Cycle:" << s_ui32_t2)
  s_ui32_t1 = (s_ui32_t1 >>
    ON_DEMAND_CLOCK_MODULATION_DUTY_CYCLE_ENABLE_START_BIT) & 1;
  DEBUGN("On-Demand Clock Modulation Enable:" << s_ui32_t1)
  //from Intel vol 3.A
  //"Table 14-1.  On-Demand Clock Modulation Duty Cycle Field Encoding"
  //     Duty Cycle|throttled
  //7 ^= 87,5%     |12,5%
  //6 ^= 75%       |25,0%
  //5 ^= 62,5%     |37,5%
  //4 ^= 50%       |50%
  //3 ^= 37,5%     |62,5%
  //2 ^= 25%       |75%
  //1 ^= 12,5%     |87,5%
  return s_ui32_t1 != 0 ? (float) s_ui32_t2 /*[0...7] */ / 8.0f
    : 1.0f;
}

/** @param ODCMdutyCycle: [0.0...1.0] 1.0:not throttled at all. */
inline BYTE SetODCMdutyCycle(const float ODCMdutyCycle, unsigned coreID)
{
  static uint32_t ui32Value1;
  unsigned num1EightDutyCycles = ODCMdutyCycle / 0.125f;
  ui32Value1 = 0;
  //ui32Value1 |= (8 << 1) <=> ui32Value1 |= ENABLE_ON_DEMAND_CLOCK_MODULATION_BIT_MASK
  if( num1EightDutyCycles < 8 )
  {
    //bits "3:1" "On-Demand Clock Modulation Duty Cycle"
    ui32Value1 |= ( (BYTE) num1EightDutyCycles << 1);
    //see "Table 14-1.  On-Demand Clock Modulation Duty Cycle Field Encoding":
    // 0 = "Reserved" value
    if( ( (ui32Value1 >> 1 ) & BITMASK_FOR_LOWMOST_3BIT ) == 0)
      ui32Value1 = 1;
    ui32Value1 |= ENABLE_ON_DEMAND_CLOCK_MODULATION_BIT_MASK;
  }
  //    g_ui32Value2 = 0;
  return WriteMSR(IA32_CLOCK_MODULATION_REG_ADDR, ui32Value1, 0, 1 << coreID);
}

#endif /* ODCM_HPP_ */
