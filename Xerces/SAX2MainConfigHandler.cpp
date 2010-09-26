//#ifdef COMPILE_WITH_XERCES
//  #include "../stdafx.h"
//  #include <global.h> //for if "COMPILE_WITH_XERCES" is defined or not

  //If not included: compiler error "C1010".
  #include "SAX2MainConfigHandler.hpp"
  #include <Controller/Logger/Logger.hpp> //class Logger
  #include <ModelData/ModelData.hpp> //class Model
  #include <UserInterface/UserInterface.hpp> //class UserInterface
  //for XercesAttributesHelper::GetAttributeValue(...)
  #include <Xerces/XercesAttributesHelper.hpp>
  #include <Xerces/XercesHelper.hpp> //XercesHelper::ToStdString(const XMLCh *)

  //class XERCES_CPP_NAMESPACE::Attributes
  #include <xercesc/sax2/Attributes.hpp>
  //for XERCES_CPP_NAMESPACE::XMLString::transcode(...)
  #include <xercesc/util/XMLString.hpp>
  #include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...) etc.

  #include <string> //std::string
  #include <sstream> //for std::istringstream
	
	//#define MB_CUR_MAX 1 
  extern Logger g_logger ;
	
	//With Xerces < Version 2.8:
	//  Add "XML_LIBRARY" to "Preprocessor Definitions" to compile with Xerces
  // statically (else many "LNK2001" and "LNK2019" and linker errors).
	//with Xerces 3.0: "XERCES_STATIC_LIBRARY"
	//And: Add "Advapi32.lib" as additional dependancy for the linker.
	//zu Linker->Bibliothek ignorieren hinzufgen:"LIBCMT.lib", sonst:
  //"LIBCMT.lib(_ctype.obj) : error LNK2005: _isspace ist bereits in
  //MSVCRT.lib(MSVCR80.dll) definiert."
	
