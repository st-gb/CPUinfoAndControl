/*
 * InstableCPUcoreOperationDetection.cpp
 *
 *  Created on: 19.06.2013
 *      Author: Stefan
 */

#include "Windows/InstableCPUcoreOperationDetection.hpp"
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Windows/multithread/GetThreadTimes_API.hpp>

#include <windows.h> // QueryThreadCycleTime(...)
#ifdef _MSC_VER
  #define _WIN32_WINNT 0x0600 //for QueryThreadCycleTime(...)
#else
  typedef BOOL (WINAPI * pfnQueryThreadCycleTime) (
    HANDLE ThreadHandle,
    PULONG64 CycleTime
  );
  static pfnQueryThreadCycleTime g_pfnquerythreadcycletime;
  static ULONGLONG gs_NumCPUcyclesElapsedForCPUcore0;
  #include <Controller/CPU-related/ReadTimeStampCounter.h> //ReadTSCinOrder();
#endif

namespace Windows
{

  InstableCPUcoreOperationDetection::InstableCPUcoreOperationDetection(
    CPUcoreData & r_cpuCoreData)
    : ::InstableCPUcoreOperationDetection(r_cpuCoreData),
    dwMilliSecondsToWait(1000)
  {
    // TODO Auto-generated constructor stub

  }

  InstableCPUcoreOperationDetection::~InstableCPUcoreOperationDetection()
  {
    // TODO Auto-generated destructor stub
  }

  ULARGE_INTEGER GetTimeDiff(FILETIME TimeBefore,FILETIME TimeAfter)
  {
    //http://msdn.microsoft.com/en-us/library/ms724284(v=vs.85).aspx:
    //"It is not recommended that you add and subtract values from the
    //FILETIME structure to obtain relative times. Instead, you should copy
    //the low- and high-order parts of the file time to a ULARGE_INTEGER
    //structure, perform 64-bit arithmetic on the QuadPart member, and copy
    //the LowPart and HighPart members into the FILETIME structure."
    ULARGE_INTEGER uliBefore = {TimeBefore.dwLowDateTime, TimeBefore.dwHighDateTime};
    ULARGE_INTEGER uliAfter = {TimeAfter.dwLowDateTime, TimeAfter.dwHighDateTime};

    ULARGE_INTEGER uli;
    uli.QuadPart = (uliAfter.QuadPart - uliBefore.QuadPart);
    return uli;
  }


  float InstableCPUcoreOperationDetection::GetCPUcoreUsageForDynLibThread(
    //FILETIME & userTimeBefore,
//    HANDLE handleThread,
    )
  {
    float fCPUcoreUsage;
    ULONG64 ul64ThreadTimeAfter, ul64KernelTimeAfter = 0ULL;
  //  if( g_pfnquerythreadcycletime )
  //  {
  //    BOOL b = (* g_pfnquerythreadcycletime)( handleThread, //ul64ThreadTimeAfter
  //      & ul64ThreadTimeAfter);
  //    if( b)
  //    {
  //      LOGN( FULL_FUNC_NAME << " QueryThreadCycleTime for thread "
  //        << handleThread << "(ID: " //<< ::GetThreadId(handleThread)
  //        << ") :"
  //        << ul64ThreadTimeAfter)
  //      ULONGLONG ull = ReadTSCinOrder( 1 );
  //      LOGN( FULL_FUNC_NAME << " timestampcounter:" << ull)
  //      ULONG64 ul64NumCPUcyclesElapsedForCPUcore0 = ull -
  //        gs_NumCPUcyclesElapsedForCPUcore0;
  //
  //      LOGN( FULL_FUNC_NAME << " timestampcounter diff:"
  //        << ul64NumCPUcyclesElapsedForCPUcore0)
  //
  //      ULONG64 ul64ThreadTimeDiff = ul64ThreadTimeAfter - ul64ThreadTimeBefore;
  //      LOGN( FULL_FUNC_NAME << " thread cycle time diff:"
  //        << ul64ThreadTimeDiff)
  //
  //      fCPUcoreUsage = (float) ( (double) ul64ThreadTimeDiff /
  //          (double) ul64NumCPUcyclesElapsedForCPUcore0);
  //      LOGN( FULL_FUNC_NAME << " CPU core usage = cycle time diff/"
  //        "timestampcounter diff =" << fCPUcoreUsage)
  //
  //      gs_NumCPUcyclesElapsedForCPUcore0 = ull;
  //    }
  //    else
  //      LOGN( FULL_FUNC_NAME << " QueryThreadCycleTime failed:"
  //        << GetErrorMessageFromLastErrorCodeA() )
  //  }
  //  else
    {
      //TODO bind to "GetThreadTimes" dynamically at runtime for win 2000
      //compatibility.
        bool b = GetThreadTimesAPI::GetTimeExecuted(
          //handleThread
          m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.GetThreadHandle(),
          ul64ThreadTimeAfter,
          ul64KernelTimeAfter);
        if( b)
        {
          LOGN_DEBUG( FULL_FUNC_NAME << " user time [100 ns steps] for instable CPU "
            "core operation detection dyn lib "
            " procedure before: " //<< userTimeBefore.dwHighDateTime
            //<< " " << userTimeBefore.dwLowDateTime
            << ul64ThreadTimeBefore << " "
            << " after:"
    //        << userTimeAfter.dwHighDateTime << " "
    //        << userTimeAfter.dwLowDateTime
            << ul64ThreadTimeAfter
            )
          LOGN_DEBUG( FULL_FUNC_NAME << " kernel time for instable CPU "
            "core operation DLL "
            " procedure after:" << ul64KernelTimeAfter )

        //ULARGE_INTEGER uli = GetTimeDiff(userTimeBefore,userTimeAfter);
        ULARGE_INTEGER uli;
        uli.QuadPart = ul64ThreadTimeAfter - ul64ThreadTimeBefore;
        LOGN( FULL_FUNC_NAME << " user time diff for instable CPU core operation DLL "
          " procedure [100ns]: " << uli.QuadPart)
        fCPUcoreUsage = //100-nanosecond intervals
          (float) ( (double) uli.QuadPart / (double) (dwMilliSecondsToWait
          //get in 100 ns: ms (10^-3) -> 100ns (10^-7) => 10^-3 - 10^-7=10^5
          * 10000) );
        LOGN( FULL_FUNC_NAME << " CPU core usage:"//"= user time diff/"
          //"# 100 nanoseconds to wait ="
          << fCPUcoreUsage)
      }
      else
        LOGN_WARNING( " calling \"GetThreadTimes\" failed: "
          << ::GetErrorMessageFromLastErrorCodeA() )
    }
  //  userTimeBefore = userTimeAfter;
    ul64ThreadTimeBefore = ul64ThreadTimeAfter;
    return fCPUcoreUsage;
  }

//    /** @brief req. min: Windows XP
//     * @return  amount of time that the thread has executed in user mode
//     *  ( number of 100-nanosecond intervals
//     *  @see http://msdn.microsoft.com/en-us/library/windows/desktop/ms724284%28v=vs.85%29.aspx)
//     *   */
//    ULONG64 GetThreadStartTimeViaGetThreadTimes(const HANDLE & hThread)
//    {
//
//    }


