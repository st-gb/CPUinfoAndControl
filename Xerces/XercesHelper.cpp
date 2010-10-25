/* 
 * File:   XercesHelper.cpp
 * Author: Stefan
 * 
 * Created on 24. August 2009, 22:11
 */
#include "../stdafx.h"
#include "XercesHelper.hpp"
#include <preprocessor_macros/logging_preprocessor_macros.h> //for LOGN(...)
#include <Xerces/XercesString.hpp> //Xerces::ToStdString(const XMLCh * )

//for XERCES_CPP_NAMESPACE::XMLString::transcode(...)
#include <xercesc/util/XMLString.hpp>

////To NOT need to prefix the xerces classes with the "xercesc...::"
//XERCES_CPP_NAMESPACE_USE
//Better not do "using namespace XERCES_CPP_NAMESPACE ;" because so it's
//clearer that Xerces is used.

//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
//#define XERCES_STATIC_LIBRARY

//Code is equal on many code places.
bool x86InfoAndControl::InitializeXerces()
{
  try
  {
    LOGN("BEFORE initializing Xerces") ;
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must initialize the Xerces system before using the API[...]"
    //Initialize() must be called _before_ any Xerces function call, else
    // SIGSEV / program crash.
    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
    LOGN("Xerces successfully initialized") ;
  }
  catch(const XERCES_CPP_NAMESPACE::XMLException & cr_xercesc_xml_exception )
  {
    char * pMsg = XERCES_CPP_NAMESPACE::XMLString::transcode(
      cr_xercesc_xml_exception.getMessage() );
    LOGN( "Error during Xerces-c Initialization.\n"
      << "  Exception message:"
      << pMsg )
    XERCES_CPP_NAMESPACE::XMLString::release(&pMsg);
  //      return 1;
    return false ;
  }
  return true ;
}

XercesHelper::XercesHelper() {
}

XercesHelper::XercesHelper(UserInterface & r_userinterface )
{
  mp_userinterface = & r_userinterface ;
}

XercesHelper::XercesHelper(const XercesHelper& orig) {
}

XercesHelper::~XercesHelper() {
}

//BYTE //SAX2MainConfigHandler::
//    XercesHelper::GetAttributeValue(
//  const Attributes & attrs,
//  const char * lpctstrAttrName,
//  DWORD & r_dwValue)
//{
//  BYTE byGetAttributeValueRetVal = 
//    XercesHelper::GetAttributeValue(
//      attrs,
//      lpctstrAttrName,
//      r_dwValue) ;
//  switch( byGetAttributeValueRetVal )
//  {
//  case FAILURE :
//    mp_userinterface->ShowMessage(
//      "failure getting attribute value for attribute" ) ;
//  case XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT :
//    mp_userinterface->ShowMessage("") ;
//  }
//}

std::string XercesHelper::ToStdString(
  const XMLCh * p_xmlch
  )
{
  return Xerces::ToStdString(p_xmlch) ;
}
