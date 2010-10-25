#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
//class DynFreqScalingThreadBase
#include <Controller/DynFreqScalingThreadBase.hpp>
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#include <Controller/Logger/Logger.hpp> //class Logger
#include <Controller/character_string/stdtstr.hpp> //std::tstring
#include <Linux/daemon/daemon.h> //for daemonize(...)
#include <Linux/daemon/CPUcontrolDaemon.hpp>//class CPUcontrolDaemon
#include <ModelData/ModelData.hpp> //class Model
#include <UserInterface/DummyUserInterface.hpp> //class DummyUserInterface
//class wxConditionBasedI_Condition
#include <wxWidgets/multithread/wxConditionBasedI_Condition.hpp>
#include <Xerces/SAX2MainConfigHandler.hpp> //class SAX2MainConfigHandler
#include <Xerces/XMLAccess.hpp> //ReadXMLdocumentInitAndTermXerces
#include <tchar.h> //_T(...) preprocessor macro

#include <signal.h> //for signal(...)

//This global (important for using preprocessor macros) object is used for
//easy logging.
Logger g_logger ;
CPUcontrolBase * gp_cpucontrolbase = NULL ;
wxConditionBasedI_Condition g_wxconditionbasedi_conditionKeepRunning ;

//Needed for the exported functions.
//I_CPUaccess * g_p_cpuaccess ;

void GetCurrentWorkingDir(std::string & r_stdstr )
{
  std::string stdstr ;
  char * pchCurentWorkingDir = NULL;
  //"Get the pathname of the current working directory,
  //   and put it in SIZE bytes of BUF.  Returns NULL if the
  //   directory couldn't be determined or SIZE was too small.
  //   If successful, returns BUF.
  pchCurentWorkingDir = getcwd(
    //"In GNU, if BUF is NULL,an array is allocated with `malloc' [...]"
    pchCurentWorkingDir,
    //"the array is SIZE
    //   bytes long, unless SIZE == 0, in which case it is as
    //   big as necessary."
    0) ;
  if( pchCurentWorkingDir )
  {
    r_stdstr = std::string(pchCurentWorkingDir) ;
    free( pchCurentWorkingDir ) ;
  }
}

static void child_handler(int nSignalNumber)
{
  switch(nSignalNumber)
  {
  case SIGCONT:
    LOGN("received CONT signal")
    { //This block is needed for the local variable.
      bool bContinue = false;
      ( (CPUcontrolServiceBase *) gp_cpucontrolbase)->
        StartDVFSviaThreadType_Inline(bContinue) ;
    }
    break;
  case SIGTERM:
    LOGN("received TERM signal")
    if( gp_cpucontrolbase )
      gp_cpucontrolbase->EndDVFS() ;
//    exit(EXIT_FAILURE);
    g_wxconditionbasedi_conditionKeepRunning.Signal() ;
    break;
  case SIGUSR1:
    LOGN("received SIGUSR1 signal")
    if( gp_cpucontrolbase )
      gp_cpucontrolbase->EndDVFS() ;
  }
}

//MinGW's g++: ../../Windows/main.cpp:168: error: `main' must return `int'
//void
int main( int argc, char *  argv[] )
{
  int nRetVal = EXIT_FAILURE ;
  const char * cp_chLockFileName = "x86IandC_daemon.lock" ;
  signal(SIGTERM,child_handler);
  signal(SIGCONT,child_handler);
  signal(SIGUSR1,child_handler);
//  LOGN("before daemonizing")
//  //daemonize _after_ the config files were read. because their pathes are
//  //_relative_ to the current working dir of this process.
//  ::daemonize( //"x86IandC_daemon.lock"
//    cp_chLockFileName ) ;
//  LOGN("after daemonizing")
  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring(_T("-config=") )
    + ptstrProgramName + std::tstring(_T("_config.xml") ) ;
  //std::string stdstrLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
  std::string stdstrLogFileName = std::string( argv[0] ) +
    std::string( "_log.txt" ) ;
  g_logger.OpenFile2( //std::string("GriffinControl_log.txt")
    stdstrLogFileName ) ;

//  Model model ;
  DummyUserInterface dummyuserinterface ;
//  std::string stdstrMainConfigFileName = "x86InfoAndControl_config.xml" ;
//  //The main cfg also contains the exclusion log message filter--load it in any
//  //case.
//  SAX2MainConfigHandler sax2mainconfighandler( model, & dummyuserinterface );
//  ReadXMLfileInitAndTermXerces(
//    //const char* xmlFile
//    stdstrMainConfigFileName.c_str()
//    , model
//    , & dummyuserinterface
//    //Base class of implementing Xerces XML handlers.
//    //This is useful because there may be more than one XML file to read.
//    //So one calls this functions with different handlers passed.
//    //DefaultHandler & r_defaulthandler
//    , sax2mainconfighandler
//    ) ;
  {
    Linux::CPUcontrolDaemon cpucontroldaemon( & dummyuserinterface ) ;
    gp_cpucontrolbase = (CPUcontrolBase *) & cpucontroldaemon ;
    bool bDVFSthreadSuccessfullyStarted = cpucontroldaemon.Start() ;
    if( bDVFSthreadSuccessfullyStarted )
    {
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
      g_wxconditionbasedi_conditionKeepRunning.LockAndWait() ;
    }
  }
  return nRetVal ;
}
