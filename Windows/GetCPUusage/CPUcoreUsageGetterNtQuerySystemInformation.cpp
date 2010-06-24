#include "CPUcoreUsageGetterNtQuerySystemInformation.hpp"
//#include <Windows.h> //GetProcAddress()
//#include <Winnt.h> //CONTEXT()
////
//// Context Frame
////
////  This frame has a several purposes: 1) it is used as an argument to
////  NtContinue, 2) is is used to constuct a call frame for APC delivery,
////  and 3) it is used in the user level thread creation routines.
////
////  The layout of the record conforms to a standard call frame.
////
//
//typedef struct _CONTEXT {
//
//    //
//    // The flags values within this flag control the contents of
//    // a CONTEXT record.
//    //
//    // If the context record is used as an input parameter, then
//    // for each portion of the context record controlled by a flag
//    // whose value is set, it is assumed that that portion of the
//    // context record contains valid context. If the context record
//    // is being used to modify a threads context, then only that
//    // portion of the threads context will be modified.
//    //
//    // If the context record is used as an IN OUT parameter to capture
//    // the context of a thread, then only those portions of the thread's
//    // context corresponding to set flags will be returned.
//    //
//    // The context record is never used as an OUT only parameter.
//    //
//
//    DWORD ContextFlags;
//
//    //
//    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
//    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
//    // included in CONTEXT_FULL.
//    //
//
//    DWORD   Dr0;
//    DWORD   Dr1;
//    DWORD   Dr2;
//    DWORD   Dr3;
//    DWORD   Dr6;
//    DWORD   Dr7;
//
//    //
//    // This section is specified/returned if the
//    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
//    //
//
//    FLOATING_SAVE_AREA FloatSave;
//
//    //
//    // This section is specified/returned if the
//    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
//    //
//
//    DWORD   SegGs;
//    DWORD   SegFs;
//    DWORD   SegEs;
//    DWORD   SegDs;
//
//    //
//    // This section is specified/returned if the
//    // ContextFlags word contians the flag CONTEXT_INTEGER.
//    //
//
//    DWORD   Edi;
//    DWORD   Esi;
//    DWORD   Ebx;
//    DWORD   Edx;
//    DWORD   Ecx;
//    DWORD   Eax;
//
//    //
//    // This section is specified/returned if the
//    // ContextFlags word contians the flag CONTEXT_CONTROL.
//    //
//
//    DWORD   Ebp;
//    DWORD   Eip;
//    DWORD   SegCs;              // MUST BE SANITIZED
//    DWORD   EFlags;             // MUST BE SANITIZED
//    DWORD   Esp;
//    DWORD   SegSs;
//
//    //
//    // This section is specified/returned if the ContextFlags word
//    // contains the flag CONTEXT_EXTENDED_REGISTERS.
//    // The format and contexts are processor specific
//    //
//
//    BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];
//
//} CONTEXT;
//
//typedef CONTEXT *PCONTEXT;

//#define _AFXDLL
//#include <Winnt.h> //LPCONTEXT
//#include <Winbase.h> //GetProcAddress()
//for "LPCONTEXT": Windows.h must include Vis Studios "Winnt.h", not Winnt.h from 
//  e.g. another platform SDK
//#define MIDL_PASS
#include <Windows.h> //GetProcAddress()
//#include "global.h" //for _T()
#ifdef __CYGWIN__ //does not have a "tchar.h" shipped.
    #include <Controller/MSVC_adaption/tchar.h> //_tcsrchr()
#else //MSC, MINGW (,...)
    #include <tchar.h> //for _T(...)
#endif

