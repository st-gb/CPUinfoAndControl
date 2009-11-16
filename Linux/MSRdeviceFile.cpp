/* 
 * File:   MSRdeviceFile.cpp
 * Author: sgebauer
 * 
 * Created on 15. November 2009, 18:38
 */

#include "MSRdeviceFile.h"
//#include <Windows.h> //for ::GetProcAddress(), ::GetCurrentDirectory
#include <strstream> //for std::ostrstream
#include "../global.h" //for DEBUG()
#include "../preprocessor_helper_macros.h"
#include <Controller/stdstring_format.hpp>
#include <fcntl.h> //O_RDONLY, ...
#include <cpuid.h>
//#include <Windows/ErrorCodeFromGetLastErrorToString.h>

//MSRdeviceFile::MSRdeviceFile()
//{
//
//}
//
//MSRdeviceFile::MSRdeviceFile(const MSRdeviceFile& orig) {
//}
//
//MSRdeviceFile::~MSRdeviceFile() {
//}

MSRdeviceFile::MSRdeviceFile(UserInterface * pui)
  //: //C++ style initialisations
  //m_hinstanceWinRingDLL ( NULL ) ,
  ////m_pfnreadpciconfigdwordex ( NULL ) ,
  ////m_pfnwritepciconfigdwordex ( NULL ) ,
  //m_pfnrdmsrex ( NULL ) ,
  //m_pfnwrmsrex (NULL) ,
  //m_pfncpuidex ( NULL )
  //,mp_userinterface(pui)
  //mp_userinterface(NULL)
{
  Init(pui) ;
}
void MSRdeviceFile::Init(UserInterface * pui)
{
  mp_userinterface = pui ;
}

//WinRing0dynLinked::WinRing0dynLinked(UserInterface * pui)
MSRdeviceFile::MSRdeviceFile()
  //: mp_userinterface(pui)
{
  //WinRing0dynLinked(NULL);
  Init(NULL) ;
}

MSRdeviceFile::~MSRdeviceFile()
{

}

BOOL MSRdeviceFile::CpuidEx(
    DWORD dwFunctionIndex,
    PDWORD p_dwEAX,
    PDWORD p_dwEBX,
    PDWORD p_dwECX,
    PDWORD p_dwEDX,
    DWORD_PTR dwAffinityMask
  )
{
  BOOL bReturn = FAILURE ;
  //__get_cpuid_max() ;
  unsigned int uiEAX ;
  unsigned int uiEBX ;
	unsigned int uiECX ;
  unsigned int uiEDX ;
  //TODO set CPU (core) affinity.
  bReturn = __get_cpuid (
    dwFunctionIndex //unsigned int __level,
	  , & uiEAX //, p_dwEAX //unsigned int *__eax,
    , & uiEBX //, p_dwEBX //unsigned int *__ebx,
	  , & uiECX //p_dwECX //unsigned int *__ecx,
    , & uiEDX //p_dwEDX //unsigned int *__edx
    ) ;
  * p_dwEAX = uiEAX ;
  * p_dwEBX = uiEBX ;
  * p_dwECX = uiECX ;
  * p_dwEDX = uiEDX ;

  return bReturn ;
}

//const char *
std::string MSRdeviceFile::getMSRFile(DWORD_PTR
  //Only 1 bit should be set in this value
  dwAffinityMask)
{
  std::string stdstrMSRfilePath ;
  BYTE byCoreID = 0 ;
  std::ostringstream ostrstr;

  //1bin ->0 10bin ->1 100bin->2
  while( dwAffinityMask >>= 1 )
    ++ byCoreID ;
//  if(affinityMask == 1)
//    byCoreID = 0 ;
//  else
//    byCoreID = 1 ;
  //msr0 = for 1st CPU
  ostrstr<<"/dev/cpu/"<<(WORD)byCoreID<<"/msr";

  //return ostrstr.str().c_str() ;
  return ostrstr.str() ;
}


BYTE MSRdeviceFile::GetNumberOfCPUCores()
{
  //return 2 ;
  BYTE byCoreNumber = 255 ;
  DWORD dwEAX;
  DWORD dwEBX;
  DWORD dwECX;
  DWORD dwEDX;
  DEBUG("WRDL--getting number of CPU cores\n");
  if( CpuidEx(
    //AMD: "CPUID Fn8000_0008 Address Size And Physical Core Count Information"
    0x80000008,
    &dwEAX,
    &dwEBX,
    &dwECX,
    &dwEDX,
    1
      )
    )
  {
    byCoreNumber = ( dwECX & BITMASK_FOR_LOWMOST_7BIT )
      //"ECX 7:0 NC: number of physical cores - 1.
      //The number of cores in the processor is NC+1 (e.g., if
      //NC=0, then there is one core).
      //See also section 2.9.2 [Number of Cores and Core Number]."
      + 1 ;
    //DEBUG("Number of CPU cores: %u\n", (WORD) byCoreNumber );
    LOG( "Number of CPU cores: " << (WORD) byCoreNumber << "\n" );
  }
  DEBUG("WRDL--end of getting number of CPU cores\n");
  return byCoreNumber ;
}

