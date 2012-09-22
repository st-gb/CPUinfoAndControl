/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH ("Trilobyte SE"), Berlin,
 * Germany 2010-at least 2012.
 * You are allowed to modify and use the source code from Trilobyte SE for free
 * if you are not making profit directly or indirectly with it or its adaption.
 * Else you may contact Trilobyte SE.
 */
/*
 * CPUcoreData_AddDefaultVoltageForFreq.cpp
 *
 *  Created on: 11.05.2012
 *      Author: Stefan
 */

#include "CPUcoreData.hpp"

/** Needed by CPU controller dynlibs to insert default voltages */
bool CPUcoreData::AddDefaultVoltageForFreq(float fValue,WORD wFreqInMHz)
{
  return AddDefaultVoltageForFreq_inline(fValue, wFreqInMHz);
}
