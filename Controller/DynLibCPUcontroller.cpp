/*
 * DynLibCPUcontroller.cpp
 *
 *  Created on: Mar 23, 2010
 *      Author: Stefan
 */

#include <Controller/exported_functions.h> //for AM_LIB_EXPORT
#include <Controller/CPUcontrolBase.hpp>
#include <Controller/I_CPUaccess.hpp>

//class I_CPUaccess ;
//class CPUcontrolBase ;

extern CPUcontrolBase * gp_cpucontrolbase ;

//used by the GUI and the service

//from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
//("Calling an Exported Function in an EXE from Within a DLL"):
// Do exactly as you would export a DLL...
extern "C"
{
  //EXPORT
  AM_LIB_EXPORT BOOL ReadMSR(
    DWORD dwIndex,    // MSR index
    PDWORD p_dweax,     // bit  0-31
    PDWORD p_dwedx,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
  )
  {
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
    I_CPUaccess * p_cpuaccess = //wxGetApp().
      gp_cpucontrolbase->
      GetCPUaccess() ;
    BOOL boolRet = p_cpuaccess->RdmsrEx(
      dwIndex,
      p_dweax,// bit  0-31 (register "EAX")
      p_dwedx,
      //m_dwAffinityMask
      affinityMask
      ) ;
    #ifdef _DEBUG
    //if( dwIndex == 0x1AD )
//      DEBUG_COUT( "exe::ReadMSR(Index,affinityMask): "
//        << dwIndex << " "
//        << *p_dweax << " "
//        << *p_dwedx << " "
//        << affinityMask
//        << "\n" )
    #endif
    return boolRet ;
  }

  AM_LIB_EXPORT BOOL WriteMSR(
    DWORD dwIndex,    // MSR index
    DWORD dwEAX,     // bit  0-31
    DWORD dwEDX,     // bit 32-63
    DWORD_PTR affinityMask  // Thread Affinity Mask
    )
  {
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
    I_CPUaccess * p_cpuaccess = //wxGetApp().
      gp_cpucontrolbase->
      GetCPUaccess() ;
    BOOL boolRet = p_cpuaccess->WrmsrEx(
      dwIndex,
      dwEAX,// bit  0-31 (register "EAX")
      dwEDX,
      //m_dwAffinityMask
      affinityMask
      ) ;
    #ifdef _DEBUG
    //if( dwIndex == 0x1AD )
//      DEBUG_COUT( "exe::WriteMSR(Index,affinityMask): "
//        << dwIndex << " "
//        << dwEAX << " "
//        << dwEDX << " "
//        << affinityMask
//        << "\n" )
    #endif
    return boolRet ;
  }
} //extern "C"

