/*
 * InstableCPUcoreVoltageDetection.hpp
 *
 *  Created on: 22.04.2012
 *      Author: Stefan
 */

#ifndef INSTABLECPUCOREVOLTAGEDETECTION_HPP_
#define INSTABLECPUCOREVOLTAGEDETECTION_HPP_

class InstableCPUcoreVoltageDetection
{
public:
  unsigned m_uiNumberOfSecondsToWaitUntilVoltageIsReduced;
  std::wstring m_std_wstrDynLibPath;

  InstableCPUcoreVoltageDetection():
    m_uiNumberOfSecondsToWaitUntilVoltageIsReduced(100),
    m_std_wstrDynLibPath( L"InstableCPUcoreVoltageDetection.dll")
  {
  }
};

#endif /* INSTABLECPUCOREVOLTAGEDETECTION_HPP_ */
