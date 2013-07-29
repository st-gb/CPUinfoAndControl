/*
 * SAX2.hpp
 *
 *  Created on: 01.01.2013
 *      Author: Stefan
 */

#ifndef SAX2_HPP_
#define SAX2_HPP_

//base class XERCES_CPP_NAMESPACE::DefaultHandler
#include <xercesc/sax2/DefaultHandler.hpp>
//Apache_Xerces::class DocumentLocationSAX2Handler
#include <Xerces/DocumentLocationSAX2Handler.hpp>

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class Model;
class UserInterface ;

namespace Xerces
{

  class SAX2CPUcontrollerConfiguration
    : public //XERCES_CPP_NAMESPACE::DefaultHandler
      Apache_Xerces::DocumentLocationSAX2Handler
  {
  private:
    Model & m_r_model;
    const UserInterface & m_r_userinterface;
    const CPUcoreData & m_c_r_cpucoredata;
    unsigned m_SuitabilityLevel, m_HighestSuitability;
//    const Model & m_r_model;
  public:
    enum suitabilityLevel { vendor = 1, family, model, stepping };
    std::string m_mostSuitableCPUinfoGetterAndOrController;
    std::string m_mostSuitableCPUcoreUsageGetter;
    std::string m_mostSuitableCPUregisterDataFile;
    SAX2CPUcontrollerConfiguration( const UserInterface & , const Model & );
    virtual
    ~SAX2CPUcontrollerConfiguration();
    void startElement(
      const XMLCh * const cpc_xmlchURI ,
      const XMLCh * const cpc_xmlchLocalName ,
      const XMLCh * const cpc_xmlchQualifiedName ,
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
      );
    void ShowMessage(const std::wstring &);
  };

} /* namespace Xerces */
#endif /* SAX2_HPP_ */
