////#ifdef WIN32 //with Linux I can't compile with xerces yet.
//#ifdef _WINDOWS //with Linux I can't compile with xerces yet.
//  #define COMPILE_WITH_XERCES
//#endif

#ifdef COMPILE_WITH_XERCES
  #include "../stdafx.h"
  #include "../global.h" //for if "COMPILE_WITH_XERCES" is defined or not

  //If not included: compiler error "C1010".
  #include "SAX2ServiceConfigHandler.hpp"
  #include "XercesHelper.hpp" //for GetAttributeValue(...)

  #include <xercesc/sax2/Attributes.hpp>
  #include <xercesc/util/xmlstring.hpp> //for XMLString::transcode(...)
  //#include "global.h" //for DEBUG(...) etc.
  #include <ModelData/ServiceAttributes.hpp>
  #include <ModelData/ModelData.hpp>

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
    #if defined(__CYGWIN__) || defined(__MINGW32__)
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
        #ifdef _MSC_VER
        :
        std::runtime_error(s)
        #endif
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
	
	SAX2ServiceConfigHandler::SAX2ServiceConfigHandler(//PStates & pstates
	  Model & model ,
	  UserInterface * p_userinterface //,
    //I_CPUcontroller * p_cpucontroller
	  )
	{
	  //m_p_pstates = & pstates ;
	  m_p_model = & model ;
	  m_p_userinterface = p_userinterface ;
    //p_cpucontroller = p_cpucontroller ;
	}

  void SAX2ServiceConfigHandler::startElement
    (
    const   XMLCh * const    uri,
    const   XMLCh * const    localname,
    const   XMLCh * const    qname,
    const   //xercesc_2_8::
      XERCES_CPP_NAMESPACE::Attributes & attrs
	  )
	{
	  char * pchXMLelementName = XMLString::transcode(localname);
	  if( pchXMLelementName )
	  {
      std::wstring stdwstrValue ;
      LOG( "XML element: " << pchXMLelementName << endl );
      m_strElementName = std::string(pchXMLelementName) ;
      //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname << endl );
      if( m_strElementName == "GUI_pathes" )
      {
  //      char archAttributeName [] = "current_dir_for_GUI" ;
        std::string stdstrAttributeName = "current_dir_for_GUI" ;
  //	    xercesc_3_1:: Get
        if( XercesHelper::GetAttributeValue
            (
              attrs,//"processor_name"
  //            archAttributeName ,
              stdstrAttributeName.c_str() ,
              stdwstrValue
            )
          )
        {
          m_p_model->m_serviceattributes.m_stdwstrGUICurrentDirFullPathTo =
            stdwstrValue ;
          LOGWN_WSPRINTF(L"set current dir for GUI to %ls" ,
            //stdwstrValue.c_str()
            m_p_model->m_serviceattributes.m_stdwstrGUICurrentDirFullPathTo.
            c_str()
            )
        }
        stdstrAttributeName = "path_to_GUI_exe" ;
        if( XercesHelper::GetAttributeValue
            (
                attrs,//"processor_name"
//                archAttributeName ,
                stdstrAttributeName.c_str() ,
                stdwstrValue
            )
          )
        {
          m_p_model->m_serviceattributes.m_stdwstrPathToGUIexe =
            stdwstrValue ;
          LOGWN_WSPRINTF(L"set path to GUI exe to %ls" , //stdwstrValue.c_str()
            m_p_model->m_serviceattributes.m_stdwstrPathToGUIexe.c_str() )
        }
      }
  //    else if( m_strElementName == "path_to_GUI_exe" )
  //    {
  //
  //    }
      //Release memory AFTER comparing.
      XMLString::release(&pchXMLelementName);
	  }
	}
	
	void SAX2ServiceConfigHandler::fatalError(const SAXParseException& exception)
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
