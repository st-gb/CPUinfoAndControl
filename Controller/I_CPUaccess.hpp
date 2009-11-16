#pragma once // include guard
#include "global.h" //for BYTE etc.
//#ifdef _WINDOWS
//  //#include <windef.h> //for BOOL etc.
//  #include <basetsd.h> //DWORD_PTR
//  //#include <WINDOWS.h> //for BOOL et.c
//#else
//  #include <Windows_compatible_typedefs.h>
//#endif
#include <Windows_compatible_typedefs.h>
#ifndef WINAPI 
  #define WINAPI __stdcall
#endif
#include <exception> //for exception
//#include "UserInterface.hpp" //for class "UserInterface"

//Base class for every single CPU access exception type as e.g.
//MSR read excpetion, CPUID exception etc.
class CPUaccessException
{
public:
  std::string m_stdstrErrorMessage ;
  DWORD m_dwErrorAsSystemError ; 
  CPUaccessException() {};
  CPUaccessException(char * p_ch) 
  {
    m_stdstrErrorMessage = std::string(p_ch) ;
  }
  CPUaccessException(DWORD dw) 
  {
    m_dwErrorAsSystemError = dw ;
  }
};

class ReadMSRexception
    : public //std::exception 
    CPUaccessException
{
//public:
//    ReadMSRexception() ;
};

//#include "Windows/CalculationThread.hpp"
//using namespace Windows_API ;
//class Windows_API::CalculationThread ;
//class CalculationThread ;
class UserInterface ;

//This class is the base class/ interface for CPU access implementations
//like via WinRing0 library or via /dev/msr0 etc.
//So this dependancy that can also be Operating System specific is decoupled.

//For succ. linling with gcc: if there is an error with vftable:
///usr/lib/gcc/i686-pc-cygwin/3.4.4/include/c++/bits/locale_facets.tcc:2498:
//  undefined reference to `vtable for ISpecificController'
//http://gcc.gnu.org/faq.html#vtables:
//"The solution is to ensure that all virtual methods that are not pure are
//defined. Note that a destructor must be defined even if it is declared
//pure-virtual [class.dtor]/7."
class //ISpecificController
  I_CPUaccess
  //: public Windows_API::CalculationThread
  //: public CalculationThread
{
protected:
  UserInterface * mp_userinterface ;

public:
  //All methods must be "virtual": so they needn't have a definition HERE.
  virtual BOOL CpuidEx(
    DWORD dwIndex,
    PDWORD p_dwEAX,
    PDWORD p_dwEBX,
    PDWORD p_dwECX,
    PDWORD p_dwEDX,
    DWORD_PTR affinityMask
  ) = 0 ;
  virtual BYTE GetNumberOfCPUCores() = 0 ;

  //It makes sense to implement the get family and model as a
  //method of the base CPU access class (instead of e.g. as a method 
  //of a CPU controller class) for the following reason:
  //the family and model is the PREREQUISITE to select the CPU 
  //controller class, e.g for vendor AMD, family 17 use the 
  //"GriffinController" class.
  bool GetFamilyAndModel(
    //BYTE & byFamily
    // values may be > 8 bits : bits 20:27 = 8 bits, + 4 bit value
    WORD & r_wFamily
    , BYTE & byModel
    ) ;
  //It makes sense to implement the get family and model as a
  //method of the base CPU access class (instead of e.g. as a method 
  //of a CPU controller class) for the following reason:
  //the family and model is the PREREQUISITE to select the CPU 
  //controller class, e.g for vendor AMD, family 17 use the 
  //"GriffinController" class.
  bool GetFamilyAndModelAndStepping(
    //BYTE & byFamily
    // values may be > 8 bits : bits 20:27 = 8 bits, + 4 bit value -> 9 bit
    WORD & r_wFamily
    // values may be <= 8 bits : extended model = bits 19:16 = 4 bits 
    //  -> max value = 1111bin
    // base model = bits 7:4 = 4 bit value
    // model = extended model + base model << 4 -> 1111bin << 4 = 11110000bin
    //  4 bit << 4 = 8 bit value, 
    , BYTE & r_byModel
    , BYTE & r_byStepping
    ) ;
  //Is the same for AMD and Intel.
  bool GetProcessorNameByCPUID( std::string & r_stdstr ) ;
  //Use the method with std::string parameter rather than this 
  //version with "char * &" where the
  //memory is allocated inside the function!
  bool GetProcessorNameByCPUID(
    //Use a pointer to an array in order to allocate the array within 
    //this method.
    char * & archCPUID 
    ) ;
  bool GetProcessorNoLeadingSpaces( std::string & r_stdstr ) ;
//It makes sense to implement the get family and model as a
//method of the base CPU access class (instead of e.g. as a method 
//of a CPU controller class) for the following reason:
//the family and model is the PREREQUISITE to select the CPU 
//controller class, e.g for vendor AMD, family 17 use the 
//"GriffinController" class.
  //http://de.wikipedia.org/wiki/CPUID
  //http://www.sandpile.org/ia32/cpuid.htm
  bool GetVendorID( std::string & r_str );

  virtual BOOL // TRUE: success, FALSE: failure
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI
  RdmsrEx(
	  DWORD index,		// MSR index
	  PDWORD eax,			// bit  0-31
	  PDWORD edx,			// bit 32-63
          //1bin =core 0; 10bin=2dec= core 1
	  DWORD_PTR affinityMask	// Thread Affinity Mask
  ) = 0 ;
  inline virtual BOOL // TRUE: success, FALSE: failure
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI
  RdpmcEx(
	  DWORD index,		// MSR index
	  PDWORD eax,			// bit  0-31
	  PDWORD edx,			// bit 32-63
          //1bin =core 0; 10bin=2dec= core 1
	  DWORD_PTR affinityMask	// Thread Affinity Mask
  ) = 0 ;
  virtual BOOL ReadPciConfigDwordEx(
    DWORD dwPCIaddress
    , DWORD dwRegAddress
    , PDWORD p_dwValue
    ) = 0 ;
  void SetUserInterface(UserInterface & r_userinterface) 
  { 
      mp_userinterface = & r_userinterface ;
  }
  virtual void Sleep(WORD wMillis) = 0 ;
  virtual BOOL 
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    WritePciConfigDwordEx ( 
    DWORD pciAddress,
    DWORD regAddress,
    DWORD value) = 0 ;
  virtual BOOL // TRUE: success, FALSE: failure
    //In g++ virtual methods can't be declared as stdcall
    //WINAPI
    WrmsrEx(
      DWORD index,		// MSR index
      DWORD dwLow ,//eax,			// bit  0-31
      DWORD dwHigh, //edx,			// bit 32-63
      DWORD affinityMask	// Thread Affinity Mask
    ) = 0 ;
  //For succ. linking with gcc: if there is an error with vftable:
  ///usr/lib/gcc/i686-pc-cygwin/3.4.4/include/c++/bits/locale_facets.tcc:2498:
  //  undefined reference to `vtable for ISpecificController'
  //http://gcc.gnu.org/faq.html#vtables:
  //"Note that a destructor must be defined even if it is declared
  //pure-virtual [class.dtor]/7."
  ~//ISpecificController()
    I_CPUaccess() {} ;
};
