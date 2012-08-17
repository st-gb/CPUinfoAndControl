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
    ) ;
  /** This operator is needed for inserting into STL sorting container like a
   * std::set. */
  bool operator < (const VoltageAndFreq & _Right) const ;
  /** For outputting to a std::ostream or derived class
  * see http://www.cplusplus.com/forum/general/20792/
  * see http://www.learncpp.com/cpp-tutorial/93-overloading-the-io-operators/:
  */
  friend std::ostream & operator << (std::ostream & std_ostream,
    const VoltageAndFreq & voltageandfreq
    ) //const
    ;
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
