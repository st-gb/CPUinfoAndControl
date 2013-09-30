/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * IPCdataHandler.cpp
 *
 *  Created on: May 18, 2010
 *      Author: Stefan
 */

#include "IPCdataGenerator.hpp"
#include <Controller/IPC/I_IPC.hpp> //for enum value "get_current_CPU_data"
#include <Controller/character_string/stdstring_format.hpp> //convertToStdString()
//format_output_data(...)
#include <Controller/character_string/format_as_string.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp> //class I_CPUcontroller
#include <Controller/CPUcontrolServiceBase.hpp>
//#include <global.h> //LOGN
#include <ModelData/ModelData.hpp> //class ModelData
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN
#include <Xerces/XercesHelper.hpp> //x86InfoAndControl::InitializeXerces()
#include <Xerces/XercesString.hpp> //for "XERCES_STRING_FROM_ANSI_STRING" macro

#include <wx/stopwatch.h> //::wxGetLocalTimeMillis()

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

namespace Xerces
{
  IPCdataHandler::IPCdataHandler(Model & r_model,
    CPUcontrolServiceBase & cpucontrolservicebase)
    :
//    m_bThreadSafe ( true) ,
    m_dwByteSize(0) ,
    m_arbyData ( NULL ) ,
    m_p_i_cpucontroller(NULL) ,
    mr_model (r_model ),
    m_r_cpucontrolservicebase(cpucontrolservicebase)
  {
    LOGN( "begin")
    //Initialize here. If Xerces initialized in "GetCurrentCPUcoreAttributeValues"
    //: program crash if more than 1 thread entered
    //"GetCurrentCPUcoreAttributeValues" and so more than 1 time Initalize() was
    // called.
    m_bXercesSuccessfullyInitialized = x86InfoAndControl::InitializeXerces() ;
    LOGN( "end")
  }

  IPCdataHandler::~IPCdataHandler()
  {
  //  if( m_arbyData )
  //    delete [] m_arbyData ;
    PossiblyReleaseMemoryThreadSafe() ;
    x86InfoAndControl::TerminateXerces();
  }

  void IPCdataHandler::CreateCPUcoreXMLelements(
    XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document,
    PerCPUcoreAttributes * arp_percpucoreattributes)
  {
    std::string stdstr ;
    XERCES_CPP_NAMESPACE::DOMElement * p_dom_elementCore ;
    XERCES_CPP_NAMESPACE::DOMElement * p_dom_elementRoot =
      p_dom_document->getDocumentElement();
    WORD wNumCPUcores = mr_model.m_cpucoredata.m_byNumberOfCPUCores ;
    for( WORD wCPUcoreIndex = 0 ; wCPUcoreIndex < wNumCPUcores ;
      ++ wCPUcoreIndex )
    {
      //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMDocument.html
      // #f5e93a6b757adb2544b3f6dffb4b461a:
      // throws DOMException
      p_dom_elementCore = p_dom_document->createElement(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"core" );
      //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html
      // #504731160f9bfff5bb9cc64afabf0e2f:
      // Exceptions: DOMException
      p_dom_elementRoot->appendChild(p_dom_elementCore);

      stdstr = convertToStdString<WORD>( wCPUcoreIndex ) ;
      //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMElement.html
      // #1a607d8c619c4aa4a59bc1a7bc5d4692:
      // exception DOMException
      p_dom_elementCore->setAttribute(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"number" ,
        XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));
  //             GetStdWstring( stdstr ).c_str() );

      stdstr = convertToStdString<float>(
        mr_model.m_cpucoredata.m_arfCPUcoreLoadInPercent[wCPUcoreIndex] ) ;
      p_dom_elementCore->setAttribute(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"load" ,
        XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));

