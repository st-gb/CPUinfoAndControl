/*
 * PDH_CPUcoreUsageGetter.cpp
 *
 *  Created on: Apr 26, 2010
 *      Author: Stefan
 */
#include "StdAfx.h"

#include "PDH_CPUcoreUsageGetter.hpp"
#ifndef _MSC_VER
  #include <Windows_compatible_typedefs.h>
#endif //#ifndef _MSC_VER
//#include <string>
//#include <exception>
//#include <stdexcept>
//#include <Pdh.h>
#include <global.h> //for DEBUGN(...)
#include <Windows/GetCurrentProcessExeFileNameWithoutDirs.hpp>

//from http://msdn.microsoft.com/en-us/library/aa373046%28v=VS.85%29.aspx:
BYTE GetPDHerrorCodeString(DWORD dwErrorCode,std::wstring & wstr )
{
  //std::wstring wstr ;
  HANDLE hPdhLibrary = NULL;
    LPWSTR pMessage = NULL;
    DWORD_PTR pArgs[] = { (DWORD_PTR)L"<collectionname>" };
    //DWORD dwErrorCode = PDH_PLA_ERROR_ALREADY_EXISTS;

    hPdhLibrary = LoadLibraryW(L"pdh.dll");
    if (NULL == hPdhLibrary)
    {
        DEBUGN(L"LoadLibrary failed with " << GetLastError() );
        return 0;
    }

    // Use the arguments array if the message contains insertion points, or you
    // can use FORMAT_MESSAGE_IGNORE_INSERTS to ignore the insertion points.

    if (!FormatMessageW(FORMAT_MESSAGE_FROM_HMODULE |
                       FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       /*FORMAT_MESSAGE_IGNORE_INSERTS |*/
                       FORMAT_MESSAGE_ARGUMENT_ARRAY,
                       hPdhLibrary, 
                       dwErrorCode,
                       0,  
                       (LPWSTR)&pMessage, 
                       0, 
                       //NULL))
                       (va_list*)pArgs))
    {
        DEBUGN(L"Format message failed with " << GetLastError() );
        return 0;
    }

    DEBUGN(L"Formatted message: " << pMessage);
    wstr = std::wstring(pMessage) ;
    LocalFree(pMessage);
    return 1 ;
}

PDH_CPUcoreUsageGetter::PDH_CPUcoreUsageGetter()
  : 
  m_lpwstrProcentProcessorTimeCounterName(NULL)
  , m_ar_pdh_cpu_core_usage_getter_per_core_atts( NULL)
{
#ifdef _DEBUG
  std::string strExeFileNameWithoutDirs = GetStdString( GetExeFileNameWithoutDirs() ) ;
  std::string stdstrFilename = strExeFileNameWithoutDirs +
      ("PDH_CPUcoreUsageGetterDLL_log.txt") ;
  g_logger.OpenFile2( stdstrFilename ) ;
  DEBUGN("this Log file is open")
#endif
  DEBUGN("PDH_CPUcoreUsageGetter::PDH_CPUcoreUsageGetter" )
#ifndef _MSC_VER
  m_hinstancePDH_DLL =
    //If the function fails, the return value is NULL.
    ::LoadLibraryA( "pdh.dll" //LPCSTR / char *
      );
  if( m_hinstancePDH_DLL )
  {
    AssignDLLfunctionPointers() ;
#endif //#ifndef _MSC_VER
    GetProcentProcessorTimeName() ;
    //StartPerfCounting() ;
#ifndef _MSC_VER
  }
  else
    throw std::exception() ;
#endif //#ifndef _MSC_VER
}

PDH_CPUcoreUsageGetter::~PDH_CPUcoreUsageGetter()
{
  //PdhCloseQuery(hQuery);
  if( m_lpwstrProcentProcessorTimeCounterName )
    delete [] m_lpwstrProcentProcessorTimeCounterName ;
  if( m_ar_pdh_cpu_core_usage_getter_per_core_atts )
  {
    for(WORD wCoreID = 0 ; wCoreID < m_wNumLogicalCPUcores ; ++ wCoreID )
      PdhCloseQuery( m_ar_pdh_cpu_core_usage_getter_per_core_atts[ wCoreID ].
        hQuery) ;
    delete [] m_ar_pdh_cpu_core_usage_getter_per_core_atts ;
  }
}

void PDH_CPUcoreUsageGetter::AssignDLLfunctionPointers()
{
#ifdef COMPILE_RUNTIME_DYN_LINKED
  std::string strFuncName ;

  strFuncName = "PdhLookupPerfNameByIndexW" ;
  m_pfnPdhLookupPerfNameByIndexW = (PdhLookupPerfNameByIndexW_type)
    ::GetProcAddress( m_hinstancePDH_DLL, strFuncName.c_str() );
#endif //#ifndef _MSC_VER
}

