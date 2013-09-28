/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
//#ifdef COMPILE_WITH_XERCES
#include "../stdafx.h"
//#include <global.h> //for if "COMPILE_WITH_XERCES" is defined or not

  //If not included: compiler error "C1010".
  #include "SAX2VoltagesForFrequencyHandler.hpp"
  #include <ModelData/ModelData.hpp> //class Model
  #include <UserInterface/UserInterface.hpp> //class UserInterface
  //for XercesAttributesHelper::GetAttributeValue(...)
  #include <Xerces/XercesAttributesHelper.hpp>
  #include <Xerces/XercesHelper.hpp> //XercesHelper::ToStdString(const XMLCh *)

  #include <xercesc/sax2/Attributes.hpp>//class XERCES_CPP_NAMESPACE::Attributes
  #include <xercesc/util/XMLString.hpp> //for XMLString::transcode(...)
  //#include "global.h" //for DEBUG(...) etc.

//	#include <string> //class std::string
  #include <sstream> //for std::istringstream
//	#include <iostream>

  //#define MB_CUR_MAX 1

  //With Xerces < Version 2.8:
  //  Add "XML_LIBRARY" to "Preprocessor Definitions" to compile with Xerces
  //statically (else many "LNK2001" and "LNK2019" and linker errors).
	//with Xerces 3.0: "XERCES_STATIC_LIBRARY"
	//And: Add "Advapi32.lib" as additional dependancy for the linker.
	//zu Linker->Bibliothek ignorieren hinzufgen:"LIBCMT.lib", sonst:
  //"LIBCMT.lib(_ctype.obj) : error LNK2005: _isspace ist bereits in
  //MSVCRT.lib(MSVCR80.dll) definiert."
	
