/*
 * Core_and_Core2_GetVoltage.hpp
 *
 *  Created on: 24.12.2012
 *      Author: Stefan
 */

#ifndef CORE_AND_CORE2_GETVOLTAGE_HPP_
#define CORE_AND_CORE2_GETVOLTAGE_HPP_

namespace Intel
{
  namespace CoreAndCore2
  {
    inline float GetVoltage(BYTE byValue)
    {
      //Diff 1.0875 (VID 30) & 0.925 (VID=17) = 0.1625V
      //1 voltage ID step = 0.1625V / (30-17) = 0.1625V / 13 = 0.0125
      //0.925- 17 * 0.0125 = 0.7125 ;
      return 0.7125 + byValue * 0.0125 ;
    }
  }
}

#endif /* CORE_AND_CORE2_GETVOLTAGE_HPP_ */
