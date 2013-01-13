/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   SAX2_CPUspecificHandler.cpp
 * Author: Stefan
 * 
 * Created on 24. August 2009, 20:20
 */

#ifdef _MSC_VER //if MicroSoft Compiler
  #include "../stdafx.h"
#endif //#ifdef _MSC_VER

#include <ModelData/ModelData.hpp> //class Model
#include <ModelData/RegisterData.hpp> //class RegisterData
#include <ModelData/ValueTables.hpp> //ARRAY_INDEX_OF_ATTR_VALUE

#include <preprocessor_macros/logging_preprocessor_macros.h> //LOG(..)
//#include <Controller/Logger/ILogFormatter.hpp> //class I_LogFormatter
#include <Controller/Logger/LogLevel.hpp> //enum namespace LogLevel::MessageType

#include <UserInterface/UserInterface.hpp> //class UserInterface

#include <Xerces/XercesAttributesHelper.hpp> //class XercesAttributesHelper
//class XercesHelper::ToStdString(const XMLCh *)
#include <Xerces/XercesHelper.hpp>

#include <xercesc/sax2/Attributes.hpp>//Class "XERCES_CPP_NAMESPACE::Attributes"
//for XERCES_CPP_NAMESPACE::XMLString::transcode(...)
#include <xercesc/util/XMLString.hpp>

#include <map> //class std::map
#include <string> //class std::string

//With Xerces < Version 2.8:
//  Add "XML_LIBRARY" to "Preprocessor Definitions" to compile with Xerces
// statically (else many "LNK2001" and "LNK2019" and linker errors).
//with Xerces 3.0: "XERCES_STATIC_LIBRARY"
//And: Add "Advapi32.lib" as additional dependancy for the linker.
//zu Linker->Bibliothek ignorieren hinzufgen:"LIBCMT.lib", sonst:
//"LIBCMT.lib(_ctype.obj) : error LNK2005: _isspace ist bereits in
//MSVCRT.lib(MSVCR80.dll) definiert."

//XERCES_CPP_NAMESPACE_USE //to NOT need to prefix the xerces classes with the
//"xerces::"

#define SUCCESS 1
#define FAILURE 0

#include "SAX2_CPUspecificHandler.hpp"

SAX2_CPUspecificHandler::SAX2_CPUspecificHandler(
  UserInterface & r_userinterface,
  Model & r_modeldata
  )
  : 
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  m_bInsideValidCPUIDelement(false)
  , m_bInsideValidMSRelement(false)
//  , m_xerceshelper(r_userinterface)
  //, m_stdvec_msrdata_riter 
{
  mp_modeldata = & r_modeldata ;
  mp_userinterface = & r_userinterface ;
  m_stdvec_msrdata_riter = mp_modeldata->m_stdvector_msrdata.rend() ;
}

SAX2_CPUspecificHandler::SAX2_CPUspecificHandler(
    const SAX2_CPUspecificHandler& orig)
{
}

SAX2_CPUspecificHandler::~SAX2_CPUspecificHandler() {
}

void SAX2_CPUspecificHandler::endElement(
  const XMLCh * const cpc_xmlchURI ,
  const XMLCh * const cpc_xmlchLocalName ,
  const XMLCh * const cpc_xmlchQualifiedName //,
  //const   Attributes &    cr_xercesc_attributes
  )
{
  char * pchXMLelementName = XERCES_CPP_NAMESPACE::XMLString::transcode(
    cpc_xmlchLocalName);
  m_strElementName = std::string(pchXMLelementName) ;
  if( m_strElementName == "MSR" )
      m_bInsideValidMSRelement = false ;
  if( m_strElementName == "CPUID" )
    m_bInsideValidCPUIDelement = false ;
  //Release memory AFTER comparing.
  XERCES_CPP_NAMESPACE::XMLString::release( & pchXMLelementName);
}

