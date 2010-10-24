/*
 * SAX2UserInterfaceConfigHandler.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

#include <ModelData/ModelData.hpp> //class Model
#include <Xerces/SAX2UserInterfaceConfigHandler.hpp>
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
    const XMLCh * const cp_xmlchURI,
    const XMLCh * const cp_xmlchLocalName,
    const XMLCh * const cp_xmlchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    std::wstring stdwstr = L"log_file_name" ;
    //_MUST use XMLString::transcode() Linux: a wcscmp comparison does not work
    // here.
    char * p_chLocalName = XERCES_CPP_NAMESPACE::XMLString::transcode(
      cp_xmlchLocalName) ;
    DEBUG_WCOUTN( L"local name:" << cp_xmlchLocalName << L"log_file_name: "
      << stdwstr )
#ifdef _DEBUG
    if( strlen(p_chLocalName ) > 3 )
    {
      char * p_ch2 = (char *) cp_xmlchLocalName ;
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
      (wchar_t *) cp_xmlchLocalName ,
      stdwstr.c_str()
      )
    if( ! ansi_or_wchar_string_compare( cp_xmlchLocalName,
        ANSI_OR_WCHAR("Dynamic_Voltage_and_Frequency_Scaling") )
      )
    {
      bool bStartDVFSatStartup ;
      if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         ,"start_at_startup"
         ,bStartDVFSatStartup )
       )
      {
        LOGN("bool value for bStartDVFSatStartup:" << bStartDVFSatStartup )
        m_p_model->m_userinterfaceattributes.m_bStartDVFSatStartup =
          bStartDVFSatStartup ;
      }
      else
        LOGN("getting bool value for bStartDVFSatStartup failed ")
    }
    if( //If strings equal.
      ! ansi_or_wchar_string_compare( cp_xmlchLocalName,
          ANSI_OR_WCHAR("log_file_name") )
      )
    {
      bool bAppendProcessID ;
      if( //ConvertXercesAttributesValue<bool>(
        XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         ,"append_process_ID"
         ,bAppendProcessID )
       )
      {
        LOGN("bool value for append_process_ID:" << bAppendProcessID )
        m_p_model->m_bAppendProcessID = bAppendProcessID ;
      }
      else
        LOGN("getting bool value for append_process_ID failed ")
    }
    else
    {
    if( //If strings equal.
      ! ansi_or_wchar_string_compare( cp_xmlchLocalName,
        ANSI_OR_WCHAR("tooltip") )
      )
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
    if( //If strings equal.
      ! ansi_or_wchar_string_compare( cp_xmlchLocalName,
        ANSI_OR_WCHAR("main_frame") )
      )
    {
      HandleMainFrameXMLelement(cr_xercesc_attributes) ;
    }
    if( //If strings equal.
      ! ansi_or_wchar_string_compare( cp_xmlchLocalName,
        ANSI_OR_WCHAR("voltage_and_frequency_settings_dialog") )
      )
    {
      HandleVoltageAndFrequencySettingsDialogXMLelement(cr_xercesc_attributes) ;
    }
    }
    XERCES_CPP_NAMESPACE::XMLString::release( & p_chLocalName ) ;
  }//SAX2UserInterfaceConfigHandler::startElement(...)
}
