/* 
 * File:   XercesHelper.cpp
 * Author: Stefan
 * 
 * Created on 24. August 2009, 22:11
 */
#include "../stdafx.h"
#include <global.h> //for if FAILURE etc.
//#include <preprocessor_macros/logging_preprocessor_macros.h> //for LOGN(...)
#include <limits.h>
#include <locale>
#include <sstream>
#include <tchar.h>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLString.hpp> //for XMLString::transcode(...)

#include "XercesHelper.hpp"
#include <UserInterface/UserInterface.hpp> //for XMLString::transcode(...)


XERCES_CPP_NAMESPACE_USE //to NOT need to prefix the xerces classes with the "xerces::"
//XERCES_CPP_NAMESPACE_BEGIN
//  class Attributes;
//XERCES_CPP_NAMESPACE_END

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
    //Initialize() must be called _before_ any Xerces function call, else SIGSEV
    // /program crash.
    XMLPlatformUtils::Initialize();
    LOGN("Xerces successfully initialized") ;
  }
  catch(const XMLException & toCatch)
  {
    char *pMsg = XMLString::transcode(toCatch.getMessage());
    LOGN( "Error during Xerces-c Initialization.\n"
         << "  Exception message:"
         << pMsg )
    XMLString::release(&pMsg);
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

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue(
  const Attributes & attrs,
  const char * pc_chAttributeName,
  bool & rbValue
  )
{
  BYTE byReturn = FAILURE;
  //Cast to XMLCh for "Attributes::getValue(...)".
  XMLCh * p_xmlchAttributeName = XMLString::transcode(pc_chAttributeName) ;
  if( p_xmlchAttributeName )
  {
      const XMLCh * cp_xmlchAttributeValue = attrs.getValue(// const XMLCh *const qName
        p_xmlchAttributeName
        //"number"
        ) ;
      //If the attribute exists.
      if(cp_xmlchAttributeValue)
      {
        char * pchAttributeValue = XMLString::transcode(cp_xmlchAttributeValue) ;
        if( pchAttributeValue )
        {
            if( //if the strings are identical
              strcmp(pchAttributeValue,"true") == 0 )
            {
              rbValue = true;
              byReturn = SUCCESS;
            }
            else
              if(strcmp(pchAttributeValue,"false") == 0 )
              {
                rbValue = false;
                byReturn = SUCCESS;
              }
              //else
              //  byReturn = FAILURE;
            //Release memory of dyn. alloc. buffer (else memory leaks).
            XMLString::release(&pchAttributeValue);
        }
      }
      //Release memory of dyn. alloc. buffer (else memory leaks).
      XMLString::release(&p_xmlchAttributeName);
  }
  return byReturn ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue(
    const Attributes & attrs,
    char * lpctstrAttrName,
    BYTE & rbyValue
    )
{
  BYTE byReturn = FAILURE;
  XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  if( p_xmlchAttributeName )
  {
      const XMLCh * cp_xmlchAttributeValue = attrs.getValue(// const XMLCh *const qName
        p_xmlchAttributeName
        //"number"
        ) ;
      //If the attribute exists.
      if(cp_xmlchAttributeValue)
      {
        char * pchAttributeValue = XMLString::transcode(cp_xmlchAttributeValue) ;
        if( pchAttributeValue )
        {
            int nAtoiResult ;
            //"casting" to a std::string is not really necessary, but I
            //want to avoid the C-function "strcmp".
            std::string strAttributeValue = std::string(pchAttributeValue);
            if(
              //atoi(pchNumber) <> 0: If pchNumber is a valid number.
              ( nAtoiResult = atoi(pchAttributeValue ) )
              //Because atoi(...) returns "0" also for errors.
              ||
              ! nAtoiResult // <=> nAtoiResult == 0
              && strAttributeValue == "0"
              )
            {
              byReturn = SUCCESS;
              rbyValue = nAtoiResult ;
            }
            //Release memory of dyn. alloc. buffer (else memory leaks).
            XMLString::release(&pchAttributeValue);
              //else
              //  byReturn = FAILURE;
        }
      }
      //Release memory of dyn. alloc. buffer (else memory leaks).
      XMLString::release(&p_xmlchAttributeName);
  }
  return byReturn ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue(
  const Attributes & attrs,
  const char * lpctstrAttrName,
  //DWORD & r_dwValue
  //StdStringToDestFormat & r_stdstringtodestformat ,
  BYTE (* pfn )(std::string & strAttributeValue ,
  void * pv_AttributeValue ) ,
  //PVOID 
  void * pv_AttributeValue
  )
{
  BYTE byReturn = FAILURE;
  XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  if( p_xmlchAttributeName )
  {
      const XMLCh * cp_xmlchAttributeValue = attrs.getValue(// const XMLCh *const qName
        p_xmlchAttributeName
        //"number"
        ) ;
      //If the attribute exists.
      if(cp_xmlchAttributeValue)
      {
        char * pchAttributeValue = XMLString::transcode(cp_xmlchAttributeValue) ;
        if( pchAttributeValue )
        {
            //int nAtoiResult ;
            std::string strAttributeValue = std::string(pchAttributeValue);
            //if(
              //atoi(pchNumber) <> 0: If pchNumber is a valid number.
              //( nAtoiResult = atoi(pchAttributeValue ) )
              ////Because atoi(...) returns "0" also for errors.
              //||
              //! nAtoiResult // <=> nAtoiResult == 0
              //&& strAttributeValue == "0"
              //)
              //r_stdstringtodestformat.ConvertDataFormat(
            byReturn =
              (*pfn)(
                strAttributeValue ,
                pv_AttributeValue 
                ) ;
            //  )
            //{
            //  byReturn = SUCCESS;
            //  //r_dwValue = nAtoiResult ;
            //}
            //else
            //  byReturn = XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT ;
            //Release memory of dyn. alloc. buffer (else memory leaks).
            XMLString::release(&pchAttributeValue);
              //else
              //  byReturn = FAILURE;
        }
        else
          byReturn = XERCES_ERROR_CONVERTING_ATTRIBUTE_VALUE_TO_C_STRING ;
      }
      else
        byReturn = XERCES_ATTRIBUTE_VALUE_DOES_NOT_EXIST ;
      //Release memory of dyn. alloc. buffer (else memory leaks).
      XMLString::release(&p_xmlchAttributeName);
  }
  else
    byReturn = XERCES_ERROR_CONVERTING_ATTRIBUTE_NAME_TO_XERCES_STRING ;
  return byReturn ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue(
  const Attributes & attrs,
  const char * lpctstrAttrName,
  //DWORD & r_dwValue
  //StdStringToDestFormat & r_stdstringtodestformat ,
  BYTE ( XercesHelper::*pfn )(
    //const XercesHelper * cp_xerceshelper ,
    std::string & strAttributeValue ,
    void * pv_AttributeValue ) ,
  //const XercesHelper * cp_xerceshelper ,
  //PVOID 
  void * pv_AttributeValue
  )
{
  BYTE byReturn = FAILURE;
  XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  if( p_xmlchAttributeName )
  {
      const XMLCh * cp_xmlchAttributeValue = attrs.getValue(// const XMLCh *const qName
        p_xmlchAttributeName
        //"number"
        ) ;
      //If the attribute exists.
      if(cp_xmlchAttributeValue)
      {
        char * pchAttributeValue = XMLString::transcode(cp_xmlchAttributeValue) ;
        if( pchAttributeValue )
        {
            //int nAtoiResult ;
            std::string strAttributeValue = std::string(pchAttributeValue);
            //if(
              //atoi(pchNumber) <> 0: If pchNumber is a valid number.
              //( nAtoiResult = atoi(pchAttributeValue ) )
              ////Because atoi(...) returns "0" also for errors.
              //||
              //! nAtoiResult // <=> nAtoiResult == 0
              //&& strAttributeValue == "0"
              //)
              //r_stdstringtodestformat.ConvertDataFormat(
            byReturn =
              (this->*pfn)(
                //cp_xerceshelper,
                //this ,
                strAttributeValue ,
                pv_AttributeValue 
                ) ;
            //  )
            //{
            //  byReturn = SUCCESS;
            //  //r_dwValue = nAtoiResult ;
            //}
            //else
            //  byReturn = XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT ;
            //Release memory of dyn. alloc. buffer (else memory leaks).
            XMLString::release(&pchAttributeValue);
              //else
              //  byReturn = FAILURE;
            //if( byReturn != SUCCESS )
            //  mp_userinterface->Confirm("") ;
        }
        else
          byReturn = XERCES_ERROR_CONVERTING_ATTRIBUTE_VALUE_TO_C_STRING ;
      }
      else
        byReturn = XERCES_ATTRIBUTE_VALUE_DOES_NOT_EXIST ;
      //Release memory of dyn. alloc. buffer (else memory leaks).
      XMLString::release(&p_xmlchAttributeName);
  }
  else
    byReturn = XERCES_ERROR_CONVERTING_ATTRIBUTE_NAME_TO_XERCES_STRING ;
  return byReturn ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue(
  const Attributes & attrs,
  const char * lpctstrAttrName,
  WORD & rwValue
  )
{
  BYTE byReturn = FAILURE;
  XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  if( p_xmlchAttributeName )
  {
      const XMLCh * cp_xmlchAttributeValue = attrs.getValue(// const XMLCh *const qName
        p_xmlchAttributeName
        //"number"
        ) ;
      //If the attribute exists.
      if(cp_xmlchAttributeValue)
      {
        char * pchAttributeValue = XMLString::transcode(cp_xmlchAttributeValue) ;
        if( pchAttributeValue )
        {
            int nAtoiResult ;
            std::string strAttributeValue = std::string(pchAttributeValue);
            if(
              //atoi(pchNumber) <> 0: If pchNumber is a valid number.
              ( nAtoiResult = atoi(pchAttributeValue ) )
              //Because atoi(...) returns "0" also for errors.
              ||
              ! nAtoiResult // <=> nAtoiResult == 0
              && strAttributeValue == "0"
              )
            {
              byReturn = SUCCESS;
              rwValue = nAtoiResult ;
              LOGN("successfully got \"" << lpctstrAttrName << 
                "\" attribute value: " << rwValue )
            }
            //Release memory of dyn. alloc. buffer (else memory leaks).
            XMLString::release(&pchAttributeValue);
              //else
              //  byReturn = FAILURE;
        }
      }
      //Release memory of dyn. alloc. buffer (else memory leaks).
      XMLString::release(&p_xmlchAttributeName);
  }
  return byReturn ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue(
  const Attributes & r_xercesc_attributes,
  const char * archAttributeName,
  DWORD & r_dwValue
  )
{
//  BYTE byReturn = FAILURE;
  //XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  //if( p_xmlchAttributeName )
  //{
  //    const XMLCh * cp_xmlchAttributeValue = attrs.getValue(// const XMLCh *const qName
  //      p_xmlchAttributeName
  //      //"number"
  //      ) ;
  //    //If the attribute exists.
  //    if(cp_xmlchAttributeValue)
  //    {
  //      char * pchAttributeValue = XMLString::transcode(cp_xmlchAttributeValue) ;
  //      if( pchAttributeValue )
  //      {
  //          int nAtoiResult ;
  //          std::string strAttributeValue = std::string(pchAttributeValue);
  //          if(
  //            //atoi(pchNumber) <> 0: If pchNumber is a valid number.
  //            ( nAtoiResult = atoi(pchAttributeValue ) )
  //            //Because atoi(...) returns "0" also for errors.
  //            ||
  //            ! nAtoiResult // <=> nAtoiResult == 0
  //            && strAttributeValue == "0"
  //            )
  //          {
  //            byReturn = SUCCESS;
  //            r_dwValue = nAtoiResult ;
  //          }
  //          else
  //            byReturn = XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT ;
  //          //Release memory of dyn. alloc. buffer (else memory leaks).
  //          XMLString::release(&pchAttributeValue);
  //            //else
  //            //  byReturn = FAILURE;
  //      }
  //    }
  //    //Release memory of dyn. alloc. buffer (else memory leaks).
  //    XMLString::release(&p_xmlchAttributeName);
  return GetAttributeValue
        (
        r_xercesc_attributes,//"processor_name"
        archAttributeName ,
        //* StdStringToDWORD::Convert ,
        //* this->ToDWORD ,
        & XercesHelper::ToDWORD ,
        //strValue
        //dwIndex
        & r_dwValue
        )
      ;
  //}
  //return byReturn ;
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

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue(
  const Attributes & attrs,
  const char * lpctstrAttrName,
  float & rfValue
  )
{
  BYTE byReturn = FAILURE;
  XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  if( p_xmlchAttributeName )
  {
      const XMLCh * pxmlch = attrs.getValue(// const XMLCh *const qName
        p_xmlchAttributeName
        //"number"
        ) ;
      //If the attribute exists.
      if(pxmlch)
      {
        char * pchAttributeValue = XMLString::transcode(pxmlch) ;
        if( pchAttributeValue )
        {
            float fAtofResult = 0.0 ;
            std::string strAttributeValue = std::string(pchAttributeValue);
            //char * pEnd;
            //double d1, d2;

            //atof returned 1 for "1.2" on Linux on German because the decimal
            //point is a "," in German.
            //http://stackoverflow.com/questions/1333451/c-locale-independent-atof:
            //"localeconv (in <locale.h>) returns a pointer to struct whose
            //decimal_point member contains that value. Note that the pointer
            //is valid until the next localeconv() or setlocale() – "
            //fAtofResult = (float) atof(pchAttributeValue ) ;
            std::istringstream istr(pchAttributeValue);

            //Ensure a "." is interpreted as decimal point no matter what the
            //current locale is.
            istr.imbue(std::locale("C"));
            istr >> fAtofResult ;

//            fAtofResult =
//              //http://www.cplusplus.com/reference/clibrary/cstdlib/strtod/:
//              //"If no valid conversion could be performed, a zero value (0.0)
//              //is returned."
//              strtod (
//              #ifdef _DEBUG
//                "1,340"
//              #else
//                pchAttributeValue
//              #endif
//                ,&pEnd);
            if(
              //atof(pchNumber) <> 0.0: If pchNumber is a valid number.
              //Because atoi(...) returns "0" also for errors.
              fAtofResult ||
              ! fAtofResult // <=> nAtoiResult == 0
              && ( strAttributeValue == "0" || strAttributeValue == "0.0" )
              )
            {
              byReturn = SUCCESS;
              rfValue = fAtofResult ;
              LOGN("successfully got \"" << lpctstrAttrName << 
                "\" attribute value: " << rfValue )
            }
            //Release memory of dyn. alloc. buffer (else memory leaks).
            XMLString::release(&pchAttributeValue);
              //else
              //  byReturn = FAILURE;
        }
      }
      //Release memory of dyn. alloc. buffer (else memory leaks).
      XMLString::release(&p_xmlchAttributeName);
  }
  return byReturn ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue
  (
  const Attributes & xercesc_attributes,
  const std::string & cr_stdstrAttributeName,
  std::string & r_strValue
  )
{
    return GetAttributeValue(
        xercesc_attributes,
        cr_stdstrAttributeName.c_str(),
        r_strValue
        ) ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue
  (
  const Attributes & attrs,
  char * lpctstrAttrName,
  std::string & r_strValue
  )
{
  BYTE byReturn = FAILURE;
  XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  if( p_xmlchAttributeName )
  {
      const XMLCh * pxmlch = attrs.getValue(// const XMLCh *const qName
        p_xmlchAttributeName
        //"number"
        ) ;
      //If the attribute exists.
      if(pxmlch)
      {
        char * pchAttributeValue = XMLString::transcode(pxmlch) ;
        if( pchAttributeValue )
        {
            r_strValue = std::string(pchAttributeValue);
            byReturn = SUCCESS;
            //Release memory of dyn. alloc. buffer (else memory leaks).
            XMLString::release(&pchAttributeValue);
        }
      }
      //Release memory of dyn. alloc. buffer (else memory leaks).
      XMLString::release(&p_xmlchAttributeName);
  }
  return byReturn ;
}

BYTE //SAX2MainConfigHandler::
    XercesHelper::GetAttributeValue
  (
  const Attributes & attrs,
  const char * lpctstrAttrName,
//  std::string & r_stdstrAttributeName ,
  std::wstring & r_stdwstrValue
  )
{
  BYTE byReturn = FAILURE;
  XMLCh * p_xmlchAttributeName = XMLString::transcode(lpctstrAttrName) ;
  if( p_xmlchAttributeName )
  {
    const XMLCh * pxmlch = attrs.getValue(// const XMLCh *const qName
      p_xmlchAttributeName
      //"number"
      ) ;
    //If the attribute exists.
    if(pxmlch)
    {
      r_stdwstrValue = std::wstring(
        //Avoid g++ error "no matching function for call to
        //‘std::basic_string<wchar_t, std::char_traits<wchar_t>,
        //std::allocator<wchar_t> >::basic_string(const XMLCh*&)’ "
        (wchar_t *)
        pxmlch);
      byReturn = SUCCESS;
    }
    //Release memory of dyn. alloc. buffer (else memory leaks).
    XMLString::release(&p_xmlchAttributeName);
  }
  return byReturn ;
}

std::string XercesHelper::ToStdString(
  const XMLCh * p_xmlch
  )
{
  std::string strValue ;
  //If the attribute exists.
  if(p_xmlch)
  {
    char * pchAttributeValue = XMLString::transcode(p_xmlch) ;
    if( pchAttributeValue )
    {
        strValue = std::string(pchAttributeValue);
        //Release memory of dyn. alloc. buffer (else memory leaks).
        XMLString::release(&pchAttributeValue);
    }
  }
  return strValue ;
}

  BYTE XercesHelper::ToDWORD(
    std::string & strAttributeValue ,
    void * pv_AttributeValue 
    )
  {
    BYTE byReturn = FAILURE ;
    //TCHAR * p_tch ;
    char * p_ch ;
    DWORD dwResult = //strtoul
      //_tcstoul( strAttributeValue.c_str(), &p_tch ,//10
      //std::string is a single byte type and so needs the "char" version. 
      strtoul( strAttributeValue.c_str(), & p_ch ,
      //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.VisualStudio.v80.de/dv_vccrt/html/38f2afe8-8178-4e0b-8bbe-d5c6ad66e3ab.htm:
      //"If base is 0, the initial characters of the string pointed to by 
      //nptr are used to determine the base."
      0
      ) ;
    //dwRes = atol(strAttributeValue.c_str() ;
    //Use atoi64 because atol return a long value 2^31 to -2^31 but a 
    //DWORD can go to 2^32
    //__int64 i64 = _atoi64(strAttributeValue.c_str() ) ;
    
    if ( //The return value is 0 for _atoi64 if the input cannot be 
      //converted to a value of that type.
      ( //i64 
      dwResult == 0 && strAttributeValue != "0" && strAttributeValue != "0x0" ) 
      || 
      //In all out-of-range cases, errno is set to ERANGE
      errno == ERANGE
      || 
      //i64 > ULONG_MAX 
      dwResult == ULONG_MAX 
      //|| 
      //i64 < 0 
      )
       //Overflow condition occurred.
    {
      byReturn = XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT ;
      std::string str = "Error converting \"" + strAttributeValue + 
        "\" (should be specified as DECIMAL number ) to a number" ;
      mp_userinterface->Confirm( str ) ;
    }
    else
    {
      //dwRes = i64 ;
      byReturn = SUCCESS;
      *((DWORD*) pv_AttributeValue ) = //(DWORD) i64; //(void*) () ;
        dwResult ;
    }
    return byReturn ; 
}

