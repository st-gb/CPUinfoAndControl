/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include <windef.h> //for BYTE, WORD
#include <ostream> //class std::ostream
#include <fastest_data_type.h> //fastestUnsignedDataType

/** Models a performance state */
class VoltageAndFreq
{
public:
  float m_fVoltageInVolt ;
  WORD m_wFreqInMHz ;
  VoltageAndFreq() ;
  VoltageAndFreq(
    float fVoltageInVolt ,
    WORD wFreqInMHz
    )
    :
      m_fVoltageInVolt (fVoltageInVolt),
      m_wFreqInMHz (wFreqInMHz)
  {

  }
  /** This operator is needed for inserting into STL sorting container like a
   * std::set. */
  bool operator < (const VoltageAndFreq & _Right) const {
    return ( m_wFreqInMHz < _Right.m_wFreqInMHz ); }

  /** For outputting to a std::ostream or derived class
  * see http://www.cplusplus.com/forum/general/20792/
  * see http://www.learncpp.com/cpp-tutorial/93-overloading-the-io-operators/:
  */
  friend std::ostream & operator << (std::ostream & std_ostream,
    const VoltageAndFreq & voltageandfreq
    ) //const
    ;
  void GetLinearExtrapolatedVoltage(
    const VoltageAndFreq & c_r_voltageandfreqLowerEqual,
    WORD wFreqInMHzToGetVoltageFrom,
    float & r_fVoltageInVolt) const;

  void GetLinearExtrapolatedFrequency(
    const VoltageAndFreq & c_r_voltageandfreqLowerEqual,
    const float fVoltageInVoltToGetFreqInMHzFrom,
    WORD & r_fFrequencyInMHz
    )
    const
  {
    //Unsigned is the fastest data type because of same bit width as CPU arch.
    const unsigned wHigherFreqInMHz = m_wFreqInMHz ;
    const unsigned wLowerFreqInMHz = c_r_voltageandfreqLowerEqual.m_wFreqInMHz ;

    const float fVoltageInVoltFromHigherFreq = m_fVoltageInVolt ;
    const float fVoltageInVoltFromLowerFreq =
      c_r_voltageandfreqLowerEqual.m_fVoltageInVolt ;

    //example: 1.148 V - 0.732V = 0,416 V
    const float fVoltageFromHigherAndLowerFreqDiff =
      fVoltageInVoltFromHigherFreq -
      fVoltageInVoltFromLowerFreq ;

    //ex.: 1800-800 = 1000
    const fastestUnsignedDataType wHigherAndLowerFreqDiffInMHz =
      wHigherFreqInMHz - wLowerFreqInMHz ;

    //ex.: 1000 MHz / 0.416V = 2403.84615384615 MHz/V
    const float fMHzPerVolt = wHigherAndLowerFreqDiffInMHz /
      fVoltageFromHigherAndLowerFreqDiff ;

    //ex.: 1800 - 600=1200
    const float fVoltageAboveMinusDesiredVoltage =
      (fVoltageInVoltFromHigherFreq - fVoltageInVoltToGetFreqInMHzFrom
       ) ;

    //ex.: 1800 MHz - 2403.84615384615 MHz/V * 0.448V = 723.076923076925 MHz
    r_fFrequencyInMHz =
      wHigherFreqInMHz -
      // (f / U) * U = f
      (WORD) (fMHzPerVolt * fVoltageAboveMinusDesiredVoltage)
      ;
  }

  void GetInterpolatedVoltage(
    const VoltageAndFreq & c_r_voltageandfreqLowerEqual,
    WORD wFreqInMHzToGetVoltageFrom,
    float & r_fVoltageInVolt) const
  {
    WORD wFreqInMHzFromNearFreqAboveWantedFreq =
      m_wFreqInMHz ;
    WORD wFreqInMHzFromNearFreqBelowWantedFreq =
      c_r_voltageandfreqLowerEqual.m_wFreqInMHz ;
    float fVoltageInVoltFromNearFreqAboveWantedFreq ;
    float fVoltageInVoltFromNearFreqBelowWantedFreq ;
    fVoltageInVoltFromNearFreqAboveWantedFreq =
      m_fVoltageInVolt ;
    fVoltageInVoltFromNearFreqBelowWantedFreq =
      c_r_voltageandfreqLowerEqual.m_fVoltageInVolt ;
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

    r_fVoltageInVolt =
      fVoltageInVoltFromNearFreqBelowWantedFreq +
      fVoltagePerMHz * (float) wWantedFreqMinusFreqBelow
      ;
//    LOGN("GetInterpolatedVoltageFromFreq("
//      << "\nnearest freq from std::set below:"
//      << wFreqInMHzFromNearFreqBelowWantedFreq
//      << "\nnearest freq from std::set above:"
//      << wFreqInMHzFromNearFreqAboveWantedFreq
//      << "\nabove - below freq=" << wFreqInMHzFromNearFreqsWantedFreqDiff
//      << "\nabove - below voltage=" << fVoltageFromFreqAboveAndBelowDiff
//      << "\nvoltage/MHz(voltage above-below/freq above-below)="
//      << std::ios::fixed << fVoltagePerMHz
//      << "\nwanted freq - freq below=" << wWantedFreqMinusFreqBelow
//      << "\ninterpolated voltage (voltage below+voltage/MHz*"
//          "\"wanted freq - freq below\"=" << r_fVoltageInVolt
//      )
  }
};

class VoltageAndMultiAndRefClock
{
public:
  float m_fMultiplier ;
  float m_fReferenceClock ;
  float m_fVoltageInVolt ;
  VoltageAndMultiAndRefClock() ;
//This operator is needed for inserting into STL sorting container like a std::set.
  bool operator < (const VoltageAndFreq & _Right) const ;
};

class CPUcoreVoltageAndFrequency
  : public VoltageAndMultiAndRefClock
{
public:
  float m_fThrottleRatio;
};

class MaxVoltageForFreq
{
public:
  float m_fVoltageInVolt ;
  WORD m_wFreqInMHz ;
  MaxVoltageForFreq() ;
  MaxVoltageForFreq(
    float fVoltageInVolt ,
    WORD wFreqInMHz
    ) ;
//This operator is needed for inserting into STL sorting container like a std::set.
  bool operator < (const MaxVoltageForFreq & _Right) const ;
  /*bool operator == (const MaxVoltageForFreq & _Right) const ;*/
};
