#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
//class DynFreqScalingThreadBase
#include <Controller/DynFreqScalingThreadBase.hpp>
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#include <Controller/character_string/stdtstr.hpp> //std::tstring
#include <Linux/daemon/daemon.h> //for daemonize(...)
#include <Linux/daemon/daemon_functions.hpp> //init_daemon()
#include <Linux/daemon/CPUcontrolDaemon.hpp>//class CPUcontrolDaemon
//GetCurrentWorkingDir(std::string & )
#include <Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.hpp>
#include <ModelData/ModelData.hpp> //class Model
#include <UserInterface/DummyUserInterface.hpp> //class DummyUserInterface
//class wxConditionBasedI_Condition
#include <wxWidgets/multithread/wxConditionBasedI_Condition.hpp>
#include <Xerces/SAX2MainConfigHandler.hpp> //class SAX2MainConfigHandler
#include <Xerces/XMLAccess.hpp> //ReadXMLdocumentInitAndTermXerces
#include <tchar.h> //_T(...) preprocessor macro

//Needed for the exported functions.
//I_CPUaccess * g_p_cpuaccess ;

//from daemon_functions.cpp
extern CPUcontrolBase * gp_cpucontrolbase ;
extern wxConditionBasedI_Condition g_wxconditionbasedi_conditionKeepRunning ;

void init_logger(int argc, char *  argv[] )
{
  std::string stdstrLogFileName = std::string( argv[0] ) +
    std::string( "_log.txt" ) ;
  g_logger.OpenFile2( //std::string("x86IandC_service_log.txt")
    stdstrLogFileName ) ;
}

//    WaitForTermination()
//    {
//      LOGN("Waiting for termination of the Dynamic Voltage and Frequency "
//        "Scaling thread.")
//      //Need to wait for the end of the DVFS thread. Else this program is exited
//      // immediately.
//  //    cpucontroldaemon.mp_dynfreqscalingthreadbase->WaitForTermination() ;
//      if( cpucontroldaemon.m_x86iandc_thread_typeDVFSthread.WaitForTermination() )
//        LOGN("Failed to wait for the termination of the Dynamic Voltage and "
//          "Frequency Scaling thread.")
//      else
//      {
//        LOGN("Successfully waited for the termination of the Dynamic Voltage and "
//          "Frequency Scaling thread.")
//  //      return 0 ;
//        nRetVal = EXIT_SUCCESS ;
//      }
//    }

//MinGW's g++: ../../Windows/main.cpp:168: error: `main' must return `int'
//void
int main( int argc, char *  argv[] )
{
  int nRetVal = EXIT_FAILURE ;
  init_logger(argc, argv);
  std::string stdstrCurrentWorkingDir ;
  init_daemon(//argc, argv
    stdstrCurrentWorkingDir ) ;

//  ReadMainConfigFile();
  DummyUserInterface dummyuserinterface ;
  Linux::CPUcontrolDaemon cpucontroldaemon( & dummyuserinterface ) ;
  gp_cpucontrolbase = (CPUcontrolBase *) & cpucontroldaemon ;
  gp_cpucontrolbase->m_model.m_stdstrExecutableStartupPath =
    stdstrCurrentWorkingDir ;
  bool bDVFSthreadSuccessfullyStarted = cpucontroldaemon.Start() ;
  GetCurrentWorkingDir(stdstrCurrentWorkingDir) ;
  LOGN("Current working directory:" << stdstrCurrentWorkingDir )
  if( bDVFSthreadSuccessfullyStarted )
  {
//    WaitForTermination();
    g_wxconditionbasedi_conditionKeepRunning.LockAndWait() ;
  }
  return nRetVal ;
}
