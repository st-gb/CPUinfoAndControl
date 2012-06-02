/*
 * performance_events.h
 *
 *  Created on: 21.04.2012
 *      Author: Stefan
 */

#ifndef PERFORMANCE_EVENTS_H_
#define PERFORMANCE_EVENTS_H_

  //MSRC001_00[03:00] Performance Event Select Register (PERF_CTL[3:0])
  #define PERF_CTL_0 0xC0010000
  //MSRC001_00[07:04] Performance Event Counter Registers (PERF_CTR[3:0])
  //47:0 CTR: performance counter value -> 48 bit wide so the max value as binary
  //has 48 bits set to "1" which equals FFFFFFFFFFFF hexadecimal
  #define MAXIMUM_PERFORMANCE_COUNTER_VALUE 0xFFFFFFFFFFFFULL

  //MSRC001_00[07:04] Performance Event Counter Registers (PERF_CTR[3:0])
  #define PERF_CTR_0 0xC0010004
  #define PERFORMANCE_EVENT_COUNTER_0_REGISTER 0xC0010004
  #define PERFORMANCE_EVENT_COUNTER_1_REGISTER 0xC0010005

  #define PERFORMANCE_EVENT_SELECT_DISPATCHED_FPU_OPERATIONS 0x000
  #define PERFORMANCE_EVENT_SELECT_RETIRED_INSTRUCTIONS 0xC0
  #define PERFORMANCE_EVENT_SELECT_RETIRED_MICRO_OPS 0xC1
  #define PERFORMANCE_EVENT_SELECT_CYCLES_IN_WHICH_THE_FPU_IS_EMPTY 0x001

  //see AMD BIOS and Kernel Dev Guide "EventSelect 076h CPU Clocks not Halted"
  #define CPU_CLOCKS_NOT_HALTED 0x76

#endif /* PERFORMANCE_EVENTS_H_ */
