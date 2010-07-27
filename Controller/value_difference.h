/*
 * value_difference.h
 *
 *  Created on: May 30, 2010
 *      Author: Stefan
 * Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany
 */

#ifndef VALUE_DIFFERENCE_H_
#define VALUE_DIFFERENCE_H_

#include <limits.h> //for ULONG_MAX

#ifndef _MSC_VER //MS compiler has _UI64_MAX defined, but not g++
  #define _UI64_MAX ULONG_LONG_MAX
#endif

#define ULONGLONG_VALUE_DIFF(current,prev) \
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
  (_UI64_MAX - (prev) ) + (current) + 1 \
  : \
  (current) - (prev)

#define ULONG_VALUE_DIFF(current,prev) \
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
  (ULONG_MAX - (prev) ) + (current) + 1 \
  : \
  (current) - (prev)

#endif /* VALUE_DIFFERENCE_H_ */
