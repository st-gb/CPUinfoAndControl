/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * IPCdataHandler.hpp
 *
 *  Created on: May 18, 2010
 *      Author: Stefan
 */

#ifndef IPC_HPP_
#define IPC_HPP_

#include <Controller/IPC/I_IPC_DataHandler.hpp>
#include <xercesc/util/XercesVersion.hpp> //for XERCES_CPP_NAMESPACE

//Forward declarations
class CPUcontrolServiceBase;
class CPUcoreData ;
class I_CPUcontroller;
class PerCPUcoreAttributes;
class Model ;
namespace XERCES_CPP_NAMESPACE
{
  class DOMDocument ;
  class DOMImplementation ;
  class DOMLSOutput ;
  class DOMLSSerializer ;
}

namespace Xerces
{
  //This class may be used e.g. from sockets or pipes.
  class IPCdataHandler
    : public I_IPC_DataHandler
  {
    bool m_bXercesSuccessfullyInitialized ;
  public:
//    bool m_bThreadSafe ;
    DWORD m_dwByteSize ;
    BYTE * m_arbyData ;
    I_CPUcontroller * m_p_i_cpucontroller;
    Model & mr_model ;
    CPUcontrolServiceBase & m_r_cpucontrolservicebase;

    IPCdataHandler( Model & r_model, CPUcontrolServiceBase & ) ;
    virtual ~IPCdataHandler( ) ;

    inline void CreateCPUcoreXMLelements(
      XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document,
      PerCPUcoreAttributes * arp_percpucoreattributes);
    inline void CreateDVFS_XMLelement(XERCES_CPP_NAMESPACE::DOMDocument * );
    inline void CreateTooHotXMLelement(XERCES_CPP_NAMESPACE::DOMDocument * );
    inline void CreateXMLdocumentChildren(
      XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document ) ;
    inline void EnterReadByIPCthreadCriticalSection() ;
    BYTE * GetCurrentCPUcoreAttributeValues(DWORD & r_dwByteSize,
      bool bThreadSafe) ;
    DWORD GetResponse(BYTE byCommand,
      BYTE * & r_arbyResponseData ) ;
    void LeaveReadByIPCthreadCriticalSection() ;
    inline void PossiblyReleaseArrayMemory() ;
    void PossiblyReleaseMemoryThreadSafe() ;
    inline void ReleaseDOMdocument(
      XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document ) ;
    inline void ReleaseXercesOutputAndSerializer(
      XERCES_CPP_NAMESPACE::DOMLSOutput * p_dom_ls_output ,
      XERCES_CPP_NAMESPACE::DOMLSSerializer * p_dom_ls_serializer) ;
    inline //BYTE *
      void SerializeXMLtoMemBuffer(
      XERCES_CPP_NAMESPACE::DOMImplementation * p_dom_implementation ,
      XERCES_CPP_NAMESPACE::DOMDocument * p_dom_document //,
//      DWORD & r_dwByteSize
      ) ;
    inline void WaitForThreadSafeReadOfCPUdataAndLog(
      CPUcoreData & cr_cpucoredata ) ;
  };
} ;

#endif /* IPC_HPP_ */
