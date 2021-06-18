/** PentiumM_GetVoltage.hpp
 *  Created on: 24.12.2012
 *  Author: Stefan Gebauer, M.Sc. Comp.Sc.*/

#ifndef PENTIUMM_GETVOLTAGE_HPP_
#define PENTIUMM_GETVOLTAGE_HPP_

namespace Intel{ namespace Pentium_M{

inline float GetVoltage(BYTE byVoltageID)
{
  /** "Intel® Pentium® M Processor on 90 nm Process with 2-MB L2 Cache"
   * Datasheet January 2006 (Document Number: 302189-008):
   * Table 3-1.  Voltage Identification Definition:
   * -0.016 Volt diff = 1 Voltage ID step
   * -lowest voltage is 0.7 V
   * Examined following values :
   * Voltage ID 0 = 0.7V 1=0.716 V,...40=1.340 Volt */
  return 0.7 + 0.016 * byVoltageID ;
}

}}///end namespaces

#endif /* PENTIUMM_GETVOLTAGE_HPP_ */
