/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   MSRdeviceFile.h
 * Author: sgebauer
 *
 * Created on 15. November 2009, 18:38
 */

#ifndef _MSRDEVICEFILE_H
#define	_MSRDEVICEFILE_H

#include <Controller/I_CPUaccess.hpp> //Base class I_CPUaccess
//#include <Windows_compatible_typedefs.h>

#include <fstream>
#include <sched.h> //CPU_ZERO(...), ...

//Exception class. If caught: don't continue (with assigning function
//pointers to DLL functions etc.).
class DLLnotLoadedException:
    public CPUaccessException
{
public:
  DLLnotLoadedException(char * p_ch)
    : CPUaccessException(p_ch)
  {
  }
};
//class MSRdeviceFile {
//public:
//  MSRdeviceFile();
//  MSRdeviceFile(const MSRdeviceFile& orig);
//  virtual ~MSRdeviceFile();
//private:
//};

//Forward declarations (faster than #include)
class Model ;
class UserInterface ;

//This class calls the winring0 functions by function pointers.
class MSRdeviceFile
  //: public ControllerBase
  : public //ISpecificController
  I_CPUaccess
  //: public wxWidgets::CalculationThread
{
private:
  //UserInterface * mp_userinterface ;
  //Using std::fstream seems to be impossible with device files
  //(there were program crashs?!)
//  std::fstream * m_arfstreamMSR ;
  BYTE m_byNumberOfLogicalCPUcores ;
  int * m_arnFileHandle ;
  //Declare as member variables (and use within the methods)
  //to not create a variable on stack each time (faster).
  BYTE m_byCoreID ;
  ssize_t m_ssize_t ;
  unsigned long long m_ullMSRvalue;
public:
  MSRdeviceFile(UserInterface * pui) ;
  MSRdeviceFile(
    UserInterface * pui ,
    BYTE byNumberOfLogicalCPUcores ,
    Model & r_model
    ) ;
  MSRdeviceFile() ;
  ~MSRdeviceFile() ;
  //const char *
  std::string getMSRFile(DWORD_PTR
    //Only 1 bit should be set in this value
    dwAffinityMask) ;
  static std::string getMSRdeviceFilePath( BYTE byCPUcoreID) ;

  void UIconfirm(const std::string & str) ;
  BOOL CpuidEx(
    DWORD index,
    PDWORD eax,
    PDWORD ebx,
    PDWORD ecx,
    PDWORD edx,
    DWORD_PTR affinityMask
  );
  //BYTE GetNumberOfCPUCores() ;
  inline BOOL 
  //WINAPI
  RdpmcEx(
    DWORD dwIndex,		// MSR index
    PDWORD p_dweax,			// bit  0-31
    PDWORD p_dwedx,			// bit 32-63
    DWORD_PTR affinityMask	// Thread Affinity Mask
    ) ;
  BOOL // TRUE: success, FALSE: failure
  //WINAPI
  RdmsrEx(
	  DWORD index,		// MSR index
	  PDWORD eax,			// bit  0-31
	  PDWORD edx,			// bit 32-63
	  DWORD_PTR affinityMask	// Thread Affinity Mask
  );

  //overrides base class' implem.
  BYTE SetThreadAffinityMask(DWORD dwThreadAffinityMask)
  {
    //http://www.kernel.org/doc/man-pages/online/pages/man3/pthread_setaffinity_np.3.html:

    //http://www.kernel.org/doc/man-pages/online/pages/man3/CPU_SET.3.html:
    cpu_set_t cpu_set;

    CPU_ZERO(//cpu_set_t *set
      & cpu_set);
    for( BYTE byCPUcoreIndex = 0; byCPUcoreIndex < 32; ++ byCPUcoreIndex)
    {
      if( dwThreadAffinityMask & ( 1 << byCPUcoreIndex) )
        CPU_SET( byCPUcoreIndex, & cpu_set);        //Add CPU cpu to set.
    }
    //http://www.kernel.org/doc/man-pages/online/pages/man2/sched_setaffinity.2.html:
    int nRetVal = ::sched_setaffinity(
        //pid_t pid
        //"Specifying pid as 0 will set the attribute for the calling thread"
        0
        , //size_t cpusetsize
        //"Normally this argument would be specified as sizeof(cpu_set_t)"
        sizeof(cpu_set_t)
        ,
        //cpu_set_t * mask
        & cpu_set
        );
    //"On success, sched_setaffinity() and sched_getaffinity() return 0."
//    return ! nRetVal; // 0 -> 1    <> 0 -> 1
    if(nRetVal == 0)
      return 1;
    return 0;
  }
  void Init(UserInterface * pui) ;
  void InitPerCPUcoreAccess(BYTE byNumCPUcores) ;
  BOOL ReadPciConfigDwordEx(
    DWORD dwPCIaddress
    , DWORD dwRegAddress
    , PDWORD p_dwValue
    );
  void SetUserInterface(UserInterface * pui) ;
  void Sleep(WORD wMillis) //{ ::Sleep(wMillis) ; }
    ;

  BOOL //WINAPI
  WritePciConfigDwordEx (
    DWORD pciAddress,
    DWORD regAddress,
    DWORD value
    );

  BOOL // TRUE: success, FALSE: failure
  //WINAPI
  WrmsrEx(
	  DWORD index,		// MSR index
	  DWORD eax,			// bit  0-31
	  DWORD edx,			// bit 32-63
	  DWORD affinityMask	// Thread Affinity Mask
  );
};

#endif	/* _MSRDEVICEFILE_H */

