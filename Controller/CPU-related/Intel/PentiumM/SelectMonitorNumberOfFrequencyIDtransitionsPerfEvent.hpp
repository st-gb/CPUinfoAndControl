/*
 * SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent.hpp
 *
 *  Created on: 17.05.2012
 *      Author: Stefan
 */

#ifndef SELECTMONITORNUMBEROFFREQUENCYIDTRANSITIONSPERFEVENT_HPP_
#define SELECTMONITORNUMBEROFFREQUENCYIDTRANSITIONSPERFEVENT_HPP_



#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULTIPLIER_IF_NO_FID_CHANGE
inline void SelectMonitorNumberOfFrequencyIDtransitionsPerfEvent()
{
  PerformanceEventSelectRegisterWrite_PentiumM(
    1 //<< byCoreID ,
    ,
    //Pentium M has 1 or 2 "Performance Event Select Register" from
    //  MSR ... to MSR ...  for
    // 1 or 2 "Performance Event Counter Registers" from
    //  ... to ...
    //  that store the 48 bit counter value
    //Performance Event Counter number
//    1 ,
    0 , //for Pentium M: If no performance event counter 0 than counter 1
    // can't be used?!
    EMON_EST_TRANS ,
    //LAST_LEVEL_CACHE_MISSES_UMASK , // 8 bit unit mask
    ONLY_FREQUENCY_TRANSITIONS_MASK ,
    1, //User Mode
    1, //OS mode
    0, //edge
    0, //pin control
    0, //APIC
    1, //enable counters
    0 , //invert counter mask
    0 //counter mask
    ) ;
  WriteMSR( //IA32_PMC1
//    IA32_PMC0
    PERFORMANCE_COUNTER_FOR_FID_CHANGE
    //Set to 1 so that next time no more 0 so that this function is called again.
    , 1
    , 0
    , 1 ) ;
  //Cause to fetch 2 values for taking a diff for _each_ value type (TSC, time,
  //  FID changes)
  gs_b2ndTimeOrLaterReadTSCandFIDchange = false ;
}
#endif //#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE

#endif /* SELECTMONITORNUMBEROFFREQUENCYIDTRANSITIONSPERFEVENT_HPP_ */