      stdstr = convertToStdString<float>(
        arp_percpucoreattributes[wCPUcoreIndex].m_fVoltageInVolt ) ;
      p_dom_elementCore->setAttribute(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"voltage_in_Volt" ,
        XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));

      stdstr = convertToStdString<float>(
          arp_percpucoreattributes[wCPUcoreIndex].m_fThrottleLevel ) ;
      p_dom_elementCore->setAttribute(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"throttle_ratio" ,
        XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));

      stdstr = convertToStdString<float>(
          arp_percpucoreattributes[wCPUcoreIndex].m_fMultiplier ) ;
      p_dom_elementCore->setAttribute(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"multiplier" ,
        XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));

      stdstr = convertToStdString<float>(
          arp_percpucoreattributes[wCPUcoreIndex].m_fReferenceClockInMhz ) ;
      p_dom_elementCore->setAttribute(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"reference_clock_in_MHz" ,
        XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));

      stdstr = convertToStdString<float>(
          arp_percpucoreattributes[wCPUcoreIndex].m_fTempInDegCelsius) ;
      p_dom_elementCore->setAttribute(
        //Cast to "const XMLCh *" to avoid Linux' g++ warning.
        (const XMLCh *) L"temp_in_deg_Celsius" ,
        XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));

      p_dom_elementRoot->appendChild(p_dom_elementCore);
    }
  }

  inline void IPCdataHandler::CreateDVFS_XMLelement(
    XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document)
  {
    std::string stdstr ;
    XERCES_CPP_NAMESPACE::DOMElement * p_dom_elementRoot =
      p_dom_document->getDocumentElement();

    //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMDocument.html
    // #f5e93a6b757adb2544b3f6dffb4b461a:  throws DOMException
    XERCES_CPP_NAMESPACE::DOMElement * p_dom_element = p_dom_document->
      createElement(
      //Cast to "const XMLCh *" to avoid Linux' g++ warning.
      (const XMLCh *) L"DVFS" );
    stdstr = convertToStdString<bool>(
      m_r_cpucontrolservicebase.m_x86iandc_thread_typeDVFSthread.IsRunning() );
    //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMElement.html
    // #1a607d8c619c4aa4a59bc1a7bc5d4692:  exception DOMException
    p_dom_element->setAttribute(
      //Cast to "const XMLCh *" to avoid Linux' g++ warning.
      (const XMLCh *) L"running" ,
      XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));
    p_dom_elementRoot->appendChild( p_dom_element ) ;
  }

  inline void IPCdataHandler::CreateTooHotXMLelement(
    XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document)
  {
    std::string stdstr ;
    XERCES_CPP_NAMESPACE::DOMElement * p_dom_elementRoot =
      p_dom_document->getDocumentElement();

    //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMDocument.html
    // #f5e93a6b757adb2544b3f6dffb4b461a:  throws DOMException
    XERCES_CPP_NAMESPACE::DOMElement * p_dom_element = p_dom_document->
      createElement(
      //Cast to "const XMLCh *" to avoid Linux' g++ warning.
      (const XMLCh *) L"too_hot" );

    stdstr = convertToStdString<wxLongLong_t>( //::wxNow()
//        ::wxGetLocalTimeMillis().GetValue()
       //mr_model.m_cpucoredata.m_llLastTimeTooHot
       m_p_i_cpucontroller->m_llLastTimeTooHot
       ) ;
    //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMElement.html
    // #1a607d8c619c4aa4a59bc1a7bc5d4692:  exception DOMException
    p_dom_element->setAttribute(
      //Cast to "const XMLCh *" to avoid Linux' g++ warning.
      (const XMLCh *) L"last_time_too_hot" ,
      XERCES_STRING_FROM_ANSI_STRING(stdstr.c_str() ));
    p_dom_elementRoot->appendChild( p_dom_element ) ;
  }

  /** throws DOMException */
  void IPCdataHandler::CreateXMLdocumentChildren(
    XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document )
  {
    //XERCES_CPP_NAMESPACE::DOMElement * p_dom_elementRoot =
      p_dom_document->getDocumentElement();
    //XERCES_CPP_NAMESPACE::DOMElement * p_dom_element ;
    PerCPUcoreAttributes * arp_percpucoreattributes =
      mr_model.m_cpucoredata.m_arp_percpucoreattributes ;
    std::string stdstr ;

    CreateCPUcoreXMLelements(p_dom_document, arp_percpucoreattributes);
//    if( mr_model.m_cpucoredata.m_bTooHot )
//    {
    CreateTooHotXMLelement(p_dom_document);
//    }
    CreateDVFS_XMLelement(p_dom_document);
  }

  inline void IPCdataHandler::EnterReadByIPCthreadCriticalSection()
  {
    //An IPC/ pipe client thread may copy from this array at the same time--
    // wait until it's copied.
    LOGN("in-program data to IPC data: before entering critical section for "
      "concurrent read by IPC thread")
    mr_model.m_cpucoredata.m_wxcriticalsectionIPCdata.Enter() ;
    LOGN("in-program data to IPC data: after entering critical section for "
      "concurrent read by IPC thread")
  }

  DWORD IPCdataHandler::GetResponse(BYTE byCommand, BYTE * & r_arbyResponseData )
  {
    DWORD wByteSizeOfData = 0 ;
    switch ( byCommand )
    {
    case get_current_CPU_data:
      GetCurrentCPUcoreAttributeValues(wByteSizeOfData, true ) ;
      break ;
    }
    return wByteSizeOfData ;
  }

  BYTE * IPCdataHandler::GetCurrentCPUcoreAttributeValues(DWORD & r_dwByteSize,
    bool bThreadSafe)
  {
//    BYTE * arby = NULL ;
    //Adapted from Xerces' "CreateDOMDocument" sample:
    // Initialize the XML4C2 system.
    //  x86InfoAndControl::InitializeXerces()
   {
     //TODO the attribute values only need to be modified, creating the
      // DOM tree every time is not necessary
      XERCES_CPP_NAMESPACE::DOMImplementation * p_dom_implementation =
       //http://xerces.apache.org/xerces-c/apiDocs-3/
       // classDOMImplementationRegistry.html#142f6dd9e0b65cda6dd6cc3dc06fc849:
       // "An implementation that has the desired features, or null if this
       //  source has none."
       XERCES_CPP_NAMESPACE::
       DOMImplementationRegistry::getDOMImplementation(
         //Cast to "const XMLCh *" to avoid Linux g++ warning.
         (const XMLCh *) L"Core" );
     LOGN("After getting DOM implementation") ;
     if ( p_dom_implementation )
     {
       CPUcoreData & r_cpucoredata = mr_model.m_cpucoredata ;
       //For DOMException by DOMImplementation::createDocument(...)
       try
       {
         XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document =
           p_dom_implementation->
           //Cites from http://xerces.apache.org/xerces-c/apiDocs-3/
           //classDOMImplementation.html#6469a752f4c2f646269ed616ddb23b65:
           //throws "DOMException"
           createDocument(
           0,  // root element namespace URI.
           // root element name
  //       XERCES_STRING_FROM_ANSI_STRING("CPU_core_data"),
           //"The qualified name of the document element to be created."
           //Cast to "const XMLCh *" to avoid Linux g++ warning.
           (const XMLCh *) L"CPU_core_data" ,
           //"The type of document to be created or null."
           0);
         LOGN("After creating XML document") ;

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

         if( bThreadSafe )
           WaitForThreadSafeReadOfCPUdataAndLog(r_cpucoredata) ;

  //       m_conditionAccess.Wait();
  //       mr_model.m_cpucoredata.m_wNumThreadsAccessingThisObject
  //       mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead.Lock() ;

         CreateXMLdocumentChildren(p_dom_document) ;

         //LOGN_TYPE
         LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues before "
           "ThreadFinishedAccess()"
           //,Logger::sync
           )
         mr_model.m_cpucoredata.ThreadFinishedAccess() ;
  //       LOGN_TYPE
         LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues after "
           "ThreadFinishedAccess()"
           //,Logger::sync
           )
  //       mr_model.DecrementNumberOfThreadsAccess

         //arby =
         SerializeXMLtoMemBuffer(
           p_dom_implementation,
           p_dom_document //,
           //r_dwByteSize
           ) ;

         ReleaseDOMdocument(p_dom_document) ;
       }
       catch (const XERCES_CPP_NAMESPACE::OutOfMemoryException & )
       {
         LOGN( "GetCurrentCPUcoreAttributeValues--Xerces: OutOfMemoryException" )
  //         errorCode = 5;
       }
       catch (const XERCES_CPP_NAMESPACE::DOMException & e)
       {
         LOGN("GetCurrentCPUcoreAttributeValues--Xerces: DOMException code "
           "is: " << e.code )
  //         errorCode = 2;
       }
       catch (...) //remaining exceptions
       {
         LOGN( "GetCurrentCPUcoreAttributeValues--Xerces: An error occurred "
           "creating the document")
  //         errorCode = 3;
       }
     }//if ( p_dom_implementation )
     else
     {
       LOGN( "GetCurrentCPUcoreAttributeValues--Xerces: Requested "
         "implementation is not supported" )
  //       errorCode = 4;
     }
    }
  //  LOGN("BEFORE terminating Xerces") ;
  //  XMLPlatformUtils::Terminate();
  //  LOGN("after terminating Xerces") ;

  //  m_arbyData = arby ;
    return //arby ;
      m_arbyData ;
  } //GetCurrentCPUcoreAttributeValues()

  inline void IPCdataHandler::LeaveReadByIPCthreadCriticalSection()
  {
    //An IPC/ pipe client thread may copy from this array at the same time--
    // wait until it's copied.
    LOGN("in-program data to IPC data: before leaving critical section for "
      "concurrent read by IPC thread")
    mr_model.m_cpucoredata.m_wxcriticalsectionIPCdata.Leave() ;
    LOGN("in-program data to IPC data: after leaving critical section for "
      "concurrent read by IPC thread")
  }

  inline void IPCdataHandler::PossiblyReleaseArrayMemory()
  {
    if( m_arbyData )
    {
      delete [] m_arbyData ;
      //Important: set to NULL for another thread (if it accesses the array it
      //  knows that it's invalid)
      m_arbyData = NULL ;
    }
  }

  void IPCdataHandler::PossiblyReleaseMemoryThreadSafe()
  {
    EnterReadByIPCthreadCriticalSection() ;

    PossiblyReleaseArrayMemory() ;

    LeaveReadByIPCthreadCriticalSection() ;
  }

  void IPCdataHandler::ReleaseDOMdocument(
    XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document )
  {
    //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMNode.html
    // #69b331aca0adcd37a41efa9492c0c9cf:
    // "Called to indicate that this Node (and its associated children)
    // is no longer in use and that the implementation may relinquish any
    // resources associated with it and its associated children.

    // If this is a document, any nodes it owns (created by
    // DOMDocument::createXXXX()) are also released.

    // Access to a released object will lead to unexpected result."
    // exception DOMException
    p_dom_document->release();
  }

  void IPCdataHandler::ReleaseXercesOutputAndSerializer(
    XERCES_CPP_NAMESPACE::DOMLSOutput * p_dom_ls_output ,
    XERCES_CPP_NAMESPACE::DOMLSSerializer * p_dom_ls_serializer
    )
  {
    //http://xerces.apache.org/xerces-c/apiDocs-3/classDOMLSOutput.html
    // #9dfe00d7e15530cc7686f6109348eb2d
    // "Called to indicate that this DOMLSOutput is no longer in use and
    // that the implementation may relinquish any resources associated
    // with it.

    // Access to a released object will lead to unexpected result."
    p_dom_ls_output->release();
    //http://xerces.apache.org/xerces-c/apiDocs-3/
    // classDOMLSSerializer.html#bc804fc58e71e48715f0f0eb9348da25:
    // "Called to indicate that this Writer is no longer in use and that
    // the implementation may relinquish any resources associated with it.

    // Access to a released object will lead to unexpected result."
    p_dom_ls_serializer->release();
  }