BOOL // TRUE: success, FALSE: failure
  MSRdeviceFile::RdmsrEx(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
)
{
  BOOL bReturn = FAILURE ;

//    //Show the detailed error message here because the details
//    //(how the access to the MSR is realised: via WinRing0)
//    //depend on the implementation.
//  UIconfirm(
//    "Reading from MSR failed. "
//    "\nerror message: \"" + strErrorMessage + "\"(error code: "
//    + to_stdstring<DWORD>( //(WORD) byModel
//      dwErrorCode //, std::hex
//      )
//    + ")\n"
//    + stdstrAdditionalInfo + "\n"
//    "Possible solutions:\n"
//    "-This program needs elevated privileges for ring 0 / CPU "
//    "access. So run it as administrator.\n"
//    "-This program uses "
//    "a DLL to communicate with the CPU for higher privileges. Does the "
//    "file \"WinRing0[...].sys\" lay within the same directory as "
//    "\"WinRing0[...].dll\"?\n");
//  throw ReadMSRexception() ;
    //RdpmcEx seemed to cause a blue screen (maybe because of wrong param values)
    //bReturn =
    //(*m_pfnrdpmcex)(
    //   dwIndex
    //   ,p_dweax
    //   ,p_dwedx
    //   ,affinityMask
    //  );
    const char * msrfile ;
    int fd;
    int result;
    unsigned long long msrvalue;
    //DEBUG("readMSR(%lx,...,%lu)\n",dwRegisterNumber,affinityMask);
    //msrfile = getMSRFile(dwAffinityMask) ;
    std::string stdstrMSRfile = getMSRFile(dwAffinityMask) ;

    if ((fd = open(//msrfile
      stdstrMSRfile.c_str() , O_RDONLY)) == -1)
    {
     // std::string stdstrMSRfile(msrfile) ;
      UIconfirm(std::string("Can't open file ") + stdstrMSRfile +
        std::string(" for reading!\n")
        );
      return FAILURE;
    }

    if (lseek(fd, dwIndex, SEEK_SET) == -1)
    {
      UIconfirm(std::string("Seeking failed in file ") + std::string(msrfile) +
        std::string("!\n")
        );
      return FAILURE;
    }

    result = (int)read(fd, &msrvalue, (size_t)8);

    if (result == -1)
    {
      printf("Read error in file \"%s\"!\n", msrfile);
      return FAILURE;
    }
    if(close(fd) < 0 )
    {
      printf("Close error in file %s!\n", msrfile);
      return FAILURE;
    }

    *p_dwedx = msrvalue >> 32;
    *p_dweax = msrvalue;

    return SUCCESS;
    //return bReturn ;
}

inline BOOL MSRdeviceFile::RdpmcEx(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
  )
{
  BOOL bReturn = FAILURE ;
  {

  }
  return bReturn ;
}

BOOL MSRdeviceFile::ReadPciConfigDwordEx(
  DWORD dwPCIaddress
  , DWORD dwRegAddress
  , PDWORD p_dwValue
  )
{
  BOOL bReturn = FAILURE ;

  return bReturn ;
}

void MSRdeviceFile::SetUserInterface(UserInterface * pui)
{
  mp_userinterface = pui ;
}

void MSRdeviceFile::Sleep(WORD wMillis)
{
  //::Sleep(wMillis) ;
}

void MSRdeviceFile::UIconfirm(const std::string & str)
{
  if ( mp_userinterface )
    mp_userinterface->Confirm(str) ;
}

BOOL //WINAPI
MSRdeviceFile::WritePciConfigDwordEx (
  DWORD dwPCIaddress, DWORD dwRegAddress, DWORD dwValue)
{
  BOOL bReturn = FAILURE ;

  return bReturn ;
}

BOOL // TRUE: success, FALSE: failure
//WINAPI
MSRdeviceFile::WrmsrEx(
	DWORD dwIndex,		// MSR index
	DWORD dwEAX,			// bit  0-31
	DWORD dwEDX,			// bit 32-63
	DWORD dwAffinityMask	// Thread Affinity Mask
)
{
  BOOL bReturn = FAILURE ;

  return bReturn ;
}