//  //to NOT need to prefix the xerces classes with the "xercesc_...::"
//	XERCES_CPP_NAMESPACE_USE
	
	SAX2MainConfigHandler::SAX2MainConfigHandler(
	  Model & model ,
	  UserInterface * p_userinterface //,
    //I_CPUcontroller * p_cpucontroller
	  )
	{
	  m_p_model = & model ;
	  m_p_userinterface = p_userinterface ;
    //p_cpucontroller = p_cpucontroller ;
	}
	
  void SAX2MainConfigHandler::HandleDynamicVoltage_and_FrequencyScaling(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes )
  {
    bool bEnableDVFS = false ;
    float fValue ;
    std::string strAttributeName = "enable" ;
    WORD wValue ;
	  if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
	    strAttributeName.c_str(), bEnableDVFS )
	    )
	  {
      m_p_model->m_cpucoredata.m_bEnableDVFS = bEnableDVFS ;
    }
    strAttributeName = "usage_poll_wait_time_in_ms" ;
    if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
      strAttributeName.c_str(),wValue))
    {
      m_p_model->m_cpucoredata.m_wMilliSecondsWaitBetweenDFVS = wValue ;
    }
    strAttributeName = "CPU_core_load_threshold_for_increase" ;
    if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
      strAttributeName.c_str(),fValue) )
    {
      m_p_model->m_cpucoredata.m_fCPUcoreLoadThresholdForIncreaseInPercent =
        fValue ;
      LOGN("using \"" << fValue << "\" as CPU core load threshold for increase" )
    }
    strAttributeName = "throttle_temp_in_deg_Celsius" ;
    if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
        strAttributeName.c_str(),fValue)
      )
    {
      m_p_model->m_cpucoredata.m_fThrottleTempInDegCelsius =
        fValue ;
      LOGN("using \"" << fValue << "\" as CPU core throttle temp thres" )
    }
    strAttributeName = "increase_factor" ;
    if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
        strAttributeName.c_str(), fValue )
      )
    {
      m_p_model->m_cpucoredata.m_fCPUcoreFreqIncreaseFactor = fValue ;
      LOGN("using increase factor " <<
        m_p_model->m_cpucoredata.m_fCPUcoreFreqIncreaseFactor )
    }
    strAttributeName = "factor" ;
    if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
        strAttributeName.c_str(), fValue )
      )
    {
      m_p_model->m_cpucoredata.m_fCPUcoreFreqFactor = fValue ;
      LOGN("using factor " <<
        m_p_model->m_cpucoredata.m_fCPUcoreFreqIncreaseFactor )
    }
  }
	
  void SAX2MainConfigHandler::HandleTopmostXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & attrs )
  {
    bool bConfirm ;
	  std::string strValue ;
      //std::string stdstrLogFilePath ;
//    if( XercesHelper::GetAttributeValue(
//              attrs,
//              "confirm"
//              ,bConfirm )
//        )
    if( XercesAttributesHelper::GetAttributeValue(
          attrs
          ,"skip_cpu_type_check"
          ,bConfirm )
      )
      m_p_model->SetSkipCPUTypeCheck(bConfirm) ;
    if( XercesAttributesHelper::GetAttributeValue
        (
          attrs,
          "enable_overvoltage_protection"
          ,bConfirm
        )
      )
      m_p_model->m_bEnableOvervoltageProtection = bConfirm ;
    if( XercesAttributesHelper::GetAttributeValue
        (
          attrs,
          "use_default_formula_for_overvoltage_protection"
          ,bConfirm
        )
      )
      m_p_model->m_bUseDefaultFormularForOvervoltageProtection = bConfirm ;
    if( XercesAttributesHelper::GetAttributeValue
        ( 
          attrs,
          //Use "( char * )" to avoid g++ Linux compiler warning
          // "deprecated conversion from string constant to ‘char*’ "
          ( char * ) "log_file_path",
            strValue
        )
      )
      m_p_model->m_stdstrLogFilePath = strValue ;
  }

  void SAX2MainConfigHandler::startElement
    (
    const XMLCh * const cpc_xmchURI,
    const XMLCh * const cpc_xmchLocalName,
    const XMLCh * const cpc_xmchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & attrs
	  )
	{
	  char * pchXMLelementName = XERCES_CPP_NAMESPACE::XMLString::transcode(
	    cpc_xmchLocalName);
	  if( pchXMLelementName )
	  {
      std::string strValue ;
      LOG( "XML element: " << pchXMLelementName << "\n" );
      m_strElementName = std::string(pchXMLelementName) ;
      //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname
      //  << endl );
      if( m_strElementName == "log_file_filter" )
      {
        if( XercesAttributesHelper::GetAttributeValue(
          attrs ,
          //Use "( char * )" to avoid g++ Linux compiler warning
          // "deprecated conversion from string constant to ‘char*’ "
          ( char * ) "exclude" ,
          strValue )
          )
        {
          LOGN("string to exclude from logging:" << strValue )
  //	      g_logger.m_stdsetstdstrExcludeFromLogging.insert( strValue) ;
          g_logger.AddExcludeFromLogging(strValue) ;
        }
      }
      else if( m_strElementName == //"main_config"
        "CPU_control_main_config" )
      {
        HandleTopmostXMLelement(attrs) ;
      }
      else if( m_strElementName == "DynamicVoltage_and_FrequencyScaling" )
        HandleDynamicVoltage_and_FrequencyScaling(attrs) ;
      //Release memory AFTER comparing.
      XERCES_CPP_NAMESPACE::XMLString::release( & pchXMLelementName);
	  } //if( pchXMLelementName )
	}
	
	void SAX2MainConfigHandler::fatalError(
	  const XERCES_CPP_NAMESPACE::SAXParseException &
	    cr_xercesc_sax_parse_exception
	  )
	{
    LOGN( "SAX2 handler: Fatal Error: "
      << XercesHelper::ToStdString( cr_xercesc_sax_parse_exception.getMessage()
        )
      << " at line: " << cr_xercesc_sax_parse_exception.getLineNumber() ) ;
	}
//#endif //#ifdef COMPILE_WITH_XERCES
