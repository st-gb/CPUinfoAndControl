/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
/** Need to properly scope any forward declarations */
namespace Attributes {
  class MainFrame;
}

#include <data_structures/Trie/NodeTrie/NodeTrie.hpp>

namespace Xerces
{

  class SAX2UserInterfaceConfigHandler
    : public XERCES_CPP_NAMESPACE::DefaultHandler
  {
  private:
    NodeTrie<BYTE> m_trieExcludeFromLogging ;
    Model * m_p_model ;
    uint16_t m_ui16t_WisdomNumber;
    UserInterface * m_p_userinterface ;
    Attributes::MainFrame & m_mainFrameAttributes;
    const XERCES_CPP_NAMESPACE::Attributes * m_p_xercesc_attributes;
  public:
    SAX2UserInterfaceConfigHandler(
      Model & model ,
      UserInterface * p_userinterface
      );
    virtual
    ~SAX2UserInterfaceConfigHandler();
    void GetFontSizeAttributeValue(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes,
      const std::string & attrName,
      int & nFontSize);
    /*template <typename T>*/ void GetAttributeValue(
      const char * const pchAttributeName, bool & value);

    /* The following functions are only called _once_. So the code is _not_
    pasted _multiple_ times because it is declared "inline". */
    inline void HandleDynamicVoltageAndFrequencyScalingXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleLogFileNameXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleMainFrameXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleServiceXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleTaskBarIconXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleTaskBarIcon_CPUcoreUsageXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleToolTipXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleVoltageAndFrequencySettingsDialogXMLelement(
      const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes ) ;
    inline void HandleWisdomXMLelement(
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
