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
#include <errno.h>
#include <ios> //std::iosbase
#include <Linux/EnglishMessageFromErrorCode.h>
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

void MSRdeviceFile::InitPerCPUcoreAccess(BYTE byNumCPUcores)
{
  //getMSRFile()
//  if(m_pcpucontroller )
//    m_pcpucontroller->GetNumberOfCPUcores()
//  BYTE byNumCPUcores = GetNumberOfCPUCores() ;
  std::string stdstrMSRfilePath ;
//  m_arfstreamMSR = new std::fstream [byNumCPUcores] ;
  m_arnFileHandle = new int [byNumCPUcores] ;
  for(BYTE byCoreIndex = 0 ; byCoreIndex <  byNumCPUcores ; ++ byCoreIndex )
  {
    stdstrMSRfilePath = getMSRFile( 1 << byCoreIndex ) ;
//    m_arfstreamMSR[byCoreIndex].exceptions (
//      std::fstream::eofbit | std::fstream::failbit | std::fstream::badbit );
//    try
//    {
//      m_arfstreamMSR[byCoreIndex].open( stdstrMSRfilePath.c_str() ,
//        //std::ios_base::binary | std::ios_base::in | std::ios_base::out
//        std::fstream::in | std::fstream::out | std::fstream::app
//        ) ;
//      //On failure, the failbit flag is set (which can be checked with member
//      //fail), and depending on the value set with exceptions an exception may
//      //be thrown.
//      if( m_arfstreamMSR[byCoreIndex].fail ( ) )
//      {
//        UIconfirm("Failed to open the file \"" + stdstrMSRfilePath + "\"" ) ;
//        throw CPUaccessException("failed to open file") ;
//      }
//    }
//    catch (std::fstream::failure e)
    if (( m_arnFileHandle[byCoreIndex] = open(//msrfile
      stdstrMSRfilePath.c_str() , //O_RDONLY
      O_RDWR )) == -1)
    {
//      int i = e.type ;
        std::string stdstrMessage = std::string("Failed to open the file \"") +
          stdstrMSRfilePath + "\" cause: " ;//" + e.what() ;
      switch (errno) {
        case EACCES:  stdstrMessage += "Permission denied.\n"
          "possible solution: run this program as administrator/root\n"
          "do this e.g. via \"sudo\"";
          break;
        //case EINVACC: stdstrMsg += "Invalid access mode.\n"; break;
        case EMFILE:  stdstrMessage += "No file handle available.\n"; break;
        case ENOENT:  
          stdstrMessage += " File or path not found.\n"
            "possible solution:\n"
            "If module \"msr\" is not installed: e.g. install via package manager\n"
            "If module \"msr\" is installed: try \"modprobe msr\" "
            "(with elevated rights/as root) \n";
          break;
        default:      stdstrMessage += "Unknown error.\n"; break;
      }
        UIconfirm( stdstrMessage ) ;
        throw CPUaccessException(stdstrMessage) ;
    }
  }
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
  BYTE byNumCPUcores = GetNumberOfCPUCores() ;
  for(BYTE byCoreIndex = 0 ; byCoreIndex <  byNumCPUcores ; ++ byCoreIndex )
  {
//    m_arfstreamMSR[byCoreIndex].close( ) ;
    close( m_arnFileHandle[byCoreIndex] ) ;
  }
//  delete [] m_arfstreamMSR ;
  delete [] m_arnFileHandle ;
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
  ostrstr << "/dev/cpu/" << (WORD) byCoreID << "/msr" ;

  //return ostrstr.str().c_str() ;
  return ostrstr.str() ;
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
//    const char * msrfile ;
//    int fd;
//    int result;
    unsigned long long msrvalue;
    //DEBUG("readMSR(%lx,...,%lu)\n",dwRegisterNumber,affinityMask);
    //msrfile = getMSRFile(dwAffinityMask) ;
    std::string stdstrMSRfile = getMSRFile(dwAffinityMask) ;
//
//    if ((fd = open(//msrfile
//      stdstrMSRfile.c_str() , O_RDONLY)) == -1)
    BYTE byCoreID = 0 ;
    //1bin ->0 10bin ->1 100bin->2
    while( dwAffinityMask >>= 1 )
      ++ byCoreID ;

//    //Set position of the get pointer.
//    m_arfstreamMSR[byCoreID].seekg(dwIndex,std::ios_base::beg) ;
//    if( m_arfstreamMSR[byCoreID].failbit ||
//        m_arfstreamMSR[byCoreID].badbit )
//    {
//      std::string stdstrMsg = "Seeking in file \"" + stdstrMSRfile +
//        std::string("\" failed!\n") ;
//      UIconfirm( stdstrMsg );
//    }
//  #ifdef _DEBUG
//    DWORD dw = m_arfstreamMSR[byCoreID].tellg ( );
//  #endif
//    char arch[8] ;
//    try
//    {
//      //read methods from std::fstream does not work with character devices?
//      m_arfstreamMSR[byCoreID].readsome( //(char *) & msrvalue
//        arch , 8 ) ;
//    }
//    catch( std::ios_base::failure fail )
//    {
//      fail.what() ;
//    }
    //m_arfstreamMSR[byCoreID].read ( (char *) & msrvalue , 8 );
    //m_arfstreamMSR[byCoreID].get( (char *) & msrvalue, 8 ) ;
    //dw = m_arfstreamMSR[byCoreID].get() ;

//      std::string stdstrMsg = "Can't open file " + stdstrMSRfile +
//        std::string(" for reading!\n") ;
     // std::string stdstrMSRfile(msrfile) ;

    //The stream was at the end of the source of characters before the
    //function was called.
//    if( m_arfstreamMSR[byCoreID].failbit )
//      dw = 0 ;
//    if( m_arfstreamMSR[byCoreID].eofbit )
//      dw = 0 ;
//    if(
//        m_arfstreamMSR[byCoreID].badbit )
//    {
//      std::string stdstrMsg = "Reading in file \"" + stdstrMSRfile +
//        std::string("\" failed!\n") ;
//      switch (errno)
//      {
//        case EACCES:  stdstrMsg += "Permission denied.\n"; break;
//        //case EINVACC: stdstrMsg += "Invalid access mode.\n"; break;
//        case EMFILE:  stdstrMsg += "No file handle available.\n"; break;
//        case ENOENT:  stdstrMsg += "File or path not found.\n"; break;
//        case ENOTTY:  stdstrMsg += "Not a typewriter\n"; break;
//        default:      stdstrMsg += "Unknown error.\n"; break;
//      }
//
//      UIconfirm( //std::string("Can't open file ") + stdstrMSRfile +
//        //std::string(" for reading!\n")
//        stdstrMsg
//        );
//      return FAILURE;
//    }
    if (lseek( m_arnFileHandle[byCoreID], dwIndex, SEEK_SET) == -1)
    {
      UIconfirm(std::string("Seeking failed in file ") //+ std::string(msrfile)
        + std::string("\n")
        );
      return FAILURE;
    }

    int result = (int)read( m_arnFileHandle[byCoreID], &msrvalue, (size_t)8);

    if (result == -1)
    {
//      printf("Read error in file \"%s\"!\n", msrfile);
      return FAILURE;
    }
//    if(close(fd) < 0 )
//    {
//      printf("Close error in file %s!\n", msrfile);
//      return FAILURE;
//    }

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
  unsigned long long ullMSRvalue ;
    //DEBUG("readMSR(%lx,...,%lu)\n",dwRegisterNumber,affinityMask);
    //msrfile = getMSRFile(dwAffinityMask) ;
//    std::string stdstrMSRfile = getMSRFile(dwAffinityMask) ;
//
//    if ((fd = open(//msrfile
//      stdstrMSRfile.c_str() , O_RDONLY)) == -1)
    BYTE byCoreID = 0 ;
    //1bin ->0 10bin ->1 100bin->2
    while( dwAffinityMask >>= 1 )
      ++ byCoreID ;

//    m_arfstreamMSR[byCoreID].seekg(dwIndex,std::ios_base::beg) ;
//    m_arfstreamMSR[byCoreID].readsome( (char *) & msrvalue,8) ;
    if (lseek( m_arnFileHandle[byCoreID], dwIndex, SEEK_SET) == -1)
    {
      UIconfirm(std::string("Seeking failed in file ") //+ std::string(msrfile)
        + std::string("\n")
        );
      return FAILURE;
    }
    ullMSRvalue = dwEDX ;
    ullMSRvalue <<= 32 ;
    ullMSRvalue |= dwEAX ;
    int result = (int)write( m_arnFileHandle[byCoreID], & ullMSRvalue, (size_t)8);

    if (result == -1)
    {
      UIconfirm( EnglishMessageFromErrorCode(errno) ) ;
      return FAILURE;
    }
//    if(close(fd) < 0 )
//    {
//      printf("Close error in file %s!\n", msrfile);
//      return FAILURE;
//    }
  return bReturn ;
}
