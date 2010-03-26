/*
 * GetCPUusageRegistry.hpp
 *
 *  Created on: Mar 20, 2010
 *      Author: Stefan
 */

#ifndef GETCPUUSAGEREGISTRY_HPP_
#define GETCPUUSAGEREGISTRY_HPP_

class WinRegistryCPUcoreUsageGetter
{
private:
  DWORD m_dwAtMask2ndTimeCPUcoreMask ;
public:
  WinRegistryCPUcoreUsageGetter() ;
  float GetPercentalUsageForCore(
    BYTE byCoreID
    ) ;
} ;

#endif /* GETCPUUSAGEREGISTRY_HPP_ */
