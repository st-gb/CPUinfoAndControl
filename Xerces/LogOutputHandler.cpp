/*
 * LogOutputHandler.cpp
 *
 *  Created on: 16.10.2012
 *      Author: Stefan
 */

#include "Xerces/LogOutputHandler.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <ModelData/ModelData.hpp> //class Model
//for XercesAttributesHelper::GetAttributeValue(...)
#include <Xerces/XercesAttributesHelper.hpp>
#include <Xerces/XercesString.hpp> //ansi_or_wchar_string_compare(...)
#include <xercesc/sax2/Attributes.hpp>//class XERCES_CPP_NAMESPACE::Attributes

namespace Xerces
{

  LogOutputHandler::LogOutputHandler(
    UserInterface & r_userinterface
    , Model & model
    )
    :
    m_r_model ( model),
    m_r_userinterface (r_userinterface)
  {
    // TODO Auto-generated constructor stub

  }

  LogOutputHandler::~LogOutputHandler()
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME )
  }

  void LogOutputHandler::startElement
  (
    const XMLCh * const cpc_xmchURI,
    const XMLCh * const cpc_xmchLocalName, // XML element name
    const XMLCh * const cpc_xmchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    std::string std_str, std_strAttributeName, * p_std_strAttrVal;
    std::wstring std_wstrAttributeName;
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME
      << " current element name:" << Xerces::ToStdString(cpc_xmchLocalName) )
//    if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"level" ) == 0)
//    {
      std_strAttributeName = "level";
//      cr_xercesc_attributes.getValue()
      if( XercesAttributesHelper::GetAttributeValue(
          cr_xercesc_attributes, std_strAttributeName, std_str) ==
          XercesAttributesHelper::getting_attribute_value_succeeded )
      {
  //      MessageType mt = LogLevel::GetAsNumber( std_str);
        g_logger.SetLogLevel(//mt
          std_str);
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME
          << " attribute value for "
          << std_strAttributeName << ":" << std_str)
      }
      else
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME
          << " failed to get attribute value for \""
          << std_strAttributeName << "\"")
//    }
//    else if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"path" )
//      == 0)
//    {
      std_strAttributeName = "path";
      if( //Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"path" )
          XercesAttributesHelper::GetAttributeValue(
          cr_xercesc_attributes, std_strAttributeName.c_str(), //std_str
          m_r_model.m_logfileattributes.m_std_wstrLogFilePath) ==
          XercesAttributesHelper::getting_attribute_value_succeeded
        )
      {
  //      m_r_model.m_logfileattributes.m_std_wstrLogFilePath = std_str
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME << " attribute value for "
          << std_strAttributeName << ":"
          << m_r_model.m_logfileattributes.m_std_wstrLogFilePath
        )
      }
      else
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME
          << " failed to get attribute value for \""
          << std_strAttributeName << "\"")
//    }
//    else if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"format" ) == 0)
//    {
      p_std_strAttrVal = & m_r_model.m_logfileattributes.m_std_strFormat;
      std_strAttributeName = "format";
      if( //Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"format" )
        XercesAttributesHelper::GetAttributeValue(
          cr_xercesc_attributes, std_strAttributeName, * p_std_strAttrVal) ==
          XercesAttributesHelper::getting_attribute_value_succeeded
        )
      {
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME
          << " attribute value for "
          << std_strAttributeName << ":" << * p_std_strAttrVal)
      }
      else
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME
          << " failed to get attribute value for \""
          << std_strAttributeName << "\"")
//    }
//    else if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName,
//      L"time format" ) == 0 )
    std_strAttributeName = "time_format";
    p_std_strAttrVal = & m_r_model.m_logfileattributes.m_std_strTimeFormat;
    if( //Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"time format" )
      XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes, std_strAttributeName, * p_std_strAttrVal) ==
        XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME << " attribute value for "
        << std_strAttributeName << ":" << * p_std_strAttrVal)
    }
    else
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( FULL_FUNC_NAME
        << " failed to get attribute value for \""
        << std_strAttributeName << "\"")
  }
} /* namespace Xerces */
