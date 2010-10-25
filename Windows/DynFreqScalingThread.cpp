//the following includes must be here above, else compiler errors
#define _WIN32_DCOM
#define _WIN32_WINNT 0x0501
//typedef __int64 long long ;
//#if defined(COMPILER_1)
//typedef long long long64;
//#elif defined(__CYGWIN32__)
//typedef __int64 long64;
//#else
//#error This compiler is not supported.
//#endif

//Indirectly included files use __int64 that is not supported on cygwin.
#  ifndef __int64        /* some versions seem to #define it already */
#    define __int64 long long
#  endif
#if defined(__CYGWIN32__)
  //#define __thiscall Thiscall
  #define __thiscall
  #define __forceinline inline
  #define _MSC_VER 1200
#endif
//#ifndef __GNUC_VA_LIST
//#define __GNUC_VA_LIST
//typedef __builtin_va_list __gnuc_va_list;
//#endif
//#include <stdarg.h>
#include <iostream> //for "std::cout"
using namespace std;
#include <Windows.h> //for CreateThread(...)
#include "DynFreqScalingThread.hpp"
#include <Controller/CPUcontrolBase.hpp>

using namespace Windows_API ;

DWORD WINAPI DynFreqScalingThreadProc(LPVOID lpParameter)
{
  DynFreqScalingThread * cp_dynfreqscalingthread =
    (DynFreqScalingThread * ) lpParameter ;
  if( cp_dynfreqscalingthread )
  {
    cp_dynfreqscalingthread->Entry() ;
    //while(cp_dynfreqscalingthread->m_vbRun ) 
    //{
    //  Sleep(700) ;
    //  MessageBeep( (WORD ) -1 ) ;
    //}
  }
  LOGN("The Dynamic Voltage and Frequency Scaling thread ends now.")
  return 0 ;
}

//DynFreqScalingThread::DynFreqScalingThread(
//  ICPUcoreUsageGetter * p_icpu
//  , I_CPUcontroller * p_cpucontroller
//  , CPUcoreData & r_cpucoredata
//  )
//  :
//  DynFreqScalingThreadBase(
//    p_icpu
//    , p_cpucontroller
//    , r_cpucoredata
//    )
//{
//}

DynFreqScalingThread::DynFreqScalingThread(
  CPUcontrolBase & r_cpucontrolbase
  , CPUcoreData & r_cpucoredata
  )
  : DynFreqScalingThreadBase(
    r_cpucontrolbase
    , r_cpucoredata
    )
{
  LOGN("win API DVFS thread--CPU controller:"
    << r_cpucontrolbase.mp_cpucontroller
    << "CPU usage getter: " << r_cpucontrolbase.mp_cpucoreusagegetter )
}

//@return 0=success
DWORD DynFreqScalingThread::Run()
{
  DWORD dwRet = ERROR_SUCCESS ;
  DWORD dwThreadId ;
  //Call the base class' method that does the work needed for subclasses that
  //(re-)start a DVFS thread.
  DynFreqScalingThreadBase::Start() ;
  m_handleThread =
    //http://msdn.microsoft.com/en-us/library/ms682453%28VS.85%29.aspx :
    //"If the function fails, the return value is NULL.
    //To get extended error information, call GetLastError."
    ::CreateThread(
    //LPSECURITY_ATTRIBUTES lpThreadAttributes,
    //"If lpThreadAttributes is NULL, the thread gets a default security
    // descriptor."
    NULL,  // default security attributes
    //SIZE_T dwStackSize,
    //"If this parameter is zero, the new thread uses the default size for the
    //executable."
    0,                      // use default stack size  
    //LPTHREAD_START_ROUTINE lpStartAddress,
    DynFreqScalingThreadProc ,       // thread function name
    //LPVOID lpParameter,
    this ,          // argument to thread function 
    //DWORD dwCreationFlags,
    // "If this value is zero, the thread runs immediately after creation."
    0, // use default creation flags
    //LPDWORD lpThreadId
    & dwThreadId
    );   // returns the thread identifier 
  if( m_handleThread )
  {
    LOGN("Creating the DVFS thread succeeded.")
  }
  else
  {
    LOGN("Creating the DVFS thread failed.")
    dwRet = GetLastError() ;
  }
  LOGN("Windows_API::DynFreqScalingThread::Run()--return" << dwRet )
  //wxThread->Run() also returns 0 on success
  return //hThread != 0 ;
    dwRet ;
}

DynFreqScalingThread::~DynFreqScalingThread()
{
  //http://msdn.microsoft.com/en-us/library/ms686724%28v=VS.85%29.aspx:
  //"When a thread terminates, its thread object is not freed until all open
  //handles to the thread are closed."
  //http://msdn.microsoft.com/en-us/library/ms724211%28v=VS.85%29.aspx:
  //"Closing a thread handle does not terminate the associated thread or remove
  //the thread object."
  //Close the thread handle here (waiting for the end of the thread via
  // WaitForSingleObject() would need another thread->not so good.)
  ::CloseHandle(m_handleThread ) ;
}

//BYTE
//@return 0=success
DWORD DynFreqScalingThread::Start()
{
  LOGN("Windows_API::DynFreqScalingThread::Start() begin")
#ifdef COMPILE_WITH_LOG
  DWORD dwRet ;
  dwRet =
    //This finally starts the DVFS thread execution.
    Run() ;
  LOGN("Windows_API::DynFreqScalingThread::Start()--return " << dwRet )
  return dwRet ;
#else
  return Run() ;
#endif
}

void * DynFreqScalingThread::WaitForTermination()
{
  LOGN("Windows_API::DynFreqScalingThread::WaitForTermination")
  return (void *) ::WaitForSingleObject(m_handleThread,INFINITE) ;
}

////#include "Controller/ICPUcoreUsageGetter.hpp"
//#include <Windows.h> //for ::SetThreadAffinityMask(...) ::CloseHandle(...)
//
//void Windows_API::DynFreqScalingThread::SetMembers(
//  ICPUcoreUsageGetter * p_icpucoreusagegetter,
//{
//  mp_icpucoreusagegetter = p_icpucoreusagegetter ;
//}
