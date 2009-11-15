#pragma once

//This class works (but only when called directly), do not modify ! .

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

typedef LONG (WINAPI *__NTQSI__)(UINT,PVOID,ULONG,PULONG);

class CPUcoreUsageNTQSI
{
  //static 
  __NTQSI__ NTQSI ;
  //static 
  HMODULE hModule ;
  //static 
  DWORD NumberOfProcessors ;
  //static 
  SYSTEM_PROCESSOR_TIMES *spt;

  //static 
  HKEY hKey, hDummy;
  //static 
  DWORD dummy, dummysize ;
  //static 
  DWORD CpuUsage, size ;

public:
  //static 
  void CleanupCpuUsageNT()
  {
	  if(spt != NULL){
		  GlobalFree(spt);
		  spt = NULL;
	    }
  }
  //static 
  void InitCpuUsageNT()
  {
	  spt = NULL;
	  SYSTEM_INFO si;
	  GetSystemInfo(&si);
	  NumberOfProcessors = si.dwNumberOfProcessors;
	  hModule = GetModuleHandle("ntdll");
	  NTQSI = (__NTQSI__)GetProcAddress(
				  hModule,
				  "NtQuerySystemInformation"
				  );

	  spt = (SYSTEM_PROCESSOR_TIMES *)GlobalAlloc(GPTR,
		    sizeof(SYSTEM_PROCESSOR_TIMES) * NumberOfProcessors);
  }

  ~CPUcoreUsageNTQSI()
  {
    CleanupCpuUsageNT() ;
  }

  CPUcoreUsageNTQSI()
  {
    NTQSI = NULL;
    hModule = NULL;
    NumberOfProcessors = 1;

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
  int GetCpuUsageNT(int type, CPU_USAGE* usage)
  {
	  if( NTQSI == NULL){
		  return -1;
	  }

	  SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	  SYSTEM_TIME_INFORMATION        SysTimeInfo;
      double                         IdleTime;
      double                         SystemTime;
      LONG                           status;
      static LARGE_INTEGER           OldIdleTime   = {0, 0};
      static LARGE_INTEGER           OldSystemTime = {0, 0};
	  static LARGE_INTEGER		   OldIdleTimeUnit[MAX_CPU_SUPPORT];
	  double						   IdleTimeUnit[MAX_CPU_SUPPORT];

	  DWORD i;

	  CPU_USAGE cu;
	  int CpuUsage = 0;
	  int CpuUsageUnit[MAX_CPU_SUPPORT];

	  status = NTQSI( SystemTimeInformation,
					  &SysTimeInfo,
					  sizeof(SysTimeInfo),
					  NULL);
	  if (status != NO_ERROR){
		  return -1;
	  }

	  status = NTQSI( SystemPerformanceInformation,
					  &SysPerfInfo,
					  sizeof(SysPerfInfo),
					  NULL);
	  if (status != NO_ERROR){
		  return -1;
	  }

  // by Unit //
	  status = NTQSI(	SystemProcessorTimes,
					  spt,
					  sizeof(SYSTEM_PROCESSOR_TIMES) * NumberOfProcessors,
					  NULL);
	  if (status != NO_ERROR){
		  return -1;
	  }
  	
	  if (OldIdleTime.QuadPart != 0)
	  {
		  IdleTime = (double)(SysPerfInfo.IdleTime.QuadPart - OldIdleTime.QuadPart);
		  SystemTime = (double)(SysTimeInfo.SystemTime.QuadPart - OldSystemTime.QuadPart);
		  IdleTime = IdleTime / SystemTime;
		  CpuUsage = (int)(100.0 - IdleTime * 100.0 / NumberOfProcessors + 0.5);
		  if(CpuUsage >= 100){CpuUsage = 100;}

		  for(i = 0; i < NumberOfProcessors; i++){
			  IdleTimeUnit[i] = (double)(spt[i].IdleTime.QuadPart - OldIdleTimeUnit[i].QuadPart);
			  IdleTimeUnit[i] = IdleTimeUnit[i] / SystemTime;
			  CpuUsageUnit[i] = (int)(100.0 - IdleTimeUnit[i] * 100.0 + 0.5);
			  if(CpuUsageUnit[i] >= 100){CpuUsageUnit[i] = 100;}
		  }
	  }
  	
	  OldIdleTime = SysPerfInfo.IdleTime;
	  OldSystemTime = SysTimeInfo.SystemTime;
  	
	  for(i = 0; i < NumberOfProcessors; i++){
		  OldIdleTimeUnit[i].QuadPart = spt[i].IdleTime.QuadPart;
	  }

	  cu.Max = 0;
	  cu.Average = CpuUsage;
	  for(i = 0; i < NumberOfProcessors; i++){
		  cu.Unit[i] = CpuUsageUnit[i];
		  if(CpuUsageUnit[i] > cu.Max){
			  cu.Max = CpuUsageUnit[i];
		  }
	  }

	  // Fill CpuUsage;
	  if(usage != NULL){
		  usage->Average = cu.Average;
		  usage->Max = cu.Max;
		  for(i = 0; i < NumberOfProcessors; i++){
			  usage->Unit[i] = cu.Unit[i];
		  }
	  }

	  if(type == CPU_USAGE_MAX){
		  return cu.Max;
	  }else if(type == CPU_USAGE_AVERAGE){
		  return cu.Average;
	  }else if(type < (int)NumberOfProcessors){
		  return cu.Unit[type];
	  }else{
		  return CpuUsage;
	  }
  }

  int GetCpuUsage(int type, CPU_USAGE* usage)
  {
    return GetCpuUsageNT(type, usage) ;
  }
};
