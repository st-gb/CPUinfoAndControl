#pragma once
#define MAX_CPU_SUPPORT              32

#define CPU_USAGE_AVERAGE			 -1
#define CPU_USAGE_MAX				 -2

#define SystemPerformanceInformation 2
#define SystemTimeInformation        3
#define SystemProcessorTimes         8

//typedef struct
//{
//    LARGE_INTEGER   IdleTime;
//    DWORD           Spare[76];
//} SYSTEM_PERFORMANCE_INFORMATION;
//
//typedef struct
//{
//    LARGE_INTEGER BootTime;
//    LARGE_INTEGER SystemTime;
//    LARGE_INTEGER ExpTimeZoneBias;
//    ULONG         CurrentTimeZoneId;
//    DWORD         Reserved;
//} SYSTEM_TIME_INFORMATION;
//
//typedef struct
//{
//	LARGE_INTEGER  IdleTime;
//	LARGE_INTEGER  KernelTime;
//	LARGE_INTEGER  UserTime;
//	LARGE_INTEGER  DpcTime;
//	LARGE_INTEGER  InterruptTime;
//	ULONG    InterruptCount;
//}SYSTEM_PROCESSOR_TIMES;
//
//typedef struct
//{
//	int Average;
//	int Max;
//	int Unit[MAX_CPU_SUPPORT];
//}CPU_USAGE;

#include "CpuUsage.h" //Crystal CPU usage getter
#ifndef _MSC_VER //->e.g also for cygwin
  #define WINAPI __stdcall
  #include <winnt.h> //LARGE_INTEGER
  #include <winbase.h> //GetModuleHandle()
  #include <Windows.h>
#endif

typedef LONG (WINAPI *__NTQSI__)(UINT,PVOID,ULONG,PULONG);

class CPUcoreUsageNTQSI
{
  //static 
  __NTQSI__ NTQSI ;
  //static 
  HMODULE m_hModule ;
  //static 
  DWORD m_dwNumberOfProcessors ;
  //static 
  SYSTEM_PROCESSOR_TIMES * p_system_processor_times ;
  LARGE_INTEGER      m_liOldIdleTime   ; //{0, 0};
  LARGE_INTEGER      m_liOldSystemTime ; //{0, 0};
	LARGE_INTEGER		   m_liOldIdleTimeUnit[MAX_CPU_SUPPORT];
  LARGE_INTEGER		   m_liOldKernelTimeUnit[MAX_CPU_SUPPORT];
  LARGE_INTEGER		   m_liOldUserTimeUnit[MAX_CPU_SUPPORT];

  //static 
  HKEY hKey, hDummy;
  //static 
  DWORD dummy, dummysize ;
  //static 
  DWORD m_dwCpuUsage, size ;

public:
  //static 
  void CleanupCpuUsageNT()
  {
	  if( p_system_processor_times != NULL )
    {
		  GlobalFree(p_system_processor_times);
		  p_system_processor_times = NULL;
	  }
  }
  //static 
  void InitCpuUsageNT()
  {
	  p_system_processor_times = NULL;
	  SYSTEM_INFO si;
	  GetSystemInfo(&si);
	  m_dwNumberOfProcessors = si.dwNumberOfProcessors;
	  m_hModule = GetModuleHandle("ntdll");
	  NTQSI = (__NTQSI__) GetProcAddress(
				  m_hModule,
				  "NtQuerySystemInformation"
				  );

	  p_system_processor_times = (SYSTEM_PROCESSOR_TIMES *)GlobalAlloc(GPTR,
		    sizeof(SYSTEM_PROCESSOR_TIMES) * m_dwNumberOfProcessors);
  }

  ~CPUcoreUsageNTQSI()
  {
    CleanupCpuUsageNT() ;
  }

