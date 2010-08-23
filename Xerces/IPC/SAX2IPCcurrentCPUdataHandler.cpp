/*
 * SAX2IPCcurrentCPUdataHandler.cpp
 *
 *  Created on: Jun 15, 2010
 *      Author: Stefan
 */
#include "SAX2IPCcurrentCPUdataHandler.hpp"
#include <ModelData/CPUcoreData.hpp>
#include <Xerces/XercesAttributesHelper.hpp>

#include <xercesc/sax2/Attributes.hpp>
#include <wx/thread.h> //for class wxCriticalSectionLocker

//void SAX2IPCcurrentCPUdataHandler::Parse( BYTE arby [] , DWORD dwSizeInBytes)
//{
//}

//SAX2IPCcurrentCPUdataHandler::SAX2IPCcurrentCPUdataHandler(
//  const CPUcoreData & r_cpucoredata )
//  :
//  mr_cpucoredata ( r_cpucoredata )
//{
//
//}

void SAX2IPCcurrentCPUdataHandler::endDocument()
{
  LOGN("SAX2IPCcurrentCPUdataHandler endDocument: before leaving IPC to "
    "in-program data crit sec")
  m_cpc_cpucoredata->wxconditionIPC2InProgramData.Leave() ;
  LOGN("SAX2IPCcurrentCPUdataHandler endDocument: after leaving IPC to "
    "in-program data crit sec")
}

BYTE SAX2IPCcurrentCPUdataHandler::GetCurrentVoltageAndFrequency(
  float & r_fVoltageInVolt
  //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
  , float & r_fMultiplier
  , float & r_fReferenceClockInMHz
  , WORD wCoreID
  )
{
  LOGN("SAX2IPCcurrentCPUdataHandler GetCurrentVoltageAndFrequency before entering "
    "critical section")
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("SAX2IPCcurrentCPUdataHandler GetCurrentVoltageAndFrequency after entering "
    "critical section")
  std::map<WORD,VoltageAndMultiAndRefClock>::const_iterator c_iter =
      m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock.find(wCoreID) ;
  if( c_iter != m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock.end() )
  {
    r_fMultiplier = c_iter->second.m_fMultiplier ;
    r_fReferenceClockInMHz = c_iter->second.m_fReferenceClock ;
    r_fVoltageInVolt = c_iter->second.m_fVoltageInVolt ;
    return 1 ;
  }
  return 0 ;
}

BYTE SAX2IPCcurrentCPUdataHandler::GetPercentalUsageForAllCores(float arf[] )
{
  LOGN("SAX2IPCcurrentCPUdataHandler GetPercentalUsageForAllCores before entering "
    "critical section")
#ifdef _DEBUG
  float fLoad ;
#endif
  WORD wIndex = 0 ;
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("SAX2IPCcurrentCPUdataHandler GetPercentalUsageForAllCores after entering "
    "critical section")
  //e.g. if called before the map has been filled.
  if( m_stdmap_wCoreNumber2fUsage.empty() )
    return 0 ;
  else
    for( //std::set<float>::const_iterator c_iter = m_stdset_fUsage.begin() ;
        std::map<WORD,float>::const_iterator c_iter =
        m_stdmap_wCoreNumber2fUsage.begin() ;
        c_iter != //m_stdset_fUsage.end()
            m_stdmap_wCoreNumber2fUsage.end() ; ++ c_iter, ++ wIndex  )
    {
  #ifdef _DEBUG
      fLoad = c_iter->second ;
      arf[ wIndex ] = fLoad ;
  #else
      arf[ wIndex ] = //*c_iter ;
          c_iter->second ;
  #endif
    }
  return 1 ;
}

WORD SAX2IPCcurrentCPUdataHandler::GetNumberOfLogicalCPUcores()
{
  LOGN("SAX2IPCcurrentCPUdataHandler GetNumberOfLogicalCPUcores before entering "
    "critical section")
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("SAX2IPCcurrentCPUdataHandler GetNumberOfLogicalCPUcores after entering "
    "critical section")
  WORD wSize = m_stdmap_wCoreNumber2fUsage.size() ;
  return wSize ;
}

float SAX2IPCcurrentCPUdataHandler::GetTemperatureInCelsius( WORD wCoreID )
{
  LOGN("SAX2IPCcurrentCPUdataHandler GetTemperatureInCelsius before entering "
    "critical section")
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("SAX2IPCcurrentCPUdataHandler GetTemperatureInCelsius after entering "
    "critical section")
  std::map<WORD,float>::const_iterator c_iter =
      m_stdmap_wCoreNumber2fTempInDegCelsius.find(wCoreID) ;
  if( c_iter != m_stdmap_wCoreNumber2fTempInDegCelsius.end() )
  {
    return c_iter->second ;
  }
  return __FLT_MIN__ ;
}

SAX2IPCcurrentCPUdataHandler::~SAX2IPCcurrentCPUdataHandler()
{
  LOGN("~SAX2IPCcurrentCPUdataHandler")
}

