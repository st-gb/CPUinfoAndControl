/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#ifndef _XMLACCESS_HPP
  #define _XMLACCESS_HPP

//#ifdef COMPILE_WITH_XERCES

#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//#include "../UserInterface.hpp"
#include <ModelData/ModelData.hpp> //class Model
//#include <xercesc/dom/DOMElement.hpp>
//class XERCES_CPP_NAMESPACE::LocalFileInputSource
#include <xercesc/framework/LocalFileInputSource.hpp>
//class XERCES_CPP_NAMESPACE::MemBufInputSource
#include <xercesc/framework/MemBufInputSource.hpp>
//class XERCES_CPP_NAMESPACE::DefaultHandler
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp> //class XERCES_CPP_NAMESPACE::XMLString
#include <winnt.h> //for LPWSTR

 // //to NOT need to prefix the xerces classes with the "xerces::"
	//XERCES_CPP_NAMESPACE_USE 

//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
//#define XERCES_STATIC_LIBRARY

  class Model ;
  class UserInterface ;

  char ReadXMLfileInitAndTermXerces(
    const char * xmlFile,//PStates & pstates
    Model & model,
    UserInterface * p_userinterface ,
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    );
  char ReadXMLdocumentInitAndTermXerces(
    BYTE arbyXMLdata [] ,
    DWORD dwByteSize ,
    const LPWSTR lpwstrBufferIdentifier ,
    Model & model,
    UserInterface * p_userinterface ,
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    ) ;
  char ReadXMLdocument(
    XERCES_CPP_NAMESPACE::InputSource & r_inputsource, //PStates & pstates
//    Model & model,
    UserInterface * p_userinterface ,
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    ) ;

  /** Warning: Xerces init (XMLPlatformUtils::Initialize(); ) must have been
  * called before calling this function. */
  inline char ReadXMLdocumentWithoutInitAndTermXerces(
    BYTE arbyXMLdata [] ,
    DWORD dwDataSizeInByte ,
    const LPWSTR lpwstrBufferIdentifier ,
    Model & model,
    UserInterface * p_userinterface ,
    //Base class of implementing Xerces XML handlers.
    //This is useful because there may be more than one XML file to read.
    //So one calls this functions with different handlers passed.
    XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    )
  {
    XERCES_CPP_NAMESPACE::MemBufInputSource membufinputsource(
      arbyXMLdata,
      (XMLSize_t) dwDataSizeInByte ,
//      L"IPC_buffer"
      //Cast to (XMLCh*) for Linux' g++, else:
      //"no matching function for call to ‘xercesc_3_0::MemBufInputSource::
      // MemBufInputSource(BYTE*&, DWORD&, WCHAR* const&)’ "
      (XMLCh *) lpwstrBufferIdentifier
      ) ;
    LOGN("before readXMLConfig (InputSource)")
    return ReadXMLdocument(
      membufinputsource ,
//      model,
      p_userinterface ,
      r_defaulthandler
      ) ;
  }
  //return: 0=success
  inline BYTE ReadXMLfileWithoutInitAndTermXercesInline(
    const char * cp_chXMLfilePath,
//    Model & model,
    UserInterface * p_userinterface ,
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    )
  {
    BYTE byReturn = 1 ;
//    LOG( "read XML configuration--filename: \"" << cp_chXMLfilePath << "\"" );
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "read XML configuration--filename: \"" << cp_chXMLfilePath << "\"" )
    //  XERCES_CPP_NAMESPACE::LocalFileInputSource xerces_localfileinputsource(
    //    XERCES_STRING_FROM_ANSI_STRING(cp_chXMLfilePath) ) ;
    XMLCh * p_xmlchXMLfilePath = XERCES_CPP_NAMESPACE::XMLString::transcode(
      cp_chXMLfilePath);
    if( p_xmlchXMLfilePath )
    {
      XERCES_CPP_NAMESPACE::LocalFileInputSource xerces_localfileinputsource(
        p_xmlchXMLfilePath ) ;
      byReturn = ! ReadXMLdocument(
        xerces_localfileinputsource ,
//        model,
        p_userinterface ,
        r_defaulthandler
        ) ;
      XERCES_CPP_NAMESPACE::XMLString::release( & p_xmlchXMLfilePath);
    }
    LOGN( FULL_FUNC_NAME << "--after reading config file")
    return byReturn ;
  }
//#endif //#ifdef COMPILE_WITH_XERCES
#endif //#ifndef _XMLACCESS_HPP
