/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * UsageGetterAndControllerBase.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Stefan
 */
#include "UsageGetterAndControllerBase.hpp"
#include "PentiumM_registers.h" //MAXIMUM_PERFORMANCE_COUNTER_VALUE

PentiumM::UsageGetterAndControllerBase::UsageGetterAndControllerBase()
{
  m_ullMaximumPerfCounterValue = MAXIMUM_PERFORMANCE_COUNTER_VALUE ;
  //#else
  //m_ullMaximumPerfCounterValue = 0xFFFFFFFFFF ; //10xF = 10x 4 bit =40 bit
  //#endif
}

  ULONGLONG PentiumM::UsageGetterAndControllerBase::
    PerformanceCounterValueDiff(
    ULONGLONG ullPerformanceCounterCurrent ,
    ULONGLONG ullPerformanceCounterPrevious
    )
  {
    ULONGLONG ullDiff =
    /* the performance counter values are increasing in time except a value wrap
    occured */ \
    (ullPerformanceCounterCurrent) < (ullPerformanceCounterPrevious) ?
    /* for understanding this calculation:
      example: wrap at 255: current value: 2 old value : 250
      -> correct value is "maximum value" minus "old value" +
      "current value" + 1 (because first value is "0")
      = 255 - 250 + 2 + 1 = 5 + 2 + 1 = 8
      example: wrap at 255: current value: 0 old value : 255: 255-255 + 0 + 1 = 1
    */ \
      (m_ullMaximumPerfCounterValue - (ullPerformanceCounterPrevious) ) +
        (ullPerformanceCounterCurrent) + 1
      : //else
      (ullPerformanceCounterCurrent) - (ullPerformanceCounterPrevious) ;
    return ullDiff ;
  }
