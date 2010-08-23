////#ifdef WIN32 //with Linux I can't compile with xerces yet.
//#ifdef _WINDOWS //with Linux I can't compile with xerces yet.
//  #define COMPILE_WITH_XERCES
//#endif

#ifdef COMPILE_WITH_XERCES
#include "../stdafx.h"
#include <global.h> //for if "COMPILE_WITH_XERCES" is defined or not

	//If not included: compiler error "C1010".
	#include "SAX2DefaultVoltageForFrequency.hpp"
  #include "XercesHelper.hpp" //for GetAttributeValue(...)
  #include <UserInterface/UserInterface.hpp>

	#include <xercesc/sax2/Attributes.hpp>
	#include <xercesc/util/xmlstring.hpp> //for XMLString::transcode(...)
	//#include "global.h" //for DEBUG(...) etc.
	
	#include <string>
	#include <sstream> //for istringstream
	#include <iostream>
  #include <exception> //for class std::exception
	#ifndef WIN32
		#include <stdexcept> //for class "runtime_error"
	#endif //#ifndef WIN32
	using namespace std;
	
	//#define MB_CUR_MAX 1 
	
	//With Xerces < Version 2.8:
	//  Add "XML_LIBRARY" to "Preprocessor Definitions" to compile with Xerces statically (else many "LNK2001" and "LNK2019" and linker errors).
	//with Xerces 3.0: "XERCES_STATIC_LIBRARY"
	//And: Add "Advapi32.lib" as additional dependancy for the linker.
	//zu Linker->Bibliothek ignorieren hinzufgen:"LIBCMT.lib", sonst: "LIBCMT.lib(_ctype.obj) : error LNK2005: _isspace ist bereits in MSVCRT.lib(MSVCR80.dll) definiert."
	
	XERCES_CPP_NAMESPACE_USE //to NOT need to prefix the xerces classes with the "xerces::"
	
	// need to properly scope any forward declarations
	XERCES_CPP_NAMESPACE_BEGIN
	  class Attributes;
	XERCES_CPP_NAMESPACE_END
	
	SAX2DefaultVoltageForFrequency::SAX2DefaultVoltageForFrequency(
	  UserInterface & p_userinterface 
	  , Model & model
	  )
	{
	  m_p_model = & model ;
	  m_p_userinterface = & p_userinterface ;
	}

  void SAX2DefaultVoltageForFrequency::handleFreqAndMaxVoltageElement(
    const   //xercesc_2_8::
      Attributes & attrs
    )
	{
	  float fValue ;
	  WORD wFreqInMHz ;
    std::string strAttributeName = "frequency_in_MHz" ;
	  if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),wFreqInMHz))
	  {
	    strAttributeName = "max_voltage_in_Volt" ;
	    if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),fValue))
	    {
	      //m_p_model->AddMaxVoltageForFreq(wFreqInMHz,fValue) ;
        m_p_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.insert(
        //m_p_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->insert(
          VoltageAndFreq(fValue,wFreqInMHz) ) ;
	    }
	    else
	    {
//	      std::ostrstream ostrstream ;
//	      ostrstream
	      std::ostringstream stdostringstream ;
	      stdostringstream << "Error getting \"" <<
	        strAttributeName << "\" attribute for \""
	        << m_strElementName << "\" element" ;
	      m_p_userinterface->Confirm(//ostrstream
	        stdostringstream ) ;
	    }
	  }
	  else
	  {
//	    std::ostrstream ostrstream ;
//	    ostrstream
      std::ostringstream stdostringstream ;
      stdostringstream << "Error getting \"" << strAttributeName << "\" for "
	      "\"" << m_strElementName << "\" element" ;
	    m_p_userinterface->Confirm(//ostrstream
	      stdostringstream ) ;
	  }
	}

  void SAX2DefaultVoltageForFrequency::handleFreqAndVoltageElement(
    const   //xercesc_2_8::
      Attributes & attrs
    )
  {
	  float fValue = 0.0 ;
	  WORD wFreqInMHz ;
    std::string strAttributeName = "frequency_in_MHz" ;
	  if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),wFreqInMHz))
	  {
      //LOGN("got \"" << strAttributeName << "\" attribute value: " << wFreqInMHz )
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
	    if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),fValue))
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
	    //  std::ostrstream ostrstream ;
	    //  ostrstream << "Error getting \"" << 
	    //    strAttributeName << "\" attribute for \""
	    //    << m_strElementName << "\" element" ;
	    //  m_p_userinterface->Confirm(ostrstream) ;
	    //}
	    strAttributeName = "min_voltage_in_Volt" ;
	    if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),fValue))
	    {
        m_p_model->m_cpucoredata.AddLowestStableVoltageAndFreq(fValue,wFreqInMHz) ;
	    }
	    strAttributeName = "voltage_in_Volt" ;
	    if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),fValue))
	    {
        //m_p_model->m_cpucoredata.m_stdsetvoltageandfreqWanted.insert( 
        //  //std::pair<WORD,float>(wFreqInMHz,fValue) 
        //  VoltageAndFreq(fValue,wFreqInMHz) ) ;
        m_p_model->m_cpucoredata.AddPreferredVoltageForFreq(fValue,wFreqInMHz) ;
	    }
	  }
	  else
	  {
//	    std::ostrstream ostrstream ;
//	    ostrstream
      std::ostringstream stdostringstream ;
      stdostringstream << "Error getting \"" << strAttributeName << "\" for "
	      "\"" << m_strElementName << "\" element" ;
	    m_p_userinterface->Confirm(//ostrstream
	      stdostringstream ) ;
	  }    
  }
	
  void SAX2DefaultVoltageForFrequency::startElement
    (
    const   XMLCh * const    uri,
    const   XMLCh * const    localname,
    const   XMLCh * const    qname,
    const   //xercesc_2_8::
      Attributes & attrs
	  )
	{
	  char * pchXMLelementName = XMLString::transcode(localname);
    if( pchXMLelementName )
    {
	    std::string strValue ;
	    LOG( "XML element: " << pchXMLelementName << endl );
	    m_strElementName = std::string(pchXMLelementName) ;
	    //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname << endl );
	    //if( strcmp(pchXMLelementName, "CPU") == 0 )
	    //if( m_strElementName == "CPU" )
	    //{
     //   char archAttributeName [] = "processor_name" ;
	    //  if( XercesHelper::GetAttributeValue
     //       (
     //           attrs,//"processor_name"
     //           archAttributeName ,
     //           strValue
     //       )
     //     )
	    //  {
	    //    if( strValue.empty() )
     //           m_p_userinterface->Confirm( "In XML file: Error: \""
     //             "processor_name\""
     //             " is empty" ) ;
     //       else
	    //      m_strProcessorName = strValue ;
	    //  }
	    //  else
	    //    m_p_userinterface->Confirm("Error getting \"processor_name\" for \"CPU\" element") ;
	    //}
	    //if( strcmp(pchXMLelementName, "freq_and_max_voltage") == 0 )
	    //else 
      if( m_strElementName == "freq_and_max_voltage" )
	    {
	  //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
	  //    m_p_model->m_strProcessorName = "AMD Turion(tm) X2 Ultra Dual-Core Mobile ZM-82";
	  //#endif
	      //if( //WithinSpecificCPUtype() && //m_p_model->m_m_parchCPUID == 
       //     //If the XML file's "processor_name" is identical or a substring 
       //     //of the name that is in the CPUID register within the CPU.
	      //  m_p_model->m_strProcessorName.find(m_strProcessorName) != 
	      //  std::string::npos 
	      //  || 
       //     //If the CPU name that is in the CPUID register within the CPU 
       //     //is identical or a substring 
       //     //of the XML file's "processor_name" .
       //     m_strProcessorName.find(m_p_model->m_strProcessorName) != 
	      //  std::string::npos 
	      //  )
	      //{
	      //  //DEBUG("CPU name matches CPU name in config file\n");
       //     LOG("CPU name \"" << m_p_model->m_strProcessorName << 
       //         "\" according to CPUID matches CPU name \"" 
       //         << m_strProcessorName 
       //         << "\" in config file\n" );
	        //Only handle the voltage/max freq combo that applies to
	        //the CPU that runs this program.
	        //handleFreqAndMaxVoltageElement(attrs) ;
	      //}
	      //return ;
	    }
      if( m_strElementName == "freq_and_voltage" )
	      handleFreqAndVoltageElement(attrs) ;
	    //Release memory AFTER comparing.
	    XMLString::release(&pchXMLelementName);
    }
	}
	
	void SAX2DefaultVoltageForFrequency::fatalError(const SAXParseException & exception)
	{
#ifdef COMPILE_WITH_LOG
	    char * message = XMLString::transcode(exception.getMessage());
#endif //#ifdef COMPILE_WITH_LOG
      //DEBUG_COUT( << "SAX2 handler: Fatal Error: " << message
	     //    << " at line: " << exception.getLineNumber()
	     //    << endl ) ;
      LOG( "SAX2 handler: Fatal Error: " << message
         << " at line: " << exception.getLineNumber()
         << endl ) ;
	}
#endif //#ifdef COMPILE_WITH_XERCES
