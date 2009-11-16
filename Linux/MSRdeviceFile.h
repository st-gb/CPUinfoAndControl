/* 
 * File:   MSRdeviceFile.h
 * Author: sgebauer
 *
 * Created on 15. November 2009, 18:38
 */

#ifndef _MSRDEVICEFILE_H
#define	_MSRDEVICEFILE_H

#include "../UserInterface.hpp" //for class "UserInterface"
#include <Controller/I_CPUaccess.hpp>
#include <Windows_compatible_typedefs.h>

//Exception class. If catched: don't continue (with assigning function
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

//This class calls the winring0 functions by function pointers.
class MSRdeviceFile
  //: public ControllerBase
  : public //ISpecificController
  I_CPUaccess
  //: public wxWidgets::CalculationThread
{
private:
  //UserInterface * mp_userinterface ;
public:
  MSRdeviceFile(UserInterface * pui) ;
  MSRdeviceFile() ;
  ~MSRdeviceFile() ;
  //const char *
  std::string getMSRFile(DWORD_PTR
    //Only 1 bit should be set in this value
    dwAffinityMask) ;

  void UIconfirm(const std::string & str) ;
  BOOL CpuidEx(
    DWORD index,
    PDWORD eax,
    PDWORD ebx,
    PDWORD ecx,
    PDWORD edx,
    DWORD_PTR affinityMask
  );
  BYTE GetNumberOfCPUCores() ;
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

