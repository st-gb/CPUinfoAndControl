////#ifdef WIN32 //with Linux I can't compile with xerces yet.
//#ifdef _WINDOWS //with Linux I can't compile with xerces yet.
//  #define COMPILE_WITH_XERCES
//#endif

//#ifdef COMPILE_WITH_XERCES
#include "stdafx.h"
#include "global.h" //for if "COMPILE_WITH_XERCES" is defined or not

	//If not included: compiler error "C1010".
	#include "SAX2MainConfigHandler.hpp"
  #include <Xerces/XercesHelper.hpp> //for GetAttributeValue(...)
	//#include "PStates.h"
	#include <xercesc/sax2/Attributes.hpp>
	#include <xercesc/util/xmlstring.hpp> //for XMLString::transcode(...)
	//#include "global.h" //for DEBUG(...) etc.
	
	#include <string>
	#include <sstream> //for istringstream
	#include <iostream>
	//#include "global.h"
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
    #ifdef _MSC_VER //MicroSoft Compiler
      std::runtime_error
    #else // __CYGWIN__, mingw
    //  ::RuntimeException
      std::exception
    #endif
	{
	public:
	   NumberFormatException(const std::string& s)
      #ifdef _MSC_VER //MicroSoft Compiler
        :
        std::runtime_error(s)
      #else //__CYGWIN__, ...
      //  ::RuntimeException
      //  std::exception()
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
	
	
  Griffin::SAX2MainConfigHandler::SAX2MainConfigHandler(//PStates & pstates
	  Model & model ,
	  UserInterface * p_userinterface ,
    GriffinController * p_pumastatectrl
	  )
	{
	  //m_p_pstates = & pstates ;
	  m_p_model = & model ;
	  m_p_userinterface = p_userinterface ;
    mp_griffincontroller = p_pumastatectrl ;
	}
	
  void Griffin::SAX2MainConfigHandler::handleFreqAndLowestStableVoltageElement(
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

  void Griffin::SAX2MainConfigHandler::handleFreqAndMaxVoltageElement(
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
	
	void Griffin::SAX2MainConfigHandler::handlePstateElement(
	  const   //xercesc_2_8::
	  Attributes & attrs
	  )
	{
	  BYTE byPstateID ;
	  //XMLCh xmlch;
	  //const XMLCh * pxmlch = XMLString::transcode("number") ;
	  ////xmlch.isNELRecognized ();
	  ////String strNumber 
	  //pxmlch = attrs.getValue(// const XMLCh *const qName
	  //  pxmlch
	  //  //"number"
	  //  ) ;
	  ////If the attribute exists.
	  //if(pxmlch)
	  //{
	
	  //All other attributes depend on the p-state number.. So this
	  //is fundamental important.
	  if(XercesHelper::GetAttributeValue(attrs,"number",byPstateID))
	  {
//	    bool bChange = false ;
	//#ifdef PRIVATE_RELEASE //hide the other possibilities
	    BYTE byFreqID;
    //#endif
	    //int nAtoiResult;
	    //BYTE byNumber ;
	    WORD //wNumber, 
	      wVID ;
	#ifdef PRIVATE_RELEASE //hide the other possibilities
	    WORD wValue ;
  #endif
	    //int nNumber;
	    //char * pchNumber = XMLString::transcode(pxmlch) ;
	    //std::stringstream istrs(std::string(pchNumber));
	    //if(! (
	      //istrs >> wNumber;
	      //) )
	//      byNumber = atoi(pchNumber) ;
	    //std::string strNumber(pchNumber) ;
	    //DEBUG("strNumber:\"%s\"\n",strNumber.c_str());
	    //if(!
	    //  ////Use "WORD" here, els it could be treated as char instead of as number.
	    //  //from_string<WORD>(wNumber, strNumber //, std::dec
	    //  //)
	    //  ( nAtoiResult=atoi(pchNumber ) )
	    //  //Because atoi(...) returns "0" also for errors.
	    //  && strNumber != "0"
	    //  )
	    //{
	    //  DEBUG("from_string err\n");
	    //  XMLString::release(&pchNumber);
	    //  throw NumberFormatException(std::string(pchNumber));
	    //}
	    //wNumber = nAtoiResult ;
	    //std::string str ;
	    //std::istringstream i(strNumber);
	    //double x;
	    //if (!(i >> nNumber))
	    //  //throw BadConversion("convertToDouble(\"" + s + "\")");
	    //  printf("iststr err\n");
	    //printf("Attr.\"number\" exists: %d\n",wNumber);
	
	    //TODO unknown behaviour if "number" is not the first attribute of 
	    //a p_state element (the order should NOT matter).
	    //Better do this: 
	    //What if "number" is not specified?
	
	    //m_p_pstates->SetNumber((BYTE)wNumber );
	    //m_p_pstates->SetNumber(byPstateID );
	    m_p_model->m_pstates.SetNumber(byPstateID );
	    //XMLString::release(&pchNumber);
	
	    //pxmlch = XMLString::transcode("VID") ;
	    //pxmlch = attrs.getValue(// const XMLCh *const qName
	    //  pxmlch
	    //  //"number"
	    //  ) ;
	    ////If the attribute exists.
	    //if(pxmlch)
	    //{
	    //  pchNumber = XMLString::transcode(pxmlch) ;
	    //  strNumber = std::string(pchNumber);
	    //  if(!
	    //    //from_string<WORD>(wVID, std::string(pchNumber)//, std::dec
	    //    //)
	    //    ( nAtoiResult=atoi(pchNumber ) )
	    //    //Because atoi(...) returns "0" also for errors.
	    //    && strNumber != "0"
	    //    )
	    //  {
	    //    printf("from_string err\n");
	    //    XMLString::release(&pchNumber);
	    //    throw NumberFormatException(std::string(pchNumber));
	    //  }
	    //  wVID = nAtoiResult ;
	    //  XMLString::release(&pchNumber);
	    //  bChange = true;
	    //}
	    if(XercesHelper::GetAttributeValue(attrs,"VID",wVID))
	      //m_p_pstates->SetPStateVID((BYTE)wNumber,(BYTE)wVID);
	      m_p_model->m_pstates.SetPStateVID(byPstateID,(BYTE)wVID);
	//#ifdef PRIVATE_RELEASE //hide the other possibilities
	    if(XercesHelper::GetAttributeValue(attrs,"FID",byFreqID))
	    {
	      //bChange = true;
	      //m_p_pstates->SetPStateFID((BYTE)wNumber,byFreqID);
	      m_p_model->m_pstates.SetPStateFID(byPstateID,byFreqID);
	    }
	    if(XercesHelper::GetAttributeValue(attrs,"DID",byFreqID))
	    {
	      //bChange = true;
	      //m_p_pstates->SetPStateDID((BYTE)wNumber,byFreqID);
	      m_p_model->m_pstates.SetPStateDID(byPstateID,byFreqID);
	    }
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
        DIDandFID didandfid = mp_griffincontroller->
          GetNearestPossibleFreqInMHzAsDIDnFID( wValue ) ;
            m_p_model->m_pstates.SetPStateDID(byPstateID, //p_didandfid->
              didandfid.m_byDivisorID);
            m_p_model->m_pstates.SetPStateFID(byPstateID, //p_didandfid->
              didandfid.m_byFreqID);
        //mp_i_cpucontroller->SetNearestFreqInMHz( wValue ) ;
	    }
      std::string stdstrAttributeName = "VoltageInVolt" ;
      float fValue ;
      if( XercesHelper::GetAttributeValue(attrs,stdstrAttributeName.c_str(), fValue) )
	    {
        LOG( "XML attribute name: \"" << stdstrAttributeName.c_str() << 
          "\"; value: " << fValue << "\n" );
        m_p_model->m_pstates.SetPStateVID( byPstateID , mp_griffincontroller->
          GetVoltageIDFromVoltageInVolt( fValue )
          );
	    }
	//#endif //#ifdef PUBLIC_RELEASE
	    //if(bChange)
	    //  m_p_pstates->SetPStateVID((BYTE)wNumber,(BYTE)wVID);
	  }
	}
	
  void Griffin::SAX2MainConfigHandler::startElement
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
	#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
	    m_p_model->m_strProcessorName = "AMD Turion(tm) X2 Ultra Dual-Core Mobile ZM-82";
	#endif
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
	  //if( strcmp(pchXMLelementName, "pstate") == 0 )
	  else if( m_strElementName == "pstate" )
	  {
	    handlePstateElement(attrs) ;
	    return ;
	  }
	  //if( strcmp(pchXMLelementName, "pumastatectrl") == 0 )
	  else if( m_strElementName == "pumastatectrl" )
	  {
	    bool bConfirm ;
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
	    if( XercesHelper::GetAttributeValue
            (
                attrs,
                "use_p_state_0_as_max_freq",
                bConfirm
            )
          )
	      m_p_model->m_bUsePstate0AsMaxFreq = bConfirm ;
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
	  //Release memory AFTER comparing.
	  XMLString::release(&pchXMLelementName);
	}
	
	void Griffin::SAX2MainConfigHandler::fatalError(
    const SAXParseException& exception )
	{
	    char* message = XMLString::transcode(exception.getMessage());
      //DEBUG_COUT( << "SAX2 handler: Fatal Error: " << message
	     //    << " at line: " << exception.getLineNumber()
	     //    << endl ) ;
      LOG( "SAX2 handler: Fatal Error: " << message
         << " at line: " << exception.getLineNumber()
         << endl ) ;
	}
//#endif //#ifdef COMPILE_WITH_XERCES