void PDH_CPUcoreUsageGetter::GetProcentProcessorTimeName()
{
  DWORD dwNameBufferSize = 0 ;
  DEBUGN("PDH_CPUcoreUsageGetter::GetProcentProcessorTimeName" )
//  LPTSTR szNameBuffer ;
  //http://msdn.microsoft.com/en-us/library/aa372648(v=VS.85).aspx:
  //Performance counter names for PdhAddCounter(...) depend on the
  //machine language. the alternative PdhAddEnglishCounter(...) exists from
  //version 6, so it is not usable for winXP.
  //To make it independent, get the name from the index.
  PdhLookupPerfNameByIndexW(
    //"If NULL, the function uses the local computer."
    NULL ,//__in   LPCTSTR szMachineName,
    // "% Processor Time" in the English language
    6, //__in   DWORD dwNameIndex,
    //"Set to NULL if pcchNameBufferSize is zero."
    NULL ,//__out  LPTSTR szNameBuffer,
    //"If zero on input, the function returns PDH_MORE_DATA and sets this
    //parameter to the required buffer size."
    & dwNameBufferSize //__in   LPDWORD pcchNameBufferSize
    ) ;
  m_lpwstrProcentProcessorTimeCounterName = new WCHAR[ dwNameBufferSize] ;
  DEBUGN("percent processor time name:" << m_lpwstrProcentProcessorTimeCounterName )
  //"If the function succeeds, it returns ERROR_SUCCESS. "
  PDH_STATUS pdh_status =
      PdhLookupPerfNameByIndexW(
    //"If NULL, the function uses the local computer."
    NULL ,//__in   LPCTSTR szMachineName,
    // "% Processor Time" in the English language
    6, //__in   DWORD dwNameIndex,
    m_lpwstrProcentProcessorTimeCounterName ,// __out  LPTSTR szNameBuffer,
    //"If zero on input, the function returns PDH_MORE_DATA and sets this
    //parameter to the required buffer size."
    & dwNameBufferSize //__in   LPDWORD pcchNameBufferSize
    ) ;
  if( pdh_status == ERROR_SUCCESS )
  {
#ifdef _DEBUG
    std::wstring wstr(m_lpwstrProcentProcessorTimeCounterName) ;
    DEBUGWN("successfully got percent processor time name:"
        << m_lpwstrProcentProcessorTimeCounterName
      << wstr )
    //DEBUGN("percent processor time name:" << m_lpwstrProcentProcessorTimeCounterName )
#endif
  }
  else
    throw std::exception() ;
}

float PDH_CPUcoreUsageGetter::GetPercentalUsageForCore(
  BYTE byCoreID
  )
{
  float fRet = -1.0 ;
  PDH_STATUS pdh_status ;
  DEBUGN("PDH_CPUcoreUsageGetter::GetPercentalUsageForCore for core " << (WORD) byCoreID )
//  printf("Starting the process...\n");

  //pdh_status = PdhCollectQueryData(
  //  //"Handle of the query for which you want to collect data. The 
  //  //PdhOpenQuery function returns this handle."
  //  hQuery );
  //if( pdh_status == ERROR_SUCCESS )
  {
    //DEBUGN("PdhCollectQueryData succeeded")
     // Read the next record
    pdh_status = PdhCollectQueryData(
      //hQuery
      m_ar_pdh_cpu_core_usage_getter_per_core_atts[byCoreID].hQuery
      );

    if (ERROR_SUCCESS == pdh_status)
    {
      PDH_FMT_COUNTERVALUE FmtValue ;
      DEBUGN("PdhCollectQueryData succeeded")
      pdh_status = 
        //http://msdn.microsoft.com/en-us/library/aa372637%28v=VS.85%29.aspx:
        //"If the function succeeds, it returns ERROR_SUCCESS. "
        PdhGetFormattedCounterValue(
        //"The PdhAddCounter function returns this handle."
        //hCounter, 
        m_ar_pdh_cpu_core_usage_getter_per_core_atts[byCoreID].hCounter ,
        PDH_FMT_DOUBLE,
        //"Receives the counter type. For a list of counter types, see 
        //the Counter Types section of the Windows Server 2003 Deployment 
        //Kit. This parameter is optional."
        NULL,
        & FmtValue 
        );
      if( pdh_status == ERROR_SUCCESS )
      {
        DEBUGN("PdhGetFormattedCounterValue succeeded")
        DEBUGN("PDH_CPUcoreUsageGetter::GetPercentalUsageForCore for core usage:" 
          << std::ios::hex << FmtValue.doubleValue )
        fRet = FmtValue.doubleValue ;
      }
      else
      {
//        std::wstring wstr ;
//        if( GetPDHerrorCodeString(pdh_status, wstr) )
//          DEBUGWN(L"PdhGetFormattedCounterValue failed: " << (DWORD) pdh_status
//            << wstr )
      }
    }
    else
    {
      DEBUGN("PdhCollectQueryData failed")
      std::wstring wstr ;
//      if( GetPDHerrorCodeString(pdh_status, wstr) )
//        DEBUGWN(L"PdhCollectQueryData failed: " << (DWORD) pdh_status
//          << wstr )
    }
  }
//  printf("The cpu usage is : %f%%\n", FmtValue.doubleValue);
  //The caller expects value in range [0...1]
  return fRet / 100.0 ;
}

