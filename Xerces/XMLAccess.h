#pragma once //include guard

#ifdef COMPILE_WITH_XERCES

  //#include "../PStates.h"
  //#include "../ModelData/ModelData.hpp"
  //#include "../UserInterface.hpp"
  //#include "../Controller/PumaStateCtrl.h"
  #include <xercesc/sax2/DefaultHandler.hpp>
#include <ModelData/ModelData.hpp> //class Model

	XERCES_CPP_NAMESPACE_USE //to NOT need to prefix the xerces classes with the "xerces::"
//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
//#define XERCES_STATIC_LIBRARY

  class Model ;
  class UserInterface ;

  char readXMLConfig(
    const char* xmlFile,//PStates & pstates
    Model & model,
    UserInterface * p_userinterface ,
    //  PumaStateCtrl * p_pumastatectrl 
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    DefaultHandler & r_defaulthandler
    );

#endif //#ifdef COMPILE_WITH_XERCES
