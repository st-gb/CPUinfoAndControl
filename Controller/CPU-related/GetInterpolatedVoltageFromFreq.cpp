/*
 * GetInterpolatedVoltageFromFreq.cpp
 *
 *  Created on: 11.04.2012
 *      Author: Stefan
 */
#include "GetInterpolatedVoltageFromFreq.hpp"

/** This function is also called from a DLL. So it does not need to include
 *   the whole I_CPUcontroller class.
 * @return:
 *   true: p-state with freq >= wanted freq and
*      p-state with freq <= wanted freq was found and
*      voltage could be interpolated */
BYTE //I_CPUcontroller::
  GetInterpolatedVoltageFromFreq(
  WORD wFreqInMHzToGetVoltageFrom
  , float & r_fVoltageInVolt
  , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
  )
{
  LOGN("GetInterpolatedVoltageFromFreq("
#ifdef _DEBUG
    << wFreqInMHzToGetVoltageFrom
#endif
    << ", ..." )
  std::set<VoltageAndFreq>::const_iterator ci_stdsetvoltageandfreq =
    r_stdsetvoltageandfreq.begin() ;
  std::set<VoltageAndFreq>::const_iterator
    ci_stdsetvoltageandfreqNearestLowerEqual = r_stdsetvoltageandfreq.end() ;
  std::set<VoltageAndFreq>::const_iterator
    ci_stdsetvoltageandfreqNearestHigherEqual = r_stdsetvoltageandfreq.end() ;
  while( ci_stdsetvoltageandfreq != r_stdsetvoltageandfreq.end() )
  {
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz <= wFreqInMHzToGetVoltageFrom )
      ci_stdsetvoltageandfreqNearestLowerEqual = ci_stdsetvoltageandfreq ;
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz >= wFreqInMHzToGetVoltageFrom )
    {
      ci_stdsetvoltageandfreqNearestHigherEqual = ci_stdsetvoltageandfreq ;
      //The entries are sorted ascending by frequency. So break to avoid
      //assigning a higher value (that would not be the nearest higher freq
      //any more).
      break ;
    }
    ++ ci_stdsetvoltageandfreq ;
  }
//  LOGN("GetInterpolatedVoltageFromFreq(...) after loop")
#ifdef COMPILE_WITH_LOG
  std::ostringstream ostringstreamLog ;
  ostringstreamLog << "GetInterpolatedVoltageFromFreq("
    << wFreqInMHzToGetVoltageFrom
    << ", ...)--lower freq:" ;
  if( ci_stdsetvoltageandfreqNearestLowerEqual != r_stdsetvoltageandfreq.end()
    )
  {
    ostringstreamLog << ci_stdsetvoltageandfreqNearestLowerEqual->
      m_wFreqInMHz ;
  }
  else
    ostringstreamLog << " no lower freq than " << wFreqInMHzToGetVoltageFrom ;
  ostringstreamLog << ", ...)--higher freq:" ;
  if( ci_stdsetvoltageandfreqNearestHigherEqual != r_stdsetvoltageandfreq.end()
    )
    ostringstreamLog << ci_stdsetvoltageandfreqNearestHigherEqual->
      m_wFreqInMHz ;
  else
    ostringstreamLog << " no higher freq than " << wFreqInMHzToGetVoltageFrom ;
