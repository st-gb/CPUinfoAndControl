/*
 * PentiumM_GetVoltage.hpp
 *
 *  Created on: 24.12.2012
 *      Author: Stefan
 */

#ifndef PENTIUMM_GETVOLTAGE_HPP_
#define PENTIUMM_GETVOLTAGE_HPP_

namespace Intel
{
  namespace Pentium_M
  {
    inline float GetVoltage(BYTE byVoltageID)
    {
      //Pentium M datasheet: Table 3-1: 0.016 Volt diff = 1 VID step
      //Examined following values :
      //Voltage ID 0 = 0.7V 1=0.716 V,...40=1.340 Volt
      return 0.7 + 0.016 * byVoltageID ;
    }
  }
}

#endif /* PENTIUMM_GETVOLTAGE_HPP_ */
