/*
 * IPCdataHandler.cpp
 *
 *  Created on: May 18, 2010
 *      Author: Stefan
 */

#include "IPCdataGenerator.hpp"
#include <Controller/IPC/I_IPC.hpp>
#include <Controller/stdstring_format.hpp> //to_stdstring()
#include <Xerces/XercesHelper.hpp> //for "X" macro

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <global.h> //LOGN

namespace Xerces
{
IPCdataHandler::IPCdataHandler(Model & r_model )
  :
    m_arbyData ( NULL ) ,
    mr_model (r_model )
{
  //Initialize here. If Xerces initialized in "GetCurrentCPUcoreAttributeValues"
  //: program crash if more than 1 thread entered
  //"GetCurrentCPUcoreAttributeValues" and so more than 1 time Initalize() was
  // called.
  m_bXercesSuccessfullyInitialized = x86InfoAndControl::InitializeXerces() ;
}

IPCdataHandler::~IPCdataHandler()
{
  if( m_arbyData )
    delete [] m_arbyData ;
  LOGN("BEFORE terminating Xerces") ;
  XMLPlatformUtils::Terminate();
  LOGN("after terminating Xerces") ;
}

DWORD IPCdataHandler::GetResponse(BYTE byCommand, BYTE * & r_arbyResponseData )
{
  DWORD wByteSizeOfData = 0 ;
  switch ( byCommand )
  {
  case get_current_CPU_data:
    GetCurrentCPUcoreAttributeValues(wByteSizeOfData) ;
    break ;
  }
  return wByteSizeOfData ;
}

BYTE * IPCdataHandler::GetCurrentCPUcoreAttributeValues(DWORD & r_dwByteSize)
{
  BYTE * arby = NULL ;
  if( m_arbyData )
    delete [] m_arbyData ;
  //from Xerces' CreateDOMDocument sample:
  // Initialize the XML4C2 system.
//  x86InfoAndControl::InitializeXerces()
 {
   //  Nest entire test in an inner block.
   //  The tree we create below is the same that the XercesDOMParser would
   //  have created, except that no whitespace text nodes would be created.

   // <company>
   //     <product>Xerces-C</product>
   //     <category idea='great'>XML Parsing Tools</category>
   //     <developedBy>Apache Software Foundation</developedBy>
   // </company>

   //TODO the attribute values only need to be modified, creating the
    // DOM tree every time is not necessary
   DOMImplementation * p_dom_implementation =
       DOMImplementationRegistry::getDOMImplementation( X("Core") );
   LOGN("After getting DOM implementation") ;
   if ( p_dom_implementation )
   {
     CPUcoreData & r_cpucoredata = mr_model.m_cpucoredata ;
     try
     {
       std::string stdstr ;
       DOMDocument * p_dom_document = p_dom_implementation->createDocument(
         0,  // root element namespace URI.
//                   X("CPU_core_data"),         // root element name
         L"CPU_core_data" ,
         0); // document type object (DTD).
       LOGN("After creating XML document") ;
       DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();

//       //LOGN_TYPE
//       LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues before lock"
//         //, Logger::sync
//         )
//       //This method atomically releases the lock on the mutex associated with
//       //this condition (this is why it must be locked prior to calling Wait)
//       mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead.Lock() ;
//       //LOGN_TYPE
//       LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues after Lock()"
//         //,Logger::sync
//         )

       //Wait for a possible change of the current CPU data to finish (these
       //data may be modified if the DVFS thread is running)
       //"Wait() atomically unlocks the mutex
       // which allows the thread to continue and starts waiting"
       // -> the DVFS thread gets the lock / continues processing after
       // mutex.lock() / calls Broadcast()

       //LOGN_TYPE
       LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues before Wait()"
         //,Logger::sync
         )
       //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
       //"This method atomically releases the lock on the mutex associated with
       //this condition (this is why it must be locked prior to calling Wait)
       //and puts the thread to sleep until Signal  or Broadcast  is called.
       //It then locks the mutex again and returns."
//       mr_model.m_cpucoredata.m_conditionCPUdataCanBeSafelyRead.Wait() ;
       r_cpucoredata.m_win32eventCPUdataCanBeSafelyRead.Wait() ;
//       DEBUGN("mutex is locked?"
//         << mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead. )
//       //http://docs.wxwidgets.org/stable/wx_wxcondition.html:
//       //"It then locks the mutex again and returns."
//       //So unlock it for other clients.
//       mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead.Unlock() ;
       //LOGN_TYPE
       LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues after Wait()"
         //,Logger::sync
         )
       //
//       m_conditionAccess.Wait();
//       mr_model.m_cpucoredata.m_wNumThreadsAccessingThisObject
//       mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead.Lock() ;

       WORD wNumCPUcores = mr_model.m_cpucoredata.m_byNumberOfCPUCores ;
       PerCPUcoreAttributes * arp_percpucoreattributes =
           mr_model.m_cpucoredata.m_arp_percpucoreattributes ;
       DOMElement * p_dom_elementCore ;
       for( WORD wCPUcoreIndex = 0 ; wCPUcoreIndex < wNumCPUcores ;
         ++ wCPUcoreIndex )
       {
         p_dom_elementCore = p_dom_document->createElement(
           L"core" );
         p_dom_elementRoot->appendChild(p_dom_elementCore);

         stdstr = to_stdstring<WORD>( wCPUcoreIndex ) ;
         p_dom_elementCore->setAttribute( L"number" , X(stdstr.c_str() ));

         stdstr = to_stdstring<float>(
           mr_model.m_cpucoredata.m_arfCPUcoreLoadInPercent[wCPUcoreIndex] ) ;
         p_dom_elementCore->setAttribute( L"load" , X(stdstr.c_str() ));

         stdstr = to_stdstring<float>(
             arp_percpucoreattributes[wCPUcoreIndex].m_fVoltageInVolt ) ;
         p_dom_elementCore->setAttribute( L"voltage_in_Volt" ,
           X(stdstr.c_str() ));

         stdstr = to_stdstring<float>(
             arp_percpucoreattributes[wCPUcoreIndex].m_fMultiplier ) ;
         p_dom_elementCore->setAttribute( L"multiplier" , X(stdstr.c_str() ));

         stdstr = to_stdstring<float>(
             arp_percpucoreattributes[wCPUcoreIndex].m_fReferenceClockInMhz ) ;
         p_dom_elementCore->setAttribute( L"reference_clock_in_MHz" ,
           X(stdstr.c_str() ));

         stdstr = to_stdstring<float>(
             arp_percpucoreattributes[wCPUcoreIndex].m_fTempInDegCelsius) ;
         p_dom_elementCore->setAttribute( L"temp_in_deg_Celsius" ,
           X(stdstr.c_str() ));

         p_dom_elementRoot->appendChild(p_dom_elementCore);

//         DOMText * prodDataVal = doc->createTextNode(X("Xerces-C"));
//         p_dom_elementCore->appendChild(prodDataVal);
//
//         DOMElement*  catElem = p_dom_document->createElement(X("category"));
//         p_dom_elementRoot->appendChild(catElem);
//
//         DOMText*    catDataVal = p_dom_document->createTextNode(
//           X("XML Parsing Tools"));
//         catElem->appendChild(catDataVal);
//
//         DOMElement*  devByElem = p_dom_document->createElement(
//           X("developedBy"));
//         p_dom_elementRoot->appendChild(devByElem);
//
//         DOMText*    devByDataVal = p_dom_document->createTextNode(
//           X("Apache Software Foundation"));
//         devByElem->appendChild(devByDataVal);
       }
       if( mr_model.m_cpucoredata.m_bTooHot )
       {
         p_dom_elementCore = p_dom_document->createElement(
           L"core" );
         p_dom_elementRoot->appendChild( p_dom_elementCore ) ;
       }
       //LOGN_TYPE
       LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues before ThreadFinishedAccess()"
         //,Logger::sync
         )
       mr_model.m_cpucoredata.ThreadFinishedAccess() ;
//       LOGN_TYPE
       LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues after ThreadFinishedAccess()"
         //,Logger::sync
         )
//       mr_model.DecrementNumberOfThreadsAccess
//       mr_m
       //
       // Now count the number of elements in the above DOM tree.
       //

//       const XMLSize_t elementCount = p_dom_document->getElementsByTagName(X("*"))->getLength();
//       XERCES_STD_QUALIFIER cout << "The tree just created contains: " << elementCount
//            << " elements." << XERCES_STD_QUALIFIER endl;