//An MSR data is 64 bits wide. And inside this storage often single different
//data is stored. E.g. the INTel PERF_CTL MSR has FreqID data bits 0 to 8
//and VoltageID data from bits 8 to 16.
void SAX2_CPUspecificHandler::HandleInsideMSRelement(
  const XERCES_CPP_NAMESPACE::Attributes & r_xercesc_attributes
  )
{
  bool bCalculateDiff ;
  BYTE byStartBit ;
  BYTE byBitLength ;
  std::string strMSRdataName = m_strElementName ;
  std::string std_strNameAttributeValue ;
  std::string std_strXMLattributeName = "name";
  if( XercesAttributesHelper::GetAttributeValue
      (
      r_xercesc_attributes,
//      //By casting explicitely: avoid Linux g++ warning
//      // "deprecated conversion from string constant to ‘char*’"
//      (char *)
      std_strXMLattributeName.c_str(),
      //strValue
      std_strNameAttributeValue
      )
    )
  {
    strMSRdataName = std_strXMLattributeName ;
  }
  std_strXMLattributeName = "startbit";
  if( XercesAttributesHelper::GetAttributeValue
      (
      r_xercesc_attributes,//"processor_name"
//      //By casting explicitely: avoid Linux g++ warning
//      // "deprecated conversion from string constant to ‘char*’"
//      (char *)
      std_strXMLattributeName.c_str() ,
      //strValue
      byStartBit
      )
    )
  {
    if( XercesAttributesHelper::GetAttributeValue
        (
        r_xercesc_attributes,//"processor_name"
        "diff" ,
        //strValue
        bCalculateDiff
        )
      )
    {
      bCalculateDiff = true ;
    }
    else
      bCalculateDiff = false ;
    std_strXMLattributeName = "bitlength";
    if( XercesAttributesHelper::GetAttributeValue
        (
        r_xercesc_attributes,
//        //Avoid Linux g++ "warning deprecated conversion from string constant
//        //to ‘char*’"
//        (char *)  ,
        std_strXMLattributeName.c_str(),
        //strValue
        byBitLength
        )
      )
    {
      LOGN(FULL_FUNC_NAME << "--adding register data element")
      RegisterData registerdata(//m_strElementName
        strMSRdataName ) ;
      registerdata.m_bCalculateDiff = bCalculateDiff ;
      registerdata.add(byStartBit,byBitLength) ;
      m_stdvec_msrdata_riter->m_stdvec_registerdata.push_back(registerdata) ;
    }
    else
    {
      std::wstring std_wstrMessage = L"no (mandatory) \""
        + std_strXMLattributeName
        + L"\" attribute name";
      LOGN_TYPE( GetStdString(std_wstrMessage),
        LogLevel::warning)
      mp_userinterface->MessageWithTimeStamp(std_wstrMessage);
    }
  }
  else
  {
    std::wstring std_wstrMessage = L"no (mandatory) \""
      + std_strXMLattributeName
      + L"\" attribute name";
    LOGN_TYPE( GetStdString(std_wstrMessage),
      LogLevel::warning)
    std::wstring std_wstrDocumentIDandLocation;
    GetDocumentIDandLocation(std_wstrDocumentIDandLocation);
    mp_userinterface->MessageWithTimeStamp(std_wstrMessage +
      std_wstrDocumentIDandLocation);
  }
  if( m_strElementName == "value_table_row")
    //A row follows. ex:
    //<value_table_row MHz="1100" FID="14" DID="2"></value_table_row>
    //1st column: the value that resukts from the other values (FID and DID
    //in this case)
  {
    //std::vector<std::string [2]> stdvecstdstrAttributeNameAndValue ;
    std::string arstdstrResultingNameAndValue [2] ;
    std::map<std::string , std::string> mapstr2str ;
//      std::string arstdstr[2] = { m_strElementName ,
//        r_xercesc_attributes.
//        }
//      stdvecstdstrAttributeNameAndValue.push_back(  )
//      ValueTableSingleRow valuetablesinglerow ;
    BYTE byNumXMLattributes = r_xercesc_attributes.getLength() ;
    if( byNumXMLattributes > 0 )
    {
      XMLSize_t index = 0 ;
      arstdstrResultingNameAndValue[ARRAY_INDEX_OF_ATTR_NAME] = 
        XercesHelper::ToStdString(r_xercesc_attributes.getLocalName( //0
	  index ) ) ;
      arstdstrResultingNameAndValue[ARRAY_INDEX_OF_ATTR_VALUE] = 
        XercesHelper::ToStdString(r_xercesc_attributes.getValue(
	  //(unsigned int) 0
	  index ) ) ;
    }
    for ( BYTE byIndex = 1 ; byIndex < r_xercesc_attributes.getLength() ;
      ++ byIndex )
//        valuetablesinglerow.add(
//          r_xercesc_attributes.getLocalName( byIndex ) ,
//            r_xercesc_attributes.getValue(byIndex)
//            ) ;
    {
      //std::string arstdstr[2] = {
      //  r_xercesc_attributes.getLocalName( byIndex ),
      //  r_xercesc_attributes.getValue(byIndex)
      //  } ;
      //stdvecstdstrAttributeNameAndValue.push_back(arstdstr[2]);
      mapstr2str.insert( std::make_pair
          ( 
        XercesHelper::ToStdString(r_xercesc_attributes.getLocalName( byIndex ) ),
        XercesHelper::ToStdString(r_xercesc_attributes.getValue(byIndex) )
          )
        ) ;
    }
    //Adds the table row to the appropriate table.
//      ValueTables.add(valuetablesinglerow) ;
    //mp_modeldata->AddValueTableRow(
    //  m_stdvec_msrdata_riter ,
    //  //stdvecstdstrAttributeNameAndValue 
    //  mapstr2str,
    //  arstdstrResultingNameAndValue
    //  ) ;

//      mp_modeldata->m_valuetables.AddValueTableRow(
//        m_stdvec_msrdata_riter,
//        mapstr2str ,
//        arstdstrResultingNameAndValue
//        ) ;
  }
}

