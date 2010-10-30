/*
 * GetReferenceClockViaTSCdiffDivMultiIfNoFiDchange.hpp
 *
 *  Created on: Aug 10, 2010
 *      Author: Stefan
 */

#ifndef GETREFERENCECLOCKVIATSCDIFFDIVMULTIIFNOFIDCHANGE_HPP_
#define GETREFERENCECLOCKVIATSCDIFFDIVMULTIIFNOFIDCHANGE_HPP_

#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE
inline void GetBaseClockViaTSCdiffdivMultiplierIfNoFIDchange(BYTE byFID)
{
  m_dwTickCountInMilliseconds = ::GetTickCount();
  //For taking a difference we need to have recorded a value before.
  if( m_b2ndTimeOrLaterReadTSCandFIDchange )
  {
    m_dwTickCountDiffInMilliseconds = ULONG_VALUE_DIFF( m_dwTickCountInMilliseconds ,
      m_dwTickCountInMillisecondsPrev ) ;
    if( m_dwTickCountDiffInMilliseconds > m_dwMinimumTimeSpanInMilliseconds )
    {
      if( mp_cpuaccess->ReadTSC( g_dwLowmostBits, g_dwHighmostBits) )
  //      if( mp_cpuaccess->RdmsrEx( IA32_TIME_STAMP_COUNTER, & dwLow, & dwHigh , 1 ) )
      {
        m_ullTSCvalue = g_dwHighmostBits ;
        m_ullTSCvalue <<= 32 ;
        m_ullTSCvalue |= g_dwLowmostBits ;
  //        //For taking a difference we need to have recorded a value before.
  //        if( m_b2ndTimeOrLaterReadTSCandFIDchange )
  //        {
        if( ReadMSR(
          //IA32_PERFEVTSEL0
          //Intel vol. 3B:
          //"IA32_PMCx MSRs start at address 0C1H and occupy a contiguous block of MSR
          //address space; the number of MSRs per logical processor is reported using
          //CPUID.0AH:EAX[15:8]."
          //"30.2.1.1 Architectural Performance Monitoring Version 1 Facilities":
          //The bit width of an IA32_PMCx MSR is reported using the
          //CPUID.0AH:EAX[23:16]
          //
          //IA32_PMC1
          PERFORMANCE_COUNTER_FOR_FID_CHANGE
          , & g_dwLowmostBits
          , & g_dwHighmostBits
          , 1 )
          )
        {
          m_ullPerformanceEventCounterNumberOfFIDchange = g_dwHighmostBits ;
          m_ullPerformanceEventCounterNumberOfFIDchange <<= 32 ;
          m_ullPerformanceEventCounterNumberOfFIDchange |= g_dwLowmostBits ;

          if( m_ullPerformanceEventCounterNumberOfFIDchange )
          {
  //        //For taking a difference we need to have recorded a value before.
  //        if( m_b2ndTimeOrLaterReadTSCandFIDchange )
  //        {
            m_ullNumberOfFIDchangeDiff = //PERFORMANCE_COUNTER_VALUE_DIFF(
              PerformanceCounterValueDiff(
              m_ullPerformanceEventCounterNumberOfFIDchange ,
              m_ullNumberOfFIDchangePrevious
              ) ;
      //      LOGN("m_ullNumberOfFIDchangeDiff:" << m_ullNumberOfFIDchangeDiff)
            //no FID change
            if( m_ullNumberOfFIDchangeDiff == 0
  //              &&
  //              //else: <expr> / 0 = infinite
  //              m_dwTickCountDiffInMilliseconds > //0
  //            m_dwMinimumTimeSpanInMilliseconds
              )
            {
              //Assign all previous values when everything succeeded at first.
              m_dwTickCountInMillisecondsPrev = m_dwTickCountInMilliseconds ;
              m_ullTSCdiff = //PERFORMANCE_COUNTER_VALUE_DIFF(
                ULONGLONG_VALUE_DIFF(
                m_ullTSCvalue ,
                m_ullTSCvaluePrevious
                ) ;
    //        }
            m_ullTSCvaluePrevious = m_ullTSCvalue ;
              //e.g. time diff is "100 ms", TSC diff is "90.000.000":
              // 90.000.000 * 1000 / 100 = 90.000.000.000 / 100 = 900.000.000(900MHz)
              m_dCurrentTSCclockInHz = (double) m_ullTSCdiff * 1000.0 /// byFID
                / (double) m_dwTickCountDiffInMilliseconds ;
      //        LOGN("TSC diff:" << m_ullTSCdiff
      //          << " milliseconds diff:" << m_dwTickCountDiffInMilliseconds
      //          << " ->MHz=TSC diff * 1000 / ms_diff="
      //            << (double) m_ullTSCdiff * 1000.0
      //            << "/ " << (double) m_dwTickCountDiffInMilliseconds
      //          << "\n=" << dMHz
      //          << "FSB(MHz) = MHz/ multiplier=" << dMHz << "/" << (float)byFID
      //          << "\n=" << dMHz / (float)byFID
      //          )
      //        r_wFreqInMHz = dMHz ;
              m_fCurrentReferenceClockInMHz = m_dCurrentTSCclockInHz /
                  (float) byFID / 1000000.0 ;
              m_ullNumberOfFIDchangePrevious =
                  m_ullPerformanceEventCounterNumberOfFIDchange ;
            }
  //        }
          }
          //After a standby or hibernate the PMC value remains 0.
          //I was not able to detect standby or hibernate on Linux to re-init
          //the performance countung, so this is a universal (also Windows)
          //workaround.
          else
          {
            MonitorNumberOfFrequencyIDtransitions() ;
          }
        }
  //        //The Pentium M model 13, stepping 8 has the effect (bug?) that it
  //        // does not increment the TimeStampCounter at a low multiplier (e,g. 6)
  //        //at the same rate as the CPU clock: e.g. 400 M timestamp ticks /s
  //        // for 800 MHz clock speed.
  //        // So try as a workaround to reset the TSC to 0.
  //        mp_cpuaccess->WrmsrEx( IA32_TIME_STAMP_COUNTER , 0 , 0 , 1 ) ;
  //        m_ullTSCvaluePrevious = 0 ;
     }
    }
  }
  else //1st time the values are read.
  {
    m_dwTickCountInMillisecondsPrev = m_dwTickCountInMilliseconds ;
    if( mp_cpuaccess->ReadTSC( g_dwLowmostBits, g_dwHighmostBits) )
    {
      m_ullTSCvaluePrevious = g_dwHighmostBits ;
      m_ullTSCvaluePrevious <<= 32 ;
      m_ullTSCvaluePrevious |= g_dwLowmostBits ;
  //    //The Pentium M model 13, stepping 8 has the effect (bug?) that it
  //    // does not increment the TimeStampCounter at a low multiplier (e,g. 6)
  //    //at the same rate as the CPU clock: e.g. 400 M timestamp ticks /s
  //    // for 800 MHz clock speed.
  //    // So try as a workaround to reset the TSC to 0.
  //    mp_cpuaccess->WrmsrEx( IA32_TIME_STAMP_COUNTER , 0 , 0 , 1) ;
  ////    m_ullTSCvalue = 0 ;
  //    m_ullTSCvaluePrevious = 0 ;
      if( mp_cpuaccess->RdmsrEx(
        //IA32_PERFEVTSEL0
        //Intel vol. 3B:
        //"IA32_PMCx MSRs start at address 0C1H and occupy a contiguous block of MSR
        //address space; the number of MSRs per logical processor is reported using
        //CPUID.0AH:EAX[15:8]."
        //"30.2.1.1 Architectural Performance Monitoring Version 1 Facilities":
        //The bit width of an IA32_PMCx MSR is reported using the
        //CPUID.0AH:EAX[23:16]
        //
  //      IA32_PMC1
        PERFORMANCE_COUNTER_FOR_FID_CHANGE
        , & g_dwLowmostBits
        , & g_dwHighmostBits
        , 1 )
        )
      {
        m_ullNumberOfFIDchangePrevious = g_dwHighmostBits ;
        m_ullNumberOfFIDchangePrevious <<= 32 ;
        m_ullNumberOfFIDchangePrevious |= g_dwLowmostBits ;
        //After a standby or hibernate the PMC value remains 0.
        //I was not able to detect standby or hibernate on Linux to re-init
        //the performance counting, so this is a universal (also Windows)
        //workaround.
       if( ! m_ullNumberOfFIDchangePrevious )
          MonitorNumberOfFrequencyIDtransitions() ;
       else
         //All values have been got successfully-> can get the 2nd values.
         m_b2ndTimeOrLaterReadTSCandFIDchange = true ;
      }
      else
      {
        LOGN("read PMC failed")
      }
  //    else
    }
  }
}

