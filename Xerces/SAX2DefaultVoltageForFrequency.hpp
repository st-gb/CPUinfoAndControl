/* 
 * File:   SAX2_CPUspecificHandler.hpp
 * Author: Stefan
 *
 * Created on 24. August 2009, 20:20
 */

#ifndef _SAX2_DEFAULT_VOLTAGE_FOR_FREQUENCY_HPP
#define	_SAX2_DEFAULT_VOLTAGE_FOR_FREQUENCY_HPP

//#ifdef COMPILE_WITH_XERCES
#include "../ModelData/PStates.h"
#include "../global.h" //for DEBUG(...) etc.
#include "../ModelData/ModelData.hpp"
#include "../UserInterface.hpp"
#include "Xerces/XercesHelper.hpp"
#include "ModelData/RegisterData.hpp"
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/Attributes.hpp> //for "xercesc_2_8::Attributes"
#include <string> //for std::string

//Needed for verzichten auf the exact namspace.
XERCES_CPP_NAMESPACE_USE

class MSRdata ;
class UserInterface ;
class Model ;
class XercesHelper ;

//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
#define XERCES_STATIC_LIBRARY

class SAX2DefaultVoltageForFrequency
  : public DefaultHandler
{
  XercesHelper m_xerceshelper ;
public:
    SAX2DefaultVoltageForFrequency(
        UserInterface & r_userinterface,
        Model & r_modeldata
        );
    SAX2DefaultVoltageForFrequency(const SAX2DefaultVoltageForFrequency & orig);
    //virtual ~SAX2DefaultVoltageForFrequency();
//private:
    UserInterface * m_p_userinterface ;
    Model * m_p_model ;
    std::string m_strElementName ;
    void endElement(
        const   XMLCh * const    cpc_xmlchURI ,
        const   XMLCh * const    cpc_xmlchLocalName ,
        const   XMLCh * const    cpc_xmlchQualifiedName ,
        const   Attributes &    cr_xercesc_attributes
      );
    void handleFreqAndMaxVoltageElement(
      const   //xercesc_2_8::
        Attributes & attrs
      ) ;
    void handleFreqAndVoltageElement(
      const   //xercesc_2_8::
        Attributes & attrs
      ) ;
    void fatalError(const SAXParseException& exception) ;
    void startElement(
        const   XMLCh * const    cpc_xmlchURI ,
        const   XMLCh * const    cpc_xmlchLocalName ,
        const   XMLCh * const    cpc_xmlchQualifiedName ,
        const   Attributes &    cr_xercesc_attributes
      );
};

#endif	/* _SAX2_DEFAULT_VOLTAGE_FOR_FREQUENCY_HPP */
