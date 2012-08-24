/*
 * InstableCPUcoreVoltageDetection.hpp
 *
 *  Created on: 22.04.2012
 *      Author: Stefan
 */

#ifndef INSTABLECPUCOREVOLTAGEDETECTION_HPP_
#define INSTABLECPUCOREVOLTAGEDETECTION_HPP_

//#include "VoltageAndFreq.hpp" //class VoltageAndFreq

class InstableCPUcoreVoltageDetection
{
public:
  unsigned m_uiNumberOfSecondsToWaitUntilVoltageIsReduced;
  std::wstring m_std_wstrDynLibPath;
//  VoltageAndFreq m_lastSetVoltageAndFreq;

  //TODO if the reference clock changes after starting to find an unstable
  //CPU core voltage then (if the ref. clock in heightened) it can be still
  //instable. But on the other hand: if the calculation of the ref. clock is
  //inaccurate then it is better to set to the same multiplier as at start of
  //find lowest voltage.

  //Use separate values for both multi and ref clock rather than a single
  //composed value for the frequency. because the mult
  float m_lastSetCPUcoreMultiplier;
  float m_lastSetCPUcoreVoltageInVolt;
  float m_fReferenceClockInMHz;

  InstableCPUcoreVoltageDetection():
    m_uiNumberOfSecondsToWaitUntilVoltageIsReduced(100),
    m_std_wstrDynLibPath( L"InstableCPUcoreVoltageDetection.dll")
  {
  }
};

#endif /* INSTABLECPUCOREVOLTAGEDETECTION_HPP_ */
