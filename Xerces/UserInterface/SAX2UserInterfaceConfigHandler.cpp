/*
 * SAX2UserInterfaceConfigHandler.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

#include <ModelData/ModelData.hpp> //class Model
//SUPPRESS_UNUSED_VARIABLE_WARNING(...)
#include <preprocessor_macros/suppress_unused_variable.h>
#include <Xerces/UserInterface/SAX2UserInterfaceConfigHandler.hpp>
//PossiblyHandleLoggingExclusionFilter_Inline(...)
#include <Xerces/XercesHelper.hpp>
//XercesAttributesHelper::GetAttributeValue(...)
#include <Xerces/XercesString.hpp> //ansi_or_wchar_string_compare(...)
#include <Xerces/XercesAttributesHelper.hpp>//ConvertXercesAttributesValue(...)

#include <wchar.h> //wcscmp(...)

  #define LOG_FILE_NAME_ANSI_LITERAL "log_file_name"
  #define LOG_FILE_NAME_WCHAR_LITERAL L ## LOG_FILE_NAME_ANSI_LITERAL

namespace Xerces
{

  SAX2UserInterfaceConfigHandler::SAX2UserInterfaceConfigHandler(
    Model & model ,
    UserInterface * p_userinterface
    )
  {
    m_p_model = & model ;
    m_p_userinterface = p_userinterface ;
  }

  SAX2UserInterfaceConfigHandler::~SAX2UserInterfaceConfigHandler()
  {
    // TODO Auto-generated destructor stub
  }

  void SAX2UserInterfaceConfigHandler::
    HandleDynamicVoltageAndFrequencyScalingXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
    char * p_chXMLattributeName =
      //Cast to avoid g++ (version 4.5.2) warning
      //"deprecated conversion from string constant to 'char*' "
      (char *)
      "start_at_startup" ;
//    bool bStartDVFSatStartup ;
    if( XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes
       ,//"start_at_startup"
       p_chXMLattributeName
       ,//bStartDVFSatStartup
       m_p_model->m_userinterfaceattributes.m_bStartDVFSatStartup
       )
     )
    {
      LOGN("bool value for \"" << p_chXMLattributeName << "\":"
        << //bStartDVFSatStartup
        m_p_model->m_userinterfaceattributes.m_bStartDVFSatStartup
        )
//      m_p_model->m_userinterfaceattributes.m_bStartDVFSatStartup =
//        bStartDVFSatStartup ;
    }
    else
      LOGN("Getting bool value for \"" << p_chXMLattributeName << "\"failed.")
  }

  void SAX2UserInterfaceConfigHandler::HandleLogFileNameXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
//    bool bAppendProcessID ;
    char * p_chXMLattributeName =
      //Cast to avoid g++ (version 4.5.2) warning
      //"deprecated conversion from string constant to 'char*' "
      (char *)
      "append_process_ID" ;
    if( //ConvertXercesAttributesValue<bool>(
      XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes
       ,//"append_process_ID"
       p_chXMLattributeName
       ,//bAppendProcessID
       m_p_model->m_bAppendProcessID
       )
     )
    {
      LOGN("bool value for \"" << p_chXMLattributeName << "\":"
        << //bAppendProcessID
        m_p_model->m_bAppendProcessID
        )
//      m_p_model->m_bAppendProcessID = bAppendProcessID ;
    }
    else
      LOGN("Getting bool value for \"" << p_chXMLattributeName << "\"failed.")
  }

  void SAX2UserInterfaceConfigHandler::HandleMainFrameXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    unsigned short w ;
    if( ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
       ,"width_in_pixels"
       ,w )
     )
     m_p_model->m_userinterfaceattributes.m_wMainFrameWidthInPixels = w ;
    if( ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
       ,"height_in_pixels"
       ,w )
     )
     m_p_model->m_userinterfaceattributes.m_wMainFrameHeightInPixels = w ;
    XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes
       ,"show_at_startup"
       ,m_p_model->m_userinterfaceattributes.m_bShowMainFrameAtStartup
      ) ;
    if( ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
       ,"top_left_corner_x_coordinate_in_pixels"
       ,w )
     )
      m_p_model->m_userinterfaceattributes.
        m_wMainFrameTopLeftCornerXcoordinateInPixels = w ;
    if( ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
       ,"top_left_corner_y_coordinate_in_pixels"
       ,w )
     )
      m_p_model->m_userinterfaceattributes.
        m_wMainFrameTopLeftCornerYcoordinateInPixels = w ;
  }

  void SAX2UserInterfaceConfigHandler::HandleServiceXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
    LOGN("service element")
    std::string stdstrAttributeName = "pipe_name" ;
    std::wstring stdwstr //= Xerces::ConvertXercesStringToStdWstring(
      //cr_xercesc_attributes.getLocalName() ) ;
      ;
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         , stdstrAttributeName.c_str()
         , m_p_model->m_stdwstrPipeName ) == SUCCESS
      )
    {
      LOGN("Getting attribute value for \"" << stdstrAttributeName
        << "\" succeeded" )
    }
    else
    {
      LOGN("Getting attribute value for \"" << stdstrAttributeName
        << "\" failed")
    }
  }

  inline void SAX2UserInterfaceConfigHandler::HandleToolTipXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
    LOGN("tooltip element")
    std::string stdstr = "delay_in_milliseconds" ;
    WORD w ;
    if( ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
       , //stdstr.c_str()
       "delay_in_milliseconds"
       ,w )
     )
    {
      LOGN("Getting attribute value for \"" << stdstr << "\" succeeded" )
      m_p_model->m_userinterfaceattributes.m_wToolTipDelay = w ;
    }
    else
    {
      LOGN("Getting attribute value for \"" << stdstr << "\" failed")
    }
  }

  void SAX2UserInterfaceConfigHandler::
    HandleVoltageAndFrequencySettingsDialogXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    float f ;
    std::string stdstrAttributeName ;
    XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes
       ,"select_all_CPU_cores"
       ,m_p_model->m_userinterfaceattributes.m_bSelectAllCPUcores
      ) ;
    if( ConvertXercesAttributesValue<float>(
      cr_xercesc_attributes
       ,"operating_safety_margin_in_Volt"
       ,f )
     )
      m_p_model->m_userinterfaceattributes.
        m_fOperatingSafetyMarginInVolt = f ;
    stdstrAttributeName = "prevent_voltage_above_default_voltage" ;
    if( XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes
       ,stdstrAttributeName.c_str()
       ,m_p_model->m_userinterfaceattributes.
         m_bPreventVoltageAboveDefaultVoltage )
     )
    {
      LOGN("bool value for " << stdstrAttributeName << ":" << m_p_model->
        m_userinterfaceattributes.m_bPreventVoltageAboveDefaultVoltage )
    }
    else
      LOGN("getting bool value for " << stdstrAttributeName << " failed ")
    stdstrAttributeName = "prevent_voltage_below_lowest_stable_voltage" ;
    if( XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes
       ,stdstrAttributeName.c_str()
       ,m_p_model->m_userinterfaceattributes.
         m_bPreventVoltageBelowLowestStableVoltage )
     )
    {
      LOGN("bool value for " << stdstrAttributeName << ":" << m_p_model->
        m_userinterfaceattributes.m_bPreventVoltageBelowLowestStableVoltage )
    }
    else
      LOGN("getting bool value for " << stdstrAttributeName << " failed ")
  }

  void SAX2UserInterfaceConfigHandler::startElement
    (
    const XMLCh * const cpc_xmlchURI,
    const XMLCh * const cpc_xmlchLocalName,
    const XMLCh * const cpc_xmlchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    std::wstring stdwstr = L"log_file_name" ;
    //_MUST use XMLString::transcode() Linux: a wcscmp comparison does not work
    // here.
    char * p_chLocalName = XERCES_CPP_NAMESPACE::XMLString::transcode(
      cpc_xmlchLocalName) ;
    DEBUG_WCOUTN( L"local name:" << cpc_xmlchLocalName << L"log_file_name: "
      << stdwstr )
#ifdef _DEBUG
    if( strlen(p_chLocalName ) > 3 )
    {
      char * p_ch2 = (char *) cpc_xmlchLocalName ;
      SUPPRESS_UNUSED_VARIABLE_WARNING(p_ch2)
      DEBUG_WCOUTN( L"local name bytes:"
        << ( * p_ch2 ? *p_ch2 : ' ' )
        << ( *( p_ch2 + 1 ) ? *( p_ch2 + 1 ) : ' ' )
        << ( *( p_ch2 + 2 ) ? *( p_ch2 + 2 ) : ' ' )
        << ( *( p_ch2 + 3 ) ? *( p_ch2 + 3 ) : ' ' )
        << ( *( p_ch2 + 4 ) ? *( p_ch2 + 4 ) : ' ' )
        << ( *( p_ch2 + 5 ) ? *( p_ch2 + 5 ) : ' ' )
        << ( *( p_ch2 + 6 ) ? *( p_ch2 + 6 ) : ' ' )
        << ( *( p_ch2 + 7 ) ? *( p_ch2 + 7 ) : ' ' )
        )
    }
#endif //#ifdef _DEBUG
//    std::string stdstrLocalName(p_chLocalName) ;
    DEBUG_WPRINTFN( L"local name:%ls log_file_name:%ls" ,
      (wchar_t *) cpc_xmlchLocalName ,
      stdwstr.c_str()
      )
    if( PossiblyHandleLoggingExclusionFilter_Inline(
      cpc_xmlchLocalName,
      cr_xercesc_attributes)
      )
      ;
    else
    //TODO inperformant string comparison if multiple strings to compare start
    //with the same prefix: "e.g." "start", "start_at_startup"
    //_possibly_ do something like this: a _trie_ with a pointer to
    // a handler function for the string at its leaves.
    if( ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("Dynamic_Voltage_and_Frequency_Scaling") )
      )
    {
      HandleDynamicVoltageAndFrequencyScalingXMLelement(cr_xercesc_attributes) ;
    }
    else if( //If strings equal.
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
          ANSI_OR_WCHAR("log_file_name") )
      )
    {
      HandleLogFileNameXMLelement( cr_xercesc_attributes);
    }
    else if( //If strings equal.
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("main_frame") )
      )
    {
      HandleMainFrameXMLelement(cr_xercesc_attributes) ;
    }
    else if( //If strings equal.
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("service") )
      )
    {
      HandleServiceXMLelement(cr_xercesc_attributes) ;
    }
    else if( //If strings equal.
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("tooltip") )
      )
    {
      HandleToolTipXMLelement(cr_xercesc_attributes) ;
    }
    else if( //If strings equal.
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("voltage_and_frequency_settings_dialog") )
      )
    {
      HandleVoltageAndFrequencySettingsDialogXMLelement(cr_xercesc_attributes) ;
    }
    XERCES_CPP_NAMESPACE::XMLString::release( & p_chLocalName ) ;
  }//SAX2UserInterfaceConfigHandler::startElement(...)
}
