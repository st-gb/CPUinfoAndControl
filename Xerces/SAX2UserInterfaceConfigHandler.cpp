/*
 * SAX2UserInterfaceConfigHandler.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

#include <ModelData/ModelData.hpp> //class Model
#include <Xerces/SAX2UserInterfaceConfigHandler.hpp>
//XercesAttributesHelper::GetAttributeValue(...)
#include <Xerces/XercesAttributesHelper.hpp>//ConvertXercesAttributesValue(...)

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
  }

  void SAX2UserInterfaceConfigHandler::startElement
    (
    const XMLCh * const cp_xmlchURI,
    const XMLCh * const cp_xmlchLocalName,
    const XMLCh * const cp_xmlchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    if( //If strings equal.
      ! wcscmp(
      //Explicitly cast to "wchar_t *" to avoid Linux g++ warning.
      (wchar_t *) cp_xmlchLocalName, L"log_file_name" )
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
    if( //If strings equal.
      ! wcscmp(
      //Explicitly cast to "wchar_t *" to avoid Linux g++ warning.
      (wchar_t *) cp_xmlchLocalName, L"tooltip" )
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
      ! wcscmp(
      //Explicitly cast to "wchar_t *" to avoid Linux g++ warning.
      (wchar_t *) cp_xmlchLocalName, L"main_frame" )
      )
    {
      HandleMainFrameXMLelement(cr_xercesc_attributes) ;
    }
    if( //If strings equal.
      ! wcscmp(
      //Explicitly cast to "wchar_t *" to avoid Linux g++ warning.
      (wchar_t *) cp_xmlchLocalName, L"voltage_and_frequency_settings_dialog" )
      )
    {
      HandleVoltageAndFrequencySettingsDialogXMLelement(cr_xercesc_attributes) ;
    }
  }
}
