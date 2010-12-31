/*
 * x86IandCservice.cpp
 *
 *  Created on: Dec 30, 2010
 *      Author: sgebauer
 */

#include "x86IandCservice.hpp"

#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
//class NonBlocking::wxServiceSocketClient
#include <wxWidgets/Controller/non-blocking_socket/client/\
wxServiceSocketClient.hpp>

#ifdef __LINUX__
  #include <Linux/daemon/daemon_functions.hpp> //init_daemon()
#endif

//from daemon_functions.cpp
extern CPUcontrolBase * gp_cpucontrolbase ;

namespace wxWidgets
{
  //Erzeugt ein wxAppConsole-Object auf dem Heap.
  IMPLEMENT_APP(x86IandCservice)

  x86IandCservice::x86IandCservice()
    : CPUcontrolServiceBase( & m_dummyuserinterface)
  {
    gp_cpucontrolbase = (CPUcontrolBase *) this ;
  }

  x86IandCservice::~x86IandCservice()
  {
    // TODO Auto-generated destructor stub
  }

//  void x86IandCservice::CreateHardwareAccessObject()
//  {
//#ifdef _WIN32
//    mp_i_cpuaccess = new MSRdeviceFile(
//      & m_dummyuserinterface ,
//      GetNumberOfLogicalCPUcores() ,
//      m_model
//      ) ;
//#endif
//  }

  int x86IandCservice::OnExit()
  {
    return 0 ;
  }
  bool x86IandCservice::OnInit()
  {
#ifdef __LINUX__
    std::string stdstrCurrentWorkingDir ;
    init_daemon(stdstrCurrentWorkingDir) ;
#endif
    return true ;
  }
}
