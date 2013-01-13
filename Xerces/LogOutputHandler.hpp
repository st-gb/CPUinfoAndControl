/*
 * LogOutputHandler.hpp
 *
 *  Created on: 16.10.2012
 *      Author: Stefan
 */

#ifndef LOGOUTPUTHANDLER_HPP_
#define LOGOUTPUTHANDLER_HPP_

//base class XERCES_CPP_NAMESPACE::DefaultHandler
#include <xercesc/sax2/DefaultHandler.hpp>

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

namespace Xerces
{

  class LogOutputHandler
    : public XERCES_CPP_NAMESPACE::DefaultHandler
  {
  private:
    Model & m_r_model;
    UserInterface & m_r_userinterface;
  public:
    LogOutputHandler(
      UserInterface & r_userinterface,
      Model & r_modeldata
      );
    virtual
    ~LogOutputHandler();
//    void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException & exception) ;
    void startElement(
      const XMLCh * const cpc_xmlchURI ,
      const XMLCh * const cpc_xmlchLocalName ,
      const XMLCh * const cpc_xmlchQualifiedName ,
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
      );
  };

} /* namespace Xerces */
#endif /* LOGOUTPUTHANDLER_HPP_ */
