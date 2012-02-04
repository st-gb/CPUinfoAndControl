/*
 * IPCdataClient.cpp
 *
 *  Created on: 22.09.2011
 *      Author: Stefan
 */
#include "IPCdataClient.hpp"

void IPCdataClient::FetchCPUcoreDataFromIPC()
{
//  LOGN("FetchCPUcoreDataFromIPC begin")
//#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
////  NamedPipeClient & r_namedpipeclient =
////      m_ipcclient ;
//  //Lock concurrent access to p_i_ipcclient from another thread.
//  wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
//    m_wxcriticalsectionIPCobject ) ;
//  IPC_Client * p_i_ipcclient = m_p_i_ipcclient ;
//  s_ipc_data.m_byCommand = get_current_CPU_data;
//  s_ipc_data.m_wDataToWriteSizeInByte = 1;
//
//  if( p_i_ipcclient)
//  {
////    p_i_ipcclient->m_crit_secIPCdata.Enter();
//    if(
//      //sending command succeeded
////    r_namedpipeclient.SendCommandAndGetResponse(get_current_CPU_data) &&
//      p_i_ipcclient->SendCommandAndGetResponse(//get_current_CPU_data
//        s_ipc_data) == IPC_Client::
//        //GetsResponseNonBlocking;
//        GotResponse &&
//  //    ::wxGetApp().m_ipcclient.SendCommand(get_current_CPU_data) ;
////  if(
////    r_namedpipeclient.m_arbyIPCdata &&
//      p_i_ipcclient->m_arbyIPCdata &&
//    // > 0 bytes
////    r_namedpipeclient.m_dwIPCdataSizeInByte
//      p_i_ipcclient->m_dwIPCdataSizeInByte
//      )
//    {
////      m_ipc_current_cpu_data_handler
////      m_sax2_ipc_current_cpu_data_handler.
////        Parse( r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwIPCdataSizeInByte ) ;
////      XERCES_CPP_NAMESPACE::MemBufInputSource membufinputsource(
//////        arby,
//////        dwSizeInBytes ,
////        r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwIPCdataSizeInByte ,
////        L"IPC_buffer" ) ;
////      wxCriticalSectionLocker locker( m_sax2_ipc_current_cpu_data_handler.
////        m_wxcriticalsection ) ;
//      p_i_ipcclient->m_crit_secIPCdata.Enter();
//      p_i_ipcclient->m_bProcessingIPCdata = //true;
//        IPC_Client::ProcessingIPCdata;
//      p_i_ipcclient->m_crit_secIPCdata.Leave();
//
////      std::wstring std_wstrXML = L
////      std::string std_strXML =
////        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n"
////          "<CPU_core_data>\n"
////          "<core "
////          "load=\"0.5\" "
////          "multiplier=\"9\" "
////          "number=\"0\" "
////          "reference_clock_in_MHz=\"133.3\" "
////          "temp_in_deg_Celsius=\"40\" "
////          "voltage_in_Volt=\"0\" "
////          "/>"
////          //"</core>"
////        "</CPU_core_data>";
////      p_i_ipcclient->m_arbyIPCdata = (BYTE *) //std_wstrXML.c_str();
////        std_strXML.c_str();
////      p_i_ipcclient->m_dwIPCdataSizeInByte = //std_wstrXML.length() * 2;
////          std_strXML.length();
//
////      ReadXMLdocumentInitAndTermXerces(
//      if( ReadXMLdocumentWithoutInitAndTermXerces(
//  //        membufinputsource,
////        r_namedpipeclient.m_arbyIPCdata ,
//        p_i_ipcclient->m_arbyIPCdata ,
////        r_namedpipeclient.m_dwIPCdataSizeInByte ,
//        p_i_ipcclient->m_dwIPCdataSizeInByte ,
//        (WCHAR * ) L"IPC_buffer" ,
//        m_model ,
//        p_wxx86infoandcontrolapp ,
//        m_sax2_ipc_current_cpu_data_handler
//        ) == FAILURE
//        )
//      {
//        //exception-> leave crit sec (endDocument(), where Leave() is
//        // called is not reached)
//        LOGN("wxconditionIPC2InProgramData.Leave")
//        m_model.m_cpucoredata.
//          wxconditionIPC2InProgramData.Leave() ;
//        LOGN("after wxconditionIPC2InProgramData.Leave")
//      }
//      p_i_ipcclient->m_crit_secIPCdata.Enter();
//      p_i_ipcclient->m_bProcessingIPCdata = //false;
//        IPC_Client:://IPCdataProcessed;
//        ReadNextIPCdata;
//      p_i_ipcclient->m_crit_secIPCdata.Leave();
//    }
////      p_cpucontroller = &
////          m_sax2_ipc_current_cpu_data_handler ;
////      p_cpucoreusagegetter = &
////          m_sax2_ipc_current_cpu_data_handler ;
////      //The number of CPU cores is known if the IPC data were got at first.
////      WORD wNumCPUcores = p_cpucoreusagegetter->GetNumberOfLogicalCPUcores() ;
////      if( wNumCPUcores < mp_cpucoredata->m_byNumberOfCPUCores )
////        mp_cpucoredata->SetCPUcoreNumber( wNumCPUcores ) ;
//  }
//  m_vbGotCPUcoreData = true ;
//#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE_
}

