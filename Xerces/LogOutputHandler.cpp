/*
 * LogOutputHandler.cpp
 *
 *  Created on: 16.10.2012
 *      Author: Stefan
 */

#include "LogOutputHandler.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//ConvertStdTStringToTypename(...)
#include <Controller/character_string/ConvertStdStringToTypename.hpp>
#include <ModelData/ModelData.hpp> //class Model
//TODO include neccessary?
#include <UserInterface/UserInterface.hpp> //class UserInterface
//for XercesAttributesHelper::GetAttributeValue(...)
#include <Xerces/XercesAttributesHelper.hpp>
#include <Xerces/XercesString.hpp> //ansi_or_wchar_string_compare(...)
#include <xercesc/sax2/Attributes.hpp>//class XERCES_CPP_NAMESPACE::Attributes
#include <preprocessor_macros/operating_system_name.h> //OS_NAME
#include <Xerces/GetErrorDescripton.hpp>

namespace Xerces
{

  LogOutputHandler::LogOutputHandler(
    UserInterface & r_userinterface
    , Model & model
    )
    :
    m_bSuitableFileFormat(false),
    m_r_model ( model),
    m_r_userinterface (r_userinterface)
  {
    // TODO Auto-generated constructor stub

  }

  LogOutputHandler::~LogOutputHandler()
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( "")
  }

  void LogOutputHandler::endElement(
    const XMLCh* const uri,
    const XMLCh* const cpc_xmchLocalName,
    const XMLCh* const qname )
  {
    if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName,
        m_r_model.m_logfileattributes.m_std_strFormat.c_str() ) )
    {
      m_bSuitableFileFormat = false;
    }
  }

  void LogOutputHandler::fatalError(
      const XERCES_CPP_NAMESPACE::SAXParseException & exception)
  {
//    OutputSAXExceptionErrorMessage(exception,
    std::wstring stdwstrMessage;
    GetDocumentIDandLocation(stdwstrMessage);
    if( stdwstrMessage.empty() )
    {
      stdwstrMessage = Xerces::GetErrorDescription(
        exception.getColumnNumber(),
        exception.getLineNumber(),
        exception.getSystemId(),
        exception
        );
    }
    m_r_userinterface.MessageWithTimeStamp(stdwstrMessage.c_str(), 0 );
//    int i = 0;
  }

  void LogOutputHandler::HandleLoggingXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes)
  {
    std::string std_strAttributeValue, std_strAttributeName = "level",
        * p_std_strAttrVal;
//      cr_xercesc_attributes.getValue()
    p_std_strAttrVal = & m_r_model.m_logfileattributes.m_std_strLevel;
    if( XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes, std_strAttributeName, * p_std_strAttrVal) ==
        XercesAttributesHelper::getting_attribute_value_succeeded )
    {
//      MessageType mt = LogLevel::GetAsNumber( std_strAttributeValue);
//      g_logger.SetLogLevel(//mt
//        std_strAttributeValue);
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
        << " attribute value for "
        << std_strAttributeName << ":" << * p_std_strAttrVal)
    }
    else
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
        << " failed to get attribute value for \""
        << std_strAttributeName << "\"")
//    }
//    else if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"path" )
//      == 0)
//    {
    std_strAttributeName = "maxNumberOfLines";
    if( //Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"path" )
        XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes, std_strAttributeName.c_str(), std_strAttributeValue) ==
        XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
      ConvertStdStringToTypename(
        m_r_model.m_logfileattributes.m_maxNumberOfLines, std_strAttributeValue);

//      m_r_model.m_logfileattributes.m_std_wstrLogFilePath = std_strAttributeValue
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
        << " attribute value for "
        << std_strAttributeName << ":"
        << m_r_model.m_logfileattributes.m_maxNumberOfLines
      )
    }
    else
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
        << " failed to get attribute value for \""
        << std_strAttributeName << "\"")
    std_strAttributeName = "path";
    if( //Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"path" )
        XercesAttributesHelper::GetAttributeValue(
        cr_xercesc_attributes, std_strAttributeName.c_str(), //std_strAttributeValue
        m_r_model.m_logfileattributes.m_std_wstrLogFilePath) ==
        XercesAttributesHelper::getting_attribute_value_succeeded
      )
    {
//      m_r_model.m_logfileattributes.m_std_wstrLogFilePath = std_strAttributeValue
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
        << " attribute value for "
        << std_strAttributeName << ":"
        << m_r_model.m_logfileattributes.m_std_wstrLogFilePath
      )
    }
    else
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
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
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
        << " attribute value for "
        << std_strAttributeName << ":" << * p_std_strAttrVal)
    }
    else
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
        "LogOutputHandler::HandleLoggingXMLelement-"
        << " failed to get attribute value for \""
        << std_strAttributeName << "\"")
  }

  void LogOutputHandler::startElement
  (
    const XMLCh * const cpc_xmchURI,
    const XMLCh * const cpc_xmchLocalName, // XML element name
    const XMLCh * const cpc_xmchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    std::string std_strAttributeName, * p_std_strAttrVal;
    std::wstring std_wstrAttributeName;
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
      "LogOutputHandler::startElement--"
      << " current element name:" << Xerces::ToStdString(cpc_xmchLocalName) )

    if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName,
        ANSI_OR_WCHAR("logging") ) ==
        XercesString::strings_are_indentical)
    {
      HandleLoggingXMLelement(cr_xercesc_attributes);
    }
    else if( Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName,
      m_r_model.m_logfileattributes.m_std_strFormat.c_str() )
      == XercesString::strings_are_indentical)
    {
      m_bSuitableFileFormat = true;
      WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE("This is the suitable XML element "
        "for the specified file format.")
    }
//    else
//      m_bSuitableFileFormat = false;
    if( m_bSuitableFileFormat &&
      Xerces::ansi_string_compare(cpc_xmchLocalName,
      /*ANSI_OR_WCHAR(OS_NAME)*/
        OS_NAME) == XercesString::strings_are_indentical )
    {
      std_strAttributeName = "time_format";
      p_std_strAttrVal = & m_r_model.m_logfileattributes.m_std_strTimeFormat;
      if( //Xerces::ansi_or_wchar_string_compare(cpc_xmchLocalName, L"time format" )
        XercesAttributesHelper::GetAttributeValue(
          cr_xercesc_attributes, std_strAttributeName, * p_std_strAttrVal) ==
          XercesAttributesHelper::getting_attribute_value_succeeded
        )
      {
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
          "LogOutputHandler::startElement--" << " attribute value for "
          << std_strAttributeName << ":" << * p_std_strAttrVal)
      }
      else
        WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE( //FULL_FUNC_NAME
          "LogOutputHandler::startElement--"
          << " failed to get attribute value for \""
          << std_strAttributeName << "\"")
    }
  }
} /* namespace Xerces */