//  BYTE *
  void IPCdataHandler::SerializeXMLtoMemBuffer(
    XERCES_CPP_NAMESPACE::DOMImplementation * p_dom_implementation ,
    XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document
//    DWORD & r_dwByteSize
    )
  {
    BYTE * ar_by = NULL ;
    //from Xerces' DOMPrint sample:
    XERCES_CPP_NAMESPACE::DOMLSOutput * p_dom_ls_output = (
      (XERCES_CPP_NAMESPACE::DOMImplementationLS * )
      p_dom_implementation )->createLSOutput();
    XERCES_CPP_NAMESPACE::DOMLSSerializer * p_dom_ls_serializer = (
      (XERCES_CPP_NAMESPACE::DOMImplementationLS * )
      p_dom_implementation)->createLSSerializer();

    XERCES_CPP_NAMESPACE::MemBufFormatTarget mem_buf_format_target ;
    p_dom_ls_output->setByteStream( & mem_buf_format_target);
    //http://xerces.apache.org/xerces-c/apiDocs-3/
    // classDOMLSSerializer.html#5bcb91b60535ae26ff294a2eff65af0a:
    // "Returns true if node was successfully serialized and false in
    // case a failure occured and the failure wasn't canceled by the
    // error handler."
    p_dom_ls_serializer->write(p_dom_document, p_dom_ls_output);
    //MemBufFormatTarget.hpp:
//     "The MemBufFormatTarget returns a NULL terminated XMLByte stream
  // upon request,
//       * through the method getRawBuffer(), and user should make its own copy of the
//       * returned buffer if it intends to keep it independent on the state of the
//       * MemBufFormatTarget."!
    XMLSize_t xmlsize_tBufferLength = mem_buf_format_target.getLen() ;
    const XMLByte * cp_xmlbyteBuffer = mem_buf_format_target.getRawBuffer() ;
    //"user should make its own copy of the
    //  * returned buffer if it intends to keep it independent on the
    //    state of the
    //  * MemBufFormatTarget."!"
    ar_by = new BYTE[ xmlsize_tBufferLength ] ;
    if( ar_by ) //If <> NULL => Allocating the array succeeded.
    {
      memcpy( ar_by, cp_xmlbyteBuffer, xmlsize_tBufferLength ) ;

      EnterReadByIPCthreadCriticalSection() ;
      //Both the size and the array address must be set thread safe/
      // insed a critical section.
//      r_dwByteSize = xmlsize_tBufferLength ;
      m_dwByteSize = xmlsize_tBufferLength ;
      PossiblyReleaseArrayMemory() ;
      m_arbyData = ar_by ;

      LeaveReadByIPCthreadCriticalSection() ;
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
    std::string stdstrAllXMLdata //( //const char * s
//           (char*) cp_xmlbyteBuffer , //size_t n
//           xmlsize_tBufferLength );
      = format_output_data(
        cp_xmlbyteBuffer,
        xmlsize_tBufferLength ,
        100 ) ;
//       LOGN(
    LOGN_DEBUG(
      "XML data: " << stdstrAllXMLdata
//           //should be NULL-terminated
//           cp_xmlbyteBuffer
      << "size:" << xmlsize_tBufferLength
      )

    ReleaseXercesOutputAndSerializer(
      p_dom_ls_output,
      p_dom_ls_serializer ) ;
//    return ar_by ;
  }

  void IPCdataHandler::WaitForThreadSafeReadOfCPUdataAndLog(
    CPUcoreData & cr_cpucoredata )
  {
    //Wait for a possible change of the current CPU data to finish (these
    //data may be modified if the DVFS thread is running)
    // "Wait() atomically unlocks the mutex
    // which allows the thread to continue and starts waiting"
    // -> the DVFS thread gets the lock / continues processing after
    // mutex.lock() / calls Broadcast()

    //LOGN_TYPE
    LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues before Wait()"
      //,Logger::sync
      )
    //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxcondition:
    // "This method atomically releases the lock on the mutex associated with
    // this condition (this is why it must be locked prior to calling Wait)
    // and puts the thread to sleep until Signal  or Broadcast  is called.
    // It then locks the mutex again and returns."
//       mr_model.m_cpucoredata.m_conditionCPUdataCanBeSafelyRead.Wait() ;
    cr_cpucoredata.m_condition_typeCPUdataCanBeSafelyRead.Wait() ;
//       DEBUGN("mutex is locked?"
//         << mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead. )
//       //http://docs.wxwidgets.org/stable/wx_wxcondition.html:
//       // "It then locks the mutex again and returns."
//       //So unlock it for other clients.
//       mr_model.m_cpucoredata.m_mutexCPUdataCanBeSafelyRead.Unlock() ;
    //LOGN_TYPE
    LOGN( "XML_IPC GetCurrentCPUcoreAttributeValues after Wait()"
      //,Logger::sync
      )
  }
}//namespace Xerces
