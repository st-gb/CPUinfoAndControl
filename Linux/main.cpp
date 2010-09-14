#include <Controller/CPUcontrolBase.hpp> //class CPUcontrolBase
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
#include <Controller/Logger/Logger.hpp> //class Logger
#include <Controller/character_string/stdtstr.hpp> //std::tstring
#include <Linux/daemon.h> //for daemonize(...)
#include <ModelData/ModelData.hpp> //class Model
#include <UserInterface/DummyUserInterface.hpp> //class DummyUserInterface
#include <Xerces/SAX2MainConfigHandler.hpp> //class SAX2MainConfigHandler
#include <Xerces/XMLAccess.hpp> //ReadXMLdocumentInitAndTermXerces
#include <tchar.h> //_T(...) preprocessor macro

//This global (important for using preprocessor macros) object is used for
//easy logging.
Logger g_logger ;
CPUcontrolBase * gp_cpucontrolbase ;
//Needed for the exported functions.
I_CPUaccess * g_p_cpuaccess ;

//MinGW's g++: ../../Windows/main.cpp:168: error: `main' must return `int'
//void
int main( int argc, char *  argv[] )
{
  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  //LPTSTR ptstrProgramArg = _T("-config=GriffinControl_config.xml") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring(_T("-config=") )
    + ptstrProgramName + std::tstring(_T("_config.xml") ) ;
  //std::string stdstrLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
  std::string stdstrLogFileName = std::string( argv[0] ) +
    std::string( "_log.txt" ) ;
  g_logger.OpenFile2( //std::string("GriffinControl_log.txt")
    stdstrLogFileName ) ;

  Model model ;
  DummyUserInterface dummyuserinterface ;
  std::string stdstrMainConfigFileName = "x86InfoAndControl_config.xml" ;
  //The main cfg also contains the exclusion log message filter--load it in any
  //case.
  SAX2MainConfigHandler sax2mainconfighandler( model, & dummyuserinterface );
  ReadXMLdocumentInitAndTermXerces(
    //const char* xmlFile
    stdstrMainConfigFileName.c_str()
    , model
    , & dummyuserinterface
    //Base class of implementing Xerces XML handlers.
    //This is useful because there may be more than one XML file to read.
    //So one calls this functions with different handlers passed.
    //DefaultHandler & r_defaulthandler
    , sax2mainconfighandler
    ) ;
  LOGN("before daemonizing")
  daemonize("x86IandC_daemon.lock") ;
  LOGN("after daemonizing")
}
