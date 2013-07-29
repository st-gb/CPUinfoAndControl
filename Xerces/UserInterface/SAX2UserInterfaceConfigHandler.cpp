/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * SAX2UserInterfaceConfigHandler.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

//ConvertStdStringToTypename(...)
#include <Controller/character_string/ConvertStdStringToTypename.hpp>
#include <ModelData/ModelData.hpp> //class Model
//SUPPRESS_UNUSED_VARIABLE_WARNING(...)
#include <preprocessor_macros/suppress_unused_variable.h>
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
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
    : m_trieExcludeFromLogging(255,0),
      m_ui16t_WisdomNumber(0),
      m_mainFrameAttributes(model.m_userinterfaceattributes.mainframe)
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
       m_p_model->m_logfileattributes.m_bAppendProcessID
       )
     )
    {
      LOGN("bool value for \"" << p_chXMLattributeName << "\":"
        << //bAppendProcessID
        m_p_model->m_logfileattributes.m_bAppendProcessID
        )
//      m_p_model->m_bAppendProcessID = bAppendProcessID ;
    }
    else
      LOGN("Getting bool value for \"" << p_chXMLattributeName << "\"failed.")
  }

  void SAX2UserInterfaceConfigHandler::GetFontSizeAttributeValue(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes,
    const std::string & attrName,
    int & nFontSize)
  {
    std::string std_strAttributeValue;
    if( XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes,
      attrName.c_str(),
      std_strAttributeValue
      ) == XercesAttributesHelper::getting_attribute_value_succeeded )
    {
      LOGN_DEBUG( FULL_FUNC_NAME << " successfully got font size for \""
        << attrName << "\"")
      bool convertError = false;
      if( std_strAttributeValue.length() > 0 && std_strAttributeValue.at(0)
          == '+' )
      {
        int nAddToFontSize;
        if( ConvertStdStringToTypename(nAddToFontSize,
            std_strAttributeValue.substr(1) ) )
          nFontSize =
            m_p_model->m_userinterfaceattributes.s_defaultFontSizeInPoint +
            nAddToFontSize;
        else
          convertError = true;
      }
      else if( std_strAttributeValue.length() > 0 && std_strAttributeValue.at(0)
          == '-' )
      {
        int nSubtractFromFontSize;
        if( ConvertStdStringToTypename(nSubtractFromFontSize,
            std_strAttributeValue.substr(1) ) )
          nFontSize =
            m_p_model->m_userinterfaceattributes.s_defaultFontSizeInPoint -
            nSubtractFromFontSize;
        else
          LOGN_ERROR( FULL_FUNC_NAME << "Failed to get font size for "
            << attrName)
      }
      else
        if( ! ConvertStdStringToTypename(nFontSize,
            std_strAttributeValue.substr(1) ) )
          convertError = true;
      if( convertError )
        LOGN_ERROR( FULL_FUNC_NAME << " Failed to get font size for \""
          << attrName << "\"")
    }
    else
      LOGN_ERROR( FULL_FUNC_NAME << " Failed to get XML attr val for \""
        << attrName << "\"")
 }

  void SAX2UserInterfaceConfigHandler::HandleMainFrameXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    //unsigned short w ;
    ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
       ,"width_in_pixels"
       ,m_mainFrameAttributes.m_widthInPixels );
    ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
      ,"height_in_pixels"
      ,m_mainFrameAttributes.m_heightInPixels );
    XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes
      ,"show_at_startup"
      ,m_p_model->m_userinterfaceattributes.m_bShowMainFrameAtStartup
      ) ;
    ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
      ,"top_left_corner_x_coordinate_in_pixels"
      ,m_mainFrameAttributes.m_topLeftCornerXcoordinateInPixels );
    ConvertXercesAttributesValue<WORD>(
      cr_xercesc_attributes
      ,"top_left_corner_y_coordinate_in_pixels"
      ,m_mainFrameAttributes.m_topLeftCornerYcoordinateInPixels );
//    ConvertXercesAttributesValue<int>(
    GetFontSizeAttributeValue(
      cr_xercesc_attributes
      ,"CPU_core_voltage_scale_point_size"
      ,m_mainFrameAttributes.m_nVoltageScaleSizeInPoint
     );
//    ConvertXercesAttributesValue<int>(
    GetFontSizeAttributeValue(
      cr_xercesc_attributes
      ,"current_CPU_core_info_point_size"
      ,m_mainFrameAttributes.m_nCurrentCPUcoreInfoSizeInPoint
     );
    ConvertXercesAttributesValue<int>(
      cr_xercesc_attributes
      ,"CPU_core_frequency_scale_point_size"
      ,m_mainFrameAttributes.m_nCPUcoreFrequencyScaleSizeInPoint
     );
  }

  void SAX2UserInterfaceConfigHandler::HandleServiceXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
    LOGN("service element")
    std::string stdstrAttributeName = "pipe_name" ;
    std::wstring stdwstr //= Xerces::ConvertXercesStringToStdWstring(
      //cr_xercesc_attributes.getLocalName() ) ;
      ;