//An MSR data is 64 bits wide. And inside this storage often single different
//data is stored. E.g. the INTel PERF_CTL MSR has FreqID data bits 0 to 8
//and VoltageID data from bits 8 to 16.
void SAX2_CPUspecificHandler::HandleInsideCPUIDelement(
  const XERCES_CPP_NAMESPACE::Attributes & r_xercesc_attributes
  )
{
//  bool bCalculateDiff ;
  BYTE byStartBit ;
  BYTE byBitLength ;
  std::string strMSRdataName = m_strElementName ;
  std::string strXMLattributeName ;
  std::string stdstrGenPurposeRegName ;
  if( XercesAttributesHelper::GetAttributeValue
      (
      r_xercesc_attributes,
      //Avoid Linux g++ "warning deprecated conversion from string constant
      //to ‘char*’"
      (char *) "name" ,
      //strValue
      strXMLattributeName
      )
    )
  {
    strMSRdataName = strXMLattributeName ;
  }
  //if( m_xerceshelper.GetAttributeValue
  //  (
  //  r_xercesc_attributes,//"processor_name"
  //  //archAttributeName ,
  //  //either "EAX", "EBX", "ECX" or "EDX".
  //  "general_purpose_register_name" ,
  //  //* StdStringToDWORD::Convert ,
  //  //m_xerceshelper.ToDWORD
  //  //strValue
  //  //dwIndex
  //  stdstrGenPurposeRegName
  //  ) 
  //  )
  //{
  //  if( stdstrGenPurposeRegName == "EAX" )

  //}
  //if( m_strElementName == "EAX" )
  //  byStartBit = 0 ;
  //if( 
  if( XercesAttributesHelper::GetAttributeValue
      (
      r_xercesc_attributes,
      //Avoid Linux g++ "warning deprecated conversion from string constant
      //to ‘char*’"
      (char *) "startbit" ,
      //strValue
      byStartBit
      )
    )
  {
    //if( XercesHelper::GetAttributeValue
    //    (
    //    r_xercesc_attributes,//"processor_name"
    //    "diff" ,
    //    //strValue
    //    bCalculateDiff
    //    )
    //  )
    //{
    //  bCalculateDiff = true ;
    //}
    //else
    //  bCalculateDiff = false ;
    if( XercesAttributesHelper::GetAttributeValue
        (
        r_xercesc_attributes,
        //Avoid Linux g++ "warning deprecated conversion from string constant
        //to ‘char*’"
        (char *) "bitlength" ,
        //strValue
        byBitLength
        )
      )
    {
      RegisterData registerdata(//m_strElementName
        strMSRdataName ) ;
      //registerdata.m_bCalculateDiff = bCalculateDiff ;
      registerdata.add(byStartBit,byBitLength) ;
      m_stdvec_cpuiddata_riter->m_stdvec_registerdata.push_back(registerdata) ;
    }
  }
}

