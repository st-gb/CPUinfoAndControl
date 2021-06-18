/** Core_and_Core2_GetVoltage.hpp
 *  Created on: 24.12.2012
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc.*/

#ifndef CORE_AND_CORE2_GETVOLTAGE_HPP_
#define CORE_AND_CORE2_GETVOLTAGE_HPP_

#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

namespace Intel{ namespace CoreAndCore2{

/** @brief voltage in Volt calculated from voltage ID.
 *    Works for Intel Core, Core2, Atom
 *  @param voltageID: use unsigned data type because fast because same bit
 *   width as CPU architecture. */
inline float GetVoltage(fastestUnsignedDataType voltageID)
{
  /** see "Intel® 64 and IA-32 Architectures Software Developer's Manual
   *  Volume 4: Model-Specific Registers,
   *  "MSRS IN THE INTEL® CORE™ 2 PROCESSOR FAMILY",
      Register Name "MSR_PERF_STATUS" */ 
  /**Diff 1.0875 (VID 30) & 0.925 (VID=17) = 0.1625V
   * 1 voltage ID step = 0.1625V / (30-17) = 0.1625V / 13 = 0.0125
   * 0.925- 17 * 0.0125 = 0.7125 */
  return 
#ifdef COMPILE_FOR_INTEL_CORE2
    /** For Core 2 Quad Q600 G0 stepping it seems to be 0.8V */
    0.8f
#else
    0.7125f
#endif
    + (float) voltageID *
    /** Document Number: 315592-005Intel® Core™2 Extreme Quad-Core Processor
     * QX6000Δ Sequence and Intel® Core™2 Quad Processor Q6000Δ Sequence
     * (Document Number: 315592-005) : 
     * "Table 2.Voltage Identification Definition"
     * -0.8500 V,  0.8625 V -> 0.0125 V voltage ID step
     * -table starts at 0.8500 V 
     * -all bits set = 0.825 V */
    0.0125f ;
}

}}///end namespaces

#endif /* CORE_AND_CORE2_GETVOLTAGE_HPP_ */
