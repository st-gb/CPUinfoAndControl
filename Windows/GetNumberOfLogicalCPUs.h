#pragma once

#include <windef.h> //for BYTE etc.
//from MSVCs Windows v7.0a winnt.h:

//other compilers than Microsoft's usually do not know the follwing.
#ifndef _MSC_VER 
  typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
      RelationProcessorCore,
      RelationNumaNode,
      RelationCache,
      RelationProcessorPackage,
      RelationGroup,
      RelationAll = 0xffff
  } LOGICAL_PROCESSOR_RELATIONSHIP;

  typedef enum _PROCESSOR_CACHE_TYPE {
      CacheUnified,
      CacheInstruction,
      CacheData,
      CacheTrace
  } PROCESSOR_CACHE_TYPE;

  typedef struct _CACHE_DESCRIPTOR {
      BYTE   Level;
      BYTE   Associativity;
      WORD   LineSize;
      DWORD  Size;
      PROCESSOR_CACHE_TYPE Type;
  } CACHE_DESCRIPTOR, *PCACHE_DESCRIPTOR;

  typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
      ULONG_PTR   ProcessorMask;
      LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
      union {
          struct {
              BYTE  Flags;
          } ProcessorCore;
          struct {
              DWORD NodeNumber;
          } NumaNode;
          CACHE_DESCRIPTOR Cache;
          ULONGLONG  Reserved[2];
      } DUMMYUNIONNAME;
  } SYSTEM_LOGICAL_PROCESSOR_INFORMATION, *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;
#endif //#ifndef _MSC_VER

WORD GetNumLogicalCPUs() ;
