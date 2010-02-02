#pragma once //include guard

#ifdef COMPILE_WITH_XERCES

  //#include "../PStates.h"
  //#include "../ModelData/ModelData.hpp"
  //#include "../UserInterface.hpp"
  //#include "../Controller/PumaStateCtrl.h"
  #include <xercesc/sax2/DefaultHandler.hpp>
  //#include <xercesc/dom/DOMElement.hpp>
#include <ModelData/ModelData.hpp> //class Model

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

XERCES_CPP_NAMESPACE_BEGIN

  //class XERCES_CPP_NAMESPACE::DOMDocument ;
  //class XERCES_CPP_NAMESPACE::DOMElement ;
  //class XERCES_CPP_NAMESPACE::DOMXPathResult ;
  class DOMDocument ;
  class DOMElement ;
  class DOMImplementation ;
  class DOMLSParser ;
  class DOMXPathNSResolver ;
  class DOMXPathResult ;

  XERCES_CPP_NAMESPACE_END

  class XercesConfigurationHandler
    //: public I_ConfigurationHandler
  {
  private:
    std::map<WORD,WORD> m_stdmapFreqInMHzInDOMtree2DOMindex ;
    XERCES_CPP_NAMESPACE::DOMDocument * mp_dom_document ;
    XERCES_CPP_NAMESPACE::DOMElement * mp_dom_elementRoot ;
    XERCES_CPP_NAMESPACE::DOMElement * mp_dom_elementFreqnVolt ;
    XERCES_CPP_NAMESPACE::DOMXPathResult * mp_domxpathresult ;
    Model * mp_model ;
    //void 
    //std::map<WORD,WORD>::const_iterator AddFrequencyToDOMtree(WORD wFreq) ;
    XERCES_CPP_NAMESPACE::DOMElement * AddFrequencyToDOMtree(WORD wFreq) ;
    void AppendDefaultVoltages() ;
    XERCES_CPP_NAMESPACE::DOMXPathNSResolver * BuildStdmapFreqInMHzInDOMtree2DOMindex(
      std::map<WORD,WORD> & r_stdmapFreqInMHzInDOMtree2DOMindex
      ) ;
    void GetDOM_XPathResultForFrequencies() ;
    void GetFreqnVoltDOMelement( WORD wFreqInMHz 
      , XERCES_CPP_NAMESPACE::DOMElement * & mp_dom_element ) ;
    bool PossiblyAddDefaultVoltages() ;
    bool PossiblyAddLowestStableVoltages() ;
    bool PossiblyAddVoltages(
      const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq 
      , //e.g. "max_voltage_in_Volt"
      const char * const cpc_XMLAttrName 
      //"true": only look if there would be changes made (useful to check
      // for the purpose: if there are changes then ask the user whether to 
      //save the changes.
      , bool bOnlySimulate 
      ) ;
    bool PossiblyAddWantedVoltages() ;
  public:
    bool ConfigurationChanged(std::string & r_strPstateSettingsFileName ) ;
    XercesConfigurationHandler(Model * p_model ) ;
    BYTE //mergeXMLfileDOM( 
      MergeWithExistingConfigFile(
      const char * p_chFullXMLFilePath
      , Model & r_model,
      std::string & r_strPstateSettingsFileName 
      ) ;
  } ;

  char readXMLConfig(
    const char* xmlFile,//PStates & pstates
    Model & model,
    UserInterface * p_userinterface ,
    //  PumaStateCtrl * p_pumastatectrl 
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
    XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    );
  BYTE readXMLfileDOM( 
    const char * p_chFullXMLFilePath 
    , XERCES_CPP_NAMESPACE::DOMDocument * & p_dom_document
    , XERCES_CPP_NAMESPACE::DOMLSParser * & parser
    , XERCES_CPP_NAMESPACE::DOMImplementation * & p_dom_implementation 
    ) ;

#endif //#ifdef COMPILE_WITH_XERCES