#ifdef _WIN32 //pre-defined preprocessor macro (also 64 bit) for Windows
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         , stdstrAttributeName.c_str()
         , m_p_model->m_stdwstrPipeName ) == //SUCCESS
           XercesAttributesHelper::getting_attribute_value_succeeded
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
#endif //#ifdef _WIN32 //pre-defined preprocessor macro (also 64 bit) for Windows
  }

  inline void SAX2UserInterfaceConfigHandler::
    HandleTaskBarIcon_CPUcoreUsageXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
    std::string std_strXMLattributeName = "colour" ;
    bool & r_bAttributeValue = m_p_model->m_userinterfaceattributes.
      m_bShowCPUcoreUsagesIconInTaskBar;
    std::string stdstrAttributeValue;
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         , std_strXMLattributeName.c_str()
         , stdstrAttributeValue
        ) == XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
      LOGN( FULL_FUNC_NAME << "--Getting attribute value for \"" <<
        std_strXMLattributeName << "\" succeeded: "
        "value is:" << stdstrAttributeValue )
//      m_p_model->m_userinterfaceattributes.m_wToolTipDelay = w ;
    }
    else
    {
      LOGN_ERROR(FULL_FUNC_NAME << "--Getting attribute value for \""
        << std_strXMLattributeName << "\" failed")
    }
    std_strXMLattributeName = "show_CPU_cores_multipliers" ;
    bool & r_bShowCPUcoresMultipliersIconInTaskBarAttributeValue =
      m_p_model->m_userinterfaceattributes.
      m_bShowCPUcoresMultipliersIconInTaskBar;
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         , std_strXMLattributeName.c_str()
         , r_bShowCPUcoresMultipliersIconInTaskBarAttributeValue
        ) == XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
      LOGN( FULL_FUNC_NAME << "--Getting attribute value for \"" <<
        std_strXMLattributeName << "\" succeeded: "
        "value is:" << r_bShowCPUcoresMultipliersIconInTaskBarAttributeValue )
//      m_p_model->m_userinterfaceattributes.m_wToolTipDelay = w ;
    }
    else
    {
      LOGN("Getting attribute value for \"" << std_strXMLattributeName <<
        "\" failed")
    }
  }

  inline void SAX2UserInterfaceConfigHandler::HandleTaskBarIconXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
    std::string std_strXMLattributeName = "show_CPU_cores_usages" ;
    bool & r_bAttributeValue = m_p_model->m_userinterfaceattributes.
      m_bShowCPUcoreUsagesIconInTaskBar;
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         , std_strXMLattributeName.c_str()
         , r_bAttributeValue
        ) == XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
      LOGN( FULL_FUNC_NAME << "--Getting attribute value for \"" <<
        std_strXMLattributeName << "\" succeeded: "
        "value is:" << r_bAttributeValue )
//      m_p_model->m_userinterfaceattributes.m_wToolTipDelay = w ;
    }
    else
    {
      LOGN("Getting attribute value for \"" << std_strXMLattributeName <<
        "\" failed")
    }
    std_strXMLattributeName = "show_CPU_cores_multipliers" ;
    bool & r_bShowCPUcoresMultipliersIconInTaskBarAttributeValue =
      m_p_model->m_userinterfaceattributes.
      m_bShowCPUcoresMultipliersIconInTaskBar;
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         , std_strXMLattributeName.c_str()
         , r_bShowCPUcoresMultipliersIconInTaskBarAttributeValue
        ) == XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
      LOGN( FULL_FUNC_NAME << "--Getting attribute value for \"" <<
        std_strXMLattributeName << "\" succeeded: "
        "value is:" << r_bShowCPUcoresMultipliersIconInTaskBarAttributeValue )
