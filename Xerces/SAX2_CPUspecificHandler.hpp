/* 
 * File:   SAX2_CPUspecificHandler.hpp
 * Author: Stefan
 *
 * Created on 24. August 2009, 20:20
 */

#ifndef _SAX2_CPUSPECIFICHANDLER_HPP
#define	_SAX2_CPUSPECIFICHANDLER_HPP

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

class SAX2_CPUspecificHandler
  : public DefaultHandler
{
  XercesHelper m_xerceshelper ;
public:
    SAX2_CPUspecificHandler(
        UserInterface & r_userinterface,
        Model & r_modeldata
        );
    SAX2_CPUspecificHandler(const SAX2_CPUspecificHandler& orig);
    virtual ~SAX2_CPUspecificHandler();
//private:
    bool m_bInsideValidMSRelement ;
    UserInterface * mp_userinterface ;
    Model * mp_modeldata ;
    std::string m_strElementName ;
    std::vector<MSRdata>::reverse_iterator m_stdvec_msrdata_riter;
    void endElement(
        const   XMLCh * const    cpc_xmlchURI ,
        const   XMLCh * const    cpc_xmlchLocalName ,
        const   XMLCh * const    cpc_xmlchQualifiedName ,
        const   Attributes &    cr_xercesc_attributes
      );
    void fatalError(const SAXParseException& exception) ;
    void startElement(
        const   XMLCh * const    cpc_xmlchURI ,
        const   XMLCh * const    cpc_xmlchLocalName ,
        const   XMLCh * const    cpc_xmlchQualifiedName ,
        const   Attributes &    cr_xercesc_attributes
      );
};

#endif	/* _SAX2_CPUSPECIFICHANDLER_HPP */