//  //To NOT need to prefix the xerces classes with the "xerces::".
//	XERCES_CPP_NAMESPACE_USE

  SAX2VoltagesForFrequencyHandler::SAX2VoltagesForFrequencyHandler(
    UserInterface & r_userinterface
    , Model & model
    )
  {
    m_p_model = & model ;
    m_p_userinterface = & r_userinterface ;
  }

  void SAX2VoltagesForFrequencyHandler::handleFreqAndVoltageElement(
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    float fValue = 0.0 ;
    WORD wFreqInMHz ;
    std::string strAttributeName = "frequency_in_MHz" ;
    if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
        strAttributeName.c_str(), wFreqInMHz )
      )
    {
      //LOGN("got \"" << strAttributeName << "\" attribute value: " <<
      //wFreqInMHz )
#ifdef _DEBUG
      LOGN("Address of service attributes: " << m_p_model)
      LOGN("CPU model: " << (WORD) m_p_model->m_cpucoredata.m_byModel )
#endif
      //LOGN("address of mp_stdsetvoltageandfreqAvailableFreq: " 
      //  << m_p_model->m_cpucoredata.mp_stdsetvoltageandfreqAvailableFreq )
      VoltageAndFreq voltageandfreq(fValue,wFreqInMHz) ;
      LOGN("m_stdsetvoltageandfreqAvailableFreq addr.:" << 
        & m_p_model->m_cpucoredata.m_stdsetvoltageandfreqAvailableFreq )
      //Error at this instruction in RELEASE version.
      m_p_model->m_cpucoredata.m_stdsetvoltageandfreqAvailableFreq.insert(
      //m_p_model->m_cpucoredata.mp_stdsetvoltageandfreqAvailableFreq->insert(
        //VoltageAndFreq(fValue,wFreqInMHz) 
        voltageandfreq
        ) ;
      //LOGN("address of mp_stdsetvoltageandfreqAvailableFreq: " 
      //  << m_p_model->m_cpucoredata.mp_stdsetvoltageandfreqAvailableFreq )
      strAttributeName = "max_voltage_in_Volt" ;
#ifdef _DEBUG
      LOGN("before getting " << strAttributeName << "att. value " )
#endif
      if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
          strAttributeName.c_str(),fValue)
        )
      {
        //LOGN("got \"" << strAttributeName << "\" attribute value: " << fValue )
        //m_p_model->AddMaxVoltageForFreq(wFreqInMHz,fValue) ;
        //m_p_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.insert(
        //m_p_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->insert(
          //VoltageAndFreq(fValue,wFreqInMHz) ) ;
        m_p_model->m_cpucoredata.AddDefaultVoltageForFreq(fValue,wFreqInMHz);
      }
      //else
      //{
      //  std::ostringstream ostrstream ;
      //  ostrstream << "Error getting \"" <<
      //    strAttributeName << "\" attribute for \""
      //    << m_strElementName << "\" element" ;
      //  m_p_userinterface->Confirm(ostrstream) ;
      //}
      strAttributeName = "min_voltage_in_Volt" ;
      if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
          strAttributeName.c_str(),fValue)
        )
      {
        m_p_model->m_cpucoredata.AddLowestStableVoltageAndFreq(fValue,
          wFreqInMHz) ;
      }
      strAttributeName = "voltage_in_Volt" ;
      if( XercesAttributesHelper::GetAttributeValue(cr_xercesc_attributes,
          strAttributeName.c_str(),fValue)
        )
      {
        //m_p_model->m_cpucoredata.m_stdsetvoltageandfreqWanted.insert( 
        //  //std::pair<WORD,float>(wFreqInMHz,fValue) 
        //  VoltageAndFreq(fValue,wFreqInMHz) ) ;
        m_p_model->m_cpucoredata.AddPreferredVoltageForFreq(fValue,
          wFreqInMHz) ;
      }
    }
    else
    {
      std::ostringstream stdostringstream ;
      stdostringstream << "Error getting \"" << strAttributeName << "\" for "
        "\"" << m_strElementName << "\" element" ;
      m_p_userinterface->Confirm(//ostrstream
        stdostringstream ) ;
    }
  }
	
  void SAX2VoltagesForFrequencyHandler::startElement
    (
    const XMLCh * const cpc_xmchURI,
    const XMLCh * const cpc_xmchLocalName,
    const XMLCh * const cpc_xmchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    char * pchXMLelementName = XERCES_CPP_NAMESPACE::XMLString::transcode(
      cpc_xmchLocalName);
    if( pchXMLelementName )
    {
      std::string strValue ;
      LOGN( "XML element: " << pchXMLelementName );
      m_strElementName = std::string(pchXMLelementName) ;
      //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname
      //<< endl );
      if( m_strElementName == "freq_and_voltage" )
        handleFreqAndVoltageElement(cr_xercesc_attributes) ;
      else if( m_strElementName == "maximum_CPU_core_multiplier" )
      {
          LOGN( "maximum_CPU_core_multiplier element")
        float fValue;
        const char * c_p_chAttributeName = "value";
        if( XercesAttributesHelper::GetAttributeValue(
            cr_xercesc_attributes,
            c_p_chAttributeName, fValue)
           )
         {
           LOGN( "getting attribute value for \"" <<
               c_p_chAttributeName << "\" succeeded.")
           m_p_model->m_cpucoredata.m_fMaximumCPUcoreMultiplier = fValue;
         }
      }
      //Release memory AFTER comparing.
      XERCES_CPP_NAMESPACE::XMLString::release(&pchXMLelementName);
    }
  }

  void SAX2VoltagesForFrequencyHandler::fatalError(
    const XERCES_CPP_NAMESPACE::SAXParseException &
      cr_xercesc_sax_parse_exception )
  {
    LOGN( "SAX2 handler: Fatal Error: "
      << XercesHelper::ToStdString( cr_xercesc_sax_parse_exception.getMessage()
        )
      << " at line: " << cr_xercesc_sax_parse_exception.getLineNumber() ) ;
  }
//#endif //#ifdef COMPILE_WITH_XERCES
