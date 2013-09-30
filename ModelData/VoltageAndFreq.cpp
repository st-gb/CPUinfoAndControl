/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "VoltageAndFreq.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h> // LOGN()
#include <ostream> //class std::ostream

VoltageAndFreq::VoltageAndFreq()
	//eclipse / g++ hatte gemeckert bei dieser C++ style initialization.
//    : m_wFreqInMHz(0)
//    ,
//    m_fVoltageInVolt(0.0)
{
  m_wFreqInMHz = 0 ;
  m_fVoltageInVolt = 0.0f ;
}

//VoltageAndFreq::VoltageAndFreq(
//  float fVoltageInVolt ,
//  WORD wFreqInMHz
//  )
//{
//  m_fVoltageInVolt = fVoltageInVolt ;
//  m_wFreqInMHz = wFreqInMHz ;
////#ifdef _DEBUG
//  //LOGN("VoltageAndFreq c'tor" << m_fVoltageInVolt << m_wFreqInMHz )
////#endif
//}

void VoltageAndFreq::GetLinearExtrapolatedVoltage(
  const VoltageAndFreq & c_r_voltageandfreqLowerEqual,
  WORD wFreqInMHzToGetVoltageFrom,
  float & r_fVoltageInVolt) const
{
  WORD wFreqInMHzFromNearFreqAboveWantedFreq = m_wFreqInMHz ;
  WORD wFreqInMHzFromNearFreqBelowWantedFreq =
    c_r_voltageandfreqLowerEqual.m_wFreqInMHz ;

  float fVoltageInVoltFromLowerFreq ;
  float fVoltageInVoltFromHigherFreq = m_fVoltageInVolt ;

  fVoltageInVoltFromLowerFreq =
    c_r_voltageandfreqLowerEqual.m_fVoltageInVolt ;

  //example: 1.148 V - 0.732V = 0,416 V
  float fVoltageFromFreqAboveAndBelowDiff =
    fVoltageInVoltFromHigherFreq -
    fVoltageInVoltFromLowerFreq ;
  LOGN( "higher and lower voltage diff:" <<
    fVoltageFromFreqAboveAndBelowDiff << "V")

  //ex.: 1800-800 = 1000
  WORD wFreqInMHzFromNearFreqsWantedFreqDiff =
    wFreqInMHzFromNearFreqAboveWantedFreq -
   wFreqInMHzFromNearFreqBelowWantedFreq ;
  LOGN( "higher and lower frequency diff:" <<
    wFreqInMHzFromNearFreqsWantedFreqDiff << "MHz")

//  float fVoltageInVoltFromHigherFreqMinusWantedVoltage =
//      fVoltageInVoltFromHigherFreq - r_fVoltageInVolt;

  //ex.: 0,416 / 1000 = 0.000416
  float fVoltageInVoltPerMHz =
    fVoltageFromFreqAboveAndBelowDiff /
      wFreqInMHzFromNearFreqsWantedFreqDiff ;
  LOGN( "voltage/frequency="
    << fVoltageFromFreqAboveAndBelowDiff << "V / "
    << wFreqInMHzFromNearFreqsWantedFreqDiff << "MHz ="
    << fVoltageInVoltPerMHz << "V/MHz")

  //ex.: 1800 - 600=1200
  WORD wFreqAboveMinusWantedFreq =
    (wFreqInMHzFromNearFreqAboveWantedFreq - wFreqInMHzToGetVoltageFrom
     ) ;
  LOGN( "higher frequency-wanted frequency:" <<
    wFreqInMHzFromNearFreqsWantedFreqDiff << "MHz")

  //ex.: 1.148 V  - 0.000416V * 1200 MHz = 0.6488V
  r_fVoltageInVolt =
    fVoltageInVoltFromHigherFreq -
    fVoltageInVoltPerMHz * (float) wFreqAboveMinusWantedFreq
    ;
  LOGN( "(higher-wanted voltage) - voltage/freq * "
      " (higher - wanted freq) =" << fVoltageInVoltFromHigherFreq
      << "MHz-" << fVoltageInVoltPerMHz << "V/MHz * " << wFreqAboveMinusWantedFreq
      << "MHZ =" << fVoltageInVoltFromHigherFreq << "-"
      << fVoltageInVoltPerMHz * wFreqAboveMinusWantedFreq << "=" <<
      r_fVoltageInVolt << "V")
}

///** This operator is needed for inserting into STL sorting container like a
//std::set. */
//bool VoltageAndFreq::operator < (const VoltageAndFreq & _Right) const
//{
//  // apply operator < to operands
//  return (
//      m_wFreqInMHz < _Right.m_wFreqInMHz );
//}

std::ostream & //VoltageAndFreq::
  operator << (std::ostream & std_ostream
  ,const VoltageAndFreq & voltageandfreq
  ) //const
{
//    std::ostream std_ostream;
  std_ostream << "(" << voltageandfreq.m_fVoltageInVolt << "V;"
    << voltageandfreq.m_wFreqInMHz << "MHz)";
  return std_ostream;
}

VoltageAndMultiAndRefClock::VoltageAndMultiAndRefClock()
  :
  m_fMultiplier(0.0)
  , m_fReferenceClock (0.0)
  , m_fVoltageInVolt(0.0)
{

}

MaxVoltageForFreq::MaxVoltageForFreq()
	//eclipse / g++ hatte gemeckert bei dieser C++ style initialization.
//    : m_wFreqInMHz(0)
//    ,
//    m_fVoltageInVolt(0.0)
{
  m_wFreqInMHz = 0 ;
  m_fVoltageInVolt = 0.0f ;
}

MaxVoltageForFreq::MaxVoltageForFreq(
  float fVoltageInVolt ,
  WORD wFreqInMHz
  )
{
  m_fVoltageInVolt = fVoltageInVolt ;
  m_wFreqInMHz = wFreqInMHz ;
}

//This operator is needed for inserting into STL sorting container like a std::set.
bool MaxVoltageForFreq::operator < (const MaxVoltageForFreq & _Right) const
{	
    // apply operator< to operands
  return ( 
      //Sort so that the element with the highest frequency is the 
      //first element.
      m_wFreqInMHz > _Right.m_wFreqInMHz );
}

//bool operator == (const MaxVoltageForFreq & _Right) const 
//{
//  return ( 
//      //Sort so that the element with the highest frequency is the 
//      //first element.
//      m_wFreqInMHz == _Right.m_wFreqInMHz );
//}
