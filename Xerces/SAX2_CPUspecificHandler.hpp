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

#ifndef _SAX2_CPUSPECIFICHANDLER_HPP
#define	_SAX2_CPUSPECIFICHANDLER_HPP

//#ifdef COMPILE_WITH_XERCES

////Base class XERCES_CPP_NAMESPACE::DefaultHandler
//#include <xercesc/sax2/DefaultHandler.hpp>
//Base class DocumentLocationSAX2Handler
#include <Xerces/DocumentLocationSAX2Handler.hpp>
#include <string> //for std::string
#include <vector> //class std::vector

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class CPUIDdata ;
class Model ;
class MSRdata ;
class UserInterface ;
class XercesHelper ;
// need to properly scope any forward declarations
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

class SAX2_CPUspecificHandler
  : public //XERCES_CPP_NAMESPACE::DefaultHandler
  Apache_Xerces::DocumentLocationSAX2Handler
{
private:
//  const XERCES_CPP_NAMESPACE::Locator * const m_p_locator;
  bool m_bInsideValidCPUIDelement ;
  bool m_bInsideValidMSRelement ;
  Model * mp_modeldata ;
  std::string m_strElementName ;
public:
  std::vector<MSRdata>::reverse_iterator m_stdvec_msrdata_riter;
  std::vector<CPUIDdata>::reverse_iterator m_stdvec_cpuiddata_riter;
  UserInterface * mp_userinterface ;

//  XercesHelper m_xerceshelper ;
  SAX2_CPUspecificHandler(
    UserInterface & r_userinterface,
    Model & r_modeldata
    );
  SAX2_CPUspecificHandler(const SAX2_CPUspecificHandler & orig);
  virtual ~SAX2_CPUspecificHandler();
  void endElement(
    const XMLCh * const cpc_xmlchURI ,
    const XMLCh * const cpc_xmlchLocalName ,
    const XMLCh * const cpc_xmlchQualifiedName// ,
    //const   Attributes &    cr_xercesc_attributes
    );
  void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException & exception) ;
  void HandleCPUIDelement( const XERCES_CPP_NAMESPACE::Attributes &
    r_xercesc_attributes) ;
  void HandleInsideCPUIDelement(const XERCES_CPP_NAMESPACE::Attributes &
    r_xercesc_attributes) ;
  void HandleInsideMSRelement(const XERCES_CPP_NAMESPACE::Attributes &
    r_xercesc_attributes) ;
  void HandleMSRelement(const XERCES_CPP_NAMESPACE::Attributes &
      cr_xercesc_attributes) ;
//  void setDocumentLocator(const XERCES_CPP_NAMESPACE::Locator * const locator);
  void startElement(
    const XMLCh * const cpc_xmlchURI ,
    const XMLCh * const cpc_xmlchLocalName ,
    const XMLCh * const cpc_xmlchQualifiedName ,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    );
};

#endif	/* _SAX2_CPUSPECIFICHANDLER_HPP */