  ULONG64 InstableCPUcoreOperationDetection::GetThreadUserModeStartTime(
    //HANDLE hThread
    )
  {
    ULONG64 ul64 = 0ULL;
  //  DWORD dwMajorVersion;
  //  DWORD dwMinorVersion;
    //see http://msdn.microsoft.com/en-us/library/ms684943(v=vs.85).aspx

    HINSTANCE hinstanceKernel32DLL =
      //If the function fails, the return value is NULL.
      ::LoadLibraryA( "Kernel32.dll" //LPCSTR
        );
    std::string strFuncName = "QueryThreadCycleTime" ;
    g_pfnquerythreadcycletime = (pfnQueryThreadCycleTime)
      ::GetProcAddress(
      hinstanceKernel32DLL, strFuncName.c_str() );
    ::FreeLibrary(hinstanceKernel32DLL);

  //  GetWindowsVersion(dwMajorVersion, dwMinorVersion);
  //  if( dwMajorVersion < 6 ) // < Win Vista <=> win XP etc.
    //if( ! g_pfnquerythreadcycletime)
    {
      ULONG64 ul64_100NsIntervalsExecutedInUserMode;
      ULONG64 ul64_100NsIntervalsExecutedInKernelMode;
      GetThreadTimesAPI::GetTimeExecuted(
        m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.GetThreadHandle(),
        ul64_100NsIntervalsExecutedInUserMode,
        ul64_100NsIntervalsExecutedInKernelMode
        );
      return //(hThread);
        ul64_100NsIntervalsExecutedInUserMode;
    }
  //  else
  //  if( g_pfnquerythreadcycletime)
  //  {
  //    BOOL b = //::QueryThreadCycleTime(
  //      (* g_pfnquerythreadcycletime)(
  //      hThread, //_In_   HANDLE ThreadHandle,
  //      & ul64 //_Out_  PULONG64 CycleTime
  //    );
  //    if( b )
  //      LOGN( FULL_FUNC_NAME << " QueryThreadCycleTime:" << ul64)
  //    else
  //      LOGN_TYPE( " calling GetThreadTimes failed: "
  //        << ::GetErrorMessageFromLastErrorCodeA(),
  //        I_LogFormatter::log_message_typeWARNING)
  //    gs_NumCPUcyclesElapsedForCPUcore0 = ReadTSCinOrder( 1 );
  //    LOGN( FULL_FUNC_NAME << " timestampcounter for 1st measurement:"
  //      << gs_NumCPUcyclesElapsedForCPUcore0)
  //  }
    return ul64;
  }

} /* namespace Windows */
