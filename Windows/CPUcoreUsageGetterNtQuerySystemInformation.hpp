#pragma once 

//for Winternl.h:
//#define _WIN32_WINNT 0x0500
//#include <Winternl.h>
#include "Controller/ICPUcoreUsageGetter.hpp"
#include <exception>
#include <Windows.h> //LARGE_INTEGER
//#include <Windows.h> //GetProcAddress()

//see http://www.netperf.org/svn/netperf2/trunk/src/netcpu_ntperf.c
// using undocumented functions and structures 

//#define SystemBasicInformation        0 
//#define SystemPerformanceInformation    2 
//#define SystemTimeInformation        3 

//convert Large integer to double
#define Li2Double(x)    ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart)) 

typedef struct 
{ 
    DWORD    dwUnknown1; 
    ULONG    uKeMaximumIncrement; 
    ULONG    uPageSize; 
    ULONG    uMmNumberOfPhysicalPages; 
    ULONG    uMmLowestPhysicalPage; 
    ULONG    UMmHighestPhysicalPage; 
    ULONG    uAllocationGranularity; 
    PVOID    pLowestUserAddress; 
    PVOID    pMmHighestUserAddress; 
    ULONG    uKeActiveProcessors; 
    BYTE    bKeNumberProcessors; 
    BYTE    bUnknown2; 
    WORD    bUnknown3; 
} SYSTEM_BASIC_INFORMATION; 

////from <Winternl.h>
//typedef struct _SYSTEM_BASIC_INFORMATION {
//    BYTE Reserved1[24];
//    PVOID Reserved2[4];
//    CCHAR NumberOfProcessors;
//} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

//from <Winternl.h>
typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER Reserved1[2];
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef struct 
{ 
    LARGE_INTEGER    liIdleTime; 
    DWORD        dwSpare[76]; 
} SYSTEM_PERFORMANCE_INFORMATION; 
//typedef struct _SYSTEM_PERFORMANCE_INFORMATION {
//    BYTE Reserved1[312];
//} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

typedef struct 
{ 
    LARGE_INTEGER    liKeBootTime; 
    LARGE_INTEGER    liKeSystemTime; 
    LARGE_INTEGER    liExpTimeZoneBias; 
    ULONG            uCurrentTimeZoneID; 
    DWORD            dwReserved; 
} SYSTEM_TIME_INFORMATION; 

// NtQuerySystemInformation 
// The function copies the system information of the specified type into a buffer 
// NTSYSAPI 
// NTSTATUS 
// NTAPI 
// NtQuerySystemInformation( 
//        IN UINT SystemInformationClass,        // information type 
//        OUT PVOID SystemInformation,        // pointer to buffer 
//        IN ULONG SystemInformationLength,    // buffer size in bytes 
//        OUT PULONG ReturnLength OPTIONAL    // pointer to a 32 bit variable that 
//                                            // receives the number of bytes written 
//                                            // to the buffer 
// ); 

//from <Winternl.h>
typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemProcessInformation = 5,
    SystemProcessorPerformanceInformation = 8,
    SystemInterruptInformation = 23,
    SystemExceptionInformation = 33,
    SystemRegistryQuotaInformation = 37,
    SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;

typedef LONG (WINAPI *PROCNTQSI) (UINT, PVOID, ULONG, PULONG); 

class CPUcoreUsageGetterNtQuerySystemInformation
  :public ICPUcoreUsageGetter
{
public:
  PROCNTQSI                        NtQuerySystemInformation; 
    SYSTEM_BASIC_INFORMATION        SysBaseInfo; 
    SYSTEM_TIME_INFORMATION            SysTimeInfo; 
    SYSTEM_PERFORMANCE_INFORMATION    SysPerfInfo; 
    LONG                            status; 
    //LARGE_INTEGER                    liOldIdleTime = {0, 0}; 
    //LARGE_INTEGER                    liOldSystemTime = {0, 0}; 
    double                            dbIdleTime; 
    double                            dbSystemTime; 
  SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * ar_sppi ;
  SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * ar_sppiAfter ;

  CPUcoreUsageGetterNtQuerySystemInformation() ;
  ~CPUcoreUsageGetterNtQuerySystemInformation() ;

  BYTE Init() ;

  BYTE GetSysProcPerfInfo(
    SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION * ar_sysprocperfinfo ,
    double & dPassedNanos
    ) ;

  BYTE GetPercentalUsageForAllCores(
    //Array for storing CPU core usge in %
    float arf[] 
    ) ;
}; //end class