//      m_p_model->m_userinterfaceattributes.m_wToolTipDelay = w ;
    }
    else
    {
      LOGN("Getting attribute value for \"" << std_strXMLattributeName <<
        "\" failed")
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

  /*template <typename T>*/ void SAX2UserInterfaceConfigHandler::GetAttributeValue(
    const char * const pchAttributeName, bool & value)
  {
    if( XercesAttributesHelper::GetAttributeValue(
        * m_p_xercesc_attributes, pchAttributeName, value) ==
        XercesAttributesHelper::getting_attribute_value_succeeded )
//    if( ConvertXercesAttributesValue(
//        * m_p_xercesc_attributes
//       , pchAttributeName
//       , value )
//     )
    {
      LOGN_DEBUG( FULL_FUNC_NAME << " value for \"" << pchAttributeName
        << "\":" << value )
    }
    else
      LOGN_WARNING(FULL_FUNC_NAME << " getting value for \""
        << pchAttributeName << "\" failed ")
  }

  void SAX2UserInterfaceConfigHandler::
    HandleVoltageAndFrequencySettingsDialogXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    float f ;
    std::string stdstrAttributeName ;
    bool * p_bValue;
    GetAttributeValue("select_all_CPU_cores",
      m_p_model->m_userinterfaceattributes.m_bSelectAllCPUcores) ;

    if( ConvertXercesAttributesValue<float>(
      cr_xercesc_attributes
       ,"operating_safety_margin_in_Volt"
       ,f )
     )
      m_p_model->m_userinterfaceattributes.m_fOperatingSafetyMarginInVolt = f ;
    if( ConvertXercesAttributesValue<float>(
      cr_xercesc_attributes
       ,"min_usage_percentage_for_find_voltage_for_lowest_stable_CPU_core_"
         "operation_thread"
       ,f )
     )
      m_p_model->m_instablecpucorevoltagedetection.m_fMinCPUcoreUsage = f ;

    p_bValue = & m_p_model->m_userinterfaceattributes.
        m_bPreventVoltageAboveDefaultVoltage;
    GetAttributeValue( "prevent_voltage_above_default_voltage" , * p_bValue);

    p_bValue = & m_p_model->m_userinterfaceattributes.
      m_bPreventVoltageBelowLowestStableVoltage;
    GetAttributeValue( "prevent_voltage_below_lowest_stable_voltage",
      * p_bValue);

    p_bValue = & m_p_model->m_userinterfaceattributes.
      m_bShowMultiplierControlsIfLessThan2ConfigurableMultipliers;
    GetAttributeValue(
      "show_multipliers_controls_if_less_than_2_configurable_multipliers",
      * p_bValue);

    p_bValue = & m_p_model->m_userinterfaceattributes.
      m_bShowVoltageControlsIfLessThan2ConfigurableVoltages;
    GetAttributeValue(
      "show_voltage_controls_if_less_than_2_configurable_voltages",
      * p_bValue);
  }

  void SAX2UserInterfaceConfigHandler::
    HandleWisdomXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    std::string std_strWisdomText;
    std::string stdstrAttributeName = "text";
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes
         , stdstrAttributeName
         , std_strWisdomText
        ) == XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
      LOGN( FULL_FUNC_NAME << "--got attribute value for \""
          << stdstrAttributeName << "\" attribute:"
          << std_strWisdomText )
      m_p_model->m_userinterfaceattributes.m_std_vec_WisdomStrings.//push_back(
        insert(
        std::make_pair( m_ui16t_WisdomNumber ++, std_strWisdomText) );
    }
  }

  void SAX2UserInterfaceConfigHandler::startElement
    (
    const XMLCh * const cpc_xmlchURI,
    const XMLCh * const cpc_xmlchLocalName,
    const XMLCh * const cpc_xmlchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    m_p_xercesc_attributes = & cr_xercesc_attributes;
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
    { //Use a block because: to avoid g++ warning "Suspicious semicolon".
      ;
    }
//    bool bRet = false;
//    if( //If the strings equal.
//        ! Xerces::ansi_or_wchar_string_compare( cpc_xmlchLocalName ,
//        ANSI_OR_WCHAR("log_file_filter")
//        )
//      )
//    {
//      std::string stdstrValue ;
//      bRet = true ;
//      if( XercesAttributesHelper::GetAttributeValue(
//        cr_xercesc_attributes ,
//        //Use "( char * )" to avoid g++ Linux compiler warning
//        // "deprecated conversion from string constant to ‘char*’ "
//        ( char * ) "exclude" ,
//        stdstrValue )
//        )
//      {
//        LOGN("string to exclude from logging:" << stdstrValue )
//  //        g_logger.m_stdsetstdstrExcludeFromLogging.insert( strValue) ;
//        m_trieExcludeFromLogging.insert_inline(
//          (unsigned char*) stdstrValue.c_str(), stdstrValue.length()
//          , FormattedLogEntryProcessor::string_end) ;
//      }
//    }
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
    else if(
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("task_bar_icon") )
      )
    {
      HandleTaskBarIconXMLelement(cr_xercesc_attributes);
    }
    else if(
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("CPU core usage") )
      )
    {
      HandleTaskBarIcon_CPUcoreUsageXMLelement(cr_xercesc_attributes);
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
    else if( //If strings equal.
      ! ansi_or_wchar_string_compare( cpc_xmlchLocalName,
        ANSI_OR_WCHAR("wisdom") )
      )
    {
      HandleWisdomXMLelement(cr_xercesc_attributes) ;
    }
    XERCES_CPP_NAMESPACE::XMLString::release( & p_chLocalName ) ;
  }//SAX2UserInterfaceConfigHandler::startElement(...)
}
