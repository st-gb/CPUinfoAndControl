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
  LOGN( "wFreqInMHzToGetVoltageFrom:" << wFreqInMHzToGetVoltageFrom << ", ..." )
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
      ci_stdsetvoltageandfreqNearestHigherEqual->GetInterpolatedVoltage(
        * ci_stdsetvoltageandfreqNearestLowerEqual, wFreqInMHzToGetVoltageFrom,
        r_fVoltageInVolt
        );
      return true ;
    }
  }
  return false ;
}
