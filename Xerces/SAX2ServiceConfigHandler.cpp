////#ifdef WIN32 //with Linux I can't compile with xerces yet.
//#ifdef _WINDOWS //with Linux I can't compile with xerces yet.
//  #define COMPILE_WITH_XERCES
//#endif

#ifdef COMPILE_WITH_XERCES
  #include "../stdafx.h"
  #include <global.h> //for if "COMPILE_WITH_XERCES" is defined or not

  //If not included: compiler error "C1010".
  #include "SAX2ServiceConfigHandler.hpp"
  //#include "global.h" //for DEBUG(...) etc.
  #include <ModelData/ModelData.hpp>//class Model
  #include <ModelData/ServiceAttributes.hpp>//class ServiceAttributes
  #include <UserInterface/UserInterface.hpp>//class UserInterface
  //for XercesAttributesHelper::GetAttributeValue(...)
  #include <Xerces/XercesAttributesHelper.hpp>
  #include <Xerces/XercesHelper.hpp> //XercesHelper::ToStdString(const XMLCh *)

//class XERCES_CPP_NAMESPACE::Attributes
  #include <xercesc/sax2/Attributes.hpp>
  //for XERCES_CPP_NAMESPACE::XMLString::transcode(...)
  #include <xercesc/util/XMLString.hpp>

  #include <string> //class std::string, class std::wstring
//  #include <sstream> //for class std::istringstream
//  #include <iostream>

	//#define MB_CUR_MAX 1 
	
	//With Xerces < Version 2.8:
	//  Add "XML_LIBRARY" to "Preprocessor Definitions" to compile with Xerces
	// statically (else many "LNK2001" and "LNK2019" and linker errors).
	//with Xerces 3.0: "XERCES_STATIC_LIBRARY"
	//And: Add "Advapi32.lib" as additional dependancy for the linker.
	//zu Linker->Bibliothek ignorieren hinzufgen:"LIBCMT.lib", sonst:
	// "LIBCMT.lib(_ctype.obj) : error LNK2005: _isspace ist bereits in
	//MSVCRT.lib(MSVCR80.dll) definiert."
	
//  using namespace std;
  //	//to NOT need to prefix the xerces classes with the "xercesc...::"
//	XERCES_CPP_NAMESPACE_USE

	SAX2ServiceConfigHandler::SAX2ServiceConfigHandler(
	  Model & model ,
	  UserInterface * p_userinterface //,
    //I_CPUcontroller * p_cpucontroller
	  )
	{
	  LOGN("SAX2ServiceConfigHandler c'tor--address of model:" << & model )
	  m_p_model = & model ;
	  m_p_userinterface = p_userinterface ;
    //p_cpucontroller = p_cpucontroller ;
	}

  void SAX2ServiceConfigHandler::startElement
    (
    const XMLCh * const cp_xmlchURI,
    const XMLCh * const cp_xmlchLocalName,
    const XMLCh * const cp_xmlchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
	  )
	{
	  char * pchXMLelementName = XERCES_CPP_NAMESPACE::XMLString::transcode(
	    cp_xmlchLocalName);
	  if( pchXMLelementName )
	  {
      std::wstring stdwstrValue ;
      LOGN( "XML element: " << pchXMLelementName );
      m_strElementName = std::string(pchXMLelementName) ;
      //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname
      // << endl );
      if( m_strElementName == "GUI_pathes" )
      {
  //      char archAttributeName [] = "current_dir_for_GUI" ;
        std::string stdstrAttributeName = "current_dir_for_GUI" ;
        LOGN("trying to get XML attribute value for \"" <<
          stdstrAttributeName << "\"")
  //	    xercesc_3_1:: Get
        if( XercesAttributesHelper::GetAttributeValue
            (
              cr_xercesc_attributes,//"processor_name"
  //            archAttributeName ,
              stdstrAttributeName.c_str() ,
              stdwstrValue
            )
          )
        {
          LOGN("XML attribute \"" << stdstrAttributeName << "\" exists")
          LOGWN_WSPRINTF(L"Before setting current dir for GUI to %ls" ,
            stdwstrValue.c_str()
            )
          LOGN("SAX2ServiceConfigHandler--address of model:" << m_p_model)
          LOGN("SAX2ServiceConfigHandler--address of service attributes:"
            << & m_p_model->m_serviceattributes.m_stdwstrGUICurrentDirFullPathTo )
          LOGN("SAX2ServiceConfigHandler--address of GUI current dir:"
            << & m_p_model->m_serviceattributes.
            m_stdwstrGUICurrentDirFullPathTo )
          LOGN("SAX2ServiceConfigHandler--address of GUI current dir char ptr:"
            << m_p_model->m_serviceattributes.
            m_stdwstrGUICurrentDirFullPathTo.c_str() )
//          LOGN("SAX2ServiceConfigHandler--model->m_strProcessorName :"
//            << m_p_model->m_strProcessorName )
//          LOGN("m_p_model->m_serviceattributes.m_stdstrPathToGUIexe:"
//            << m_p_model->m_serviceattributes.m_stdstrPathToGUIexe )
          //TODO program crash here
          m_p_model->m_serviceattributes.m_stdwstrGUICurrentDirFullPathTo =
            stdwstrValue ;
          //Use wide string because the character set for the language may
          //need more than 256 characters.
          LOGWN_WSPRINTF(L"set current dir for GUI to %ls" ,
            //stdwstrValue.c_str()
            m_p_model->m_serviceattributes.m_stdwstrGUICurrentDirFullPathTo.
            c_str()
            )
        }
        stdstrAttributeName = "path_to_GUI_exe" ;
        LOGN("trying to get XML attribute value for \"" <<
          stdstrAttributeName << "\"")
        if( XercesAttributesHelper::GetAttributeValue
            (
              cr_xercesc_attributes,//"processor_name"
//                archAttributeName ,
              stdstrAttributeName.c_str() ,
              stdwstrValue
            )
          )
        {
          m_p_model->m_serviceattributes.m_stdwstrPathToGUIexe =
            stdwstrValue ;
          //Use wide string because the character set for the language may
          //need more than 256 characters.
          LOGWN_WSPRINTF(L"set path to GUI exe to %ls" , //stdwstrValue.c_str()
            m_p_model->m_serviceattributes.m_stdwstrPathToGUIexe.c_str() )
        }
      }
  //    else if( m_strElementName == "path_to_GUI_exe" )
  //    {
  //
  //    }
      //Release memory AFTER comparing.
      XERCES_CPP_NAMESPACE::XMLString::release(&pchXMLelementName);
	  }
	}
	
	void SAX2ServiceConfigHandler::fatalError(
	  const XERCES_CPP_NAMESPACE::SAXParseException &
	    cr_xercesc_sax_parse_exception
	  )
	{
    LOG( "SAX2 handler: Fatal Error: "
      << XercesHelper::ToStdString(cr_xercesc_sax_parse_exception.getMessage()
        )
     << " at line: " << cr_xercesc_sax_parse_exception.getLineNumber()
     << "\n" ) ;
	}
#endif //#ifdef COMPILE_WITH_XERCES
