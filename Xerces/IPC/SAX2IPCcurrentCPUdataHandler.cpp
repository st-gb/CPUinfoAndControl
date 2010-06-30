/*
 * SAX2IPCcurrentCPUdataHandler.cpp
 *
 *  Created on: Jun 15, 2010
 *      Author: Stefan
 */
#include "SAX2IPCcurrentCPUdataHandler.hpp"
#include <Xerces/XercesAttributesHelper.hpp>

#include <xercesc/sax2/Attributes.hpp>
#include <wx/thread.h> //for class wxCriticalSectionLocker

//void SAX2IPCcurrentCPUdataHandler::Parse( BYTE arby [] , DWORD dwSizeInBytes)
//{
//}

BYTE SAX2IPCcurrentCPUdataHandler::GetCurrentVoltageAndFrequency(
  float & r_fVoltageInVolt
  //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
  , float & r_fMultiplier
  , float & r_fReferenceClockInMHz
  , WORD wCoreID
  )
{
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
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
#ifdef _DEBUG
  float fLoad ;
#endif
  WORD wIndex = 0 ;
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
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
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  WORD wSize = m_stdmap_wCoreNumber2fUsage.size() ;
  return wSize ;
}

float SAX2IPCcurrentCPUdataHandler::GetTemperatureInCelsius( WORD wCoreID )
{
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  std::map<WORD,float>::const_iterator c_iter =
      m_stdmap_wCoreNumber2fTempInDegCelsius.find(wCoreID) ;
  if( c_iter != m_stdmap_wCoreNumber2fTempInDegCelsius.end() )
  {
    return c_iter->second ;
  }
  return __FLT_MIN__ ;
}

void SAX2IPCcurrentCPUdataHandler::startDocument()
{
//  m_stdset_fUsage.clear() ;
  //While modifying the map prevent the concurrent reading of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
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
    //While modifying the map prevent the concurrent reading of the map.
    wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
    if( ! wcscmp( cp_xmlchLocalName, L"core" )  )
    {
      float fValue ;
      WORD wValue ;
      if( ConvertXercesAttributesValue<WORD>(
          cr_xerces_attributes
          , wValue
          , L"number" )
        )
      {
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
//          m_stdmap_wCoreNumber2fVoltageInVolt.insert( std::pair<WORD,float> (
//              wValue, fValue ) ) ;
          voltageandmultiandrefclock.m_fVoltageInVolt = fValue ;
        }
        m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock.insert(
          std::pair<WORD,VoltageAndMultiAndRefClock>( wValue ,
              voltageandmultiandrefclock )
          ) ;
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
