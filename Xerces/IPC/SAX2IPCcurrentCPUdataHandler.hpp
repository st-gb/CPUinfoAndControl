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

//forwars decl.
class XERCES_CPP_NAMESPACE::Attributes ;
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
  CPUcoreData * m_cpc_cpucoredata ;
  wxCriticalSection m_wxcriticalsection ;
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
  void startDocument();
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
