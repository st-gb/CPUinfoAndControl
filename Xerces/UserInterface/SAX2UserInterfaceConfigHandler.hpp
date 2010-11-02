/*
 * SAX2UserInterfaceConfigHandler.hpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

#ifndef SAX2USERINTERFACECONFIGHANDLER_HPP_
#define SAX2USERINTERFACECONFIGHANDLER_HPP_

//Base class XERCES_CPP_NAMESPACE::DefaultHandler
#include <xercesc/sax2/DefaultHandler.hpp>

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class Model ;
class UserInterface ;
// need to properly scope any forward declarations
XERCES_CPP_NAMESPACE_BEGIN
  class Attributes;
XERCES_CPP_NAMESPACE_END

namespace Xerces
{

  class SAX2UserInterfaceConfigHandler
    : public XERCES_CPP_NAMESPACE::DefaultHandler
  {
  private:
    Model * m_p_model ;
    UserInterface * m_p_userinterface ;
  public:
    SAX2UserInterfaceConfigHandler(
      Model & model ,
      UserInterface * p_userinterface
      );
    virtual
    ~SAX2UserInterfaceConfigHandler();
    //The following functions are only called _once_. So the code is _not_
    //pasted _multiple_ times because it is declared "inline".
    inline void HandleDynamicVoltageAndFrequencyScalingXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleLogFileNameXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleMainFrameXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleServiceXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleToolTipXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleVoltageAndFrequencySettingsDialogXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    void startElement(
      const XMLCh * const cp_xmlchURI,
      const XMLCh * const cp_xmlchLocalName,
      const XMLCh * const cp_xmlchQualifiedName,
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    );
  };

}

#endif /* SAX2USERINTERFACECONFIGHANDLER_HPP_ */
