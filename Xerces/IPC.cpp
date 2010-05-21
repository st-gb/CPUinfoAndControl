/*
 * IPCdataHandler.cpp
 *
 *  Created on: May 18, 2010
 *      Author: Stefan
 */

#include "IPC.hpp"
#include <Controller/IPC/I_IPC.hpp>
#include <Controller/stdstring_format.hpp> //to_stdstring()
#include "XercesHelper.hpp" //for "X" macro

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

namespace Xerces
{
IPCdataHandler::IPCdataHandler(Model & r_model )
  : mr_model (r_model )
{

}

DWORD IPCdataHandler::GetResponse(BYTE byCommand, BYTE * & r_arbyResponseData )
{
  DWORD wByteSizeOfData = 0 ;
  switch ( byCommand )
  {
  case get_current_CPU_data:
    GetCurrentCPUcoreAttributeValues() ;
    break ;
  }
  return wByteSizeOfData ;
}

void IPCdataHandler::GetCurrentCPUcoreAttributeValues()
{
  //from Xerces' CreateDOMDocument sample:
  // Initialize the XML4C2 system.
  try
  {
      XMLPlatformUtils::Initialize();
  }
  catch(const XMLException & toCatch)
  {
      char *pMsg = XMLString::transcode(toCatch.getMessage());
      XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
           << "  Exception message:"
           << pMsg;
      XMLString::release(&pMsg);
//      return 1;
  }
 {
   //  Nest entire test in an inner block.
   //  The tree we create below is the same that the XercesDOMParser would
   //  have created, except that no whitespace text nodes would be created.

   // <company>
   //     <product>Xerces-C</product>
   //     <category idea='great'>XML Parsing Tools</category>
   //     <developedBy>Apache Software Foundation</developedBy>
   // </company>

   DOMImplementation * p_dom_implementation =
       DOMImplementationRegistry::getDOMImplementation( X("Core") );

   if ( p_dom_implementation )
   {
     try
     {
       std::string stdstr ;
       DOMDocument * p_dom_document = p_dom_implementation->createDocument(
                   0,                    // root element namespace URI.
                   X("CPU_core_data"),         // root element name
                   0);                   // document type object (DTD).

       DOMElement * p_dom_elementRoot = p_dom_document->getDocumentElement();


       mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead.Lock() ;

       //Wait for a possible change of the current CPU data to finish (these
       //data may be modified if the DVFS thread is running)
       //"Wait() atomically unlocks the mutex
       // which allows the thread to continue and starts waiting"
       // -> the DVFS thread gets the lock / continues processing after
       // mutex.lock() / calls Broadcast()

       //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
       //"This method atomically releases the lock on the mutex associated with
       //this condition (this is why it must be locked prior to calling Wait)
       //and puts the thread to sleep until Signal  or Broadcast  is called.
       //It then locks the mutex again and returns."
       mr_model.m_cpucoredata.m_conditionCPUdataCanBeSafelyRead.Wait() ;
       //
//       m_conditionAccess.Wait();
//       mr_model.m_cpucoredata.m_wNumThreadsAccessingThisObject
//       mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead.Lock() ;

       WORD wNumCPUcores = mr_model.m_cpucoredata.m_byNumberOfCPUCores ;
       for( WORD wCPUcoreIndex = 0 ; wCPUcoreIndex < wNumCPUcores ;
         ++ wCPUcoreIndex )
       {
         DOMElement * p_dom_elementCore = p_dom_document->createElement(
           X("core") );
         p_dom_elementRoot->appendChild(p_dom_elementCore);

         stdstr = to_stdstring<WORD>( wCPUcoreIndex ) ;
         p_dom_elementCore->setAttribute(X("number"), X(stdstr.c_str() ));

         stdstr = to_stdstring<float>(
           mr_model.m_cpucoredata.m_arfCPUcoreLoadInPercent[wCPUcoreIndex] ) ;
         p_dom_elementCore->setAttribute(X("load"), X(stdstr.c_str() ));
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
       mr_model.m_cpucoredata.ThreadFinishedAccess() ;
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
       mem_buf_format_target.getLen() ;
       mem_buf_format_target.getRawBuffer() ;

       theOutputDesc->release();
       theSerializer->release();

       p_dom_document->release();
     }
     catch (const OutOfMemoryException&)
     {
//         XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" <<
//             XERCES_STD_QUALIFIER endl;
//         errorCode = 5;
     }
     catch (const DOMException& e)
     {
//         XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code
//             << XERCES_STD_QUALIFIER endl;
//         errorCode = 2;
     }
     catch (...)
     {
//         XERCES_STD_QUALIFIER cerr << "An error occurred creating the document"
//             << XERCES_STD_QUALIFIER endl;
//         errorCode = 3;
     }
   }  // (inpl != NULL)
   else
   {
//       XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported"
//           << XERCES_STD_QUALIFIER endl;
//       errorCode = 4;
   }
 }
 XMLPlatformUtils::Terminate();
}
}

