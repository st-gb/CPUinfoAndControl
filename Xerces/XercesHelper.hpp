/* 
 * File:   XercesHelper.hpp
 * Author: Stefan
 *
 * Created on 24. August 2009, 22:11
 */
#ifndef _XERCESHELPER_HPP
#define	_XERCESHELPER_HPP

#include <preprocessor_macros/Windows_compatible_typedefs.h> //__int64
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <Xerces/XercesString.hpp> //Xerces::ansi_or_wchar_string_compare(...)
//XercesAttributesHelper::GetAttributeValue(...)
#include <Xerces/XercesAttributesHelper.hpp>

//from C standard library
#include <errno.h> //for "ERANGE"
#include <limits.h> //ULONG_MAX
#include <stdlib.h> //atoi()
#include <string> //class std::string

#include <windef.h> //for CONST etc. for winnt.h
#include <winnt.h> //for __int64
//#include <windows.h> //for __int64

////from Apache Xerces header directory
//#include <xercesc/sax2/Attributes.hpp> //for "XERCES_CPP_NAMESPACE::Attributes"
//for XERCES_CPP_NAMESPACE::XMLString::transcode(...)
#include <xercesc/util/XMLString.hpp> //class XERCES_CPP_NAMESPACE::XMLString
#include <xercesc/util/Xerces_autoconf_config.hpp> //for XMLCh
#include <xercesc/util/XercesVersion.hpp> //XERCES_CPP_NAMESPACE

#define XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT 2

extern Logger g_logger ;

//Needed for avoiding the exact namespace.
//XERCES_CPP_NAMESPACE_USE

//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
//#define XERCES_STATIC_LIBRARY

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
//class Attributes ;
class UserInterface ;
//Need to properly scope any forward declarations.
XERCES_CPP_NAMESPACE_BEGIN
  class Attributes;
XERCES_CPP_NAMESPACE_END

namespace x86InfoAndControl
{
  bool InitializeXerces() ;
  inline void TerminateXerces()
  {
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    //When Terminate() was called in another block (even if in a function that
    //is called in the same block) than program crash.
    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
    LOGN("Xerces access terminated")
  }
}

class StdStringToDestFormat
{
public:
  static BYTE Convert(
    std::string & strAttributeValue ,
    void * pv_AttributeValue 
  ) ;
};

class StdStringToDWORD
  : public StdStringToDestFormat
{
public:
  static BYTE Convert(
    std::string & strAttributeValue ,
    void * pv_AttributeValue 
    )
  {
    BYTE byReturn = 0 ;
    //DWORD dwRes ;
    //dwRes = atol(strAttributeValue.c_str() ;
    //Use atoi64 because atol return a long value 2^31 to -2^31 but a 
    //DWORD can go to 2^32
//#ifdef WIN32 //if Windows (MinGW etc.)
    __int64 i64 =
//#else
//    int64 i64 =
//#endif
      #ifdef _MSC_VER //if MS-compiler
      _atoi64
      #else
      atoi 
//      strtoul
      #endif
      (strAttributeValue.c_str() ) ;
    if ( //The return value is 0 for _atoi64 if the input cannot be 
      //converted to a value of that type.
      ( i64 == 0 && strAttributeValue != "0" ) 
      || 
      errno == ERANGE
      || 
      i64 >
      //Avoid g++ warning "comparison between signed and unsigned integer
      // expressions"
      (__int64)
      ULONG_MAX
      || 
      i64 < 0 
      )
       //Overflow condition occurred.
    {
      byReturn = XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT ;
    }
    else
    {
      //dwRes = i64 ;
      byReturn = 1 ;
      *((DWORD*) pv_AttributeValue ) = (DWORD) i64; //(void*) () ;
    }
    return byReturn ;
    //memset() ;
  }
};

class XercesHelper
{
  UserInterface * mp_userinterface ;
public:
  XercesHelper();
  XercesHelper(UserInterface & r_userinterface );
  XercesHelper(const XercesHelper & orig);
  virtual ~XercesHelper();
//private:
  static std::string ToStdString(
    const XMLCh * p_xmlch
    ) ;
};

//@return true = "log_file_filter" element occured
inline bool PossiblyHandleLoggingExclusionFilter_Inline(
  const XMLCh * const cpc_xmchLocalName ,
  const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes)
{
  bool bRet = false ;

  if( //If the strings equal.
      ! Xerces::ansi_or_wchar_string_compare( cpc_xmchLocalName ,
      ANSI_OR_WCHAR("log_file_filter") )
    )
  {
    std::string stdstrValue ;
    bRet = true ;
    if( XercesAttributesHelper::GetAttributeValue(
      cr_xercesc_attributes ,
      //Use "( char * )" to avoid g++ Linux compiler warning
      // "deprecated conversion from string constant to ‘char*’ "
      ( char * ) "exclude" ,
      stdstrValue )
      )
    {
      LOGN("string to exclude from logging:" << stdstrValue )
//        g_logger.m_stdsetstdstrExcludeFromLogging.insert( strValue) ;
      g_logger.AddExcludeFromLogging(stdstrValue) ;
    }
  }
  return bRet ;
}

#endif	/* _XERCESHELPER_HPP */
