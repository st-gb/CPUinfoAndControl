/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   SAX2_CPUspecificHandler.hpp
 * Author: Stefan
 *
 * Created on 24. August 2009, 20:20
 */

#ifndef _SAX2_DEFAULT_VOLTAGE_FOR_FREQUENCY_HPP
#define	_SAX2_DEFAULT_VOLTAGE_FOR_FREQUENCY_HPP

//#ifdef COMPILE_WITH_XERCES
//#include <global.h> //for DEBUG(...) etc.
//#include <ModelData/RegisterData.hpp>
//#include <Xerces/XercesHelper.hpp>
#include <xercesc/sax2/DefaultHandler.hpp> //base class DefaultHandler
#include <string> //for std::string

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
//class MSRdata ;
class UserInterface ;
class Model ;
//class XercesHelper ;
//Need to properly scope any forward declarations.
XERCES_CPP_NAMESPACE_BEGIN
  class Attributes;
XERCES_CPP_NAMESPACE_END

//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
//#define XERCES_STATIC_LIBRARY

class SAX2VoltagesForFrequencyHandler
  : public XERCES_CPP_NAMESPACE::DefaultHandler
{
//  XercesHelper m_xerceshelper ;
public:
  SAX2VoltagesForFrequencyHandler(
    UserInterface & r_userinterface,
    Model & r_modeldata
    );
  SAX2VoltagesForFrequencyHandler(const SAX2VoltagesForFrequencyHandler & orig);
  //virtual ~SAX2VoltagesForFrequencyHandler();
//private:
  UserInterface * m_p_userinterface ;
  Model * m_p_model ;
  std::string m_strElementName ;
  //void endElement(
  //    const   XMLCh * const    cpc_xmlchURI ,
  //    const   XMLCh * const    cpc_xmlchLocalName ,
  //    const   XMLCh * const    cpc_xmlchQualifiedName //,
  //    //const   Attributes &    cr_xercesc_attributes
  //  );
  void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException & exception) ;
  void handleFreqAndMaxVoltageElement(
    const XERCES_CPP_NAMESPACE::Attributes & attrs
    ) ;
  void handleFreqAndVoltageElement(
    const XERCES_CPP_NAMESPACE::Attributes & attrs
    ) ;
  void startElement(
    const XMLCh * const cpc_xmlchURI ,
    const XMLCh * const cpc_xmlchLocalName ,
    const XMLCh * const cpc_xmlchQualifiedName ,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    );
};

#endif	/* _SAX2_DEFAULT_VOLTAGE_FOR_FREQUENCY_HPP */