#ifdef COMPILE_RUNTIME_DYN_LINKED
PDH_FUNCTION
PDH_CPUcoreUsageGetter::PdhLookupPerfNameByIndexW(
  __in_opt LPCWSTR szMachineName,
  __in     DWORD   dwNameIndex,
  __out_ecount_opt(* pcchNameBufferSize) LPWSTR  szNameBuffer,
  __inout  LPDWORD pcchNameBufferSize
  )
{
  return 0 ;
}
#endif //#ifdef COMPILE_RUNTIME_DYN_LINKED

void PDH_CPUcoreUsageGetter::SetNumberOfCPUcores( WORD wNumLogicalCPUcores )
{
  DEBUGN("PDH_CPUcoreUsageGetter::SetNumberOfCPUcores(" 
    << wNumLogicalCPUcores << ")" )
  m_wNumLogicalCPUcores = wNumLogicalCPUcores ;
  m_ar_pdh_cpu_core_usage_getter_per_core_atts = new 
    PDH_CPUcoreUsageGetterPerCoreAtts[ wNumLogicalCPUcores ] ;
  StartPerfCounting() ;
}

void PDH_CPUcoreUsageGetter::StartPerfCounting( )
{
  PDH_STATUS pdh_status ;
  //std::wstring stdwstr(L"\\Processor(0)\\") ;
  std::wstring stdwstr( //L"\\Processor(_Total)\\"
    L"\\Processor(\\") ;
//  DEBUGWN("percent processor time name:" <<
//      m_lpwstrProcentProcessorTimeCounterName )
  for( WORD wCPUcoreID = 0 ; wCPUcoreID < m_wNumLogicalCPUcores ; ++ wCPUcoreID )
  {
    m_lpwstrCPUcoreNumber = _itow( wCPUcoreID, m_ar_wch, 10 );
//    DEBUGWN( L"core number as string:" << m_lpwstrCPUcoreNumber )
    std::wstring stdwstr( L"\\Processor(") ;
    stdwstr += m_lpwstrCPUcoreNumber + std::wstring(L")\\") ;
    stdwstr += m_lpwstrProcentProcessorTimeCounterName ;
//    DEBUGWN("full query:" << stdwstr )
    //http://msdn.microsoft.com/en-us/library/aa372652%28v=VS.85%29.aspx:
    //"If the function succeeds, it returns ERROR_SUCCESS."
    pdh_status = PdhOpenQuery(
      //"If NULL, performance data is collected from a real-time data source. "
      NULL,
      //"User-defined value to associate with this query. To retrieve the user 
      //data later, call PdhGetCounterInfo and access the dwQueryUserData 
      //member of PDH_COUNTER_INFO."
      //0, 
      wCPUcoreID ,
      //"Handle to the query."
      //& hQuery
      & m_ar_pdh_cpu_core_usage_getter_per_core_atts[wCPUcoreID].hQuery
      );
    if( pdh_status == ERROR_SUCCESS )
      DEBUGN("PdhOpenQuery succeeded")
    pdh_status = PdhAddCounterW( 
      //hQuery,
      m_ar_pdh_cpu_core_usage_getter_per_core_atts[wCPUcoreID].hQuery ,
      //http://technet.microsoft.com/en-us/library/cc784617%28WS.10%29.aspx:
      //"An instance called _Total instance is available on most objects and
      //represents the sum of the values for all instances of the object for a
      //specific counter."
      //L"\\Processor(_Total)\\% Processor Time" ,
      //L"\\Processor(0)\\% Processor Time" ,
      stdwstr.c_str() ,
      //"User-defined value. This value becomes part of the counter information. 
      //To retrieve this value later, call the PdhGetCounterInfo function and 
      //access the dwUserData member of the PDH_COUNTER_INFO structure."
      //0,
      wCPUcoreID ,
      //& hCounter 
      & m_ar_pdh_cpu_core_usage_getter_per_core_atts[wCPUcoreID].hCounter
      );
    if( pdh_status == ERROR_SUCCESS )
    {
      DEBUGN("PdhAddCounterW succeeded")
    }
  }
}
