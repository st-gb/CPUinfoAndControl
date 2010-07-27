#pragma once //include guard
#ifndef _XMLACCESS_HPP
#define _XMLACCESS_HPP

//#ifdef COMPILE_WITH_XERCES

//#include "../UserInterface.hpp"
#include <ModelData/ModelData.hpp> //class Model
//#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

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

  char ReadXMLdocumentInitAndTermXerces(
    const char* xmlFile,//PStates & pstates
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
  char readXMLConfig(
    XERCES_CPP_NAMESPACE::InputSource & r_inputsource, //PStates & pstates
    Model & model,
    UserInterface * p_userinterface ,
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    ) ;
  //warning: Xerces init (XMLPlatformUtils::Initialize(); ) must have been
  //called before calling this function.
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
      dwDataSizeInByte ,
//      L"IPC_buffer"
      lpwstrBufferIdentifier
      ) ;
    LOGN("before readXMLConfig (InputSource)")
    return readXMLConfig(
      membufinputsource ,
      model,
      p_userinterface ,
      r_defaulthandler
      ) ;
  }
//#endif //#ifdef COMPILE_WITH_XERCES
#endif //#ifndef _XMLACCESS_HPP
