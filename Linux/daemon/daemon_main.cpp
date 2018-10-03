/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
//class DynFreqScalingThreadBase
#include <Controller/DynFreqScalingThreadBase.hpp>
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#include <Controller/character_string/stdtstr.hpp> //std::tstring
#include <OperatingSystem/Linux/daemon/daemon.h> //for daemonize(...)
#include <Linux/daemon/daemon_functions.hpp> //init_daemon()
#include <Linux/daemon/CPUcontrolDaemon.hpp>//class CPUcontrolDaemon
//OperatingSystem::GetErrorMessageFromLastErrorCodeA()
#include <OperatingSystem/GetErrorMessageFromLastErrorCode.hpp>
//OperatingSystem::GetCurrentWorkingDirA_inl(std::string & )
#include <OperatingSystem/Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.hpp>
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
CommandLineArgs<char> g_commandlineargs;

bool init_logger(int argc, char *  argv[] )
{
//  std::string std_strLogFileName = std::string(
//    //argv[CommandLineArgs::FullProgramPathArgIndex]
//    //g_commandlineargs.GetFullProgramPath() ) +
//    gp_cpucontrolbase->GetExecutableFileName(g_commandlineargs.GetFullProgramPath() ) +
//    std::string( "_log." ) ;

  std::wstring std_wstrFullProgramPath = GetStdWstring(
    g_commandlineargs.GetProgramPath() );

  std::wstring std_wstrLogFilePath = L".";
  std::wstring std_wstrLogFileName = //GetStdWstring(std_strLogFileName);
    gp_cpucontrolbase->GetExecutableFileName(std_wstrFullProgramPath.c_str() );

//  if( ! g_logger.OpenFileA( //std::string("x86IandC_service_log.txt")
//    stdstrLogFileName ) )
  bool logFileIsOpen = gp_cpucontrolbase->GetLogFilePropertiesAndOpenLogFile(
    //std_strLogFileName
    std_wstrLogFilePath,
    std_wstrLogFileName);
  if( ! logFileIsOpen )
    std::wcerr << L"Failed to open log file \"" << //std_strLogFileName
      std_wstrLogFilePath << L"\":"
      << GetStdWstring(OperatingSystem::GetErrorMessageFromLastErrorCodeA()) << std::endl;
  return logFileIsOpen;
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
  g_commandlineargs.Set(argc, argv);
  int nRetVal = EXIT_FAILURE ;

//  ReadMainConfigFile();
  DummyUserInterface dummyuserinterface ;
  Linux::CPUcontrolDaemon cpucontroldaemon( & dummyuserinterface ) ;
  gp_cpucontrolbase = (CPUcontrolBase *) & cpucontroldaemon ;
  cpucontroldaemon.m_ar_ar_ch_programArgs = argv;
  cpucontroldaemon.m_argumentCount = argc;
  init_logger(argc, argv);
  std::string stdstrCurrentWorkingDir ;
  init_daemon(//argc, argv,
    stdstrCurrentWorkingDir ) ;
  gp_cpucontrolbase->m_model.m_stdstrExecutableStartupPath =
    stdstrCurrentWorkingDir ;

  bool bDVFSthreadSuccessfullyStarted = cpucontroldaemon.Start() ;
  LOGN(/*"main(..)-"*/ "return value of starting the daemon:"
    << bDVFSthreadSuccessfullyStarted )
  OperatingSystem::GetCurrentWorkingDirA_inl(stdstrCurrentWorkingDir) ;
  LOGN(/*"main(..)-"*/ "current working directory:" << stdstrCurrentWorkingDir )
  if( bDVFSthreadSuccessfullyStarted )
  {
//    WaitForTermination();
    g_wxconditionbasedi_conditionKeepRunning.LockAndWait() ;
  }
  LOGN(/*"main(..)-"*/ "end return " << nRetVal)
  return nRetVal ;
}
