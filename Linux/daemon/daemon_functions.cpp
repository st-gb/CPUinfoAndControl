/*
 * daemon_functions.c
 *
 *  Created on: Dec 30, 2010
 *      Author: sgebauer
 */
#include "daemon_functions.hpp"

#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
#include <Controller/CPUcontrolServiceBase.hpp> //class CPUcontrolServiceBase
#include <Controller/Logger/Logger.hpp> //class Logger
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//GetCurrentWorkingDir(std::string & )
#include <Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.hpp>
//class wxConditionBasedI_Condition
#include <wxWidgets/multithread/wxConditionBasedI_Condition.hpp>

#include <signal.h> //for signal(...)

//This global (important for using preprocessor macros) object is used for
//easy logging.
Logger g_logger ;
CPUcontrolBase * gp_cpucontrolbase = NULL ;
wxConditionBasedI_Condition g_wxconditionbasedi_conditionKeepRunning ;

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

void init_daemon(//int argc, char *  argv[]
  std::string & stdstrCurrentWorkingDir )
{
  const char * cp_chLockFileName = "x86IandC_daemon.lock" ;
//  std::string stdstrCurrentWorkingDir ;
  GetCurrentWorkingDir( stdstrCurrentWorkingDir) ;
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
  LOGN("Current working dir: " << stdstrCurrentWorkingDir )

//  Model model ;
//  DummyUserInterface dummyuserinterface ;
}

//  ReadMainConfigFile()
//  {
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
//  }
