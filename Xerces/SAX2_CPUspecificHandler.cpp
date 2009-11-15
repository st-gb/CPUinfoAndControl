/* 
 * File:   SAX2_CPUspecificHandler.cpp
 * Author: Stefan
 * 
 * Created on 24. August 2009, 20:20
 */

#include "SAX2_CPUspecificHandler.hpp"
#include "../stdafx.h"
#include "../global.h" //for if "COMPILE_WITH_XERCES" is defined or not
#include "XercesHelper.hpp"
#include "ModelData/RegisterData.hpp"
#include "ModelData/ValueTables.hpp"

//#include "PStates.h"
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/xmlstring.hpp> //for XMLString::transcode(...)
#include <ostream> //for "endl"

//#include "global.h" //for DEBUG(...) etc.

//#include <string>
//#include <sstream> //for istringstream
//#include <iostream>
//#include "../global.h"
//#include <exception> //for class std::exception
#ifndef WIN32
        #include <stdexcept> //for class "runtime_error"
#endif //#ifndef WIN32
//using namespace std;

//#define MB_CUR_MAX 1


//With Xerces < Version 2.8:
//  Add "XML_LIBRARY" to "Preprocessor Definitions" to compile with Xerces statically (else many "LNK2001" and "LNK2019" and linker errors).
//with Xerces 3.0: "XERCES_STATIC_LIBRARY"
//And: Add "Advapi32.lib" as additional dependancy for the linker.
//zu Linker->Bibliothek ignorieren hinzufgen:"LIBCMT.lib", sonst: "LIBCMT.lib(_ctype.obj) : error LNK2005: _isspace ist bereits in MSVCRT.lib(MSVCR80.dll) definiert."

XERCES_CPP_NAMESPACE_USE //to NOT need to prefix the xerces classes with the "xerces::"

// need to properly scope any forward declarations
XERCES_CPP_NAMESPACE_BEGIN
  class Attributes;
XERCES_CPP_NAMESPACE_END


SAX2_CPUspecificHandler::SAX2_CPUspecificHandler(
    UserInterface & r_userinterface,
    Model & r_modeldata
  )
  : m_bInsideValidMSRelement(false)
  , m_xerceshelper(r_userinterface)
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
    const   XMLCh * const    cpc_xmlchURI ,
    const   XMLCh * const    cpc_xmlchLocalName ,
    const   XMLCh * const    cpc_xmlchQualifiedName ,
    const   Attributes &    cr_xercesc_attributes
  )
{
  char * pchXMLelementName = XMLString::transcode(cpc_xmlchLocalName);
  m_strElementName = std::string(pchXMLelementName) ;
  if( m_strElementName == "MSR" )
      m_bInsideValidMSRelement = false ;
  //Release memory AFTER comparing.
  XMLString::release(&pchXMLelementName);
}

void SAX2_CPUspecificHandler::startElement
  (
  const   XMLCh * const    cpc_xmlchURI,
  const   XMLCh * const    cpc_xmlchLocalName,
  const   XMLCh * const    cpc_xmlchQualifiedName,
  const   //xercesc_2_8::
    Attributes & r_xercesc_attributes
  )
{
  char * pchXMLelementName = XMLString::transcode(cpc_xmlchLocalName);
  std::string strValue ;
  LOG( "XML element: " << pchXMLelementName << std::endl );
  m_strElementName = std::string(pchXMLelementName) ;
  //LOG( "uri:" << uri << " localname:" << localname << " qname:" << qname << endl );
  //if( strcmp(pchXMLelementName, "CPU") == 0 )
  if( m_bInsideValidMSRelement )
  {
    bool bCalculateDiff ;
    BYTE byStartBit ;
    BYTE byBitLength ;
    std::string strMSRdataName = m_strElementName ;
    std::string strXMLattributeName ;
    if( XercesHelper::GetAttributeValue
        (
        r_xercesc_attributes,//"processor_name"
        "name" ,
        //strValue
        strXMLattributeName
        )
      )
    {
      strMSRdataName = strXMLattributeName ;
    }
    if( XercesHelper::GetAttributeValue
        (
        r_xercesc_attributes,//"processor_name"
        "startbit" ,
        //strValue
        byStartBit
        )
      )
    {
      if( XercesHelper::GetAttributeValue
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
      if( XercesHelper::GetAttributeValue
          (
          r_xercesc_attributes,//"processor_name"
          "bitlength" ,
          //strValue
          byBitLength
          )
        )
      {
        RegisterData registerdata(//m_strElementName
          strMSRdataName ) ;
        registerdata.m_bCalculateDiff = bCalculateDiff ;
        registerdata.add(byStartBit,byBitLength) ;
        m_stdvec_msrdata_riter->m_stdvec_registerdata.push_back(registerdata) ;
      }
    }
    //else
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
        mp_modeldata->m_valuetables.AddValueTableRow(
          m_stdvec_msrdata_riter,
          mapstr2str ,
          arstdstrResultingNameAndValue
          ) ;
    }
  }
//  if( m_strElementName == "table_entry" )
//  {
//
//  }
  if( m_strElementName == "MSR" )
  {
    DWORD dwIndex ;
    char archAttributeName [] = "index" ;
//    std::string stdstrAttributeName = "MSR" ;
    if( //XercesHelper::GetAttributeValue
      m_xerceshelper.GetAttributeValue
        (
        r_xercesc_attributes,//"processor_name"
        archAttributeName ,
        //* StdStringToDWORD::Convert ,
        //m_xerceshelper.ToDWORD
        //strValue
        //dwIndex
        dwIndex
        )
      == SUCCESS
      )
    {
      m_bInsideValidMSRelement = true ;
      mp_modeldata->m_stdvector_msrdata.push_back(MSRdata(dwIndex)) ;
      m_stdvec_msrdata_riter = mp_modeldata->m_stdvector_msrdata.rbegin() ;
    }
  }
  //Release memory AFTER comparing.
  XMLString::release(&pchXMLelementName);
}

	void SAX2_CPUspecificHandler::fatalError(const SAXParseException& exception)
	{
	    char* message = XMLString::transcode(exception.getMessage());
      //DEBUG_COUT( << "SAX2 handler: Fatal Error: " << message
	     //    << " at line: " << exception.getLineNumber()
	     //    << endl ) ;
      LOG( "SAX2 handler: Fatal Error: " << message
         << " at line: " << exception.getLineNumber()
         << "\n" ) ;
	}
