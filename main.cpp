#ifdef _WINDOWS
	//If not included: compiler error "C1010".
	#include "stdafx.h"
#endif //#ifdef _WINDOWS
#include "Console.h"
#include "Controller/PumaStateCtrl.h"
#ifdef COMPILE_WITH_XERCES
 // #include "XMLAccess.h"
#endif

#ifdef WIN32
  #include <process.h> //for beginthreadex
  extern HANDLE g_handleEvent ;
#endif

//For debug logging.
FILE * fileDebug ;

//std::string arstrOption = "-config=";
class CmdLineArgAndpFunction
{
  std::string m_strOption ;
   //CmdLineArgAndpFunction(
   //  const std::string & strOption,
   //  const std::string & strDescription,
   //  //function pointer.
   //  BYTE PumaStateCtrl:: (*pt2Func)())
   //  )
   //{
   //  m_strOption = strOption;
   //}
};

//void Console::showHelp()
//{
//  std::cout<<"Possible options:\n" ;
//
//}

//class PStates;
#ifdef WIN32
void Console::outputOtherRelevantVoltageInfo(//PumaStateCtrl & pstatectrl
  )
{
  BYTE byNbVid, byPstateMaxVal, byAltVid;
  std::string str ;
  if(mp_pumastatectrl->GetValuesOfClockPower_TimingControl2Register(
    byNbVid,byPstateMaxVal,byAltVid))
    printf("NorthBridge VoltageID: %u, maximum value for P-state:%u alternate VID:%u\n", 
      byNbVid,byPstateMaxVal,byAltVid);
  else
    printf("Reading from TimingControl2Register failed\n");
  mp_pumastatectrl->GetValuesOfPowerControlMiscellaneousRegister(str);
  mp_pumastatectrl->GetValuesOfReportedTemperatureControlRegister();
}

//UINT WINAPI 
void ThreadProc( LPVOID pParam )
{
  GriffinController * p_pstatectrl = (GriffinController *)pParam;
  DEBUG("ThreadProc\n");
//  if(p_pstatectrl)
//    p_pstatectrl->messageLoop();
  
  //return 0 ;
}
#endif//#ifdef WIN32

//#ifdef WIN32
#ifdef _WINDOWS
	int _tmain(int argc, _TCHAR* argv[])
#else
	int main(int argc, _TCHAR* argv[])
#endif
{
	fileDebug = fopen ("Pumastatectrl_debug.txt","w");
	DEBUG("main\n");
  //char * str = "";
  Console * p_console = new Console() ;
  //int vid = 0 ;
  //PumaStateCtrl pstatectrl(argc, argv,p_console) ;
  //HINSTANCE hinstanceWinRingDLL;
  //wxString wxstrDLLName=wxT("WinRing0.dll");
  //wxDynamicLibrary wxdynamiclibraryWinRing;
  ////Returns true if the library was successfully loaded, false otherwise.
  //if(wxdynamiclibraryWinRing.Load(//wxstrDLLName
  //  wxstrDLLName
  //  )
  //  )
  //TODO check whether this is a Puma CPU or not.
//  #ifdef WIN32
//  if(pstatectrl.InitWinRing0())
//  {
//  #endif
//    //CmdLineArgAndpFunction arcmdlineargandpfunction [] = {
//    //  CmdLineArgAndpFunction(
//    //    "-config=",
//    //    "load settings from a configuration file",
//    //    &pstatectrl.ReadXMLconfigAndApplyAllPstates())
//    //  );
//    //pstatectrl.handleCmdLineArgs(//argc, //argv
//    //  //pstatectrl
//    //  ) ;
//    //pstatectrl.MyRegisterClass();
//    //DEBUG("after pstatectrl.MyRegisterClass();\n");
//    //pstatectrl.messageLoop();
//    //AfxBeginThread(ThreadProc, &pstatectrl);
//    //UINT uThreadId;
//    //HANDLE hThread = (HANDLE)_beginthreadex(
//    //  NULL //security
//    //  , 0 //stack_size
//    //  , &ThreadProc,
//    //  NULL //arglist
//    //  ,0 //Initial state of a new thread (0 for running)
//    //  , NULL);
//    //HANDLE hThread = (HANDLE)
//    #ifdef WIN32
//    uintptr_t uintptr = _beginthread(&ThreadProc, 0, NULL);
//    //_beginthread returns 1L on an error
//    if(uintptr == 1L)
//      DEBUG("_beginthread(...) failed\n");
//    #endif
//    //{
//    //  wxMessageBox("Function \"" + wxstrFuncName+ "\" available");
//    //InitializeOls();
//#ifdef WIN32
//  }
//  else
//    printf("WinRing0 library failed to load!\n");
//#else
//  //pstatectrl.handleCmdLineArgs();
//#endif
  printf("Press any key to exit\n");
  fgetc(stdin);
	return 0;
}