  CPUcoreUsageNTQSI()
  {
    NTQSI = NULL;
    m_hModule = NULL;
    m_dwNumberOfProcessors = 1;
    m_liOldIdleTime.QuadPart = 0 ;//   = {0, 0};
    m_liOldSystemTime.QuadPart = 0 ; //{0, 0};

    dummysize = sizeof(DWORD);
    size = sizeof(DWORD);
   // FlagNT = IsNT();
	  //if(FlagNT)
   // {
		  InitCpuUsageNT();
	  //}
   // else
   // {
		 // FlagWinTopVxd = InitCpuUsageWinTopVxd();
		 // if(FlagWinTopVxd){
		 // }else{
			//  InitCpuUsage9x();
		 // }
	  //}
  }

  //static 
  //Does only work if the time interval is > ca. 0.5 seconds. 
  //Else the CPU usage is often reported as 0%.
  int GetCpuUsageNT(int type, CPU_USAGE * p_cpu_usage)
  {
	  if( NTQSI == NULL)
    {
		  return -1;
	  }

	  SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	  SYSTEM_TIME_INFORMATION        SysTimeInfo;
      double                         dIdleTime;
      double                         dSystemTime;
      LONG                         status;
	  double						   ardIdleTimeUnit[MAX_CPU_SUPPORT];

	  DWORD dwCPUcoreIndex;

	  CPU_USAGE cpu_usage;
	  int nCpuUsage = 0;
	  int arnCpuUsageUnit[MAX_CPU_SUPPORT];

	  status = NTQSI( SystemTimeInformation,
					  & SysTimeInfo,
					  sizeof(SysTimeInfo),
					  NULL);
	  if ( status != NO_ERROR)
    {
		  return -1;
	  }

	  status = NTQSI( SystemPerformanceInformation,
					  & SysPerfInfo,
					  sizeof(SysPerfInfo),
					  NULL);
	  if ( status != NO_ERROR)
    {
		  return -1;
	  }

  // by Unit //
	  status = NTQSI(	SystemProcessorTimes,
					  p_system_processor_times,
					  sizeof(SYSTEM_PROCESSOR_TIMES) * m_dwNumberOfProcessors,
					  NULL);
	  if ( status != NO_ERROR)
    {
		  return -1;
	  }

    //If the idle time was got from a revious call (else no difference are
    // possible.
	  if ( m_liOldIdleTime.QuadPart != 0 )
	  {
      //overall (all CPUs) info--begin
		  dIdleTime = (double)( SysPerfInfo.IdleTime.QuadPart -
        m_liOldIdleTime.QuadPart );
		  dSystemTime = (double)(SysTimeInfo.SystemTime.QuadPart -
        m_liOldSystemTime.QuadPart);
		  dIdleTime = dIdleTime / dSystemTime;
		  nCpuUsage = (int)(100.0 - dIdleTime * 100.0 / m_dwNumberOfProcessors + 0.5);
		  if(nCpuUsage >= 100)
      {
        nCpuUsage = 100;
      }
      //overall (all CPUs) info--end

      LARGE_INTEGER liIdleTimeDiff ;
      LARGE_INTEGER liKernelTimeDiff ;
      LARGE_INTEGER liUserTimeDiff ;
      //per-CPU-core info--begin
		  for( dwCPUcoreIndex = 0; dwCPUcoreIndex < m_dwNumberOfProcessors;
        dwCPUcoreIndex ++ )
      {
          //"latest idle time" minus "previous idle time".
			  ardIdleTimeUnit[dwCPUcoreIndex] = (double)(
          p_system_processor_times[dwCPUcoreIndex].IdleTime.QuadPart -
          m_liOldIdleTimeUnit[dwCPUcoreIndex].QuadPart);
        liIdleTimeDiff.QuadPart = p_system_processor_times[dwCPUcoreIndex].IdleTime.
          QuadPart - m_liOldIdleTimeUnit[dwCPUcoreIndex].QuadPart ;
        liKernelTimeDiff.QuadPart = 
          p_system_processor_times[dwCPUcoreIndex].KernelTime.QuadPart - 
          m_liOldKernelTimeUnit[dwCPUcoreIndex].QuadPart ;
        m_liOldKernelTimeUnit[dwCPUcoreIndex].QuadPart = 
          p_system_processor_times[dwCPUcoreIndex].KernelTime.QuadPart ;

        liUserTimeDiff.QuadPart = p_system_processor_times[dwCPUcoreIndex].UserTime.
          QuadPart - m_liOldUserTimeUnit[dwCPUcoreIndex].QuadPart ;
        m_liOldUserTimeUnit[dwCPUcoreIndex].QuadPart = 
          p_system_processor_times[dwCPUcoreIndex].UserTime.QuadPart ;

        //difference from "latest idle time minus previous idle time" / systime.
			  ardIdleTimeUnit[dwCPUcoreIndex] =
          //ardIdleTimeUnit[dwCPUcoreIndex] 
          (double)liIdleTimeDiff.QuadPart / dSystemTime ;
			  arnCpuUsageUnit[dwCPUcoreIndex] = (int)(100.0 -
          //"difference from latest idle time minus previous idle time
          //   / systime ".
          ardIdleTimeUnit[dwCPUcoreIndex]
          * 100.0 + 0.5);
#ifdef _DEBUG
          if( arnCpuUsageUnit[dwCPUcoreIndex] == 0 )
          {
            int i = 0 ;
          }
#endif
			  if(arnCpuUsageUnit[dwCPUcoreIndex] >= 100)
        {
          arnCpuUsageUnit[dwCPUcoreIndex] = 100;
        }
		  }
      //per-CPU-core info--end
	  }
  	
	  m_liOldIdleTime = SysPerfInfo.IdleTime;
	  m_liOldSystemTime = SysTimeInfo.SystemTime;

    //Copy to old times for the next call to this function.
	  for( dwCPUcoreIndex = 0; dwCPUcoreIndex < m_dwNumberOfProcessors;
      dwCPUcoreIndex ++)
    {
		  m_liOldIdleTimeUnit[dwCPUcoreIndex].QuadPart =
        p_system_processor_times[dwCPUcoreIndex].IdleTime.QuadPart ;
	  }

	  cpu_usage.Max = 0;
	  cpu_usage.Average = nCpuUsage;
	  for( dwCPUcoreIndex = 0; dwCPUcoreIndex < m_dwNumberOfProcessors;
      dwCPUcoreIndex++ )
    {
		  cpu_usage.Unit[dwCPUcoreIndex] = arnCpuUsageUnit[dwCPUcoreIndex];
		  if(arnCpuUsageUnit[dwCPUcoreIndex] > cpu_usage.Max)
      {
			  cpu_usage.Max = arnCpuUsageUnit[dwCPUcoreIndex];
		  }
	  }

	  // Fill CpuUsage ( copy p_cpu_usage member to the passed parameter)  ;
    // Wouldn't memcpy be faster?!
	  if( p_cpu_usage != NULL )
    {
		  p_cpu_usage->Average = cpu_usage.Average;
		  p_cpu_usage->Max = cpu_usage.Max;
		  for( dwCPUcoreIndex = 0; dwCPUcoreIndex < m_dwNumberOfProcessors;
        dwCPUcoreIndex ++ )
      {
			  p_cpu_usage->Unit[dwCPUcoreIndex] = cpu_usage.Unit[dwCPUcoreIndex];
		  }
	  }

	  if(type == CPU_USAGE_MAX)
    {
		  return cpu_usage.Max;
	  }
    else if(type == CPU_USAGE_AVERAGE)
    {
		  return cpu_usage.Average;
	  }
    else if( type < (int) m_dwNumberOfProcessors )
    {
      if( cpu_usage.Unit[type] == 0 )
      {
        int i = 0 ;
        //TRACE("core %u: cpu_usage.Unit[type]:%d\n",type,cpu_usage.Unit[type]) ;
      }
		  return cpu_usage.Unit[type];
	  }
    else
    {
		  return nCpuUsage;
	  }
  }

  int GetCpuUsage(int type, CPU_USAGE* usage)
  {
    return GetCpuUsageNT(type, usage) ;
  }
};
