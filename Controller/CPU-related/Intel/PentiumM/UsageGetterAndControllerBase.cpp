/*
 * UsageGetterAndControllerBase.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Stefan
 */
#include "UsageGetterAndControllerBase.hpp"

PentiumM::UsageGetterAndControllerBase::UsageGetterAndControllerBase()
{
  //for Pentium Ms the perf counter bit width seems to be 40:
  //                                    <8 bit > <8 bit > <8 bit > <8 bit > <8 bit >
  //previous value: 5497519814387   100 11111111 11111101 10110111 00110110 11110011
  //current value:  4398069455221   100 00000000 00000001 01011110 00011001 01110101
  //#infdef _MSC_VER
  //suffix "ULL" is not needed by MS compiler, but by g++
  m_ullMaximumPerfCounterValue = 0xFFFFFFFFFFULL ; //10xF = 10x 4 bit =40 bit
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
