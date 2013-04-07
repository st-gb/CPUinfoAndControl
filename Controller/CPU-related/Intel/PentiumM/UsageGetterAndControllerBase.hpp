/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
