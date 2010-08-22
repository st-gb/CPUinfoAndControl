/* 
 * File:   XercesHelper.hpp
 * Author: Stefan
 *
 * Created on 24. August 2009, 22:11
 */
#ifndef _XERCESHELPER_HPP
#define	_XERCESHELPER_HPP

#include <preprocessor_macros/Windows_compatible_typedefs.h> //__int64

//from C standard library
#include <errno.h> //for "ERANGE"
#include <limits.h> //ULONG_MAX
#include <stdlib.h> //atoi()

//from Apache Xerces header directory
#include <xercesc/sax2/Attributes.hpp> //for "xercesc_2_8::Attributes"
#include <xercesc/util/XMLString.hpp>

//// need to properly scope any forward declarations
//XERCES_CPP_NAMESPACE_BEGIN
//  class Attributes;
//XERCES_CPP_NAMESPACE_END

//Needed for avoiding the exact namespace.
//XERCES_CPP_NAMESPACE_USE

//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
//#define XERCES_STATIC_LIBRARY

#define XERCES_ATTRIBUTE_VALUE_INVALID_DATA_FORMAT 2
#define XERCES_ERROR_GETTING_ATTRIBUTE_NAME 3
#define XERCES_ERROR_CONVERTING_ATTRIBUTE_VALUE_TO_C_STRING 4
#define XERCES_ATTRIBUTE_VALUE_DOES_NOT_EXIST 5
#define XERCES_ERROR_CONVERTING_ATTRIBUTE_NAME_TO_XERCES_STRING 6

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
//class Attributes ;
class UserInterface ;

//from Xerces sourcecode (CreateDOMDocument sample, CreateDOMDocument.cpp) :
// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XercesString
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XercesString(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);
    }

    ~XercesString()
    {
      XERCES_CPP_NAMESPACE::XMLString::release(&fUnicodeForm);
    }
    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }
private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};
#define XERCES_STRING_FROM_ANSI_STRING(str) XercesString(str).unicodeForm()

namespace x86InfoAndControl
{
  bool InitializeXerces() ;
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
  :public StdStringToDestFormat
{
public:
  static BYTE Convert(
    std::string & strAttributeValue ,
    void * pv_AttributeValue 
  )
  {
    BYTE byReturn = FAILURE ;
    //DWORD dwRes ;
    //dwRes = atol(strAttributeValue.c_str() ;
    //Use atoi64 because atol return a long value 2^31 to -2^31 but a 
    //DWORD can go to 2^32
    __int64 i64 = 
      #ifdef _MSC_VER //if MS-compiler
      _atoi64
      #else
      atoi 
      #endif
      (strAttributeValue.c_str() ) ;
    if ( //The return value is 0 for _atoi64 if the input cannot be 
      //converted to a value of that type.
      ( i64 == 0 && strAttributeValue != "0" ) 
      || 
      errno == ERANGE
      || 
      i64 > ULONG_MAX 
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
      byReturn = SUCCESS;
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
  XercesHelper(const XercesHelper& orig);
  virtual ~XercesHelper();
//private:

  static BYTE GetAttributeValue(
    const XERCES_CPP_NAMESPACE::Attributes & attrs,
    const char * pc_chAttributeName,
    bool & rbValue
    ) ;

  static inline BYTE //SAX2MainConfigHandler::
      GetAttributeValue
    (
    const XERCES_CPP_NAMESPACE::Attributes & xercesc_attributes,
    const std::string & cr_stdstrAttributeName,
    std::string & r_strValue
    ) ;

  static BYTE GetAttributeValue(
      const XERCES_CPP_NAMESPACE::Attributes & attrs,
      char * lpctstrAttrName,
      BYTE & rbyValue
    ) ;

  BYTE //SAX2MainConfigHandler::
      GetAttributeValue(
    const XERCES_CPP_NAMESPACE::Attributes & attrs,
    const char * lpctstrAttrName,
    //DWORD & r_dwValue
    //StdStringToDestFormat & r_stdstringtodestformat ,
    BYTE ( * pfn)(
      std::string & strAttributeValue ,
      void * pv_AttributeValue ) ,
    //PVOID 
    void * pv_AttributeValue
    ) ;

  //Decrease code redundancy:
  //This function has the code to get the Xerces attribute value
  //that should be identical for all data types and calls the 
  //coverter functions (member functions of this class).
  //For possibility to interact with the user interface in case of convert errors
  //the converter functions are member functions and so they have
  //the userinterface member variable.
  BYTE //SAX2MainConfigHandler::
    GetAttributeValue(
    const XERCES_CPP_NAMESPACE::Attributes & attrs,
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
    ) ;

    static BYTE GetAttributeValue(
    const XERCES_CPP_NAMESPACE::Attributes & attrs,
    const char * lpctstrAttrName,
    WORD & rwValue
    ) ;

    //static 
    BYTE //SAX2MainConfigHandler::
        GetAttributeValue(
      const XERCES_CPP_NAMESPACE::Attributes & attrs,
      const char * lpctstrAttrName,
      DWORD & r_dwValue) ;

    static BYTE GetAttributeValue(
    const XERCES_CPP_NAMESPACE::Attributes & attrs,
    const char * lpctstrAttrName,
    float & rfValue
    ) ;

  static BYTE GetAttributeValue(
    const XERCES_CPP_NAMESPACE::Attributes & attrs,
    char * lpctstrAttrName,
//    std::string & r_stdstrAttributeName ,
    std::string & r_strValue
    ) ;
  static BYTE GetAttributeValue
    (
    const XERCES_CPP_NAMESPACE::Attributes & attrs,
    const char * lpctstrAttrName,
    std::wstring & r_stdwstrValue
    ) ;
  BYTE ToDWORD(
    std::string & strAttributeValue ,
    void * pv_AttributeValue
    ) ;
  static std::string ToStdString(
    const XMLCh * p_xmlch
    ) ;
};

#endif	/* _XERCESHELPER_HPP */
