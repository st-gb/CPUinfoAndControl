#pragma once

#include <Controller/I_CPUaccess.hpp>

class UserInterface ;

class WinRing0_1_3LoadTimeDynLinked
  : public I_CPUaccess
{
public:
  WinRing0_1_3LoadTimeDynLinked( UserInterface * p_ui );
  //~WinRing0_1_3LoadTimeDynLinked(void);
    BOOL CpuidEx(
    DWORD index,
    PDWORD eax,
    PDWORD ebx,
    PDWORD ecx,
    PDWORD edx,
    DWORD_PTR affinityMask
  );
  ~WinRing0_1_3LoadTimeDynLinked() ;
  BYTE GetNumberOfCPUCores() ;
  inline BOOL 
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI 
    RdpmcEx(
    DWORD dwIndex,		// MSR index
    PDWORD p_dweax,			// bit  0-31
    PDWORD p_dwedx,			// bit 32-63
    DWORD_PTR affinityMask	// Thread Affinity Mask
    ) ;
  BOOL // TRUE: success, FALSE: failure
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI 
  RdmsrEx(
	  DWORD index,		// MSR index
	  PDWORD eax,			// bit  0-31
	  PDWORD edx,			// bit 32-63
	  DWORD_PTR affinityMask	// Thread Affinity Mask
  );
  void Init(UserInterface * pui) ;
  BOOL ReadPciConfigDwordEx(
    DWORD dwPCIaddress
    , DWORD dwRegAddress
    , PDWORD p_dwValue
    );
  void SetUserInterface(UserInterface * pui) ;
  void Sleep(WORD wMillis) //{ ::Sleep(wMillis) ; }
    ;

  BOOL 
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI 
    WritePciConfigDwordEx (
    DWORD pciAddress, 
    DWORD regAddress, 
    DWORD value
    );

  BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI 
    WrmsrEx(
	  DWORD index,		// MSR index
	  DWORD eax,			// bit  0-31
	  DWORD edx,			// bit 32-63
	  DWORD affinityMask	// Thread Affinity Mask
  );
};