#endif //#ifdef COMPILE_WITH_LOG
  LOGN( ostringstreamLog.str() )
  if( ci_stdsetvoltageandfreqNearestLowerEqual != r_stdsetvoltageandfreq.end()
    && ci_stdsetvoltageandfreqNearestHigherEqual !=
      r_stdsetvoltageandfreq.end()
    )
  {
    if( //This is the case if wFreqInMHzToGetVoltageFrom has the
      //same freq as one of the p-states.
      //This case must be caught, else wrong values by the
      //log_x() function (  log_1(1)  is calculated then ) .
      //p_pstateGreaterEqual == p_pstateLowerEqual
      ci_stdsetvoltageandfreqNearestLowerEqual ==
      ci_stdsetvoltageandfreqNearestHigherEqual
      )
    {
      //r_fVoltageInVolt = p_pstateGreaterEqual->GetVoltageInVolt() ;
      r_fVoltageInVolt = ci_stdsetvoltageandfreqNearestLowerEqual->
        m_fVoltageInVolt ;
//      DEBUGN("I_CPUcontroller::GetInterpolatedVoltageFromFreq()"
//        "voltage found: " << r_fVoltageInVolt )
      return true ;
    }
    else
    {
      WORD wFreqInMHzFromNearFreqAboveWantedFreq =
        //p_pstateGreaterEqual->GetFreqInMHz() ;
        ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz ;
      WORD wFreqInMHzFromNearFreqBelowWantedFreq =
        //p_pstateLowerEqual->GetFreqInMHz() ;
        ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
    //if( mp_model->m_pstates.m_arp_pstate[1] &&
    //  mp_model->m_pstates.m_arp_pstate[1]->GetFreqInMHz() <
    //  wFreqInMHzToGetVoltageFrom
    //  )
    //{
      float fVoltageInVoltFromNearFreqAboveWantedFreq ;
      float fVoltageInVoltFromNearFreqBelowWantedFreq ;
      fVoltageInVoltFromNearFreqAboveWantedFreq =
        //mp_model->m_pstates.m_arp_pstate[0]->GetVoltageInVolt() ;
        //p_pstateGreaterEqual->GetVoltageInVolt() ;
        ci_stdsetvoltageandfreqNearestHigherEqual->
          m_fVoltageInVolt ;
      fVoltageInVoltFromNearFreqBelowWantedFreq =
        //mp_model->m_pstates.m_arp_pstate[1]->GetVoltageInVolt() ;
        //p_pstateLowerEqual->GetVoltageInVolt() ;
        ci_stdsetvoltageandfreqNearestLowerEqual->
          m_fVoltageInVolt ;
      //example: 1.0625 V - 0.85V = 0,2125 V
      float fVoltageFromFreqAboveAndBelowDiff =
        fVoltageInVoltFromNearFreqAboveWantedFreq -
        fVoltageInVoltFromNearFreqBelowWantedFreq ;
      WORD wFreqInMHzFromNearFreqsWantedFreqDiff =
        wFreqInMHzFromNearFreqAboveWantedFreq -
       wFreqInMHzFromNearFreqBelowWantedFreq ;
      float fVoltagePerMHz =
        fVoltageFromFreqAboveAndBelowDiff /
          wFreqInMHzFromNearFreqsWantedFreqDiff ;
      WORD wWantedFreqMinusFreqBelow =
        (wFreqInMHzToGetVoltageFrom -
        wFreqInMHzFromNearFreqBelowWantedFreq
         ) ;

      //example: 2200 MHz / 1050 MHz ~= 2,0952380952
//      double dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom =
//          (double) wFreqInMHzFromNearFreqAboveWantedFreq /
//          (double) wFreqInMHzToGetVoltageFrom ;

      r_fVoltageInVolt =
        ////fVoltageOfOvVoltProtVnf_pairHigherEqualWantedFreq
        //// 1,0625 - 0,22676176661993910555202664192874
        ////  = 0,835738233380060894447973358072
        //fVoltageInVoltFromNearFreqAboveWantedFreq -
        //  //log10(2,0952380952380952380952380952381) =
        //  //  0,32123338175226816317043359884426
        //  //log10(2) = 0,30102999566398119521373889472449
        //  // => log2(2,0952380952380952380952380952381) =
        //  //  0,32123338175226816317043359884426 /
        //  //  0,30102999566398119521373889472449
        //  //  = 1,067114195858536967303654785547
        //  // 1,067114195858536967303654785547 * 0,2125 =
        //  //   0,22676176661993910555202664192874
        //  //log_dualis
        //  log_x
        //  (
        //    (double) wFreqInMHzFromNearFreqAboveWantedFreq /
        //    (double) wFreqInMHzFromNearFreqBelowWantedFreq
        //    ,
        //  //wFreqOfOvVoltProtVnf_pairHigherEqualWantedFreq
        //    //example: 2200 MHz / 1050 MHz ~= 2.0952380952
        //    //example: 2200 MHz / 550 MHz = 4
        //    (double) wFreqInMHzFromNearFreqAboveWantedFreq /
        //    (double) wFreqInMHzToGetVoltageFrom
        //  )
        //  *
        //  //fVoltageDiffBetwOvVoltProtVnf_pairs
        //  (
        //    //example: 1.0625 V - 0.85V = 0,2125 V
        //    //example: 1.0625 V - 0.75V = 0.3125 V
        //    fVoltageInVoltFromNearFreqAboveWantedFreq -
        //    fVoltageInVoltFromNearFreqBelowWantedFreq
        //  )
        fVoltageInVoltFromNearFreqBelowWantedFreq +
        fVoltagePerMHz * (float) wWantedFreqMinusFreqBelow
        ;
      LOGN("GetInterpolatedVoltageFromFreq("
        << "\nnearest freq from std::set below:"
        << wFreqInMHzFromNearFreqBelowWantedFreq
        << "\nnearest freq from std::set above:"
        << wFreqInMHzFromNearFreqAboveWantedFreq
        << "\nabove - below freq=" << wFreqInMHzFromNearFreqsWantedFreqDiff
        << "\nabove - below voltage=" << fVoltageFromFreqAboveAndBelowDiff
        << "\nvoltage/MHz(voltage above-below/freq above-below)="
        << std::ios::fixed << fVoltagePerMHz
        << "\nwanted freq - freq below=" << wWantedFreqMinusFreqBelow
        << "\ninterpolated voltage (voltage below+voltage/MHz*"
            "\"wanted freq - freq below\"=" << r_fVoltageInVolt
        )
      return true ;
    }
  }
  return false ;
}