void SAX2IPCcurrentCPUdataHandler::startDocument()
{
//  m_stdset_fUsage.clear() ;
  LOGN("SAX2IPCcurrentCPUdataHandler startDocument: before entering IPC to "
    "in-program data crit sec")
  m_cpc_cpucoredata->wxconditionIPC2InProgramData.Enter() ;
  //While modifying the map prevent the concurrent reading of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("SAX2IPCcurrentCPUdataHandler startDocument: after entering IPC to "
    "in-program data crit sec")

  m_stdmap_wCoreNumber2fUsage.clear() ;
  m_stdmap_wCoreNumber2fMultiplier.clear() ;
  m_stdmap_wCoreNumber2fReferenceClock.clear() ;
  m_stdmap_wCoreNumber2fTempInDegCelsius.clear() ;
  m_stdmap_wCoreNumber2fVoltageInVolt.clear() ;
  m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock.clear() ;
}

  void SAX2IPCcurrentCPUdataHandler::startElement
    (
      const   XMLCh * const    cp_xmlchURI,
      const   XMLCh * const    cp_xmlchLocalName,
      const   XMLCh * const    cp_xmlchQualifiedName,
      const   XERCES_CPP_NAMESPACE::Attributes & cr_xerces_attributes
    )
  {
    if( //If strings equal.
      ! wcscmp( cp_xmlchLocalName, L"core" )
      )
    {
      float fValue ;
      WORD wValue ;
      if( ConvertXercesAttributesValue<WORD>(
          cr_xerces_attributes
          , wValue
          , L"number" )
        )
      {
        //While modifying the map prevent the concurrent reading of the map.
  //      wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
        LOGN("SAX2IPCcurrentCPUdataHandler startElement before entering "
          "critical section")
        m_wxcriticalsection.Enter() ;
        LOGN("SAX2IPCcurrentCPUdataHandler startElement after entering "
          "critical section")

        LOGN("SAX2IPCcurrentCPUdataHandler core"
#ifdef _DEBUG
          ":" << wValue
#endif
          )
        if( ConvertXercesAttributesValue<float>(
          cr_xerces_attributes
          , fValue
          , L"load" )
          )
        {
  //        m_stdset_fUsage.insert( fValue ) ;
          m_stdmap_wCoreNumber2fUsage.insert( std::pair<WORD,float> (
              wValue, fValue ) ) ;
        }
        if( ConvertXercesAttributesValue<float>(
          cr_xerces_attributes
          , fValue
          , L"temp_in_deg_Celsius" )
          )
        {
          m_stdmap_wCoreNumber2fTempInDegCelsius.insert( std::pair<WORD,float> (
              wValue, fValue ) ) ;
        }
        VoltageAndMultiAndRefClock voltageandmultiandrefclock ;
        if( ConvertXercesAttributesValue<float>(
          cr_xerces_attributes
          , fValue
          , L"multiplier" )
          )
        {
//          m_stdmap_wCoreNumber2fMultiplier.insert( std::pair<WORD,float> (
//              wValue, fValue ) ) ;
          voltageandmultiandrefclock.m_fMultiplier = fValue ;
        }
        if( ConvertXercesAttributesValue<float>(
          cr_xerces_attributes
          , fValue
          , L"reference_clock_in_MHz" )
          )
        {
          LOGN("SAX2IPCcurrentCPUdataHandler reference_clock_in_MHz attribute"
#ifdef _DEBUG
            ":" << fValue
#endif
            )
//          m_stdmap_wCoreNumber2fReferenceClock.insert( std::pair<WORD,float> (
//              wValue, fValue ) ) ;
          voltageandmultiandrefclock.m_fReferenceClock = fValue ;
        }
        if( ConvertXercesAttributesValue<float>(
          cr_xerces_attributes
          , fValue
          , L"voltage_in_Volt" )
          )
        {
          LOGN("SAX2IPCcurrentCPUdataHandler voltage_in_Volt attribute")
//          m_stdmap_wCoreNumber2fVoltageInVolt.insert( std::pair<WORD,float> (
//              wValue, fValue ) ) ;
          voltageandmultiandrefclock.m_fVoltageInVolt = fValue ;
        }
        LOGN("SAX2IPCcurrentCPUdataHandler before inserting into container")
        m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock.insert(
          std::pair<WORD,VoltageAndMultiAndRefClock>( wValue ,
              voltageandmultiandrefclock )
          ) ;
        LOGN("SAX2IPCcurrentCPUdataHandler startElement before leaving "
          "critical section")
        m_wxcriticalsection.Leave() ;
        LOGN("SAX2IPCcurrentCPUdataHandler startElement after leaving "
          "critical section")
      }
//      if( ConvertXercesAttributesValue<float>(
//          cr_xerces_attributes
//          , fValue
//          , L"voltage" )
//        )
//      {
//        m_stdset_fUsage.insert( fVoltage ) ;
//      }
    }
  }
