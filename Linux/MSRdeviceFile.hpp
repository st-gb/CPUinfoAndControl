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

