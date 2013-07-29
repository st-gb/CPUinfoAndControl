/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * daemon_functions.c
 *
 *  Created on: Dec 30, 2010
 *      Author: sgebauer
 */
#include "daemon_functions.hpp"

#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
#include <Controller/CPUcontrolServiceBase.hpp> //class CPUcontrolServiceBase
#include <Controller/CommandLineParams.h> //IsWithinCmdLineArgs(...)
#include <Controller/Logger/Logger.hpp> //class Logger
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
//GetCurrentWorkingDir(std::string & )
#include <Linux/FileSystem/GetCurrentWorkingDir/GetCurrentWorkingDir.hpp>
#include <Linux/daemon/daemon.h> //daemonize(...)
//class wxConditionBasedI_Condition
#include <wxWidgets/multithread/wxConditionBasedI_Condition.hpp>

#include <signal.h> //for signal(...)

//This global (important for using preprocessor macros) object is used for
//easy logging.
//Logger g_logger ;
CPUcontrolBase * gp_cpucontrolbase = NULL ;
wxConditionBasedI_Condition g_wxconditionbasedi_conditionKeepRunning ;
extern CommandLineArgs<char> g_commandlineargs;

static void child_handler(int nSignalNumber)
{
  switch(nSignalNumber)
  {
  case SIGCONT:
    LOGN("received CONT signal")
    { //This block is needed for the local variable.
//      bool bContinue = false;
      ( (CPUcontrolServiceBase *) gp_cpucontrolbase)->
        StartDVFSviaThreadType(//bContinue
          ) ;
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
    break;
  }
}

#define DO_NOT_DAEMONIZE_ARG "nodaemon"
#define DAEMONIZE_ARG "daemonize"

void PossiblyCloseStdInAndOutFiles()
{
  bool closeStandardInAndOutputFiles = g_commandlineargs.contains(
    "closeStdFiles");
  LOGN( FULL_FUNC_NAME << "-closeStdFiles?: " << closeStandardInAndOutputFiles)
  if( closeStandardInAndOutputFiles)
  {
    LOGN( FULL_FUNC_NAME << "-closing standard input and output files")
    CloseStandardInAndOutputFiles();
  }
}

bool PossiblyDeamonize(const char * cp_chLockFileName)
{
  const bool shouldDeamonize = g_commandlineargs.contains(DAEMONIZE_ARG);
  LOGN( FULL_FUNC_NAME << "-should deamonize? (" << DAEMONIZE_ARG
    << "is specified as command line argument:" << shouldDeamonize)
  if( shouldDeamonize )
  {
//    const char * firstCmdLineArg = gp_cpucontrolbase->m_ar_ar_ch_programArgs[1];
//    LOGN(FULL_FUNC_NAME << "-1st Command line argument: " << firstCmdLineArg)
//    const bool shouldDeamonize = strcmp( firstCmdLineArg,
//      DAEMONIZE_ARG) == 0;
//    LOGN(FULL_FUNC_NAME << "-\"" << DAEMONIZE_ARG << "\" specified as 1st "
//      "command line argument?: " << (shouldDeamonize ? "yes" : "no") )
    if( shouldDeamonize )
    {
      LOGN("before daemonizing")
      //daemonize _after_ the config files were read. because their pathes are
      //_relative_ to the current working dir of this process.
      ::daemonize( //"x86IandC_daemon.lock"
        cp_chLockFileName ) ;
      LOGN("after daemonizing")
    }
  }
  return shouldDeamonize;
}

void PossiblyChangeCurrWorkDir()
{

}

void init_daemon(//int argc, char * argv[],
  std::string & stdstrCurrentWorkingDir )
{
  const char * cp_chLockFileName = "x86IandC_daemon.lock" ;
//  std::string stdstrCurrentWorkingDir ;
  GetCurrentWorkingDir( stdstrCurrentWorkingDir) ;
  signal(SIGTERM, child_handler);
  signal(SIGCONT, child_handler);
  signal(SIGUSR1, child_handler);

  if( ! PossiblyDeamonize(cp_chLockFileName) )
  {
    PossiblyCloseStdInAndOutFiles();
//    PossiblyChangeCurrWorkDir();
  }

  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring(_T("-config=") )
    + ptstrProgramName + std::tstring(_T("_config.xml") ) ;
  //std::string stdstrLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
  LOGN("init_daemon(...)-current working dir: " << stdstrCurrentWorkingDir )
  LOGN("Parent process ID:" << getppid() )
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
