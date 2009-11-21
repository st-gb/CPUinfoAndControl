////#ifdef WIN32 //with Linux I can't compile with xerces yet.
//#ifdef _WINDOWS //with Linux I can't compile with xerces yet.
//  #define COMPILE_WITH_XERCES
//#endif

#ifdef COMPILE_WITH_XERCES
#include "../stdafx.h"
#include "../global.h" //for if "COMPILE_WITH_XERCES" is defined or not

    //If not included: compiler error "C1010".
    #include "SAX2MainConfigHandler.hpp"
    #include "XercesHelper.hpp" //for GetAttributeValue(...)
    //#include "PStates.h"
    #include <xercesc/sax2/Attributes.hpp>
    #include <xercesc/util/xmlstring.hpp> //for XMLString::transcode(...)
    //#include "global.h" //for DEBUG(...) etc.

    #include <string>
    #include <sstream> //for istringstream
    #include <iostream>
    #include "../global.h"
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
	
	//#ifdef WIN32
	class NumberFormatException 
    : public
    #ifdef __CYGWIN__
    //  ::RuntimeException
      std::exception
    #else
      std::runtime_error
  #endif
	{
	public:
	   NumberFormatException(const std::string& s)
      #ifdef __CYGWIN__
      //  ::RuntimeException
      //  std::exception()
      #else
        :
        std::runtime_error(s)
      #endif
      {}
	};
	//#endif
	
	template <class T>
	bool from_string(
	   T& t, 
	   const std::string& s
	   //,std::ios_base& (*f)(std::ios_base&)
	  )
	{
	  //DEBUG("from_string:%s\n",s);
	  LOG("from_string:" << s << "\n" );
	  std::istringstream iss(s);
	  return !(iss //>> f 
	    >> t
	    ).fail();
	}
	
	
	SAX2MainConfigHandler::SAX2MainConfigHandler(//PStates & pstates
	  Model & model ,
	  UserInterface * p_userinterface ,
    //GriffinController * p_pumastatectrl
    I_CPUcontroller * p_cpucontroller
	  )
	{
	  //m_p_pstates = & pstates ;
	  m_p_model = & model ;
	  m_p_userinterface = p_userinterface ;
    //mp_pumastatectrl = p_pumastatectrl ;
    p_cpucontroller = p_cpucontroller ;
	}
	
  void SAX2MainConfigHandler::HandleDynamicVoltage_and_FrequencyScaling(
    const Attributes & attrs)
  {
    bool bEnableDVFS = false ;
    std::string strAttributeName = "enable" ;
    WORD wValue ;
	  if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),bEnableDVFS))
	  {
      m_p_model->m_cpucoredata.m_bEnableDVFS = bEnableDVFS ;
    }
    strAttributeName = "usage_poll_wait_time_in_ms" ;
    if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),wValue))
    {
      m_p_model->m_cpucoredata.m_wMilliSecondsWaitBetweenDFVS = wValue ;
    }
  }

  void SAX2MainConfigHandler::handleFreqAndLowestStableVoltageElement(
    const   //xercesc_2_8::
      Attributes & attrs
    )
	{
    float fValue ;
	  WORD wFreqInMHz ;
    std::string strAttributeName = "frequency_in_MHz" ;
	  if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),wFreqInMHz))
	  {
	    strAttributeName = "voltage_in_Volt" ;
	    if(XercesHelper::GetAttributeValue(attrs,strAttributeName.c_str(),fValue))
	    {
        m_p_model->m_cpucoredata.AddLowestStableVoltageAndFreq(fValue,wFreqInMHz) ;
	    }
	    else
	    {
	      std::ostrstream ostrstream ;
	      ostrstream << "Error getting \"" << 
	        strAttributeName << "\" attribute for \""
	        << m_strElementName << "\" element" ;
	      m_p_userinterface->Confirm(ostrstream) ;
	    }
	  }
	  else
	  {
	    std::ostrstream ostrstream ;
	    ostrstream << "Error getting \"" << strAttributeName << "\" for "
	      "\"" << m_strElementName << "\" element" ;
	    m_p_userinterface->Confirm(ostrstream) ;
	  }
  }

  void SAX2MainConfigHandler::handleFreqAndMaxVoltageElement(
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
	      m_p_model->AddMaxVoltageForFreq(wFreqInMHz,fValue) ;
	    }
	    else
	    {
	      std::ostrstream ostrstream ;
	      ostrstream << "Error getting \"" << 
	        strAttributeName << "\" attribute for \""
	        << m_strElementName << "\" element" ;
	      m_p_userinterface->Confirm(ostrstream) ;
	    }
	  }
	  else
	  {
	    std::ostrstream ostrstream ;
	    ostrstream << "Error getting \"" << strAttributeName << "\" for "
	      "\"" << m_strElementName << "\" element" ;
	    m_p_userinterface->Confirm(ostrstream) ;
	  }
	}
	
	void SAX2MainConfigHandler::handlePstateElement(
	  const   //xercesc_2_8::
	  Attributes & attrs
	  )
	{
	    bool bChange = false ;
	//#ifdef PRIVATE_RELEASE //hide the other possibilities
	    //BYTE byFreqID;
    //#endif
	    //WORD //wNumber, 
	    //  wVID ;
	#ifdef PRIVATE_RELEASE //hide the other possibilities
	    WORD wValue ;
  #endif
	//    if(XercesHelper::GetAttributeValue(attrs,"VID",wVID))
	//      //m_p_pstates->SetPStateVID((BYTE)wNumber,(BYTE)wVID);
	//      m_p_model->m_pstates.SetPStateVID(byPstateID,(BYTE)wVID);
	////#ifdef PRIVATE_RELEASE //hide the other possibilities
	//    if(XercesHelper::GetAttributeValue(attrs,"FID",byFreqID))
	//    {
	//      //bChange = true;
	//      //m_p_pstates->SetPStateFID((BYTE)wNumber,byFreqID);
	//      m_p_model->m_pstates.SetPStateFID(byPstateID,byFreqID);
	//    }
	    ////Maximim voltage for current p-state.
	    //if(GetAttributeValue(attrs,"max_voltage_in_volt",fMaxVoltage))
	    //{
	    //  //bChange = true;
	    //  m_p_pstates->SetPStateMaxVoltageInVolt((BYTE)wNumber,byFreqID);
	    //}
      WORD wValue ;
	    if( XercesHelper::GetAttributeValue(attrs,"FreqInMHz",wValue) )
	    {
        LOG( "XML attribute name: \"FreqInMHz\"; value: " << wValue << "\n" );
        //Gets the nearest possoble freq and stores it into the data model.
        //mp_cpucontroller->SetNearestFreqInMHz( wValue ) ;
	    }
      std::string stdstrAttributeName = "VoltageInVolt" ;
      float fValue ;
      if( XercesHelper::GetAttributeValue(attrs,stdstrAttributeName.c_str(), fValue) )
	    {
        LOG( "XML attribute name: \"" << stdstrAttributeName.c_str() << 
          "\"; value: " << fValue << "\n" );
        //m_p_model->m_pstates.SetPStateVID( byPstateID , 
        //  mp_cpucontroller->
        //  GetVoltageIDFromVoltageInVolt( fValue )
        //  );
	    }
	//#endif //#ifdef PUBLIC_RELEASE
	    //if(bChange)
	    //  m_p_pstates->SetPStateVID((BYTE)wNumber,(BYTE)wVID);
	}
	
  void SAX2MainConfigHandler::HandleTopmostXMLelement(
    const Attributes & attrs )
  {
    bool bConfirm ;
	  std::string strValue ;
      //std::string stdstrLogFilePath ;
    if( XercesHelper::GetAttributeValue(
              attrs,
              "confirm"
              ,bConfirm ) 
        )
      //m_p_pstates->m_bConfirm = bConfirm ;
      m_p_model->m_pstates.m_bConfirm = bConfirm ;
    if( XercesHelper::GetAttributeValue(
              attrs
              ,"skip_cpu_type_check"
              ,bConfirm )
         )
      m_p_model->SetSkipCPUTypeCheck(bConfirm) ;
    //if( XercesHelper::GetAttributeValue
   //       (
   //           attrs,
   //           "use_p_state_0_as_max_freq",
   //           bConfirm
   //       )
   //     )
    //  m_p_model->m_bUsePstate0AsMaxFreq = bConfirm ;
    if( XercesHelper::GetAttributeValue
          (
              attrs,
              "enable_overvoltage_protection"
              ,bConfirm
          )
        )
      m_p_model->m_bEnableOvervoltageProtection = bConfirm ;
    if( XercesHelper::GetAttributeValue
          ( 
            attrs,
          "use_default_formula_for_overvoltage_protection"
            ,bConfirm
          ) 
        )
      m_p_model->m_bUseDefaultFormularForOvervoltageProtection = bConfirm ;
    if( XercesHelper::GetAttributeValue
        ( 
            attrs,
          "log_file_path", 
            strValue
        ) 
      )
      m_p_model->m_stdstrLogFilePath = strValue ;
  }

  void SAX2MainConfigHandler::startElement
    (
    const   XMLCh * const    uri,
    const   XMLCh * const    localname,
    const   XMLCh * const    qname,
    const   //xercesc_2_8::
      Attributes & attrs
	  )
	{
	  char * pchXMLelementName = XMLString::transcode(localname);
	  std::string strValue ;
	  LOG( "XML element: " << pchXMLelementName << endl );
	  m_strElementName = std::string(pchXMLelementName) ;
	  //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname << endl );
	  //if( strcmp(pchXMLelementName, "CPU") == 0 )
	  if( m_strElementName == "CPU" )
	  {
      char archAttributeName [] = "processor_name" ;
	    if( XercesHelper::GetAttributeValue
          (
              attrs,//"processor_name"
              archAttributeName ,
              strValue
          )
        )
	    {
	      if( strValue.empty() )
              m_p_userinterface->Confirm( "In XML file: Error: \""
                "processor_name\""
                " is empty" ) ;
          else
	        m_strProcessorName = strValue ;
	    }
	    else
	      m_p_userinterface->Confirm("Error getting \"processor_name\" for \"CPU\" element") ;
	  }
    else if( m_strElementName == "freq_and_lowest_stable_voltage" )
    {
	    handleFreqAndLowestStableVoltageElement(attrs) ;
    }
	  //if( strcmp(pchXMLelementName, "freq_and_max_voltage") == 0 )
	  else if( m_strElementName == "freq_and_max_voltage" )
	  {
	    if( //WithinSpecificCPUtype() && //m_p_model->m_m_parchCPUID == 
          //If the XML file's "processor_name" is identical or a substring 
          //of the name that is in the CPUID register within the CPU.
	      m_p_model->m_strProcessorName.find(m_strProcessorName) != 
	      std::string::npos 
	      || 
          //If the CPU name that is in the CPUID register within the CPU 
          //is identical or a substring 
          //of the XML file's "processor_name" .
          m_strProcessorName.find(m_p_model->m_strProcessorName) != 
	      std::string::npos 
	      )
	    {
	      //DEBUG("CPU name matches CPU name in config file\n");
          LOG("CPU name \"" << m_p_model->m_strProcessorName << 
              "\" according to CPUID matches CPU name \"" 
              << m_strProcessorName 
              << "\" in config file\n" );
	      //Only handle the voltage/max freq combo that applies to
	      //the CPU that runs this program.
	      handleFreqAndMaxVoltageElement(attrs) ;
	    }
	    return ;
	  }
	  ////if( strcmp(pchXMLelementName, "pstate") == 0 )
	  //else if( m_strElementName == "pstate" )
	  //{
	  //  handlePstateElement(attrs) ;
	  //  return ;
	  //}
	  //if( strcmp(pchXMLelementName, "pumastatectrl") == 0 )
	  else if( m_strElementName == //"main_config" 
      "CPU_control_main_config" )
	  {
      HandleTopmostXMLelement(attrs) ;
	  }
    else if( m_strElementName == "DynamicVoltage_and_FrequencyScaling" )
      HandleDynamicVoltage_and_FrequencyScaling(attrs) ;
	  //Release memory AFTER comparing.
	  XMLString::release(&pchXMLelementName);
	}
	
	void SAX2MainConfigHandler::fatalError(const SAXParseException& exception)
	{
	    char* message = XMLString::transcode(exception.getMessage());
      //DEBUG_COUT( << "SAX2 handler: Fatal Error: " << message
	     //    << " at line: " << exception.getLineNumber()
	     //    << endl ) ;
      LOG( "SAX2 handler: Fatal Error: " << message
         << " at line: " << exception.getLineNumber()
         << endl ) ;
	}
#endif //#ifdef COMPILE_WITH_XERCES
