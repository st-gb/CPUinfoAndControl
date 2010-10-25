/* 
 * File:   MSRdeviceFile.cpp
 * Author: sgebauer
 * 
 * Created on 15. November 2009, 18:38
 */

#include <global.h> //for SUCCESS, FAILURE
#include "MSRdeviceFile.hpp" //header file of this class
//#include <Controller/character_string/stdstring_format.hpp>
//Linker? error when <errno.h> was _also_ (->twice) included (indirectly) from
// other include files before.
//for GetErrorMessageFromLastErrorCodeA(...)
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Controller/character_string/stdstring_format.hpp> //to_stdstring(T )
#include <preprocessor_macros/logging_preprocessor_macros.h> //for DEBUG()
//#include <preprocessor_macros/bitmasks.h>
//#include <Linux/EnglishMessageFromErrorCode.h>
#include <UserInterface/UserInterface.hpp> //for class "UserInterface"

#include <cpuid.h> //__get_cpuid(...)
#include <errno.h> //for "errno"
#include <fcntl.h> //O_RDONLY, ...
#include <ios> //std::iosbase
#include <sstream> //for class std::stringstream
#include <stdlib.h> //system(...)
#include <unistd.h> // execvp(...)
////from www.xml.com/ldd/...:
//#include <linux/kmod.h> //must be from an appropriate directory
////#include "/usr/src/linux-headers-2.6.32-25-generic/include/linux/kmod.h"

#define POSSIBLE_SOLUTION_LITERAL "Possible solutions:\n"\
"-This program needs elevated privileges for ring 0 / CPU "\
"access. So run it as administrator.\n"\
"-This program uses a device file to communicate with the CPU for higher "\
"privileges. Does the program have sufficient rights for accessing "\
"(read,write[,...]) the file ?\n"

#define MSR_KERNEL_MODULE_NAME_ANSI "msr"
//extern int errno;

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
  //,mp_userinterface(pui)
  //mp_userinterface(NULL)
{
  Init(pui) ;
}

MSRdeviceFile::MSRdeviceFile(
  UserInterface * pui ,
  BYTE byNumberOfLogicalCPUcores
  )
//  : m_byNumberOfLogicalCPUcores (byNumberOfLogicalCPUcores)
{
  Init(pui) ;
  m_byNumberOfLogicalCPUcores = byNumberOfLogicalCPUcores ;
  InitPerCPUcoreAccess( byNumberOfLogicalCPUcores) ;
}

//This function should be called by all constructors.
void MSRdeviceFile::Init(UserInterface * pui)
{
  mp_userinterface = pui ;
  m_byNumberOfLogicalCPUcores = 0 ;
}