void SAX2_CPUspecificHandler::HandleCPUIDelement(
  const XERCES_CPP_NAMESPACE::Attributes & r_xercesc_attributes
  )
{
  DWORD dwIndex ;
  char archAttributeName [] = "index" ;
  char archCoreIDattributeName [] = "core" ;
//    std::string stdstrAttributeName = "MSR" ;
  if( ConvertXercesAttributesValue<DWORD>
      (
      r_xercesc_attributes,
      dwIndex ,
      archAttributeName
      )
    == SUCCESS
    )
  {
    m_bInsideValidCPUIDelement = true ;
    std::string stdstrRegisterName ;
    XercesAttributesHelper::GetAttributeValue
      (
      r_xercesc_attributes,
      //archAttributeName ,
      //Avoid Linux g++ "warning deprecated conversion from string constant
      //to ‘char*’"
      (char *) "name" ,
      //* StdStringToDWORD::Convert ,
      //m_xerceshelper.ToDWORD
      //strValue
      //dwIndex
      stdstrRegisterName
      ) ;
    mp_modeldata->m_stdvector_cpuiddata.push_back( 
      CPUIDdata(dwIndex , stdstrRegisterName //, stdstrGenPurposeRegName 
        )
      ) ;
    m_stdvec_cpuiddata_riter = mp_modeldata->m_stdvector_cpuiddata.rbegin() ;
    if( //XercesHelper::GetAttributeValue
        ConvertXercesAttributesValue<DWORD>
        (
        r_xercesc_attributes ,
        dwIndex ,
        archCoreIDattributeName
        )
      == SUCCESS
      )
    {
  		m_stdvec_cpuiddata_riter->m_byCoreID = dwIndex ;
    }
  }
}

void SAX2_CPUspecificHandler::HandleMSRelement(
  const XERCES_CPP_NAMESPACE::Attributes & r_xercesc_attributes
  )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  DWORD dwIndex ;
  char archAttributeName [] = "index" ;
  char archCoreIDattributeName [] = "core" ;
  std::string std_strAttributeName = "index";
