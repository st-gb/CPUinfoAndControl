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

bool CPUcoreData::AddDefaultVoltageForFreq(float fValue,WORD wFreqInMHz)
{
  bool bInserted = false ;
#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  m_wxcriticalsection.Enter() ;
#endif
  std::pair <std::set<VoltageAndFreq>::iterator, bool>
    stdpairstdsetvoltageandfreq = m_stdsetvoltageandfreqDefault.insert(
    VoltageAndFreq(fValue,wFreqInMHz) ) ;
  bInserted = stdpairstdsetvoltageandfreq.second ;

//  //Used by I_CPUcontroller::GetMaximumFrequencyInMHz(),
//  //I_CPUcontroller::GetMinimumFrequencyInMHz()
//  m_stdsetvoltageandfreqAvailableFreq.insert(
//    VoltageAndFreq(fValue,wFreqInMHz) ) ;

#ifndef COMPILE_FOR_CPUCONTROLLER_DYNLIB
  m_wxcriticalsection.Leave() ;
#endif
  return bInserted ;
}

