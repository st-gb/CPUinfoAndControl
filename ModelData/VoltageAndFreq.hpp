#pragma once //include guard

#include "../global.h" //for BYTE, WORD

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