//    std::string stdstrAttributeName = "MSR" ;
  XercesAttributesHelper xercesattributeshelper;
  if( //template method can't get hexadecimal numbers:
//    ConvertXercesAttributesValue<DWORD>
//      (
//      r_xercesc_attributes,//"processor_name"
//      dwIndex ,
//      archAttributeName
//      //* StdStringToDWORD::Convert ,
//      //m_xerceshelper.ToDWORD
//      //strValue
//      )
//    == SUCCESS
    //Can get dec or hex numbers;
    xercesattributeshelper.GetAttributeValue(
      r_xercesc_attributes, //const XERCES_CPP_NAMESPACE::Attributes & attrs,
//      archAttributeName, //const char * lpctstrAttrName,
      std_strAttributeName.c_str(),
      dwIndex //DWORD & r_dwValue
      ) == XercesAttributesHelper::getting_attribute_value_succeeded
    )
  {
    LOGN( FULL_FUNC_NAME << "--got " << archAttributeName
      << " attribute value:" << dwIndex)
    m_bInsideValidMSRelement = true ;
    std::string stdstrRegisterName ;
    std_strAttributeName = "name";
    if( XercesAttributesHelper::GetAttributeValue
        (
        r_xercesc_attributes,
        //archAttributeName ,
//        //Avoid Linux g++ "warning deprecated conversion from string constant
//        //to ‘char*’"
//        (char *) "name" ,
        std_strAttributeName,
        //* StdStringToDWORD::Convert ,
        //m_xerceshelper.ToDWORD
        //strValue
        stdstrRegisterName
        )
      == //SUCCESS
        XercesAttributesHelper::getting_attribute_value_succeeded
      && stdstrRegisterName != ""
      )
    {
      LOGN( FULL_FUNC_NAME << "--got " << std_strAttributeName
        << " attribute value:" << stdstrRegisterName)
      mp_modeldata->m_stdvector_msrdata.push_back( MSRdata(
        dwIndex, stdstrRegisterName ) ) ;
      LOGN( FULL_FUNC_NAME << "--added element for containing MSR data")
    }
    else
      mp_modeldata->m_stdvector_msrdata.push_back(MSRdata(dwIndex)) ;
    m_stdvec_msrdata_riter = mp_modeldata->m_stdvector_msrdata.rbegin() ;
  }
  std_strAttributeName = "core";
  if( ConvertXercesAttributesValue<DWORD>
      (
      r_xercesc_attributes,
      dwIndex ,
      archCoreIDattributeName
      )
      == SUCCESS
    )
  {
    LOGN( FULL_FUNC_NAME << "--got " << std_strAttributeName
      << " attribute value:" << dwIndex)
    m_stdvec_msrdata_riter->m_byCoreID = dwIndex ;
  }
}

//void SAX2_CPUspecificHandler::setDocumentLocator(const Locator* const locator)
//{
//  m_p_locator = locator;
//}

void SAX2_CPUspecificHandler::startElement
  (
  const XMLCh * const cpc_xmlchURI,
  const XMLCh * const cpc_xmlchLocalName,
  const XMLCh * const cpc_xmlchQualifiedName,
  const XERCES_CPP_NAMESPACE::Attributes & r_xercesc_attributes
  )
{
  char * pchXMLelementName = XERCES_CPP_NAMESPACE::XMLString::transcode(
    cpc_xmlchLocalName);
  std::string strValue ;
  // g++ error:
  //"no match for 'operator<<' in
  // '(+(&stringstream)->css::basic_stringstream<_CharT>::operator<<
  // [with _CharT = wchar_t](((const char*)"XML element: ")))->
  // css::basic_stringstream<_CharT>::operator<<
  // [with _ValueT = char*, _CharT = wchar_t](pchXMLelementName)
  // << std::endl' "
//  LOG( "XML element: " << pchXMLelementName << std::endl );
  LOGN( "XML element: " << std::string(pchXMLelementName) );
  m_strElementName = std::string(pchXMLelementName) ;
  //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname << endl );
  //if( strcmp(pchXMLelementName, "CPU") == 0 )
  if( m_bInsideValidMSRelement )
  {
    HandleInsideMSRelement(r_xercesc_attributes) ;
  }
  else
  if( m_bInsideValidCPUIDelement )
  {
    HandleInsideCPUIDelement(r_xercesc_attributes) ;
  }
//  if( m_strElementName == "table_entry" )
//  {
//
//  }
  else if( m_strElementName == "MSR" )
  {
    HandleMSRelement(r_xercesc_attributes) ;
  }
  else if( m_strElementName == "CPUID" )
  {
    HandleCPUIDelement(r_xercesc_attributes) ;
  }
  //Release memory AFTER comparing.
  XERCES_CPP_NAMESPACE::XMLString::release( & pchXMLelementName);
}

//TODO why is this function called and not the exception handler inside
//ReadXMLdocument?
void SAX2_CPUspecificHandler::fatalError(
  const XERCES_CPP_NAMESPACE::SAXParseException & r_xercesc_sax_parse_exception)
{
  LOGN_TYPE( FULL_FUNC_NAME << ": fatal Error: " << XercesHelper::ToStdString(
    r_xercesc_sax_parse_exception.getMessage() )
   << " at line: " << r_xercesc_sax_parse_exception.getLineNumber()
   , LogLevel::error )
}
