#include "VoltageAndFreq.hpp"

VoltageAndFreq::VoltageAndFreq()
	//eclipse / g++ hatte gemeckert bei dieser C++ style initialization.
//    : m_wFreqInMHz(0)
//    ,
//    m_fVoltageInVolt(0.0)
{
  m_wFreqInMHz = 0 ;
  m_fVoltageInVolt = 0.0f ;
}

VoltageAndFreq::VoltageAndFreq(
  float fVoltageInVolt ,
  WORD wFreqInMHz
  )
{
  m_fVoltageInVolt = fVoltageInVolt ;
  m_wFreqInMHz = wFreqInMHz ;
//#ifdef _DEBUG
  //LOGN("VoltageAndFreq c'tor" << m_fVoltageInVolt << m_wFreqInMHz )
//#endif
}

//This operator is needed for inserting into STL sorting container like a std::set.
bool VoltageAndFreq::operator < (const VoltageAndFreq & _Right) const
{	
    // apply operator< to operands
  return ( 
      m_wFreqInMHz < _Right.m_wFreqInMHz );
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