void MSRdeviceFile::InitPerCPUcoreAccess(BYTE byNumCPUcores)
{
  //getMSRFile()
//  if(m_pcpucontroller )
//    m_pcpucontroller->GetNumberOfCPUcores()
//  BYTE byNumCPUcores = GetNumberOfCPUCores() ;

//  //from kernelbook.sourceforge.net:
//  if(
//    //load msr module in order to create device files
//    // /dev/cpu/>>CPU core ID<</msr
//   //"
//   request_module("msr")
//    )
//  {
//    std::string stdstrMessage = "failed to load kernel module"
//      MSR_KERNEL_MODULE_NAME_ANSI ;
//    UIconfirm( stdstrMessage ) ;
//    throw CPUaccessException(stdstrMessage) ;
//  }
  system( "chmod 777 load_kernel_module.sh") ;
  //from www.opengroup.org:
  if( //execlp( "sh", "sh", "./load_kernel_module.sh"
      //from www.yolinux.com
      system(
      "./load_kernel_module.sh"
//      , //NULL
//      (char *) 0
      ) == -1
    )
  {
    std::string stdstrErrorMessage = GetErrorMessageFromLastErrorCodeA() ;
    UIconfirm( "error executing load_kernel_module.sh: " + stdstrErrorMessage) ;
  }
  std::string stdstrMSRfilePath ;
//  m_arfstreamMSR = new std::fstream [byNumCPUcores] ;
  m_arnFileHandle = new int [byNumCPUcores] ;
  for(BYTE byCoreIndex = 0 ; byCoreIndex <  byNumCPUcores ; ++ byCoreIndex )
  {
    stdstrMSRfilePath = getMSRFile( 1 << byCoreIndex ) ;
    //Using std::fstream seems to be impossible with device files
    //(program crash)
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
//    catch (const std::fstream::failure & e)
    if ( ( m_arnFileHandle[ byCoreIndex ] =
      //http://lkml.org/lkml/1997/2/27/59:
      //" To read/write, you open the device [...]" ->
      //http://linux.die.net/man/2/open
      open( stdstrMSRfilePath.c_str() , //O_RDONLY
      O_RDWR )) ==
      //"-1 if an error occurred "
      -1
      )
    {
//      int i = e.type ;
      std::string stdstrMessage = std::string("Failed to open the file \"") +
        stdstrMSRfilePath + "\" cause: " ;//" + e.what() ;
      //TODO exchange i by errno
//      int nErrno = 0 ;
      switch ( errno
        //nErrno
        )
      {
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
      LOGN(stdstrMessage)
      UIconfirm( stdstrMessage ) ;
      throw CPUaccessException(stdstrMessage) ;
    }
    else
      LOGN("successfully opened file \"" << stdstrMSRfilePath << "\""
        "for reading and writing ")
  }
}

MSRdeviceFile::MSRdeviceFile()
  //: mp_userinterface(pui)
{
  Init(NULL) ;
}

MSRdeviceFile::~MSRdeviceFile()
{
//  BYTE byNumCPUcores = GetNumberOfCPUCores() ;
  for(BYTE byCoreIndex = 0 ; byCoreIndex <  m_byNumberOfLogicalCPUcores ;
    ++ byCoreIndex )
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
//  unsigned int uiEAX ;
//  unsigned int uiEBX ;
//  unsigned int uiECX ;
//  unsigned int uiEDX ;
  //TODO set CPU (core) affinity,
  //TODO use parameters _directly_ for "__get_cpuid(...)"
  bReturn = __get_cpuid (
    dwFunctionIndex //unsigned int __level,
//    , & uiEAX //
    , (unsigned int *) p_dwEAX //unsigned int *__eax,
//    , & uiEBX //
    , (unsigned int *) p_dwEBX //unsigned int *__ebx,
//    , & uiECX //
    , (unsigned int *) p_dwECX //unsigned int *__ecx,
//    , & uiEDX //
    , (unsigned int *) p_dwEDX //unsigned int *__edx
    ) ;
//  * p_dwEAX = uiEAX ;
//  * p_dwEBX = uiEBX ;
//  * p_dwECX = uiECX ;
//  * p_dwEDX = uiEDX ;

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
  //msr0 = for 1st CPU
  ostrstr << "/dev/cpu/" << (WORD) byCoreID << "/msr" ;

  //return ostrstr.str().c_str() ;
  return ostrstr.str() ;
}

std::string MSRdeviceFile::getMSRdeviceFilePath(BYTE byCPUcoreID )
{
  std::ostringstream ostrstr;
  // "msr0" = for 1st CPU
  ostrstr << "/dev/cpu/" << (WORD) byCPUcoreID << "/msr" ;
  return ostrstr.str() ;
}

//Using std::fstream seems to be impossible with device files
//(there were program crashs?!)
//inline AccessMSRviaStdFileStreams()
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
//}

BOOL // TRUE: success, FALSE: failure
  MSRdeviceFile::RdmsrEx(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
)
{
//  BOOL bReturn = FAILURE ;
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
//    + stdstrAdditionalInfo + "\n");
//  throw ReadMSRexception() ;
  //DEBUG("readMSR(%lx,...,%lu)\n",dwRegisterNumber,affinityMask);
  m_byCoreID = 0 ;
  //1bin ->0 10bin ->1 100bin->2
  while( dwAffinityMask >>= 1 )
    ++ m_byCoreID ;

//    AccessMSRviaStdFileStreams()s

  if (
    //http://lkml.org/lkml/1997/2/27/59:
    //"To read/write, you open the device and seek to the index of the
    //needed register. E.g. for register Nr.10, this becomes "lseek(f,10,0);"
    lseek( //"int fildes" ("file descriptor")
      m_arnFileHandle[m_byCoreID], dwIndex, SEEK_SET) ==
    //http://www.opengroup.org/onlinepubs/009695399/functions/lseek.html:
    //"Upon successful completion, the resulting offset, as measured in bytes
    //from the beginning of the file, shall be returned.
    //Otherwise, (off_t)
    //-1 shall be returned, errno shall be set to indicate the error, and
    //the file offset shall remain unchanged."
    -1
    )
  {
    std::string stdstrErrMsg = ("Seeking failed in file ") + getMSRdeviceFilePath(
      m_byCoreID) //+ std::string("\n")
      + "to offset" + to_stdstring(dwIndex) + ":"
      + GetErrorMessageFromLastErrorCodeA() ;
    LOGN(stdstrErrMsg)
//    UIconfirm( stdstrErrMsg );
    return FAILURE;
  }
  m_ssize_t =
    //http://www.opengroup.org/onlinepubs/009695399/functions/read.html:
    //"Upon successful completion, read() [XSI] [Option Start]  and pread()
    //[Option End] shall return a non-negative integer indicating the number
    //of bytes actually read. Otherwise, the functions shall return -1 and
    //set errno to indicate the error."
    read( m_arnFileHandle[m_byCoreID],
    //http://lkml.org/lkml/1997/2/27/59:
    //"Regardless of the specified size, always 8 bytes (two long ints,
    //64 bits) are copied (actual size of a register).
    //The high dword (edx) is followed by the low dword (eax)."
    & m_ullMSRvalue,
    (size_t) 8 );
  if ( m_ssize_t == -1)
  {
    UIconfirm( std::string("Reading failed from file ") + getMSRdeviceFilePath(
      m_byCoreID) + GetErrorMessageFromLastErrorCodeA() + std::string("\n")
      );
    return FAILURE;
  }
  * p_dwedx = m_ullMSRvalue >> 32;
  * p_dweax = m_ullMSRvalue;
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
//  BOOL bReturn = FAILURE ;
  //DEBUG("readMSR(%lx,...,%lu)\n",dwRegisterNumber,affinityMask);
  m_byCoreID = 0 ;
  //1bin ->0 10bin ->1 100bin->2
  while( dwAffinityMask >>= 1 )
    ++ m_byCoreID ;

//    m_arfstreamMSR[byCoreID].seekg(dwIndex,std::ios_base::beg) ;
//    m_arfstreamMSR[byCoreID].readsome( (char *) & msrvalue,8) ;
  if (
    //http://lkml.org/lkml/1997/2/27/59:
    //"To read/write, you open the device and seek to the index of the
    //needed register. E.g. for register Nr.10, this becomes "lseek(f,10,0);"
    lseek( m_arnFileHandle[m_byCoreID], dwIndex, SEEK_SET) ==
    //http://www.opengroup.org/onlinepubs/009695399/functions/lseek.html:
    //"Upon successful completion, the resulting offset, as measured in bytes
    //from the beginning of the file, shall be returned.
    //Otherwise, (off_t)
    //-1 shall be returned, errno shall be set to indicate the error, and
    //the file offset shall remain unchanged."
    -1
    )
  {
    UIconfirm( std::string("Seeking failed in file ")
      + getMSRdeviceFilePath(m_byCoreID)
      + GetErrorMessageFromLastErrorCodeA()
      + std::string("\n")
      + std::string(POSSIBLE_SOLUTION_LITERAL)
      );
    return FAILURE;
  }
  m_ullMSRvalue = dwEDX ;
  m_ullMSRvalue <<= 32 ;
  m_ullMSRvalue |= dwEAX ;
  m_ssize_t = (int) write( m_arnFileHandle[m_byCoreID],
    //http://lkml.org/lkml/1997/2/27/59:
    //"Regardless of the specified size, always 8 bytes (two long ints,
    //64 bits) are copied (actual size of a register).
    //The high dword (edx) is followed by the low dword (eax)."
    & m_ullMSRvalue,
    (size_t) 8 );

  if ( m_ssize_t == -1 )
  {
    UIconfirm( //EnglishMessageFromErrorCode(errno)
      std::string("Writing to file ")
      + getMSRdeviceFilePath(m_byCoreID)
      + std::string("failed:")
      + GetErrorMessageFromLastErrorCodeA()
      + std::string("\n")
      + std::string(POSSIBLE_SOLUTION_LITERAL)
      ) ;
    return FAILURE;
  }
//  return bReturn ;
  return SUCCESS ;
}
