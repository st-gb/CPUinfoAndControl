/*
 * SAX2IPCcurrentCPUdataHandler.hpp
 *
 *  Created on: Jun 15, 2010
 *      Author: Stefan
 */

#ifndef SAX2IPCCURRENTCPUDATAHANDLER_HPP_
#define SAX2IPCCURRENTCPUDATAHANDLER_HPP_

#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <ModelData/VoltageAndFreq.hpp> //for class VoltageAndMultiAndRefClock
#include <wx/thread.h> //for class wxCriticalSection
#include <xercesc/sax2/DefaultHandler.hpp>
#include <map>
//#include <set>

//#include <windef.h> //for BYTE

//Forward declarations.
namespace XERCES_CPP_NAMESPACE
{
  class Attributes ;
}
class CPUcoreData ;

class SAX2IPCcurrentCPUdataHandler
  :
  public XERCES_CPP_NAMESPACE::DefaultHandler
  //For drawing the current CPU usage via a subclass of ICPUcoreUsageGetter
  , public ICPUcoreUsageGetter
  //For drawing the current CPU p-state via a subclass of I_CPUcontroller
  , public I_CPUcontroller
{
//  const CPUcoreData * const m_cpc_cpucoredata ;
public:
//  bool m_bTooHotLastTime;
  CPUcoreData * m_cpc_cpucoredata ;
  wxCriticalSection m_wxcriticalsection ;
  wxLongLong_t m_wxlonglong_tLastTimeTooHotFromFromIPC;
  void endDocument() ;
  BYTE GetCurrentVoltageAndFrequency(
    float & r_fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float & r_fMultiplier
    , float & r_fReferenceClockInMHz
    , WORD wCoreID
    ) ;
  BYTE GetPercentalUsageForAllCores(float arf[] ) ;
  WORD GetNumberOfLogicalCPUcores() ;
//  std::set<float> m_stdset_fUsage ;
  std::map<WORD,float> m_stdmap_wCoreNumber2fUsage ;
  std::map<WORD,float> m_stdmap_wCoreNumber2fMultiplier ;
  std::map<WORD,float> m_stdmap_wCoreNumber2fReferenceClock ;
  std::map<WORD,float> m_stdmap_wCoreNumber2fTempInDegCelsius ;
  std::map<WORD,float> m_stdmap_wCoreNumber2fVoltageInVolt ;
  std::map<WORD,VoltageAndMultiAndRefClock>
    m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock ;
  float GetTemperatureInCelsius( WORD wCoreID ) ;
  inline void HandleCoreXMLelement_Inline(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xerces_attributes);
  void startDocument();
  SAX2IPCcurrentCPUdataHandler(//Model & r_model
    )
    :
//    I_CPUcontroller::mp_model(& r_model)
//    mp_model(& r_model)
    m_wxlonglong_tLastTimeTooHotFromFromIPC(0)
  {
//    m_bTooHotLastTime = false;
  }
  ~SAX2IPCcurrentCPUdataHandler() ;
  void startElement(
      const   XMLCh* const    uri,
      const   XMLCh* const    localname,
      const   XMLCh* const    qname,
      const   XERCES_CPP_NAMESPACE::Attributes &     attrs
  );
//  SAX2IPCcurrentCPUdataHandler( const CPUcoreData & r_cpucoredata ) ;
//  {
//
//    m_wxcriticalsection.Enter() ;
//  }
} ;

#endif /* SAX2IPCCURRENTCPUDATAHANDLER_HPP_ */