CPUcoreUsageGetterNtQuerySystemInformation::
  CPUcoreUsageGetterNtQuerySystemInformation()
    : ar_sppi(NULL)
  {
    NtQuerySystemInformation = (PROCNTQSI)::GetProcAddress(
      ::GetModuleHandle(//_T(
      //This param is either ASCII or wide char (if _UNICODE is #defined)
      _T("ntdll") )
      //)
      , "NtQuerySystemInformation"); 

    if (NtQuerySystemInformation) 
    { 
      status = NtQuerySystemInformation(
        SystemBasicInformation, //SystemInformationClass
        &SysBaseInfo, 
        sizeof(SysBaseInfo), 
        NULL
        ); 

      if (status == NO_ERROR) 
      { 
        ar_sppi = new SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[
          //SysBaseInfo.NumberOfProcessors
          SysBaseInfo.bKeNumberProcessors ] ;
        ar_sppiAfter = new SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION[
          //SysBaseInfo.NumberOfProcessors
          SysBaseInfo.bKeNumberProcessors ] ;
      }
    }
    //else
    //  throw std::exception ;
  }

  CPUcoreUsageGetterNtQuerySystemInformation::
    ~CPUcoreUsageGetterNtQuerySystemInformation()
  {
    if( ar_sppi )
      delete [] ar_sppi ;
    delete [] ar_sppiAfter ;
  }

  BYTE CPUcoreUsageGetterNtQuerySystemInformation::GetSysProcPerfInfo(
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * ar_sysprocperfinfo ,
    double & dPassedNanos)
  {
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * ar_sysprocperfinfoBefore ;
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * ar_sysprocperfinfoAfter ;

    status = NtQuerySystemInformation(
      //SystemTimeInformation, 
      //SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ,
      SystemProcessorPerformanceInformation ,
      //&SysTimeInfo, 
      ar_sppi ,
      sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * 
      //SysBaseInfo.NumberOfProcessors,
      SysBaseInfo.bKeNumberProcessors , 
      NULL
      );    

    //http://www.netperf.org/svn/netperf2/trunk/src/netcpu_ntperf.c:
    //"KernelTime needs to be fixed-up; it includes both idle &
    // true kernel time "
    ar_sppi[0].KernelTime.QuadPart -= ar_sppi[0].IdleTime.QuadPart ;
    WORD wMillis = 500;
    ::Sleep(wMillis);
    dPassedNanos = (double) wMillis * 1000000 ;
    //SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ar_sppi[2] ;
    // get system time 
    status = NtQuerySystemInformation(
      //SystemTimeInformation, 
      //SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ,
      SystemProcessorPerformanceInformation ,
      //&SysTimeInfo, 
      ar_sppiAfter ,
      sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * 
      //SysBaseInfo.NumberOfProcessors,
      SysBaseInfo.bKeNumberProcessors , 
      NULL
      );
    //http://www.netperf.org/svn/netperf2/trunk/src/netcpu_ntperf.c:
    //"KernelTime needs to be fixed-up; it includes both idle &
    // true kernel time "
    ar_sppiAfter[0].KernelTime.QuadPart -= ar_sppiAfter[0].IdleTime.QuadPart ;

    ar_sysprocperfinfo[0].IdleTime.QuadPart = 
      ar_sppiAfter[0].IdleTime.QuadPart - ar_sppi[0].IdleTime.QuadPart ;
    ar_sysprocperfinfo[0].KernelTime.QuadPart = 
      ar_sppiAfter[0].KernelTime.QuadPart - ar_sppi[0].KernelTime.QuadPart ;
    ar_sysprocperfinfo[0].UserTime.QuadPart = 
      ar_sppiAfter[0].UserTime.QuadPart - ar_sppi[0].UserTime.QuadPart ;

    LARGE_INTEGER liKernelMinusIdle ;
    liKernelMinusIdle.QuadPart =
      ar_sysprocperfinfo[0].KernelTime.QuadPart
      - ar_sysprocperfinfo[0].IdleTime.QuadPart ;

//    LARGE_INTEGER liOverall ;
//    liOverall.QuadPart =
//      //liKernelMinusIdle.QuadPart +
//      ar_sysprocperfinfo[0].KernelTime.QuadPart +
//      ar_sysprocperfinfo[0].IdleTime.QuadPart +
//      ar_sysprocperfinfo[0].UserTime.QuadPart ;
//
//    "idle:%f kernel:%f kernel-idle:%f user:%f dPassedNanos:%f",
//      (double) ar_sysprocperfinfo[0].IdleTime.QuadPart /
//        (double) liOverall.QuadPart ,
//      (double) ar_sysprocperfinfo[0].KernelTime.QuadPart /
//        (double) liOverall.QuadPart ,
//      (double) liKernelMinusIdle.QuadPart /
//        (double) liOverall.QuadPart ,
//      (double) ar_sysprocperfinfo[0].UserTime.QuadPart /
//        (double) liOverall.QuadPart ,
//        dPassedNanos
//      )
    return 0 ;
  }

  // this functions return the CPU usage of one second using 
  // undocumented Windows NT API's 
  // this code will not run on Win 9x 
  //double GetCPUusages(float arf[2]) 
  BYTE CPUcoreUsageGetterNtQuerySystemInformation::GetPercentalUsageForAllCores(
    //Array for storing CPU core usge in %
    float arf[] 
    )
  { 
    // get system time 
    status = NtQuerySystemInformation(
      //SystemTimeInformation, 
      //SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ,
      SystemProcessorPerformanceInformation ,
      //&SysTimeInfo, 
      ar_sppi ,
      sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * 
      //SysBaseInfo.NumberOfProcessors ,
      SysBaseInfo.bKeNumberProcessors, 
      NULL
      ); 
    //arf[0] = 
    //  //The IdleTime element contains the amount of time that the 
    //  //system has been idle, in 1/100ths of a nanosecond.
    //  ar_sppi[0].IdleTime ;

    WORD wMillis = 100 ;
    // wait milliseconds
    ::Sleep(//1000
      wMillis ); 

    //SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ar_sppi[2] ;
    // get system time 
    status = NtQuerySystemInformation(
      //SystemTimeInformation, 
      //SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ,
      SystemProcessorPerformanceInformation ,
      //&SysTimeInfo, 
      ar_sppiAfter ,
      sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * 
      //SysBaseInfo.NumberOfProcessors,
      SysBaseInfo.bKeNumberProcessors , 
      NULL
      ); 
    //__int64 i , k; 
    LARGE_INTEGER liDiff ;
    LARGE_INTEGER liCurrentUserPlusKernelTime ;
    LARGE_INTEGER liPreviousUserPlusKernelTime ;
    double dPassedNanos = (double) wMillis * 1000000 ;
    for(BYTE byCPUindex = 0 ; byCPUindex < //SysBaseInfo.NumberOfProcessors ;
      SysBaseInfo.bKeNumberProcessors ;
      ++ byCPUindex )
    {
      //i = ar_sppiAfter[byCPUindex].IdleTime ;
      //k = ar_sppi[byCPUindex].IdleTime ;
        //arf[byCPUindex] = 
        liDiff.QuadPart =
          ////The IdleTime element contains the amount of time that the 
          ////system has been idle, in 1/100ths of a nanosecond.
          ( ar_sppiAfter[byCPUindex].IdleTime.QuadPart - 
          ar_sppi[byCPUindex].IdleTime.QuadPart ) ;
          //( i - k ) 
      arf[byCPUindex] = //100.0 - 
        ( 
        //( 
        ////ticks/ second "the ticks really represent 100 nanoseconds"
        //10000.0 * (double) wMillis ) -
        (double)liDiff.QuadPart 
         / //( 
        //100 billion (milliarde)
        //ticks/ second "the ticks really represent 100 nanoseconds"
        ( 10000.0 * (double) wMillis )

        //* 100.0
        )
        ;
      //http://www.masm32.com/board/index.php?PHPSESSID=a67682b33592d144df2d28b99b84b4b4&action=printpage;topic=11854.0:
      //BYTE cpuusage = (BYTE) 
      // ( 100 - 
      //  (
      //   (
      //    (ar_sppiAfter[byCPUindex].IdleTime.QuadPart - 
      //    ar_sppi[byCPUindex].IdleTime.QuadPart) 
      //    * 100
      //   ) /
      //   ( (ar_sppiAfter[byCPUindex].KernelTime.QuadPart + 
      //      ar_sppiAfter[byCPUindex].UserTime.QuadPart) 
      //     - 
      //     (ar_sppi[byCPUindex].KernelTime.QuadPart + 
      //     ar_sppi[byCPUindex].UserTime.QuadPart)
      //   )
      //  )
      // );
      //arf[byCPUindex] = cpuusage ;
      //"
      //liCurrentUserPlusKernelTime.QuadPart = 
      //  ar_sppiAfter[byCPUindex].KernelTime.QuadPart + 
      //  ar_sppiAfter[byCPUindex].UserTime.QuadPart ;
      //liPreviousUserPlusKernelTime.QuadPart = 
      //  ar_sppi[byCPUindex].KernelTime.QuadPart + 
      //  ar_sppi[byCPUindex].UserTime.QuadPart ;
      //arf[byCPUindex] = 100.0 - 
      // (
      //  ( (double) liDiff.QuadPart * 100.0 ) / 
      //  (double) ( liCurrentUserPlusKernelTime.QuadPart - 
      //  liPreviousUserPlusKernelTime.QuadPart ) 
      // ) ;
    }
  //http://www.netperf.org/svn/netperf4/trunk/src/netsysstats_windows.c:
  //"      KernelTime needs to be fixed-up; it includes both idle &
  // true kernel time Note that kernel time also includes DpcTime &
  // InterruptTime. We remove the interrupt time and the idle time but
  // leave the DpcTime.

  // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/ntquerysysteminformation.asp
  // asserts that each CPU tick represents 1/100ths of a nanosecond
  // (well, at least that is the claim as this is being written.
  // However, others have said, and empirical evidence suggests that
  // the ticks really represent 100 nanoseconds (eg 10 MHz).  So, for
  // our calibration value we take the wall-clock time and multiply it
  // by 10MHz to use as the calibration value.  raj 2006-04-12 "

    if (status == NO_ERROR) 
    {
      // get system idle time 
      status = NtQuerySystemInformation(
        SystemPerformanceInformation, 
        //SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ,
        &SysPerfInfo, 
        sizeof(SysPerfInfo), 
        NULL
        ); 

      if (status == NO_ERROR) 
      { 
        LARGE_INTEGER liOldIdleTime = SysPerfInfo.liIdleTime; 
        LARGE_INTEGER liOldSystemTime = SysTimeInfo.liKeSystemTime; 

        // wait one second 
        ::Sleep(500); 

        // get new System time 
        status = NtQuerySystemInformation(
          //SystemTimeInformation, 
          SystemTimeOfDayInformation ,
          &SysTimeInfo, 
          sizeof(SysTimeInfo), NULL
          ); 

        if (status == NO_ERROR) 
        { 
          // get new system idle time 

          status = NtQuerySystemInformation(
            SystemPerformanceInformation, 
            &SysPerfInfo, 
            sizeof(SysPerfInfo), 
            NULL
            ); 

          if (status == NO_ERROR) 
          { 
            // current value = new value - old value 
            dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - 
              Li2Double(liOldIdleTime); 
            dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - 
              Li2Double(liOldSystemTime); 

            // currentCpuIdle = IdleTime / SystemTime; 
            dbIdleTime = dbIdleTime / dbSystemTime; 

            // currentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors 
            dbIdleTime = 100.0 - dbIdleTime * 100.0 / 
              (double)SysBaseInfo.bKeNumberProcessors + 0.5; 
            //arf[0] = dbIdleTime ;
          } 
        } 
      } 
    } 
    return //dbIdleTime; 
      0 ;
  }

  BYTE CPUcoreUsageGetterNtQuerySystemInformation::Init(){
    return 0;
  }