//inline void NumberOfFreqIDchanges()
//{
//  if(
//    (*g_pfnreadmsr)(
//    //IA32_PERFEVTSEL0
//    //Intel vol. 3B:
//    //"IA32_PMCx MSRs start at address 0C1H and occupy a contiguous block of MSR
//    //address space; the number of MSRs per logical processor is reported using
//    //CPUID.0AH:EAX[15:8]."
//    //"30.2.1.1 Architectural Performance Monitoring Version 1 Facilities":
//    //The bit width of an IA32_PMCx MSR is reported using the
//    //CPUID.0AH:EAX[23:16]
//    //
//    IA32_PMC1
//    , g_dwLowmostBits
//    , g_dwHighmostBits
//    , 1
//    )
//  {
//  g_ullPerformanceEventCounterNumberOfFIDchange = dwHigh ;
//  g_ullPerformanceEventCounterNumberOfFIDchange <<= 32 ;
//  g_ullPerformanceEventCounterNumberOfFIDchange |= dwLow ;
//  //For taking a difference we need to have recorded a value before.
//  if( m_b2ndTimeOrLaterReadTSCandFIDchange )
//  {
//    m_ullNumberOfFIDchangeDiff = PERFORMANCE_COUNTER_VALUE_DIFF(
//      m_ullPerformanceEventCounterNumberOfFIDchange ,
//      m_ullNumberOfFIDchangePrevious
//      ) ;
//    LOGN("m_ullNumberOfFIDchangeDiff:" << m_ullNumberOfFIDchangeDiff)
//    //no FID change
//    if( m_ullNumberOfFIDchangeDiff == 0 &&
//        //else: <expr> / 0 = infinite
//        m_dwTickCountDiffInMilliseconds > 0 )
//    {
//      //e.g. time diff is "100 ms", TSC diff is "90.000.000":
//      // 90.000.000 * 1000 / 100 = 90.000.000.000 / 100 = 900.000.000(900MHz)
//      double dMHz = (double) m_ullTSCdiff * 1000.0 /// byFID
//        / (double) m_dwTickCountDiffInMilliseconds ;
//  //        LOGN("TSC diff:" << m_ullTSCdiff
//  //          << " milliseconds diff:" << m_dwTickCountDiffInMilliseconds
//  //          << " ->MHz=TSC diff * 1000 / ms_diff="
//  //            << (double) m_ullTSCdiff * 1000.0
//  //            << "/ " << (double) m_dwTickCountDiffInMilliseconds
//  //          << "\n=" << dMHz
//  //          << "FSB(MHz) = MHz/ multiplier=" << dMHz << "/" << (float)byFID
//  //          << "\n=" << dMHz / (float)byFID
//  //          )
//      r_wFreqInMHz = dMHz ;
//    }
//  }
//  else
//    m_b2ndTimeOrLaterReadTSCandFIDchange = true ;
//  m_ullNumberOfFIDchangePrevious =
//      m_ullPerformanceEventCounterNumberOfFIDchange ;
//}
#endif //#ifdef GET_BASE_CLOCK_VIA_TSC_DIFF_DIV_MULIPLIER_IF_NO_FID_CHANGE

#endif /* GETREFERENCECLOCKVIATSCDIFFDIVMULTIIFNOFIDCHANGE_HPP_ */
