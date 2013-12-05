/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * SAX2IPCcurrentCPUdataHandler.cpp
 *
 *  Created on: Jun 15, 2010
 *      Author: Stefan
 */
#include "SAX2IPCcurrentCPUdataHandler.hpp"
#include <ModelData/CPUcoreData.hpp> //class CPUcoreData
#include <ModelData/ModelData.hpp> //class ModelData
#include <Xerces/XercesString.hpp> //Xerces::ansi_or_wchar_string_compare(...)
//ConvertXercesAttributesValue(...)
#include <Xerces/XercesAttributesHelper.hpp>

#include <xercesc/sax2/Attributes.hpp>
#include <wx/defs.h> //wxLongLong_t
#include <wx/stopwatch.h> //::wxGetLocalTimeMillis()
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
  LOGN("before leaving IPC to in-program data crit sec")
  m_cpc_cpucoredata->wxconditionIPC2InProgramData.Leave() ;
  LOGN("after leaving IPC to in-program data crit sec")
}

BYTE SAX2IPCcurrentCPUdataHandler::GetCurrentVoltageAndFrequency(
  float & r_fVoltageInVolt
  //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
  , float & r_fMultiplier
  , float & r_fReferenceClockInMHz
  , WORD wCoreID
  )
{
  LOGN("before entering critical section")
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("after entering critical section")

  std::map<WORD,/*VoltageAndMultiAndRefClock*/ CPUcoreVoltageAndFrequency>::
    const_iterator c_iter =
      m_stdmap_wCoreNumber2CPUcoreVoltageAndFrequency.find(wCoreID) ;
  if( c_iter != m_stdmap_wCoreNumber2CPUcoreVoltageAndFrequency.end() )
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
  LOGN("before entering critical section")
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("after entering critical section")

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
  LOGN("before entering critical section")
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("after entering critical section")

  WORD wSize = m_stdmap_wCoreNumber2fUsage.size() ;
  return wSize ;
}

float SAX2IPCcurrentCPUdataHandler::GetTemperatureInCelsius( WORD wCoreID )
{
  LOGN("before entering critical section")
  //Prevent the concurrent modification of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("after entering critical section")

  std::map<WORD,float>::const_iterator c_iter =
      m_stdmap_wCoreNumber2fTempInDegCelsius.find(wCoreID) ;
  if( c_iter != m_stdmap_wCoreNumber2fTempInDegCelsius.end() )
  {
    return c_iter->second ;
  }
  return __FLT_MIN__ ;
}

float SAX2IPCcurrentCPUdataHandler::GetThrottleLevel(unsigned coreID)
{
  LOGN( "before entering critical section")
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN( "after entering critical section")

  std::map<WORD,/*VoltageAndMultiAndRefClock*/ CPUcoreVoltageAndFrequency>::
    const_iterator c_iter =
      m_stdmap_wCoreNumber2CPUcoreVoltageAndFrequency.find(coreID) ;
  if( c_iter != m_stdmap_wCoreNumber2CPUcoreVoltageAndFrequency.end() )
  {
    return c_iter->second.m_fThrottleRatio;
  }
  return 0.0f;
}

