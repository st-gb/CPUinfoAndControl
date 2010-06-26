/*
 * PerformanceCounterValueDiff.h
 *
 *  Created on: May 30, 2010
 *      Author: Stefan
 */

#ifndef PERFORMANCECOUNTERVALUEDIFF_H_
#define PERFORMANCECOUNTERVALUEDIFF_H_

#define PERFORMANCE_COUNTER_VALUE_DIFF(current,prev) \
  /* the performance counter values are increasing in time except a value wrap
  occured */ \
  (current) < (prev) ? \
  /* for understanding this calculation:
    example: wrap at 255: current value: 2 old value : 250
    -> correct value is "maximum value" minus "old value" +
    "current value" + 1 (because first value is "0")
    = 255 - 250 + 2 + 1 = 5 + 2 + 1 = 8
    example: wrap at 255: current value: 0 old value : 255: 255-255 + 0 + 1 = 1
  */ \
  (MAXIMUM_PERFORMANCE_COUNTER_VALUE - (prev) ) + (current) + 1 \
  : \
  (current) - (prev)

#endif /* PERFORMANCECOUNTERVALUEDIFF_H_ */
