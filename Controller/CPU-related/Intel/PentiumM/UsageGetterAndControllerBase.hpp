/*
 * UsageGetterAndControllerBase.hpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Stefan
 */

#ifndef USAGEGETTERANDCONTROLLERBASE_HPP_
#define USAGEGETTERANDCONTROLLERBASE_HPP_

#include <windef.h> //ULONGLONG

namespace PentiumM
{
  class UsageGetterAndControllerBase
  {
    ULONGLONG m_ullMaximumPerfCounterValue ;
  public:
    UsageGetterAndControllerBase() ;
  protected:
    ULONGLONG PerformanceCounterValueDiff(
      ULONGLONG ullPerformanceCounterCurrent ,
      ULONGLONG ullPerformanceCounterPrevious ) ;
  };
}

#endif /* USAGEGETTERANDCONTROLLERBASE_HPP_ */