inline void SAX2IPCcurrentCPUdataHandler::HandleCoreXMLelement_Inline(
  const XERCES_CPP_NAMESPACE::Attributes & cr_xerces_attributes
  )
{
  float fValue ;
  WORD wValue ;
  if( ConvertXercesAttributesValue<WORD>(
      cr_xerces_attributes
      , wValue
      , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
      (const XMLCh *) L"number" )
    )
  {
    //While modifying the map prevent the concurrent reading of the map.
  //      wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
    LOGN("before entering critical section")
    m_wxcriticalsection.Enter() ;
    LOGN("after entering critical section")

    LOGN("core"
  #ifdef _DEBUG
      ":" << wValue
  #endif
      )
    if( ConvertXercesAttributesValue<float>(
      cr_xerces_attributes
      , fValue
      , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
      (const XMLCh *) L"load" )
      )
    {
  //        m_stdset_fUsage.insert( fValue ) ;
      m_stdmap_wCoreNumber2fUsage.insert( std::pair<WORD,float> (
          wValue, fValue ) ) ;
    }
    if( ConvertXercesAttributesValue<float>(
      cr_xerces_attributes
      , fValue
      , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
      (const XMLCh *) L"temp_in_deg_Celsius" )
      )
    {
      m_stdmap_wCoreNumber2fTempInDegCelsius.insert( std::pair<WORD,float> (
          wValue, fValue ) ) ;
    }
    //VoltageAndMultiAndRefClock voltageandmultiandrefclock ;
    CPUcoreVoltageAndFrequency cpucorevoltageandfrequency;
    if( ConvertXercesAttributesValue<float>(
      cr_xerces_attributes
      , fValue
      , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
      (const XMLCh *) L"throttle_ratio" )
      )
    {
      /*voltageandmultiandrefclock*/cpucorevoltageandfrequency.m_fThrottleRatio
        = fValue ;
    }
    if( ConvertXercesAttributesValue<float>(
      cr_xerces_attributes
      , fValue
      , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
      (const XMLCh *) L"multiplier" )
      )
    {
  //          m_stdmap_wCoreNumber2fMultiplier.insert( std::pair<WORD,float> (
  //              wValue, fValue ) ) ;
      /*voltageandmultiandrefclock*/cpucorevoltageandfrequency.m_fMultiplier
        = fValue ;
    }
    if( ConvertXercesAttributesValue<float>(
      cr_xerces_attributes
      , fValue
      , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
      (const XMLCh *) L"reference_clock_in_MHz" )
      )
    {
      LOGN("reference_clock_in_MHz attribute"
  #ifdef _DEBUG
        ":" << fValue
  #endif
        )
  //          m_stdmap_wCoreNumber2fReferenceClock.insert( std::pair<WORD,float> (
  //              wValue, fValue ) ) ;
        /*voltageandmultiandrefclock*/cpucorevoltageandfrequency.
          m_fReferenceClock = fValue ;
    }
    if( ConvertXercesAttributesValue<float>(
      cr_xerces_attributes
      , fValue
      , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
      (const XMLCh *) L"voltage_in_Volt" )
      )
    {
      LOGN("voltage_in_Volt attribute")
  //          m_stdmap_wCoreNumber2fVoltageInVolt.insert( std::pair<WORD,float> (
  //              wValue, fValue ) ) ;
    /*voltageandmultiandrefclock*/cpucorevoltageandfrequency.m_fVoltageInVolt
      = fValue ;
    }
    LOGN("before inserting into container")
    m_stdmap_wCoreNumber2CPUcoreVoltageAndFrequency.insert(
      std::pair<WORD,/*VoltageAndMultiAndRefClock*/ CPUcoreVoltageAndFrequency>
      ( wValue, /*voltageandmultiandrefclock*/ cpucorevoltageandfrequency ) );

    LOGN("before leaving critical section")
    m_wxcriticalsection.Leave() ;
    LOGN("after leaving critical section")
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

SAX2IPCcurrentCPUdataHandler::~SAX2IPCcurrentCPUdataHandler()
{
  LOGN("")
}

void SAX2IPCcurrentCPUdataHandler::startDocument()
{
//  m_stdset_fUsage.clear() ;
  LOGN("before entering IPC to in-program data crit sec")
  m_cpc_cpucoredata->wxconditionIPC2InProgramData.Enter() ;
  LOGN("after entering IPC to in-program data crit sec")

  LOGN("before locking data crit sec")
  //While modifying the map prevent the concurrent reading of the map.
  wxCriticalSectionLocker wxcriticalsectionlocker( m_wxcriticalsection ) ;
  LOGN("after locking the data crit sec")

  m_stdmap_wCoreNumber2fUsage.clear() ;
//  m_stdmap_wCoreNumber2fMultiplier.clear() ;
//  m_stdmap_wCoreNumber2fReferenceClock.clear() ;
  m_stdmap_wCoreNumber2fTempInDegCelsius.clear() ;
//  m_stdmap_wCoreNumber2fVoltageInVolt.clear() ;
  m_stdmap_wCoreNumber2CPUcoreVoltageAndFrequency.clear() ;
  LOGN("before destroying the data crit sec locking object")
}

  void SAX2IPCcurrentCPUdataHandler::startElement
    (
      const XMLCh * const cp_xmlchURI,
      const XMLCh * const cp_xmlchLocalName,
      const XMLCh * const cp_xmlchQualifiedName,
      const XERCES_CPP_NAMESPACE::Attributes & cr_xerces_attributes
    )
  {
    std::string std_strLocalName = Xerces::ToStdString(cp_xmlchLocalName);
    LOGN_DEBUG("XML element: " << std_strLocalName)
    if( //If strings equal.
        ! Xerces::ansi_or_wchar_string_compare(
        //Explicitly cast to "wchar_t *" to avoid Linux g++ warning.
        //(wchar_t *)
          cp_xmlchLocalName, ANSI_OR_WCHAR("core")
        )
      )
    {
      HandleCoreXMLelement_Inline(cr_xerces_attributes);
    }
    if( //If strings equal.
        ! Xerces::ansi_or_wchar_string_compare(
        //Explicitly cast to "wchar_t *" to avoid Linux g++ warning.
        //(wchar_t *)
          cp_xmlchLocalName, ANSI_OR_WCHAR("DVFS")
        )
      )
    {
      LOGN_DEBUG("before locking")
      wxCriticalSectionLocker wxcriticalsectionlocker(m_wxcriticalsection) ;
//      bool bRunning;
      LOGN_DEBUG("after locking")
      if( ConvertXercesAttributesValue<bool>(
          cr_xerces_attributes
          //This value is the last timecode of the last overheat at the time
          //on the machine with the service.
          , m_bDVFSfromServiceIsRunning
          , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
          (const XMLCh *) L"running" )
        )
      {
        LOGN_DEBUG("running:" << m_bDVFSfromServiceIsRunning)
      }
    }
    if( //If strings equal.
        ! Xerces::ansi_or_wchar_string_compare(
        //Explicitly cast to "wchar_t *" to avoid Linux g++ warning.
        //(wchar_t *)
          cp_xmlchLocalName, ANSI_OR_WCHAR("too_hot")
        )
      )
    {
//      LOGN("\"too hot\" XML element")
      wxCriticalSectionLocker wxcriticalsectionlocker(m_wxcriticalsection) ;
      wxLongLong_t wxlonglong_t;
      if( ConvertXercesAttributesValue<wxLongLong_t>(
          cr_xerces_attributes
          //This value is the last timecode of the last overheat at the time
          //on the machine with the service.
          , wxlonglong_t
          , //Explicitly cast to "const XMLCh *" to avoid Linux g++ warning.
          (const XMLCh *) L"last_time_too_hot" )
        )
      {
        LOGN("Attribute value for \"last_time_too_hot\":" << wxlonglong_t)
//        if( wxlonglong_t )
//        {
//          if( m_wxlonglong_tLastTimeTooHot )
//          {
//            if( wxlonglong_t - m_wxlonglong_tLastTimeTooHotInMillis < 1000 )
//              m_bTooHotLastTime = true;
//            else
//              m_bTooHotLastTime = false;
//          }
//          else
//            m_bTooHotLastTime = false;
//          m_wxlonglong_tLastTimeTooHotInMillis = wxlonglong_t;
//        }
//        else
//          m_bTooHotLastTime = false;

        //Must specify 1 of the base classes where "mp_model" is declared
        //because g++ error: "error: reference to `mp_model' is ambiguous"
//        I_CPUcontroller::mp_model->m_cpucoredata.m_llLastTimeTooHot =
//          wxlonglong_t;

        if( m_wxlonglong_tLastTimeTooHotFromFromIPC)
        {
          if( m_wxlonglong_tLastTimeTooHotFromFromIPC < wxlonglong_t)
          {
            //The host where the data comes from may significantly differ in its
            //time. So get the current time for this machine.
            m_llLastTimeTooHot = //wxlonglong_t;
              ::wxGetLocalTimeMillis().GetValue();
            m_wxlonglong_tLastTimeTooHotFromFromIPC = wxlonglong_t;
          }
        }
        else //=0 <=> Not assigned yet.
          m_wxlonglong_tLastTimeTooHotFromFromIPC = wxlonglong_t;
      }
//      m_bTooHotLastTime = true;
    }
  }