       //from Xerces' DOMPrint sample:
       DOMLSOutput   * theOutputDesc = ((DOMImplementationLS*)
           p_dom_implementation )->createLSOutput();
       DOMLSSerializer * theSerializer = ((DOMImplementationLS*)
          p_dom_implementation)->createLSSerializer();

       MemBufFormatTarget mem_buf_format_target ;
       theOutputDesc->setByteStream(& mem_buf_format_target);
       theSerializer->write(p_dom_document, theOutputDesc);
//     "The MemBufFormatTarget returns a NULL terminated XMLByte stream upon request,
//       * through the method getRawBuffer(), and user should make its own copy of the
//       * returned buffer if it intends to keep it independent on the state of the
//       * MemBufFormatTarget."!
       XMLSize_t xmlsize_tBufferLength = mem_buf_format_target.getLen() ;
       const XMLByte* cp_xmlbyteBuffer = mem_buf_format_target.getRawBuffer() ;
       //"user should make its own copy of the
       //       * returned buffer if it intends to keep it independent on the state of the
       //       * MemBufFormatTarget."!"
       arby = new BYTE[ xmlsize_tBufferLength ] ;
       if( arby )
       {
         memcpy( arby, cp_xmlbyteBuffer, xmlsize_tBufferLength ) ;
         r_dwByteSize = xmlsize_tBufferLength ;
         m_dwByteSize = xmlsize_tBufferLength ;
       }
#ifdef TEST_VERSION
       std::ofstream stdofstreamData ;
       char * pchFileName = "XML_IPC_data.txt" ;
       stdofstreamData.open(  , std::_S_in ) ;
       if( stdofstreamData.is_open() )
       {
         stdofstreamData.write( cp_xmlbyteBuffer , xmlsize_tBufferLength ) ;
         stdofstreamData.close() ;
       }
       else
         LOGN( pchFileName << "could not be opened")
#endif
       std::string stdstrAllXMLdata( //const char * s
         (char*) cp_xmlbyteBuffer , //size_t n
         xmlsize_tBufferLength );
//       LOGN(
       DEBUGN(
         "XML data: " << //stdstrAllXMLdata
         //should be NULL-terminated
         cp_xmlbyteBuffer )
       theOutputDesc->release();
       theSerializer->release();
       p_dom_document->release();
     }
     catch (const OutOfMemoryException&)
     {
       LOGN( "GetCurrentCPUcoreAttributeValues--Xerces: OutOfMemoryException" )
//         errorCode = 5;
     }
     catch (const DOMException& e)
     {
       LOGN("GetCurrentCPUcoreAttributeValues--Xerces: DOMException code is:  " << e.code )
//         errorCode = 2;
     }
     catch (...)
     {
       LOGN( "GetCurrentCPUcoreAttributeValues--Xerces: An error occurred creating the document")
//         errorCode = 3;
     }
   }//if ( p_dom_implementation )
   else
   {
     LOGN( "GetCurrentCPUcoreAttributeValues--Xerces: Requested implementation is not supported" )
//       errorCode = 4;
   }
  }
//  LOGN("BEFORE terminating Xerces") ;
//  XMLPlatformUtils::Terminate();
//  LOGN("after terminating Xerces") ;
  m_arbyData = arby ;
  return arby ;
} //GetCurrentCPUcoreAttributeValues()
}//namespace Xerces

