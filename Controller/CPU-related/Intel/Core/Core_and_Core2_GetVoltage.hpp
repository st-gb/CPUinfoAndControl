/*
 * Core_and_Core2_GetVoltage.hpp
 *
 *  Created on: 24.12.2012
 *      Author: Stefan
 */

#ifndef CORE_AND_CORE2_GETVOLTAGE_HPP_
#define CORE_AND_CORE2_GETVOLTAGE_HPP_

#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

namespace Intel
{
  namespace CoreAndCore2
  {
    /** @brief voltage in Volt calc. from voltage ID.
     *    Works for Intel Core, Core2, Atom
     * @param voltageID: use unsigned data type because fast because same bit
     *  width as CPU architecture.*/
    inline float GetVoltage(fastestUnsignedDataType voltageID)
    {
      //Diff 1.0875 (VID 30) & 0.925 (VID=17) = 0.1625V
      //1 voltage ID step = 0.1625V / (30-17) = 0.1625V / 13 = 0.0125
      //0.925- 17 * 0.0125 = 0.7125 ;
      return 
#ifdef COMPILE_FOR_INTEL_CORE2
        /** For Core 2 Quad Q600 G0 stepping it seems to be 0.8V */
        0.8f
#else
        0.7125f
#endif
       + (float) voltageID * 0.0125f ;
    }
  }
}

#endif /* CORE_AND_CORE2_GETVOLTAGE_HPP_ */
