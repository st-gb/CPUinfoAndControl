// PumaStateCtrl.cpp : Definiert den Einstiegspunkt fr die Konsolenanwendung.
//
#define _AFXDLL //for building with "/MD[d]"
//#ifdef _WINDOWS
#ifndef __WXMSW__
  //If not included: compiler error "C1010" under MSVC.
  //WinRing0's stdafx.h includes <windows.h>
  //But this cuases: fatal error C1189: #error :  WINDOWS.H already included.  MFC apps must not #include <windows.h>
	#include "../stdafx.h"
#endif
//#endif //#ifdef _WINDOWS
#include <iostream> //for "cout"
#include "GriffinController.hpp"
#include <preprocessor_helper_macros.h> //for BITMASK_FOR_7BIT
#include <Controller/CPUindependentHelper.h>
#include <Controller/Sleep.h>
#include <Controller/tchar_conversion.h> //for GetCharPointer(...)
#include <ModelData/ModelData.hpp>
#include <ModelData/PStates.h>
#include <Windows/CalculationThread.hpp>
#include <math.h> //for pow(...);
#include <ios> //for ostream formatters "hex" and "dec"
#include <global.h>
#include <iomanip> //std::setprecision(...)
//#undef COMPILE_WITH_XERCES
//#ifdef WIN32 //with Linux I can't compile with xerces yet.
#ifdef _WINDOWS //with Linux I can't compile with xerces yet.
  //#define COMPILE_WITH_XERCES
  //#include <OlsApiInitDef.h> //for CPUid
#else
  #include <Windows_compatible_typedefs.h>
#endif
#ifdef COMPILE_WITH_XERCES
  #include "Xerces/XMLAccess.h" //for "readXMLConfig(...)"
  #include "Xerces/SAX2MainConfigHandler.hpp"
  #include "Xerces/SAX2_CPUspecificHandler.hpp"
#endif
//#include "../ISpecificController.hpp" //for class ReadMSRexception
//need to define this for GetConsoleWindow():
//#define _WIN32_WINNT 0x0500 
#ifndef _MFC_VER //"MFC apps must not #include <windows.h>"
  //#include <windows.h>
#endif
//#include "olsapi.h" //WinRing API
//#include <wx/dynlib.h> //for wxWidgets loading DLLs

typedef unsigned long ULONG;
typedef unsigned long * PULONG;

typedef unsigned long DWORD ;
//#ifndef NTSTATUS
//  typedef DWORD NTSTATUS;
//#endif
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)

//pfnRdMsrExDef g_pfnRdMsrEx ;
//pfnReadIoPortByteEx g_pfnreadioportbyteex = NULL ;
//pfnReadIoPortDWordEx g_pfnreadioportdwordex = NULL ;
//#ifdef WIN32
//HANDLE g_handleEvent ;
//#endif //#ifdef WIN32

using namespace std;

//LRESULT CALLBACK	DialogProc(HWND, UINT, WPARAM, LPARAM);


///* Busy waiting, no better solution found :-(, nanosleep operate like the same to 1 ms */

//void waitStabilisationTime()
//{
//  if(!g_pfnreadioportdwordex)
//    g_pfnreadioportdwordex = (pfnReadIoPortDWordEx)GetProcAddress(
//      g_hinstanceWinRingDLL,"ReadPciConfigDwordEx");
//  if( g_pfnreadioportdwordex )
//  {
//    DWORD dwClockPower_TimingControl1Register ;
//	  if ( 
//      //If the function succeeds, the return value is TRUE. 
//      (*g_pfnreadioportdwordex)(0xD8, &dwClockPower_TimingControl1Register) )
//    {
//      BYTE byStabTime ;
//      WORD wStabilizationTime ;
//      byStabTime = wClockPower_TimingControl1Register&
//        BITMASK_FOR_STABILIZATION_TIME/*AMD: "2:0 VSSlamTime" <=> 1st to 3rd bit */ ;
//	    /*AMD:
//	     * "Bits Time  Bits Time
//		    000b 10 us 100b 60 us
//		    001b 20 us 101b 100 us
//		    010b 30 us 110b 200 us
//		    011b 40 us 111b 500 us"
//	     */
//	    //switch(byStabTime)
//	    //{
//	    //  case 0:
//		   //  wStabilizationTime = 10 ;
//		   //  break ;
//	    //  case 1:
//		   //  wStabilizationTime = 20 ;
//		   //  break ;
//	    //  case 2:
//		   //  wStabilizationTime = 30 ;
//		   //  break ;
//	    //  case 3:
//		   //  wStabilizationTime = 40 ;
//		   //  break ;
//	    //  case 0:
//		   //  wStabilizationTime = 60 ;
//		   //  break ;
//	    //  case 0:
//		   //  wStabilizationTime = 100 ;
//		   //  break ;
//	    //  case 0:
//		   //  wStabilizationTime = 200 ;
//		   //  break ;
//	    //  case 0:
//		   //  wStabilizationTime = 500 ;
//		   //  break ;
//	    //}
//		  //if (wStabilizationTime > 100)
//			 // usleep(wStabilizationTime);
//		  //else
//			 // msr_usleep(stabilization_time);
//      //Wait 1 millisecond (> maximum stabilization time).
//      Sleep(1) ;
//    }
//  }
//}

  //If called after the 1st time (by Windows Power event handler function)
  BYTE GriffinController::ApplyAllPStates()
  {
    //DEBUG("void ApplyAllPStates()--address of THIS PumaStateCtrl object:%p\n",
    //  this);
    //DEBUG("void ApplyAllPStates()--address of m_pstates:%p\n",//&m_model.m_pstates);
    //  mp_model->m_pstates);
    return ApplyAllPStates(//m_model.m_pstates);
      mp_model->m_pstates);
  }

  //It seems: setting a p-state for more than 1 core at a time does NOT work.
  //so call this method "n" times if you want the same p-state for "n" cores.
  BYTE GriffinController::changePstate(BYTE byNewPstate,DWORD dwCoreBitmask)
  {
    BYTE byReturn = FAILURE ;

    //Safety check.
    if( byNewPstate < NUMBER_OF_PSTATES )
    {
      DWORD dwMSRlow = byNewPstate ;
  //#ifndef LINK_TO_WINRING0_STATICALLY
  //  fnWrMsrExDef * pfnwrmsrex ;
  //	#ifdef WIN32
  //	  //unsigned char byFreqID ;
  //    std::string strFuncName = "WrmsrEx" ;
  //    pfnwrmsrex = (fnWrMsrExDef *)GetProcAddress(m_hinstanceWinRingDLL,
  //      strFuncName.c_str() );
  //	#else
  //	    //pfnwrmsrex = msr_write ;
  //	    pfnwrmsrex = msr_write ;
  //	#endif//#ifdef WIN32
  //#endif //#ifndef LINK_TO_WINRING0_STATICALLY

  //#ifndef LINK_TO_WINRING0_STATICALLY
  //    if(pfnwrmsrex)
  //    {
  //#endif //#ifndef LINK_TO_WINRING0_STATICALLY
        dwMSRlow = (BYTE) byNewPstate ;
        //DEBUG("For core bitmask %lu: setting to pstate %u\n", dwCoreBitmask, byNewPstate);
        LOG( "For core bitmask " << dwCoreBitmask << ": setting to pstate "
            << byNewPstate << "\n" );
        //DEBUG("the low 32 bits: %s\n", getBinaryRepresentation(arch,dwMSRlow) );
	      //printf("  would write:  %s to MSR %lx\n", getBinaryRepresentation(&msrvalue,arch), msr_register_number);
	      //waitForEnter("um in MSR zu schreiben") ;
	      //if ((msr_write(msrfile, msr_register_number, &msrvalue)) != OK)
	      //	printf("MSR write failed\n");
  //#ifndef LINK_TO_WINRING0_STATICALLY
  //      if((*pfnwrmsrex)(
  //#else
  //      if( WrmsrEx (
  //#endif //#ifndef LINK_TO_WINRING0_STATICALLY
        //DEBUG("Adress of mp_cpuaccess: %lx\n", mp_cpuaccess);
#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
        if(
          //CONTROLLER_PREFIX
          mp_cpuaccess->
          WrmsrEx(
            P_STATE_CONTROL_REGISTER,
            dwMSRlow,
            0,
            dwCoreBitmask
            )
          )
        {
          DEBUG("Setting p-state succeeded\n");
          byReturn = SUCCESS ;
          //Wait 1 millisecond (> maximum stabilization time).
          SLEEP_1_MILLI_SECOND
        }
        else
        {
          DEBUG("Setting p-state failed\n");
        }
        byReturn = SUCCESS ;
#endif //_EMULATE_TURION_X2_ULTRA_ZM82
  //#ifndef LINK_TO_WINRING0_STATICALLY
  //    }
  //#endif //#ifdef LINK_TO_WINRING0_STATICALLY
    }
    return byReturn ;
  }

  bool GriffinController::cmdLineParamsContain(
    _TCHAR * ptcharOption
    , std::string & strValue
    )
  {
    bool bcmdLineParamsContain = false ;
    int nIndex = 1 ;
    signed short wPos = 0 ;
    DEBUG("cmdLineParamsContain begin\n");
    for ( ;nIndex < m_byNumberOfCmdLineArgs ; ++ nIndex )
    {
      std::string strCmdArg( GetCharPointer( m_arartcharCmdLineArg[ nIndex ] ) );
      wPos = (WORD) strCmdArg.find( std::string( GetCharPointer( 
        ptcharOption ) ) +
        //TCHAR("=")
        std::string("=") ) ;
      if( wPos != std::string::npos && wPos == 0 )
      {
//#ifdef WIN32
#ifdef MS_COMPILER
        strValue = strCmdArg.substr(_tcslen(ptcharOption)
          //Start after "="
          +1);
#else
        strValue = strCmdArg.substr( //_tcslen(): _UNICODE: ->wcslen(), else ->strlen()
          _tcslen(ptcharOption)
          //Start after "="
          + 1 );
#endif //#ifdef WIN32
        bcmdLineParamsContain = true ;
        break ;
      }
    }
    DEBUG("cmdLineParamsContain end\n");
    return bcmdLineParamsContain ;
  }

  bool GriffinController::cmdLineParamsContain(_TCHAR * ptcharOption, BYTE & rbyOption)
  {
    bool bcmdLineParamsContain = false ;
    int nIndex = 1 ;
	//DEBUG("cmdLineParamsContain(%s)\n",ptcharOption);
    LOG( "Checking if the command line parameters oontain the string \""
        << ptcharOption << "\"\n" );
    for ( ;nIndex < m_byNumberOfCmdLineArgs ; ++ nIndex )
    {
//#ifdef WIN32
#ifdef _WINDOWS
      if(_tcscmp(ptcharOption,m_arartcharCmdLineArg[nIndex]) == 0 )
#else
	  //DEBUG("cmdLineParamsContain: %s\n",m_arartcharCmdLineArg[nIndex]);
      //LOG( "cmdLineParamsContain: " << m_arartcharCmdLineArg[nIndex] << "\n" );
      if(strcmp(ptcharOption,m_arartcharCmdLineArg[nIndex]) == 0 )
#endif
        if( nIndex < ( m_byNumberOfCmdLineArgs - 1) )
        {
//#ifdef WIN32
#ifdef MS_COMPILER
          rbyOption = _tstoi(m_arartcharCmdLineArg[nIndex +1 ] ) ;
#else
          rbyOption = atoi( GetCharPointer( m_arartcharCmdLineArg[ 
            nIndex + 1 ] ) ) ;
#endif
          bcmdLineParamsContain = true ;
          break ;
        }
    }
    return bcmdLineParamsContain ;
  }

#ifdef WIN32

  void GriffinController::DisableFrequencyScalingByOS()
  {
    DEBUG("calling specific OS's DVFS disabler\n");
    if( mp_dynfreqscalingaccess )
      m_bFrequencyScalingByOSDisabled = mp_dynfreqscalingaccess->
          DisableFrequencyScalingByOS();
  }//void DisableFrequencyScaling()

  void GriffinController::EnableFrequencyScalingByOS()
  {
    DEBUG("calling specific OS's DVFS disabler\n");
    //m_bFrequencyScalingByOSDisabled = 
    if( mp_dynfreqscalingaccess )
      mp_dynfreqscalingaccess->EnableFrequencyScalingByOS();
    m_bFrequencyScalingByOSDisabled = false ;
    //return true ;
  }//void DisableFrequencyScaling()

  BYTE GriffinController::GetVoltageIDFromVoltageInVolt(float fVoltageInVolt)
  {
    return PState::GetVoltageID(fVoltageInVolt) ;
  }

//LRESULT CALLBACK //PumaStateCtrl::
//  DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//  DEBUG("DialogProc\n");
//	switch (message)
//  {
//		case WM_POWERBROADCAST:
//
//			switch (wParam)
//			{
//				case (PBT_APMRESUMESUSPEND):
//					//AfxMessageBox("OS-Event: Resume Event ( PBT_APMRESUMESUSPEND ) Occured & Captured.\nSystem is resuming after suspension.", "OS-Event", MB_ICONWARNING);
//          cout << "PBT_APMRESUMESUSPEND" << endl;
//          //m_event.SetEvent()
//          SetEvent(g_handleEvent);
//					break;
//
//				case (PBT_APMRESUMEAUTOMATIC):
//					//AfxMessageBox("OS-Event: Automatic Resume Event ( PBT_APMRESUMEAUTOMATIC ) Occured & Captured.\nSystem is resuming after suspension.", "OS-Event", MB_ICONWARNING);
//          cout << "PBT_APMRESUMEAUTOMATIC" << endl;
//          //SetEvent(m_handleEvent);
//          SetEvent(g_handleEvent);
//					break;
//
//      }
//  }
//  return 0;
//}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
//ATOM PumaStateCtrl::MyRegisterClass(//HINSTANCE hInstance
//                     )
//{
//  ATOM atom ;
//  WNDCLASSEX wcex;
//  HWND hwnd ;
//  DEBUG("PumaStateCtrl::MyRegisterClass begin\n");
//  hwnd = ::GetConsoleWindow();
//  if(hwnd== NULL)
//  {
//    DEBUG("there is no such associated console.\n");
//  }
//  else
//    DEBUG("hwnd: %lu\n", //(DWORD)
//      hwnd);
//  DEBUG("PumaStateCtrl::MyRegisterClass before ::GetModuleHandle(NULL)\n");
//  //The following is only for getting standby and hibernate 
//  //notifications for console apps (but it does not work yet).
//#ifndef X64 //does not compile under x64 because of 
//  //"Fehler	2	error C2065: 'GWL_HINSTANCE': nichtdeklarierter Bezeichner	"
//  m_hinstanceThisModule = (HINSTANCE) ::GetWindowLong(
//    hwnd,
//    GWL_HINSTANCE//Retrieves a handle to the application instance.
//    );
//#endif
//  ::SetWindowLongPtr(hwnd,GWLP_WNDPROC, (LONG)&DialogProc);
//  //If the function fails, the return value is zero.
//  if(m_hinstanceThisModule == 0 )
//  {
//    DEBUG("::GetWindowLong(...) failed\n");
//  }
//  else
//    DEBUG("::GetWindowLong(...) succeeded\n");
//  //m_hinstanceThisModule = //(HANDLE)
//  //  ::GetModuleHandle(NULL); //If this parameter is NULL, GetModuleHandle returns a handle to the file used to create the calling process (.exe file).
//  ////If the function fails, the return value is zero.
//  //if(m_hinstanceThisModule == 0 )
//  //{
//  //  DEBUG("::GetModuleHandle(...) failed\n");
//  //}
//  //else
//  //  DEBUG("::GetModuleHandle(...) succeeded\n");
//
//  DEBUG("PumaStateCtrl::MyRegisterClass begin\n");
//  wcex.cbSize = sizeof(WNDCLASSEX); 
//
//  wcex.style			= CS_HREDRAW | CS_VREDRAW;
//  //wcex.lpfnWndProc	= (WNDPROC)WndProc;
//  wcex.lpfnWndProc	= (WNDPROC)DialogProc;
//  wcex.cbClsExtra		= 0;
//  //Reserve extra window memory by specifying a nonzero value in the 
//  //cbWndExtra member of the WNDCLASSEX structure used with the RegisterClassEx function.
//  wcex.cbWndExtra		= 0;
//  //wcex.hInstance		= hInstance;
//  wcex.hInstance		= m_hinstanceThisModule ;
//  //wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_OS_EVENTS);
//  wcex.hIcon = 0 ;
//  //wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//  wcex.hCursor		= 0;
//  wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
//  //wcex.lpszMenuName	= (LPCSTR)IDC_OS_EVENTS;
//  wcex.lpszMenuName	= 0;
//  //wcex.lpszClassName	= szWindowClass;
//  wcex.lpszClassName	= _T("PumaStateCtrl");
//  //wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//  wcex.hIconSm		= 0;
//
//  DEBUG("before RegisterClassEx(&wcex)\n");
//  atom = RegisterClassEx(&wcex);
//  //If the function fails, the return value is zero.
//  if( atom == 0 )
//    DEBUG("RegisterClassEx(...) failed. ::GetLastError():%lu\n",
//      ::GetLastError());
//  DEBUG("PumaStateCtrl::MyRegisterClass before return\n");
//  return atom ;
//}
#endif//#ifdef WIN32

//Method that does the essential things and thus should be called in 
//the body of other constructors of this class->avoid redundancy.
BYTE GriffinController::Init(//Model * mp_model
                    )
{
  //mp_model->SetGriffinController(this) ;
  mp_model->SetCPUcontroller(this);
  mp_model->SetNumberOfCPUCores( mp_cpuaccess->GetNumberOfCPUCores() );
  marp_calculationthread = new ICalculationThread * [ 
      mp_model->GetNumberOfCPUCores() ] ;
  if( marp_calculationthread )
    memset(marp_calculationthread, 0, mp_model->GetNumberOfCPUCores() * 
        sizeof(ICalculationThread * ) 
    ); 
  
  DWORD dwEAX, dwEDX;
//#ifndef EMULATE_EXECUTION_AS_SERVICE
#ifndef EMULATE_TURION_X2_ULTRA_ZM82
  //TODO the hardware access may be done after "restrict to this model" 
  //(safety if (accidentally) used with not supported CPU ) at first?!
  if(//mp_pstatectrl->
    RdmsrEx(
    COFVID_STATUS_REGISTER,
    dwEAX,			// bit  0-31
    dwEDX,			// bit 32-63
    1	// Thread Affinity Mask
      )
    )
  {
    //m_modelData.m_cpucoredata.m_byMaxVoltageID = 
    mp_model->m_cpucoredata.m_byMaxVoltageID = 
      //"48:42 MinVid: minimum voltage." (HIGHEST VID 
      //sent to voltage regulator module by CPU)->7 bit
      ( dwEDX >> 
        //( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VOLTAGE - 32 ) 
        ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VID - 32 )
      ) & BITMASK_FOR_LOWMOST_7BIT ;
    //m_modelData.m_cpucoredata.m_byMinVoltageID = 
    mp_model->m_cpucoredata.m_byMinVoltageID = 
      //"41:35 MaxVid: maximum VOLTAGE." (lowest VID
      //sent to voltage regulator module by CPU)->7 bit
      ( dwEDX >> 
        //( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VOLTAGE - 32 ) 
        ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
      ) & BITMASK_FOR_LOWMOST_7BIT ;
    //m_modelData.m_cpucoredata.m_byMainPLLoperatingFrequencyIDmax =
    mp_model->m_cpucoredata.//m_byMainPLLoperatingFrequencyIDmax =
      SetMainPLLoperatingFrequencyIDmax(
      //"41:35 MaxVid: maximum VOLTAGE." (lowest VID
      //sent to voltage regulator module by CPU)->7 bit
      ( dwEDX >> 
        ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAIN_PLL_OP_FREQ_ID_MAX - 32 ) 
      ) & BITMASK_FOR_LOWMOST_7BIT 
      ) ;
    //DEBUG("%s-> max. VID:%u min VID:%u\n",
    //  ::getBinaryRepresentation(dwEDX).c_str()
    //  ////"48:42 MinVid: minimum VOLTAGE."->7 bit
    //  //,( dwEDX >> 
    //  //  ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VOLTAGE - 32 ) 
    //  //) & BITMASK_FOR_LOWMOST_7BIT
    //  //, m_modelData.m_cpucoredata.m_byMaxVoltageID
    //  , mp_model->m_cpucoredata.m_byMaxVoltageID
    //  ////"41:35 MaxVid: maximum VOLTAGE (lowest VID)."->7 bit
    //  //,( dwEDX >> 
    //  //  ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VOLTAGE - 32 ) 
    //  //) & BITMASK_FOR_LOWMOST_7BIT
    //  //, m_modelData.m_cpucoredata.m_byMinVoltageID
    //  , mp_model->m_cpucoredata.m_byMinVoltageID
    //  );
    LOG( ::getBinaryRepresentation(dwEDX).c_str() 
        << "-> maximum Voltage ID:" << 
        //Convert to integer in order to be output as number and not as (ASCII) character.
        (WORD) mp_model->m_cpucoredata.m_byMaxVoltageID 
        << " minimum Voltage ID:" << 
        //Convert to integer in order to be output as number and not as (ASCII) character.
        (WORD) mp_model->m_cpucoredata.m_byMinVoltageID 
        << "\n" );
  }
#endif
  return 1 ;
}

//version without user interface parameter (so it can be called before 
//the user interface is created.
GriffinController::GriffinController(
  int argc
  , _TCHAR * argv[]
  , Model & m_modelData 
  , //ISpecificController 
    I_CPUaccess * p_cpuaccess 
  , ICalculationThread * p_calculationthread 
  , IDynFreqScalingAccess & p_dynfreqscalingaccess 
  )
  //C++ style initialisations
  : 
  m_byNumberOfCmdLineArgs(argc) 
  , m_arartcharCmdLineArg ( argv )
  //, mp_cpuaccess(p_cpuaccess)
  , mp_calculationthread(p_calculationthread)
  , m_bPstateSet(false)
  , m_byPstateID(0)
  , mp_dynfreqscalingaccess(&p_dynfreqscalingaccess)
  , mp_model ( & m_modelData )
  , mp_userinterface(NULL)
  , m_bFrequencyScalingByOSDisabled(false)
{
  mp_cpuaccess = p_cpuaccess ;
  //mp_userinterface->mp_pumastatectrl = this ;
  //mp_model->SetNumberOfCPUCores( mp_cpuaccess->GetNumberOfCPUCores() );
  //PumaStateCtrl();
  Init();
}

GriffinController::GriffinController(
  int argc, 
  _TCHAR * argv[] , 
  //This param must be a pointer because we want to pass concrete classes
  //inherited from the abstract class "UserInterface".
  UserInterface * p_userinterface ,
  Model & m_modelData ,
  //ISpecificController 
    I_CPUaccess * p_cpuaccess ,
  ICalculationThread * p_calculationthread ,
  IDynFreqScalingAccess & p_dynfreqscalingaccess 
  )
  //: m_pstates(NULL)
  : 
  //mp_cpuaccess(p_cpuaccess)
  mp_calculationthread(p_calculationthread)
  , m_bPstateSet(false)
  , m_byPstateID(0)
  , mp_dynfreqscalingaccess(&p_dynfreqscalingaccess)
  , m_bFrequencyScalingByOSDisabled(false)
{
  mp_cpuaccess = p_cpuaccess ;
  m_byNumberOfCmdLineArgs = argc ;
  m_arartcharCmdLineArg = argv ;
  //m_pstates = NULL ;
//#ifdef _DEBUG
//  TRACE("Address of argv: %lx %lx\n",argv,&argv);
//  if ( argc > 1 )
//    TRACE("Adress of argv[1]: %lx %lx\n",argv[1], &argv[1] );
//#endif
  mp_userinterface = p_userinterface ;
  //mp_userinterface->mp_pumastatectrl = this ;
//#ifdef WIN32
//  //m_handleEvent = CreateEvent(
//  //g_handleEvent = CreateEvent(
//  //  NULL, 
//  //  FALSE,  //bManualReset
//  //  FALSE,  //bInitialState
//  //  NULL //LPTSTR lpName 
//  //);
//#else
//  g_pfnRdMsrEx = readMSR ;
//#endif
  //m_model.SetNumberOfCPUCores( mp_cpuaccess->GetNumberOfCPUCores() );
  mp_model = & m_modelData ;
  //PumaStateCtrl();
  Init();
}


GriffinController::~GriffinController()
{
#ifdef _WINDOWS
  //If deinitialized here it is ensured for any kind of view /user interface.
  //DeInitWinRing0() ;
#endif //#ifdef _WINDOWS

    //Release memory.
    if( marp_calculationthread )
    {
        BYTE byNumberOfCPUcores = mp_model->GetNumberOfCPUCores() ;
      for( BYTE byCPUcoreIndex = 0 ; byCPUcoreIndex < byNumberOfCPUcores ;
          ++ byCPUcoreIndex )
      {
          if( marp_calculationthread[ byCPUcoreIndex ] )
              delete marp_calculationthread[ byCPUcoreIndex ] ;
      }
      delete [] marp_calculationthread ;
    }
}

bool GriffinController::ApplyAllPStates(const PStates & pstates)
{
  //bool bSuccess = false ;
  bool bSuccess = true ;
  BYTE byIndex = 0; 
  //BYTE byCoreIndex = 0 ;
  DWORD dwLow;
  DWORD dwHigh;
  BYTE byCoreID ;
  PState * p_pstateCurrent ;
  PState pstateFromMSR ;
  //DEBUG("void PSC::ApplyAllPStates(const PStates & pstates) begin\n");
  LOG("Apply all p-states--begin\n");
  //DEBUG("void PSC::ApplyAllPStates(const PStates &) address of "
  //  "pstates.m_arp_pstate:%p\n", pstates.m_arp_pstate);
  //LOG( "void PSC::ApplyAllPStates(const PStates &) address of "
  //  "pstates.m_arp_pstate:" << hex << pstates.m_arp_pstate << 
  //  //Reset output to decimal.
  //  dec << "\n" );
  BYTE byCoreCount = mp_model->GetNumberOfCPUCores() ;
  //BYTE byCoreCount = m_model.GetNumberOfCPUCores() ;
  //m_pstates = & pstates;
  //TODO determine number of cores by "CPUID Fn8000_0008 ECX 7:0 NC: number of physical cores - 1."
  for( ; byIndex < NUMBER_OF_PSTATES ; ++ byIndex )
  {
    //DEBUG("void PSC::ApplyAllPStates(const PStates &) p-state index:%u\n", byIndex);
    LOG( "apply all p-states: p-state index:" << (WORD) byIndex << "\n" );
    p_pstateCurrent = pstates.m_arp_pstate[byIndex] ;
    //DEBUG("void PSC::ApplyAllPStates(const PStates &) p_pstateCurrent's "
    //  "address:%p\n", p_pstateCurrent);
    //LOG("void PSC::ApplyAllPStates(const PStates &) p_pstateCurrent's "
    //  "address:" << p_pstateCurrent << "\n" );
    //Is NULL if none exits for a p-state ID.
    if( p_pstateCurrent )
    {
      for( 
        //Reset to 0 for each p-state
        //TODO get the number of cores from CPU register (CPUID or MSR)
        byCoreID = 0 ; 
        byCoreID < byCoreCount ; ++ byCoreID )
        if( GetPStateFromMSR(//p_pstateCurrent->m_byNumber
          byIndex,dwLow,dwHigh,
          pstateFromMSR,byCoreID) )
        {
          //if(pstates.IsSafe(pstateCurrent) )
          //if(p_pstateCurrent->IsSafeInCombinationTo(pstateFromMSR) )
          p_pstateCurrent->AssignChangesToPstateFromMSR(pstateFromMSR);
          //p_pstateCurrent->AssignChangesFromPstateFromMSR(pstateFromMSR);
          if( WriteToPstateOrCOFVIDcontrolRegister(
            GetMSRregisterForPstate(p_pstateCurrent->m_byNumber),
            //pstateFromMSR.m_byVoltageID,
            //pstateFromMSR.m_byFreqID,
            //pstateFromMSR.m_byDivisorID,
            pstateFromMSR,
            *p_pstateCurrent,
            dwHigh,
            dwLow,
            byCoreID)
            )
            //bSuccess = true;
            ;
          //The p-state change only works if the p-state is set as current p-state.
          //This issue also stands in the AMD documentation.
          //The following line solves this issue.
          SetPstate(p_pstateCurrent->m_byNumber, byCoreID ) ;
        }
        else
        {
          //mp_userinterface->Confirm("Getting p-state info "
          //  "failed. This program needs elevated privileges for ring 0 "
          //  "access. So run it as administrator.");
          //fullfill outer loop break condition.
          byIndex = NUMBER_OF_PSTATES ;
          bSuccess = false;
          break;
        }
      //To show the values of merged p-state  from MSR and from user 
      //in the view / user interface.
      //*p_pstateCurrent = pstateFromMSR ;
      //Use "AssignChangesFromPstateFromMSR(...)" because it only changes 
      //values that correspond to MSR (and NOT other attributes like 
      //"m_bIsSafe" etc.)
      p_pstateCurrent->AssignChangesFromPstateFromMSR(pstateFromMSR);
    }//if( p_pstateCurrent )
  }//Loop for alle possible p-states.
  return bSuccess ;
}


  void GriffinController::PerformanceEventSelectRegisterWrite(
    DWORD dwAffinityBitMask ,
    //Griffin has 4 "Performance Event Select Register" from 
    //  MSR 0xC001_0000_00 to MSRC001_00_03  for 
    // 4 "Performance Event Counter Registers" from 
    //  MSR 0xC001_0004 to 0xC001_0007 
    //  that store the 48 bit counter value
    BYTE byPerformanceEventSelectRegisterNumber ,
    WORD wEventSelect ,
    BYTE byCounterMask ,
    bool bInvertCounterMask ,
    bool bEnablePerformanceCounter,
    bool bEnableAPICinterrupt,
    bool bEdgeDetect,
    bool bOSmode,
    bool bUserMode,
    BYTE byEventQualification
  )
  {
    DWORD dwLow = 0 |
      ( byCounterMask << 24 ) |
      ( bInvertCounterMask << 23 ) |
      ( bEnablePerformanceCounter << 22 ) |
      ( bEnableAPICinterrupt << 20 ) |
      ( bEdgeDetect << 18 ) |
      ( bOSmode << 17 ) |
      ( bUserMode << 16 ) |
      ( byEventQualification << 8 ) |
      ( wEventSelect & BITMASK_FOR_LOWMOST_8BIT )
      ;
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
#else
    mp_cpuaccess->WrmsrEx(
      PERF_CTL_0 + byPerformanceEventSelectRegisterNumber ,
      dwLow ,
      wEventSelect >> 8 ,
      //1=core 0
      //1
      dwAffinityBitMask 
      ) ;
#endif //_EMULATE_TURION_X2_ULTRA_ZM82
  }

  //this method is for this purpose: 
  //"To accurately start counting with the write that enables the counter, 
  //disable the counter when changing the event and then enable the counter 
  //with a second MSR write."
  void GriffinController::AccuratelyStartPerformanceCounting(
    DWORD dwAffinityBitMask ,
    BYTE byPerformanceCounterNumber ,
    WORD wEventSelect ,
    bool bInvertCounterMask
    )
  {
    PerformanceEventSelectRegisterWrite(
      dwAffinityBitMask ,
      byPerformanceCounterNumber ,
      wEventSelect ,
      //0x0C1 ,
//      0x076 ,
      //byCounterMask: 00h: The corresponding PERF_CTR[3:0] register is incremented by the number of events
      //occurring in a clock cycle. Maximum number of events in one cycle is 3.
      //0,
      //"When Inv = 0, the corresponding PERF_CTR[3:0] register is 
      //incremented by 1, if the number of events occurring in a clock 
      //cycle is greater than or equal to the CntMask"
      1 ,
      //bInvertCounterMask
      0,
      //bEnablePerformanceCounter
      0,
      //bEnableAPICinterrupt
      0,
      //bEdgeDetect
      0,
      //bOSmode[...]1=Events are only counted when CPL=0. 
      //(CPL=Current Privilege Level?) http://en.wikipedia.org/wiki/Current_privilege_level
      1,
      //bUserMode
      1,
     //byEventQualification
      0
      ) ;
    PerformanceEventSelectRegisterWrite(
      dwAffinityBitMask ,
      byPerformanceCounterNumber ,
      //wEventSelect
      //0x0C1 ,
      wEventSelect ,
//      0x076 ,
      //byCounterMask: 00h: The corresponding PERF_CTR[3:0] register is incremented by the number of events
      //occurring in a clock cycle. Maximum number of events in one cycle is 3.
      0,
      //bInvertCounterMask
      0,
      //bEnablePerformanceCounter
      1,
      //bEnableAPICinterrupt
      0,
      //bEdgeDetect
      0,
      //bOSmode
      //0
      true
      ,
      //bUserMode
      true
      ,
     //byEventQualification
      0
      ) ;
  }
  //float
  ULONGLONG GriffinController::GetCurrentCPUload(BYTE byCPUcoreID)
  {
#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
    //AMD Basic Performance Measurements for AMD Athlon™ 64,
    //AMD Opteron™ and AMD Phenom™ Processors:
    //chapter 4.2.1. Instructions per cycle (IPC).
    DWORD dwLow, dwHigh;
    mp_cpuaccess->RdmsrEx(
      PERF_CTR_0,
      & dwLow,
      & dwHigh,
      //1=core 0
      //1
      byCPUcoreID 
      ) ;
    ULONGLONG ull = dwHigh ;
    ull <<= 32 ;
    ull |= dwLow ;
    float f = //dwHigh
      ull ;
//    f *= 4294967296.0f ,
//    f += dwLow ;
    float fDiff = f - mp_model->m_dPreviousPERF_CTRvalue ;
    mp_model->m_dPreviousPERF_CTRvalue = f ;
//    f = dwLow ;
    //BKDG: 
    //"To accurately start counting with the write that enables the counter,
    //disable the counter when changing the
    //event and then enable the counter with a second MSR write."
//    mp_cpuaccess->WrmsrEx(PERF_CTL_0,
//      //1=core 0
//      1) ;

    //EventSelect 0C0h Retired Instructions
//    EventSelect 0C1h Retired uops
//    EventSelect 076h CPU Clocks not Halted
//    EventSelect 0E8h Thermal Status
    WORD wEventSelect = //0x0C0
      0x0C1
      //0xC0 //Retired Instructions
      ;
    AccuratelyStartPerformanceCounting(
      1 << byCPUcoreID ,
      PERF_CTR_0 ,
      wEventSelect ,
      false );

//    DWORD dwPerfCounterValue ;
    //LARGE_INTEGER liPerfCounterValue ;
    //::SetThreadAffinityMask(::GetCurrentThread(), 0);
    //::QueryPerformanceCounter(&liPerfCounterValue);
    DWORD dwMilliSecondsEleapsedSinceOSstart = ::GetTickCount();
    //if( GetTickCount() - dwStart >= TIMELIMIT )

    return //f
      ull
      //mp_model->m_dPreviousPERF_CTRvalue
      //fDiff ;
//      dwPerfCounterValue 
      ;
#else
    return 0;
#endif //_EMULATE_TURION_X2_ULTRA_ZM82
  }

  //Used to get values from 
  //-p-state registers
  //-COFVID Status Register
  //-MSRC001_0070 COFVID Control Register
  void GriffinController::GetVIDmFIDnDID(
    DWORD dwLowmostMSRvalue,
    PState & r_pstate
    )
  {
    r_pstate.m_byFreqID = (BYTE)(dwLowmostMSRvalue & 63) ;
    r_pstate.m_byDivisorID = (BYTE) (
      (dwLowmostMSRvalue & 448//=111000000bin
      )>>START_BIT_FOR_CPU_CORE_DIVISOR_ID) ; //<=>bits 6-8 shifted to     }
    r_pstate.m_byVoltageID = (BYTE) (
      (dwLowmostMSRvalue & BITMASK_FOR_CPU_CORE_VOLTAGE_ID//=1111111000000000bin
      )>>9) ; //<=>bits 9-15 shifted   }
  }

  //Overrides virtual BYTE GetCurrentPstate(...) from base class.
  BYTE GriffinController::GetCurrentPstate(
    WORD & r_wFreqInMHz 
    , float & r_fVoltageInVolt
    , BYTE byCoreID 
    )
  {
	  DWORD dwIndex ;		// MSR index
	  DWORD dwLowmostBits ;			// bit  0-31 (register "EAX")
	  DWORD dwHighmostBits ;			// bit 32-63 (register "EDX")
	  DWORD_PTR dwAffinityMask ;	// Thread Affinity Mask
    if( RdmsrEx(
	    COFVID_STATUS_REGISTER ,		// MSR index
	    dwLowmostBits,			// bit  0-31 (register "EAX")
	    dwHighmostBits,			// bit 32-63 (register "EDX")
	    1 << byCoreID //dwAffinityMask	// Thread Affinity Mask
        )
      )
    {
      PState pstate ;
      GetVIDmFIDnDID(dwLowmostBits,pstate ) ;
      r_wFreqInMHz = pstate.GetFreqInMHz() ;
      r_fVoltageInVolt = pstate.GetVoltageInVolt() ;
    }
    return 0 ; 
  }

  int GriffinController::GetPStateFromMSR(
    BYTE byPStateID, 
    DWORD & dwLow, 
    DWORD & dwHigh, 
    PState & r_pstate, 
    BYTE byCoreID //core 0 = first core
    //BYTE byThreadAffinityBitmask 
    )
  {
    DWORD dwRegisterNumber = GetMSRregisterForPstate(byPStateID) ;
    DWORD dwThreadAffinityBitmask = 1 << byCoreID ;
    //DEBUG("GPSFMSR for p-state #%u:"// will Read from MSR register #%lx
    //  "\n", byPStateID//, dwRegisterNumber
    //  );
    LOG("Getting p-state from MSR for p-state # " << 
        //Cast to WORD in order to put out as number and not as ASCII char
        //for the number(=ASCII code).
        (WORD) byPStateID << ":"// will Read from MSR register #%lx
        "\n" );
    //If the function succeeds, the return value is TRUE.
    int nReturn = 
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
      TRUE ;
#else
      //#ifndef LINK_TO_WINRING0_STATICALLY
//      (*g_pfnRdMsrEx)
//#else
//      RdmsrEx
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
      mp_cpuaccess->
      RdmsrEx
      (dwRegisterNumber, &dwLow, &dwHigh, 
      //byThreadAffinityBitmask 
      dwThreadAffinityBitmask
      ) ;
#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    switch(dwRegisterNumber)
    {
    case COFVID_STATUS_REGISTER:
      dwLow = ( ( 64 << 9 | 2 ) << 6 ) | 14;
      dwHigh = 
        //MainPllOpFreqIdMax
        ( 14 << 17 )
        //minimum voltage (highest VID code).
        | ( 64 << 10 )
        //maximum voltage.
        | ( 28 << 3 )
        //startup P-state number
        | 0 
        ;
      break ;
    case MSR_P_STATE_0:
      //set the default values for p-state 1 for a ZM-82 CPU
      dwLow = ( 
          //Voltage ID
          36 << 9 ) | ( 
          //Divisor ID
          0 << 6 ) | 
          //Frequency ID
          14;
      //1=The P-state specified by this MSR is valid.
      dwHigh = 1 << 31 ;
      break ;
    case MSR_P_STATE_1:
      //set the default values for p-state 1 for a ZM-82 CPU
      dwLow = ( 
          //Voltage ID
          48 << 9 ) | ( 
          //Divisor ID
          1 << 6 ) | 
          //Frequency ID
          14;
      //1=The P-state specified by this MSR is valid.
      dwHigh = 1 << 31 ;
      break ;
    case MSR_P_STATE_2:
      //set the default values for p-state 1 for a ZM-82 CPU
      dwLow = ( 
          //Voltage ID
          64 << 9 ) | ( 
          //Divisor ID
          2 << 6 ) | 
          //Frequency ID
          14;
      //1=The P-state specified by this MSR is valid.
      dwHigh = 1 << 31 ;
      break ;
    case MSR_P_STATE_3:
      //set the default values for p-state 1 for a ZM-82 CPU
      dwLow = ( 
          //Voltage ID
          48 << 9 ) | ( 
          //Divisor ID
          1 << 6 ) | 
          //Frequency ID
          14;
      //1=The P-state specified by this MSR is valid.
      dwHigh = 1 << 31 ;
      break ;
    case MSR_P_STATE_4:
    case MSR_P_STATE_5:
      {
      DIDandFID didandfid = GetNearestPossibleFreqInMHzAsDIDnFID(800) ;
      //set the default values for p-state 1 for a ZM-82 CPU
      dwLow = ( 
          //Voltage ID
          PState::GetVoltageID(0.95) << 9 ) | ( 
          //Divisor ID
          didandfid.m_byDivisorID << 6 ) | 
          //Frequency ID
          didandfid.m_byFreqID ;
      //1=The P-state specified by this MSR is valid.
      dwHigh = 1 << 31 ;
      }
      break ;
    case MSR_P_STATE_6:
    case MSR_P_STATE_7:
      {
      DIDandFID didandfid = GetNearestPossibleFreqInMHzAsDIDnFID(1100) ;
      //set the default values for p-state 1 for a ZM-82 CPU
      dwLow = ( 
          //Voltage ID
          PState::GetVoltageID(1.2) << 9 ) | ( 
          //Divisor ID
          didandfid.m_byDivisorID << 6 ) | 
          //Frequency ID
          didandfid.m_byFreqID ;
      //1=The P-state specified by this MSR is valid.
      dwHigh = 1 << 31 ;
      }
      break ;
    }
#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    if( nReturn )
    {
      DEBUG("Reading from MSR succeeded\n");
      GetVIDmFIDnDID(dwLow, r_pstate) ;

      //DEBUG("GPSFMSR(...)--FID:%u DivID:%u -> %u MHz; VID:%u -> %f V\n",
      //  r_pstate.m_byFreqID,r_pstate.m_byDivisorID,
      //  r_pstate.GetFreqInMHz(), r_pstate.m_byVoltageID, 
      //  r_pstate.GetVoltageInVolt() );
      LOG("Get p-state from MSR--Frequency ID:" << (WORD) r_pstate.m_byFreqID 
          << " Divisor ID:" << 
          //Convert to integer in order to output as number 
          (WORD) r_pstate.m_byDivisorID 
          << " -> " << r_pstate.GetFreqInMHz() 
          << " MHz; Voltage ID:" << 
          //Convert to integer in order to output as number 
          (WORD) r_pstate.m_byVoltageID 
          << " -> " << r_pstate.GetVoltageInVolt() << " V\n" );
    }
    else
    {
      DEBUG("Reading from MSR failed\n");
      //printf("Reading from MSR failed. This program uses a DLL to comminicate with the CPU for higher privileges. Does the file \"WinRing0.sys\" lay within the same directory as \"WinRing0.dll\"?\n");

      //Throw an exception because this is easier to handle among multiple
      //call hierarchies.
      throw ReadMSRexception() ;
    }
    return nReturn ;
  }

BYTE GriffinController::handleCmdLineArgs(//int argc// _TCHAR* argv[]
  //,PumaStateCtrl & pstatectrl
  )
{
  BYTE byReturn = FAILURE ;
  //DEBUG("handling command line arguments--command line argument count "
  //  "including implicit arguments:%u\n",m_byNumberOfCmdLineArgs);
  LOG( "Handling command line arguments--command line argument count "
    "including implicit arguments:" << m_byNumberOfCmdLineArgs << "\n" );
  //mp_userinterface->Confirm("jjj");
  if(m_byNumberOfCmdLineArgs == 1 )
  {
//#ifdef WIN32
#ifdef _WINDOWS
    BYTE bySBISMBusRegisterData = 0;
    //BYTE bySVIsendByteAddressDescription = 6//=110b
#endif //#ifdef WIN32
    //  <<4; //Intersil: "BITS 6:4 Always 110b" (bit 4 = 5th bit)
    int vid ;
    DEBUG("Just 1 command line argument\n");
    mp_userinterface->showHelp();
    mp_userinterface->outputAllPstates(//str,
      2,vid);
    //outputOtherRelevantVoltageInfo(pstatectrl);
    mp_userinterface->outputOtherRelevantVoltageInfo();
    //ggf. noch interessant:
    //F3x1E4 SBI Control Register: fr core temperatur, ggf. um etwas ber SMBus 
    //fr Spannung zu erfahren.
    //F3x1EC SBI Data Register
    //F3x1E8 SBI Address Register: Bits 0-7: SbiRegAddr: SBI SMBus register address.
    //pstatectrl.GetValuesOfSBIControlRegister();
//#ifdef WIN32
#ifdef _WINDOWS
//  #ifndef LINK_TO_WINRING0_STATICALLY
//    GetValuesOfSBIControlRegister();
//    //pstatectrl.GetValuesOfSBIAddressRegister();
//    GetValuesOfSBIAddressRegister();
//    do//(; bySBISMBusRegisterData<255 ; )
//    {
//      //pstatectrl.SetSBIAddressRegister(bySBISMBusRegisterData);
//      SetSBIAddressRegister(bySBISMBusRegisterData);
//      //pstatectrl.GetValuesOfSBIDataRegister();
//      GetValuesOfSBIDataRegister();
//      ++ bySBISMBusRegisterData ;
//    }
//    while(bySBISMBusRegisterData != 0);
//  #endif //#ifndef LINK_TO_WINRING0_STATICALLY
#endif//#ifdef WIN32
    //the following is an attemp to set the VID directly to the voltage regulator:
    ////VDD1, if set then the following data byte contains the VID for VDD1
    //bySVIsendByteAddressDescription |= (1<<2);
    //68//=0,7V
    //If bySBISMBusRegisterData was 255, it get 0 after increment.

    //TODO the states must also be set after standby (ACPI state 3) and also after hibernate?
  }
  else
  {
    //BYTE by;
    //PStates pstates ;
    std::string strValue ;
    DEBUG("NOT 1 command line argument\n");
    //if(pstatectrl.cmdLineParamsContain(_T("-config"),strValue) )
    if( cmdLineParamsContain(_T("-config"),strValue) )
    {
#ifdef COMPILE_WITH_XERCES
      SAX2MainConfigHandler saxhandler(//pstates
        *mp_model ,
        mp_userinterface ,
        this
         );
      if( 
        readXMLConfig(//"config.xml"

        //mp_configurationHandler->LoadConfiguration(

        //strValue.c_str(),pstates) )
        //strValue.c_str(),pstatectrl.m_pstates) )
        strValue.c_str(), //m_pstates
        //m_model,
        *mp_model,
        mp_userinterface ,
        //this
        saxhandler
        )
        )
      {
        //if( pstates.AreSafe() )
        //{
        //  ChangePStates();
        //}
        //else
        //  printf("Unsafe values/voltage for p-state->not applying values\");
        //pstatectrl.ApplyAllPStates(pstates);
        //pstatectrl.ApplyAllPStates();

        //If the file is NOT assigned/ opened yet.
        //if( fileDebug == NULL )
        if( //An empty string means: do NOT write to the log file.
            ! mp_model->m_stdstrLogFilePath.empty()
            &&
            ! //g_logger.m_ofstream.is_open() 
            g_logger.IsOpen() 
          )
            //g_logger = new Logger(mp_model->m_stdstrLogFilePath);
            g_logger.OpenFile( mp_model->m_stdstrLogFilePath ) ;

#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
        byReturn = SUCCESS ;
#else
      if( //! mp_pstatectrl->m_model.m_bSkipCPUtypeCheck && 
        ! mp_model->m_bSkipCPUtypeCheck && 
        ! IsSupportedCPUModel() 
        )
      {
        mp_userinterface->Confirm("This CPU model is not supported by this program."
          "Use \"skip_cpu_type_check\" (see help) if you think it makes "
          "sense.\n->exiting");
        return byReturn ;
      }

        byReturn = ApplyAllPStates();

#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
      }
      else
        mp_userinterface->Confirm("Error reading the XML-configuration file");
#ifdef _TEST_PENTIUM_M
      SAX2_CPUspecificHandler sax2cpuspecifichandler(
        *mp_userinterface ,
        *mp_model) ;
      //if(
      //  readXMLConfig(
      //    "Pentium_M.xml", //m_pstates
      //    //m_model,
      //    *mp_model,
      //    mp_userinterface ,
      //    //this
      //    sax2cpuspecifichandler
      //    )
      //  )
      //{
      //  //Just for debugging:
      //  int i = 0 ;
      //}
#endif
#else //COMPILE_WITH_XERCES
      byReturn = SUCCESS ;
#endif //COMPILE_WITH_XERCES
    }
    else
    {
      BYTE byP_State ;
      DEBUG("no -config option\n");
      //pstatectrl.handleNBVIDOption();
#ifdef WIN32
  #ifndef LINK_TO_WINRING0_STATICALLY
      handleNBVIDOption();
  #endif //#ifndef LINK_TO_WINRING0_STATICALLY
#endif
      if( 
        //pstatectrl.isValidPstateOption(byP_State)
        isValidPstateOption(byP_State)
        )
      {
		    DEBUG("valid p-state option\n");
#ifdef PRIVATE_RELEASE //hide the other possibilities
        BYTE byVID, byDivID, byFreqID ;
        //if( pstatectrl.isModifierOptionSpecified(
        if( isModifierOptionSpecified(
          byVID, byDivID, byFreqID) )
        {
            BYTE byCoreID = 0;
            DWORD dwRegisterNumber, dwLow, dwHigh ;
            PState pstateFromMSR ;
            //int nSuccess ;
            printf("You spec'd to modify ") ;
            if(byVID != 255 )
              printf("VID: %u", byVID);
            if(byDivID != 255 )
              printf(",DivID: %u", byDivID);
            if(byFreqID != 255 )
              printf(",FreqID: %u", byFreqID);
            printf(" for the p-state %u\n",byP_State) ;

            dwRegisterNumber = 
              GriffinController::GetMSRregisterForPstate(byP_State) ;
    	      //nSuccess = (*g_pfnRdMsrEx)(dwRegisterNumber, &dwLow, &dwHigh, byCoreID );
            //nSuccess = pstatectrl.ReadMSR(dwRegisterNumber, dwLow, dwHigh, byCoreID );
            //if( nSuccess )
            //{
            //if( pstatectrl.GetPStateFromMSR(byP_State,dwLow,dwHigh,
            if( GetPStateFromMSR(byP_State,dwLow,dwHigh,
              pstateFromMSR,byCoreID) )
            {
              PState pstateCurrent(byVID, byDivID, byFreqID) ;
              pstateCurrent.AssignChangesToPstateFromMSR(pstateFromMSR);
              //pstatectrl.setVidAndFrequencyForPState_Puma(
              setVidAndFrequencyForPState_Puma(
                dwRegisterNumber,
                //pstateFromMSR.m_byVoltageID,
                //pstateFromMSR.m_byFreqID,
                //pstateFromMSR.m_byDivisorID,
                pstateFromMSR,
                pstateCurrent,
                dwHigh,dwLow,byCoreID);
            }
            else
              printf("Failed to get MSR value from register %x hex "
                "for CPU core %u\n",dwRegisterNumber,byCoreID);
            byCoreID = 1 ;
    	      //nSuccess = (*g_pfnRdMsrEx)(dwRegisterNumber, &dwLow, &dwHigh, byCoreID );
            //nSuccess = pstatectrl.ReadMSR(dwRegisterNumber, dwLow, dwHigh, byCoreID );
            //if( nSuccess )
            //{
            //if( pstatectrl.GetPStateFromMSR(byP_State,dwLow,dwHigh,
            if( GetPStateFromMSR(byP_State,dwLow,dwHigh,
              pstateFromMSR,byCoreID) )
            {
              PState pstateCurrent(byVID, byDivID, byFreqID) ;
              //if(PStates::IsSafe() )
              pstateCurrent.AssignChangesToPstateFromMSR(pstateFromMSR);
                //pstatectrl.setVidAndFrequencyForPState_Puma(
                setVidAndFrequencyForPState_Puma(
                  dwRegisterNumber,
                  //pstateFromMSR.m_byVoltageID,
                  //pstateFromMSR.m_byFreqID,
                  //pstateFromMSR.m_byDivisorID,
                  pstateFromMSR,
                  pstateCurrent,
                  dwHigh,dwLow, byCoreID );
            }
            else
              printf("Failed to get MSR value from register %x hex "
                "for CPU core %u\n",dwRegisterNumber,byCoreID);
        }
        else
#endif //#ifdef PUBLIC_RELEASE
        {
          BYTE byCurrentMacPstate ;
          //pstatectrl.changePstate(byP_State,0);
          if(GetMaxPState(byCurrentMacPstate) && byCurrentMacPstate < byP_State )
            SetMaxPState(byP_State) ;
          if( (byReturn = changePstate(byP_State,1)) )
          {
            //fgetc(stdin);
            //pstatectrl.changePstate(byP_State,1);
            if( (byReturn = changePstate(byP_State,2)) )
              //Only the p-state was changed, so exit and show no dialog.
              byReturn = EXIT ;
            //byReturn = SUCCESS ;
          }
        }
      }
    }
  }
  return byReturn ;
}

//#ifdef COMPILE_WITH_WINRING0
  bool GriffinController::handleNBVIDOption()
  {
    std::string str ;
    DEBUG("handleMBVIDOption()\n");
    if(cmdLineParamsContain(_T("-nbvid"),str) 
      )
    {
      DEBUG("-nbvid arg.\n");
      //if(str..length()>1 && str[0]=='=')
      if(str == "--" )
      {
        BYTE byNbVid,byPstateMaxVal,byAltVid;
        DEBUG("-nbvid-- arg.\n");
        if(GetValuesOfClockPower_TimingControl2Register(
          byNbVid,byPstateMaxVal,byAltVid)
          )
          SetValuesOfClockPower_TimingControl2Register(
            ++byNbVid,byPstateMaxVal,byAltVid);
      }
    }
    return true;
  }
//#endif //#ifdef COMPILE_WITH_WINRING0
//#ifdef WIN32

//  void PumaStateCtrl::DeInitWinRing0()
//  {
//    DEBUG("Deinitializing ring 0 access\n") ;
//#ifdef LINK_TO_WINRING0_STATICALLY
//    ////TODO which of the 2 fucntions is the correct one?
//    //DeinitializeDll() ;
//    //DeinitOpenLibSys() ;
//    DeinitializeOls() ;
//#else
//    if (m_hinstanceWinRingDLL != 0)
//    {
//      //  //Returns pointer to symbol name in the library or NULL if the library contains no such symbol.
//      //  GetSymbol(wxstrFuncName,&bSuccess);
//      void * pfn=GetProcAddress(m_hinstanceWinRingDLL,"DeinitializeOls");
//      if(pfn==NULL)
//      {
//        //wxMessageBox("no " +wxstrFuncName);
//        printf("no DeinitializeOls");
//      }
//      else
//      {
//        //std::string strFuncName ;
//        printf("function InitializeOls exists");
//        //DeinitializeOls() ausfhren.
//        (*(void (*)())pfn)();
//        //DeinitOpenLibSys() ;
//      }
//    }
//#endif //#ifdef LINK_TO_WINRING0_STATICALLY
//  }

//#ifndef LINK_TO_WINRING0_STATICALLY
//#ifdef COMPILE_WITH_WINRING0
  BYTE GriffinController::GetValuesOfClockPower_TimingControl2Register(
    BYTE & byNbVid, BYTE & byPstateMaxVal, BYTE & byAltVid)
  {
    BYTE byRet = FAILURE; 
    DEBUG("GetValuesOfClockPower_TimingControl2Register\n");
//    std::string strFuncName = "ReadPciConfigDwordEx";
//    if(!m_pfnreadpciconfigdwordex)
//      m_pfnreadpciconfigdwordex = (pfnReadPciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnreadpciconfigdwordex)
    {
      //char arch[33];
      DWORD dwValue ;
      //info is at "F3xDC" (Function "3", address "DC");
      DWORD dwPCIAddress = 3;//bits 0- 2 Function Number 
      //bits 3- 7 Device Number 
      dwPCIAddress |= (24<<3) ;//24 = AMD CPU Misc. Control.
      //dwPCIAddress = PciBusDevFunc(0, 0x18, 3) ;
//      DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
      DEBUG("PCI address:" << getBinaryRepresentation(dwPCIAddress).c_str() 
        << "\n");
//      if((*m_pfnreadpciconfigdwordex)(dwPCIAddress,0xDC,&dwValue)
      if( mp_cpuaccess->ReadPciConfigDwordEx(dwPCIAddress,0xDC,&dwValue)
        )
      {
//        DEBUG("Calling %s succeeded\n", strFuncName.c_str());
        byRet = SUCCESS ;
        byAltVid = (BYTE) ( dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID );
        byPstateMaxVal = (BYTE) ( (dwValue>>8)&7);//bits 10:8 PstateMaxVal
        byNbVid = (BYTE) ((dwValue>>12) & //BITMASK_FOR_7BIT
          BITMASK_FOR_LOWMOST_7BIT );//18:12  NbVid
//        DEBUG("value read: %s\n", this->getBinaryRepresentation(arch,dwValue));
        DEBUG("value read: " << getBinaryRepresentation(dwValue).c_str() 
          << "\n" );
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
      }
//      else
//        DEBUG("Calling %s failed\n", strFuncName.c_str());
    }
//    else
//    {
//      DEBUG("error getting function pointer to %s(...)\n", strFuncName.c_str());
//    }
     return byRet;
  }

#ifdef COMPILE_WITH_WINRING0
  BYTE GriffinController::GetValuesOfPowerControlMiscellaneousRegister(
    //BYTE & byNbVid, BYTE & byPstateMaxVal, BYTE & byAltVid
    std::string & r_str
    )
  {
    BYTE byRet = FAILURE;
    BYTE byGanged, byPstateID ;
//    std::string strFuncName = "ReadPciConfigDwordEx";
    std::ostringstream ostringstrm ;
//    if(!m_pfnreadpciconfigdwordex)
//      m_pfnreadpciconfigdwordex = (pfnReadPciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnreadpciconfigdwordex)
    {
//      char arch[33];
      DWORD dwValue ;
      //info is at "F3xDC" (Function "3", address "DC");
      DWORD dwPCIAddress = 3;//bits 0- 2 Function Number 
      //bits 3- 7 Device Number 
      dwPCIAddress |= (24<<3) ;//24 = AMD CPU Misc. Control.
      //dwPCIAddress = PciBusDevFunc(0, 18, 3) ;
//      DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
      DEBUG("dwPCIAddress: %s\n", getBinaryRepresentation(dwPCIAddress).c_str());
//      if((*m_pfnreadpciconfigdwordex)(dwPCIAddress,0xA0,&dwValue)
      if( mp_cpuaccess->ReadPciConfigDwordEx(dwPCIAddress,0xA0,&dwValue)
        )
      {
        BYTE bPSI_LbitVIDenable ;
//        DEBUG("Calling %s succeeded\n", strFuncName.c_str());
        byRet = SUCCESS ;
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
        //byPstateMaxVal = (dwValue>>8)&7;//bits 10:8 PstateMaxVal
        //byNbVid = (dwValue>>12)&BITMASK_FOR_7BIT;//18:12 ? NbVid
        DEBUG("value read: %s\n", getBinaryRepresentation(dwValue).c_str());
        //DEBUG("value read: %s\n", this->getBinaryRepresentation(arch,dwValue));
        byGanged = (BYTE) ( dwValue>>30&1 ) ;
        byPstateID = (BYTE) ( (dwValue>>16)&BIMASK_FOR_12BITS );
        DEBUG("VddCpuGanged:%u->%s PstateId:%lu(%lxhex) PsiVidEn: %s PsiVid:%u\n",
          byGanged, 
          byGanged?"dual plane":"triple plane",
          byPstateID,
          byPstateID,
          dwValue&128?"yes":"no",
          dwValue&127);
        bPSI_LbitVIDenable = (BYTE)( dwValue >> 7 ) & 1 ;
        ostringstrm << "PSI_L bit VID enable: " << (WORD)bPSI_LbitVIDenable ;
        if( bPSI_LbitVIDenable )
          ostringstrm << "Control over the PSI_L bit is as specified by the PsiVid field of this register." ;
        else
          ostringstrm << "The PSI_L bit is always high" ;
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
        ostringstrm <<"PSI_L bit VID threshold: "<< 
          (dwValue & //BITMASK_FOR_7BIT
          BITMASK_FOR_LOWMOST_7BIT ) ;
        r_str = ostringstrm.str() ;
      }
//      else
//      {
//        DEBUG("Calling %s failed\n", strFuncName.c_str());
//        ostringstrm << "Calling " << strFuncName << " failed\n" ;
//        r_str = ostringstrm.str() ;
//      }
    }
//    else
//    {
//      DEBUG("error getting function pointer to %s(...)\n", strFuncName.c_str());
//    }
    return byRet;
  }

  BYTE GriffinController::GetValuesOfReportedTemperatureControlRegister(
    //BYTE & byNbVid, BYTE & byPstateMaxVal, BYTE & byAltVid
    )
  {
    BYTE byRet = FAILURE;
//    std::string strFuncName = "ReadPciConfigDwordEx";
//    if(!m_pfnreadpciconfigdwordex)
//      m_pfnreadpciconfigdwordex = (pfnReadPciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnreadpciconfigdwordex)
    {
      //char arch[33];
      DWORD dwValue ;
      //info is at "F3xDC" (Function "3", address "DC");
      DWORD dwPCIAddress = 3;//bits 0- 2 Function Number 
      //bits 3- 7 Device Number 
      dwPCIAddress |= (24<<3) ;//24 = AMD CPU Misc. Control.
      //dwPCIAddress = PciBusDevFunc(0, 18, 3) ;
//      DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
      DEBUG("dwPCIAddress: %s\n", getBinaryRepresentation(dwPCIAddress).c_str());
//      if((*m_pfnreadpciconfigdwordex)(dwPCIAddress,0xA4,&dwValue)
      if( mp_cpuaccess->ReadPciConfigDwordEx(dwPCIAddress,0xA4,&dwValue)
        )
      {
        DWORD dwCurrentTemperature = dwValue>>21 ;
//        DEBUG("Calling %s succeeded\n", strFuncName.c_str());
        byRet = SUCCESS ;
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
        //byPstateMaxVal = (dwValue>>8)&7;//bits 10:8 PstateMaxVal
        //byNbVid = (dwValue>>12)&BITMASK_FOR_7BIT;//18:12 ? NbVid
//        DEBUG("value read: %s\n", this->getBinaryRepresentation(arch,dwValue));
        DEBUG("value read: %s\n", getBinaryRepresentation(dwValue).c_str());
        DEBUG("CurTmp:%u %f\n",
          dwCurrentTemperature,
          (float)dwCurrentTemperature/8.0f);
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
      }
//      else
//        DEBUG("Calling %s failed\n", strFuncName.c_str());
    }
//    else
//    {
//      DEBUG("error getting function pointer to %s(...)\n", strFuncName.c_str());
//    }
     return byRet;
  }

  BYTE GriffinController::GetValuesOfSBIAddressRegister()
  {
    BYTE byRet = FAILURE;
//    std::string strFuncName = "ReadPciConfigDwordEx";
//    if(!m_pfnreadpciconfigdwordex)
//      m_pfnreadpciconfigdwordex = (pfnReadPciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnreadpciconfigdwordex)
    {
      //char arch[33];
      DWORD dwValue ;
      //info is at "F3xDC" (Function "3", address "DC");
      DWORD dwPCIAddress = 3;//bits 0- 2 Function Number 
      //bits 3- 7 Device Number 
      dwPCIAddress |= (24<<3) ;//24 = AMD CPU Misc. Control.
      //dwPCIAddress = PciBusDevFunc(0, 18, 3) ;
      //DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
//      if((*m_pfnreadpciconfigdwordex)(dwPCIAddress,
      if(  mp_cpuaccess->ReadPciConfigDwordEx( dwPCIAddress ,
        //AMD: "F3x1E8 SBI Address Register"
        0x1E8,&dwValue) 
        )
      {
        DWORD dwCurrentTemperature = dwValue>>21 ;
//        DEBUG("Calling %s succeeded\n", strFuncName.c_str());
        byRet = SUCCESS ;
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
        //byPstateMaxVal = (dwValue>>8)&7;//bits 10:8 PstateMaxVal
        //byNbVid = (dwValue>>12)&BITMASK_FOR_7BIT;//18:12  NbVid
//        DEBUG("value read: %s\n", this->getBinaryRepresentation(arch,dwValue));
        DEBUG("value read: %s\n", getBinaryRepresentation(dwValue).c_str());
        DEBUG("SBI SMBus register address: %u\n", dwValue&0xFF);
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
      }
//      else
//        DEBUG("Calling %s failed\n", strFuncName.c_str());
    }
//    else
//    {
//      DEBUG("error getting function pointer to %s(...)\n", strFuncName.c_str());
//    }
     return byRet;
  }

  BYTE GriffinController::GetValuesOfSBIControlRegister()
  {
    BYTE byRet = FAILURE;
//    std::string strFuncName = "ReadPciConfigDwordEx";
//    if(!m_pfnreadpciconfigdwordex)
//      m_pfnreadpciconfigdwordex = (pfnReadPciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnreadpciconfigdwordex)
    {
//      char arch[33];
      DWORD dwValue ;
      //info is at "F3xDC" (Function "3", address "DC");
      DWORD dwPCIAddress = 3;//bits 0- 2 Function Number 
      //bits 3- 7 Device Number 
      dwPCIAddress |= (24<<3) ;//24 = AMD CPU Misc. Control.
      //dwPCIAddress = PciBusDevFunc(0, 18, 3) ;
//      DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
      DEBUG("dwPCIAddress: %s\n", getBinaryRepresentation(dwPCIAddress).c_str());
//      if((*m_pfnreadpciconfigdwordex)(dwPCIAddress,
      if( mp_cpuaccess->ReadPciConfigDwordEx( dwPCIAddress ,
        //AMD: "F3x1E4 SBI Control Register"
        0x1E4,&dwValue) 
        )
      {
        DWORD dwCurrentTemperature = dwValue>>21 ;
//        DEBUG("Calling %s succeeded\n", strFuncName.c_str());
        byRet = SUCCESS ;
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
        //byPstateMaxVal = (dwValue>>8)&7;//bits 10:8 PstateMaxVal
        //byNbVid = (dwValue>>12)&BITMASK_FOR_7BIT;//18:12  NbVid
//        DEBUG("value read: %s\n", this->getBinaryRepresentation(arch,dwValue));
        DEBUG("value read: %s\n", getBinaryRepresentation(dwValue).c_str());
        DEBUG("SMBus-based sideband interface address: %u\n", 
          (dwValue>>4)&BITMASK_FOR_LOWMOST_3BIT);
        DEBUG("The processor %s SMBus-based SBI thermal sensor protocol.\n",
          dwValue&1?"does not support":"supports");
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
      }
//      else
//        DEBUG("Calling %s failed\n", strFuncName.c_str());
    }
//    else
//    {
//      DEBUG("error getting function pointer to %s(...)\n", strFuncName.c_str());
//    }
     return byRet;
  }
  
  BYTE GriffinController::GetValuesOfSBIDataRegister()
  {
    BYTE byRet = FAILURE;
//    std::string strFuncName = "ReadPciConfigDwordEx";
//    if(!m_pfnreadpciconfigdwordex)
//      m_pfnreadpciconfigdwordex = (pfnReadPciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnreadpciconfigdwordex)
    {
//      char arch[33];
      DWORD dwValue ;
      //info is at "F3xDC" (Function "3", address "DC");
      DWORD dwPCIAddress = 3;//bits 0- 2 Function Number 
      //bits 3- 7 Device Number 
      dwPCIAddress |= (24<<3) ;//24 = AMD CPU Misc. Control.
      //dwPCIAddress = PciBusDevFunc(0, 18, 3) ;
      DEBUG("dwPCIAddress: %s\n", getBinaryRepresentation(//arch,
        dwPCIAddress).c_str() );
//      if((*m_pfnreadpciconfigdwordex)(dwPCIAddress,
      if( mp_cpuaccess->ReadPciConfigDwordEx( dwPCIAddress ,
        //AMD: "F3x1EC SBI Data Register"
        0x1EC,&dwValue) 
        )
      {
        DWORD dwCurrentTemperature = dwValue>>21 ;
//        DEBUG("Calling %s succeeded\n", strFuncName.c_str());
        byRet = SUCCESS ;
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
        //byPstateMaxVal = (dwValue>>8)&7;//bits 10:8 PstateMaxVal
        //byNbVid = (dwValue>>12)&BITMASK_FOR_7BIT;//18:12  NbVid
//        DEBUG("value read: %s\n", this->getBinaryRepresentation(arch,dwValue));
        DEBUG("value read: %s\n", getBinaryRepresentation(dwValue).c_str());
        DEBUG("SBI SMBus register data.: %u\n", dwValue&0xFF);
        //byAltVid = dwValue&BITMASK_FOR_PCI_CONFIG_ALTVID;
      }
//      else
//        DEBUG("Calling %s failed\n", strFuncName.c_str());
    }
//    else
//    {
//      DEBUG("error getting function pointer to %s(...)\n", strFuncName.c_str());
//    }
     return byRet;
  }
#endif //#ifndef LINK_TO_WINRING0_STATICALLY

//  bool GriffinController::InitWinRing0()
//  {
//    bool bOk = false ;
//    //mp_userinterface->Confirm("initialisiere WinRing0");
//#ifdef LINK_TO_WINRING0_STATICALLY
//    ////TODO which of the 2 fucntions is the correct one?
//    //InitOpenLibSys() ;
//    //InitializeDll() ;
//    //InitializeOls() ;
//    bOk = true ;
//#else
//
//#endif //#ifdef LINK_TO_WINRING0_STATICALLY
//    return bOk ;
//  }

//  void PumaStateCtrl::messageLoop()
//  {
//	  MSG msg;
//	  HANDLE hWnd = CreateWindow(_T("Stand By Detector window"),
//      NULL, WS_OVERLAPPEDWINDOW,
//			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//      NULL, NULL, this->m_hinstanceThisModule, NULL);
//    // Main message loop:
//    DEBUG("just before while (GetMessage(&msg, NULL, 0, 0)) \n");
//	  while (GetMessage(&msg, NULL, 0, 0))
//	  {
//		  //if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//		  //{
//			 // TranslateMessage(&msg);
//			 // DispatchMessage(&msg);
//		  //}
//      DEBUG("while (GetMessage(&msg, NULL, 0, 0)) \n");
//	  }
//    while(true)
//    {
//      //wait for power state change event notification
//      //::WaitForSingleEvent(m_event., INFINITE);
//      //::WaitForSingleObject(m_handleEvent, INFINITE);
//      ::WaitForSingleObject(g_handleEvent, INFINITE);
//    }
//  }

#ifdef COMPILE_WITH_WINRING0
  BYTE GriffinController::SetSBIAddressRegister(BYTE bySBISMBusRegisterData)
  {
    BYTE byReturnValue = FAILURE ;
//    std::string strFuncName = "WritePciConfigDwordEx";
    //DEBUG("SetSBIAddressRegister(%u)\n",bySBISMBusRegisterData);
    LOG("Set SBI Address Register: " << (WORD) bySBISMBusRegisterData << " \n" );
//#ifndef LINK_TO_WINRING0_STATICALLY
//    if(!m_pfnwritepciconfigdwordex)
//      m_pfnwritepciconfigdwordex = (pfnWritePciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnwritepciconfigdwordex)
//    {
//#endif
      //char arch[33];
      //int nDummy;
      DWORD dwValue = bySBISMBusRegisterData ;
      DWORD dwPCIAddress = PciBusDevFunc(0, 0x18, 3) ;

//      DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
      //DEBUG("dwPCIAddress: %s\n", getBinaryRepresentation(dwPCIAddress).c_str());
      LOG( "PCI Address: " << getBinaryRepresentation(dwPCIAddress).c_str() 
          << "\n" );
//      DEBUG("dwValue:    %s\n", this->getBinaryRepresentation(arch,dwValue));
      //DEBUG("dwValue:    %s\n", getBinaryRepresentation(dwValue).c_str());
      LOG(" Value:    " << getBinaryRepresentation(dwValue).c_str() << "\n" );
      //std::cin >> nDummy ;
      //std::cin.p
      //std::cin.sync() ; //to flush (sonst wurde dieser Teil bersprungen nach dem 1ten Aufruf)
//#ifdef __cplusplus
//      DEBUG("to write: input at least 1 char and press ENTER\n");
//      //"_getche" (note the underline prefix ) is the C++ version of getche()
//      _getche();
//#endif
      DEBUG("to write: input at A NUMBER and press ENTER\n");
      int nDummyInput ;
      std::cin >> nDummyInput ;
      if(
//#ifndef LINK_TO_WINRING0_STATICALLY
//        (*m_pfnwritepciconfigdwordex)
//#else
//        WritePciConfigDwordEx
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
        mp_cpuaccess->WritePciConfigDwordEx
        (dwPCIAddress,
        //AMD: "F3x1E8 SBI Add6ress Register"
        0x1E8,dwValue) 
        )
      {
        DEBUG("successfully written to PCI config\n");
        byReturnValue = SUCCESS;
      }
      else
        DEBUG("failed to write to PCI config\n");
//#ifndef LINK_TO_WINRING0_STATICALLY
//    }
//#endif
    return byReturnValue ;
  }
#endif //#ifdef COMPILE_WITH_WINRING0

//#ifndef LINK_TO_WINRING0_STATICALLY
//#ifdef COMPILE_WITH_WINRING0
  void GriffinController::SetValuesOfClockPower_TimingControl2Register(
    BYTE byNbVid, BYTE byPstateMaxVal, BYTE byAltVid)
  {
//    std::string strFuncName = "WritePciConfigDwordEx";
    DEBUG("SetValuesOfClockPower_TimingControl2Register\n");
//    if(!m_pfnwritepciconfigdwordex)
//      m_pfnwritepciconfigdwordex = (pfnWritePciConfigDwordEx)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//
//    if(m_pfnwritepciconfigdwordex)
    {
//      char arch[33];
      int nDummy;
      DWORD dwValue = 0 | (byNbVid<<12) | (byPstateMaxVal<<8) |
        byAltVid ;
      DWORD dwPCIAddress = PciBusDevFunc(0, 0x18, 3) ;

//      DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
      DEBUG("dwPCIAddress: " << getBinaryRepresentation(dwPCIAddress).c_str() 
        << "\n" );
//      DEBUG("dwValue:    %s\n", this->getBinaryRepresentation(arch,dwValue));
      DEBUG("dwValue:    " << getBinaryRepresentation(dwValue).c_str() 
        << "\n" );
      DEBUG("to write: input at least 1 char and press ENTER\n");
      std::cin >> nDummy ;
//      if((*m_pfnwritepciconfigdwordex)(dwPCIAddress,0xDC,dwValue)
      if( mp_cpuaccess->WritePciConfigDwordEx( dwPCIAddress,0xDC,dwValue)
        )
      {
        DEBUG("successfully written to PCI config\n");
      }
      else
        DEBUG("failed to write to PCI config\n");
    }
  }
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
//#endif //#ifdef COMPILE_WITH_WINRING0

  void GriffinController::FindLowestOperatingVoltage(BYTE byPstate, BYTE byCoreID)
  {
    BYTE byCoreIndex = 0 , byCoreCount = 
      //mp_modeldata->GetNumberOfPhysicalCores() ;
      //m_model.GetNumberOfPhysicalCores() ;
      //m_model.GetNumberOfCPUCores() ;
      mp_model->GetNumberOfCPUCores() ;
    m_byPstateForFindingLowVoltage = byPstate ;
    //BYTE byVID ;
    DWORD dwLow;
    DWORD dwHigh;
    //PState pstate ;
    //DEBUG("Find lowest operating voltage for p-state %u\n", (WORD)byPstate )
    LOG("Trying to find lowest operating voltage for p-state " << 
        (WORD) byPstate << "\n" )
      //mp_cpuaccess->GetNumberOfPhysicalCores() ;
      //mp_cpuidaccess->GetNumberOfPhysicalCores() ;
    //GetPStateFromMSR(byPstate,dwLow, dwHigh, pstate, byCoreID ) ;
    GetPStateFromMSR(byPstate,dwLow, dwHigh, m_pstate, byCoreID ) ;
    m_bPstateSet = true ;
    //byVID = pstate.m_byVoltageID ;
    //mp_cpuaccess->StartCalculationThread(0);
    mp_calculationthread->StartCalculationThread(0);
    //mp_cpuaccess->RdmsrEx( GetMSRregisterForPstate( byPstate) ) ;
    while( 
      //64 is the VID for the lowest voltage that the CPU sends to 
      //the voltage regulator.
      //byVID > 64 
      //++ pstate.m_byVoltageID < 65 
      ++ m_pstate.m_byVoltageID < 65 )
    {
      std::ostrstream ostrstream ;
      //ostrstream.clear();
      DEBUG("Find lowest operating voltage for p-state--begin of loop\n");
      ostrstream << "Lowering voltage for p-state " << 
        //(WORD)byPstate << " (" << pstate.GetFreqInMHz() << 
        //" MHz) to Voltage ID " << (WORD)pstate.m_byVoltageID << "(" <<
        //pstate.GetVoltageInVolt() << " V)\n"
        (WORD)byPstate << " (" << m_pstate.GetFreqInMHz() << 
        " MHz) to Voltage ID " << (WORD) m_pstate.m_byVoltageID << "(" <<
        m_pstate.GetVoltageInVolt() << " V)\n"
        "Remember:\n"
        "-the OS' dynamic frequency scaling should be deactivated (see help)\n"
        "-your OS will surely freeze if voltage is too low->safe all your work";
      DEBUG("Find lowest operating voltage for p-state--before sleep\n");
      //ostrstream.clear();
      //mp_timecontroller points to the wxwidgets controller.
      //mp_timecontroller->sleep();
      //::Sleep(1000);
      //::wxSleep(1);
      SLEEP_1_MILLI_SECOND
      //for( byCoreIndex = 0 ; byCoreIndex < byCoreCount ; ++byCoreIndex )
      //mp_userinterface->Confirm("");
      if( ! mp_userinterface->Confirm(ostrstream) )
        break ;
      //++ pstate.m_byVoltageID ;
        if( WriteToPstateOrCOFVIDcontrolRegister(
          GetMSRregisterForPstate(//p_pstateCurrent->m_byNumber
          byPstate),
          //pstateFromMSR.m_byVoltageID,
          //pstateFromMSR.m_byFreqID,
          //pstateFromMSR.m_byDivisorID,
          //pstate,
          //pstate,
          m_pstate,
          m_pstate,
          dwHigh,
          dwLow,
          byCoreID)
          ) 
          //Finally (re)set the p-state (even it's the same as before) 
          //for the changes to take effect.
          SetPstate(byPstate,//1 << byCoreID 
            byCoreID ) ;
    }
    //mp_calculationthread->EndCalculationThread();
  }

  BYTE GriffinController::GetCurrentPstate(DWORD dwAffinityMask)
  {
    BYTE byReturn = 255 ;
    DWORD dwEAX;			// bit  0-31
    DWORD dwEDX;			// bit 32-63
    DEBUG("getting current p-state.\n");
    if( mp_cpuaccess->RdmsrEx(
	    P_STATE_STATUS_REGISTER,		// MSR index
	    &dwEAX,			// bit  0-31
	    &dwEDX,			// bit 32-63
	    dwAffinityMask
      )
      )
      //"2:0 CurPstate: current P-state. Read-only.
      //Cold reset: values vary by product. The current P-state of the
      //core. 0=P-state 0; 1=P-state 1; etc.
      //The value of this field is updated when the COF transitions to a
      //new value associated with a P-state. See section 2.4.2 [P-states]."
      byReturn = dwEAX & BITMASK_FOR_LOWMOST_3BIT ;
    return byReturn ;
  }

  bool GriffinController::GetCPUID(
    //last/rightmost/least significant 2 digits of function bumber 
    DWORD dwFunctionIndex,
    DWORD & dwEAX,
    DWORD & dwEBX,
    DWORD & dwECX,
    DWORD & dwEDX
    )
  {
    bool bSuccess = false ;
    //mp_userinterface->Confirm("PumaStateCtrl::IsSupportedCPUModel()");
//    std::string strFuncName = "CpuidEx" ;
//#ifndef LINK_TO_WINRING0_STATICALLY
//    _CpuidEx pfnCPU_ID;
//    pfnCPU_ID = (_CpuidEx)GetProcAddress(m_hinstanceWinRingDLL,
//      strFuncName.c_str() );
//    if(pfnCPU_ID)
//    {
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
//      DEBUG("PumaStateCtrl::GetCPUID--Got function for CPUID\n");
      //"If the function succeeds, the return value is TRUE."
      if( 
//#ifndef LINK_TO_WINRING0_STATICALLY
//        (*pfnCPU_ID)
//#else
//        CpuidEx
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
        mp_cpuaccess->CpuidEx
        (dwFunctionIndex,&dwEAX,&dwEBX,&dwECX,&dwEDX,1) 
        )
      {
        //DEBUG("PumaStateCtrl::GetCPUID--Calling CPUID succeeded:dwEDX:%lu\n",
        //  dwEDX);
        LOG( "Calling CPUID succeeded: value:" << dwEDX << "\n" );
        bSuccess = true;
      }
#ifdef COMPILE_WITH_DEBUG
      else
        DEBUG("Get CPUID--Calling CPUID failed\n");
#endif
//#ifndef LINK_TO_WINRING0_STATICALLY
//    }
//  #ifdef COMPILE_WITH_DEBUG
//    else
//      DEBUG("PumaStateCtrl::GetCPUID--Did not get function for CPUID\n");
//  #endif
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
    return bSuccess ;
  }

  //void 
  DIDandFID GriffinController::GetNearestHigherPossibleFreqInMHzAsDIDnFID(
      WORD wFreqInMHz
      //, PState & r_pstate 
      )
  {
    WORD wNewFreqInMhz ;
    //e.g. Max freq = 2200 MHz
    //e.g.: 2200 / 2200 = 1 - 1 = 0 = divID = log2(1) = 0
    //e.g.: 2200 / 2199 ~ 1,0004 ~=1 - 1 = 0
    //e.g.: 2200 / 1101 ~ 1,99 ~=1 - 1 = 0 = divID
        //( FID + 8 ) * 100 = FreqInMHz | :100
        // FID + 8 = FreqInMHz : 100 | - 8
        // FID = FreqInMHz : 100 - 8

        //(FID+8)*100/2^0(divID) = 1101 
        //<=> (FID+8)*100/1 = 1101 
        //<=> (FID+8)*100 = 1101 |:100
        //FID+8 = 11,01~=11 |-8 <=> FID=3
        //Test: (FID+8)*100/2^DivisorID = 1100 <=> 
        //        (3+8)*100/2^0         = 1100 <=>
        //           11*100/1           = 1300 <=> 
        //             1100/1           = 1100 <=> 
        //                         1100 = 1100 <=> true

    //  1101 : 100 ~ 11,01~=11 - 8 = 3
    //e.g.: 2200 / 1100 = 2 - 1 = 1
    //e.g.: 2200 / 550 = 4 - 1 = 3 ; log2(4) = 2 = DivID
        //(FID+8)*100/2^divID    = 550 <=> 
        //(FID+8)*100/2^2        = 550 <=> 
        //(FID+8)*100/4          = 550 |:100/4
        //(FID+8)                = 550*4/100 <=> 
        // FID+8                 = 2200/100
        // FID+8                 = 22 |-8
        // FID                   = 14
        //FID+8 = 11,01~=11 |-8 <=> FID=3
        //Test: (FID+8)*100/2^DivisorID = 1100 <=> 
        //        (3+8)*100/2^0         = 1100 <=>
        //           11*100/1           = 1300 <=> 
        //             1100/1           = 1100 <=> 
        //                         1100 = 1100 <=> true

    //byDivisorID = 0 ;
    //while( byDividend != 1 )
    //{
    //    byDividend >> 1 ;
    //    ++ byDivisorID ;
    //}

    //hint: log2(dividend) = log10(dividend) / log10(2)
    //            byDividend | dividend binary | divID | ceil (log2(dividend) )
    //2200/2200 =  1         | 1               | 0     | 0
    //2200/2199 =  ~1,0004   | 1               | 0     | 0
    //2200/1100 =  2         | 10              | 1     | 1
    //2200/700  =  3         | 11              | 1     | = ceil( ~ 1,58) = 2
    //2200/550  =  4         | 100             | 2     | 2

    float fMaxFreqDivMaxWantedFreq = 
        (float) //wMaxFreq
        //mp_pumastatectrl->
        //GetMaximumFrequencyInMHz() / (float) wFreqInMHz ;
        mp_model->m_cpucoredata.m_wMaxFreqInMHz / (float) wFreqInMHz ;
    //MSVC++ has no log2() function (in <math.h>).
    //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
    float fLog2 = 
      log10//(
      //(double) 
      ( 
        fMaxFreqDivMaxWantedFreq
            ////Correction to get  the Divisor higher by 1 at the.
            ////transition where FID > maxFID / FID < maxFID/2
            //* 1.1f
      )
      / 
      log10(2.0f) ;
    BYTE byDivisorID ;

    //if( //If fMaxFreqDivMaxWantedFreq is too low the log2-value gets nearly 0:
    //    //e.g. 2200/2199 = 1,0004547521600727603456116416553, 
    //    //log2(1,0004547521600727603456116416553) = 6,5591955705389838192418833283849e-4
    //    // ~0,0006559
    //    //And if rounded up to "1" the FreqID would be to high: 
    //    //  2199 * 2^1 / 100 - 8 = 2199 * 2 / 100 - 8 = 4398 / 100 - 8 = 43,98 - 8
    //    //  = 35,98
    //    fMaxFreqDivMaxWantedFreq >= 2.0f )
    //  byDivisorID = ceil
    //      ( 
    //        fLog2
    //      ) ;
    //else
        byDivisorID = fLog2 ;
    //wMaxFreq / wFreq - 1 = byDivisorID ;

    //(FID+8)*100/2^divID = freqInMHz | : 100/2^divID
    //(FID+8)             = freqInMHz : ( 100/2^divID ) | - 8
    //(FID+8)             = freqInMHz * ( 2^divID ) : 100 | - 8
    // FID                = freqInMHz * ( 2^divID ) : 100 - 8
    //Test:
    // FID                = 1101 * ( 2^0 ) : 100 - 8
    // FID                = 1101 *    1    : 100 - 8
    // FID                = 1101           : 100 - 8
    // FID                = 11.01                - 8
    // FID                = 3

    // FID                = 1100 * ( 2^1 ) : 100 - 8
    // FID                = 1100 *    2    : 100 - 8
    // FID                = 2200           : 100 - 8
    // FID                = 22                   - 8
    // FID                = 14
    BYTE byFID = (float) wFreqInMHz * pow(2.0,byDivisorID) / 100.0f - 8 ;
    wNewFreqInMhz = PState::GetFreqInMHz(byFID,byDivisorID) ;
    //if( //byFID < //MainPllOpFreqId/2
    //    //mp_pumastatectrl->
    //    //mp_model->m_cpucoredata.
    //    //  m_byMainPLLoperatingFrequencyIDmax / 2 
    //    //AMD BIOS and kernel dev guide for family 11h: 
    //    //"The frequency specified by (100 MHz * (CpuFid + 08h)) must 
    //    //always be >50% of and <= 100% of the frequency specified by 
    //    //F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]"

    //    //->If NOT ">50%", e.g. FID = 3, MainPLLoperatingFrequencyIDmax = 14:
    //    //  FID + 8 > MainPLLoperatingFrequencyIDmax + 8
    //    //  <=> 3 + 8 > (14 + 8) / 2      11 > 22 / 2   <=>  11 > 11 : "false"
    //    ( byFID + 8 ) <= 
    //    ( mp_model->m_cpucoredata.
    //      m_byMainPLLoperatingFrequencyIDmax + 8 ) / 2 
    //  )

    //    //AMD BIOS and kernel dev guide for family 11h: "The frequency specified 
    //    //by (100 MHz * (CpuFid + 08h)) must always be >50% ..."
    //    //byFID = //mp_pumastatectrl->
    //    //  //mp_model->m_cpucoredata.
    //    ////"...of and <= 100% of the frequency specified by 
    //    ////F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]"
    //    //  //m_byMainPLLoperatingFrequencyIDmax / 2 + 1;
    //    //  (mp_model->m_cpucoredata.m_byMainPLLoperatingFrequencyIDmax + 8 )
    //    //  / 2 + 1;
    //    //If FID was 3 it gets 4 now and so the condition 
    //    //FID + 8 > (MainPLLoperatingFrequencyIDmax + 8 ) / 2 
    //    //gets true 
    //    ++ byFID ;
    //else
      if( wNewFreqInMhz <= wFreqInMHz )
          //Higher FID at the same Divisor ID = higher frequency.
          ++ byFID ;
    ////AMD BIOS and kernel dev guide for family 11h: "The frequency specified 
    ////by (100 MHz * (CpuFid + 08h)) must always be >50% of and <= 100% of 
    ////the frequency specified by F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
    //if( byFID > //MainPllOpFreqId/2
    //    //mp_pumastatectrl->
    //    mp_model->m_cpucoredata.
    //      m_byMainPLLoperatingFrequencyIDmax
    //    && byDivisorID > 0 
    //  )
    //{
    //    -- byDivisorID ;
    //    byFID = //MainPllOpFreqId 
    //      //mp_pumastatectrl->
    //      //mp_model->m_cpucoredata.
    //      //  m_byMainPLLoperatingFrequencyIDmax / 2 ;
    //      //e.g.: MainPLLoperatingFrequencyIDmax: 13; 13 + 8 = 21 ^= 2100 MHz
    //      // 21 / 2 = 10^=  1000 MHz ; 10 - 7 = FID
    //      (mp_model->m_cpucoredata.
    //        m_byMainPLLoperatingFrequencyIDmax + 8 ) / 2 - 7 ;
    //}
    //r_pstate
    //PState pstate(
    DIDandFID didandfid(byDivisorID,byFID) ;
    return didandfid ;
  }

  DIDandFID GriffinController::GetNearestPossibleFreqInMHzAsDIDnFID(
     WORD wFreqInMHz )
  {
    DIDandFID didandfid ;
    DIDandFID * p_didandfid ;
//  //bChange = true;
//  //m_p_pstates->SetPStateFreq((BYTE)wNumber,wValue);
//  //m_p_pstates->SetBruteForceFIDandDIDbyFreq((BYTE)wNumber,wValue);
//  m_p_model->m_pstates.SetBruteForceFIDandDIDbyFreq(byPstateID,wValue);
    DIDandFID didandfidH = //mp_pumastatectrl->
      GetNearestHigherPossibleFreqInMHzAsDIDnFID(
        wFreqInMHz ) ;
    LOG( "Freq In MHz higher than wanted freq: " << 
        didandfidH.GetFreqInMHz() << "\n" );
    DIDandFID didandfidL = //mp_pumastatectrl->
      GetNearestLowerPossibleFreqInMHzAsDIDnFID(
        wFreqInMHz ) ;
    LOG( "Freq In MHz lower than wanted freq: " << 
        didandfidL.GetFreqInMHz() << "\n" );
    //m_p_model->m_cpucoredata.m_wMaxFreqInMHz must have been set here.
    LOG( "CPU core Max Freq In MHz: " << 
        //m_p_model->
        mp_model->m_cpucoredata.m_wMaxFreqInMHz << "\n" );
    assert( //m_p_model->
      mp_model->m_cpucoredata.m_wMaxFreqInMHz != 0 ) ;
    if( didandfidH.GetFreqInMHz() > //m_p_model->
      mp_model->m_cpucoredata.m_wMaxFreqInMHz )
        p_didandfid = & didandfidL ;
    else
    {
        //abs( m_p_model->m_cpucoredata.m_wMaxFreqInMHz - didandfidL.GetFreqInMHz )
        WORD wDiffL = didandfidL.GetDiff( 
            wFreqInMHz ) ;
        WORD wDiffH = didandfidH.GetDiff( 
            wFreqInMHz ) ;
        LOG( "Absolute difference between wanted Freq In MHz and "
            "lower freq: " << wDiffL << "\n" );
        LOG( "Absolute difference between wanted Freq In MHz and "
            "higher freq: " << wDiffH << "\n" );
        p_didandfid = ( wDiffH < wDiffL ) ? &didandfidH : &didandfidL ;
    }
    return didandfid = *p_didandfid ;
  }

  DIDandFID GriffinController::GetNearestLowerPossibleFreqInMHzAsDIDnFID(
     WORD wFreqInMHz )
  {
    //e.g. Max freq = 2200 MHz
    //e.g.: 2200 / 2200 = 1 - 1 = 0 = divID = log2(1) = 0
    //e.g.: 2200 / 2199 ~ 1,0004 ~=1 - 1 = 0
    //e.g.: 2200 / 1101 ~ 1,99 ~=1 - 1 = 0 = divID
        //( FID + 8 ) * 100 = FreqInMHz | :100
        // FID + 8 = FreqInMHz : 100 | - 8
        // FID = FreqInMHz : 100 - 8

        //(FID+8)*100/2^0(divID) = 1101 
        //<=> (FID+8)*100/1 = 1101 
        //<=> (FID+8)*100 = 1101 |:100
        //FID+8 = 11,01~=11 |-8 <=> FID=3
        //Test: (FID+8)*100/2^DivisorID = 1100 <=> 
        //        (3+8)*100/2^0         = 1100 <=>
        //           11*100/1           = 1300 <=> 
        //             1100/1           = 1100 <=> 
        //                         1100 = 1100 <=> true

    //  1101 : 100 ~ 11,01~=11 - 8 = 3
    //e.g.: 2200 / 1100 = 2 - 1 = 1
    //e.g.: 2200 / 550 = 4 - 1 = 3 ; log2(4) = 2 = DivID
        //(FID+8)*100/2^divID    = 550 <=> 
        //(FID+8)*100/2^2        = 550 <=> 
        //(FID+8)*100/4          = 550 |:100/4
        //(FID+8)                = 550*4/100 <=> 
        // FID+8                 = 2200/100
        // FID+8                 = 22 |-8
        // FID                   = 14
        //FID+8 = 11,01~=11 |-8 <=> FID=3
        //Test: (FID+8)*100/2^DivisorID = 1100 <=> 
        //        (3+8)*100/2^0         = 1100 <=>
        //           11*100/1           = 1300 <=> 
        //             1100/1           = 1100 <=> 
        //                         1100 = 1100 <=> true

    //byDivisorID = 0 ;
    //while( byDividend != 1 )
    //{
    //    byDividend >> 1 ;
    //    ++ byDivisorID ;
    //}

    //hint: log2(dividend) = log10(dividend) / log10(2)
    //            byDividend | dividend binary | divID | ceil (log2(dividend) )
    //2200/2200 =  1         | 1               | 0     | 0
    //2200/2199 =  ~1,0004   | 1               | 0     | 0
    //2200/1100 =  2         | 10              | 1     | 1
    //2200/700  =  3         | 11              | 1     | = ceil( ~ 1,58) = 2
    //2200/550  =  4         | 100             | 2     | 2

    float fMaxFreqDivMaxWantedFreq = 
        (float) //wMaxFreq
        //mp_pumastatectrl->
        //GetMaximumFrequencyInMHz() / (float) wFreqInMHz ;
        mp_model->m_cpucoredata.m_wMaxFreqInMHz / (float) wFreqInMHz ;
    //MSVC++ has no log2() function (in <math.h>).
    //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
    float fLog2 = 
      log10//(
      //(double) 
      ( 
        fMaxFreqDivMaxWantedFreq
      )
      / 
      log10(2.0f) ;
    BYTE byDivisorID ;

    //if( //If fMaxFreqDivMaxWantedFreq is too low the log2-value gets nearly 0:
    //    //e.g. 2200/2199 = 1,0004547521600727603456116416553, 
    //    //log2(1,0004547521600727603456116416553) = 6,5591955705389838192418833283849e-4
    //    // ~0,0006559
    //    //And if rounded up to "1" the FreqID would be to high: 
    //    //  2199 * 2^1 / 100 - 8 = 2199 * 2 / 100 - 8 = 4398 / 100 - 8 = 43,98 - 8
    //    //  = 35,98
    //    fMaxFreqDivMaxWantedFreq >= 2.0f )
    //  byDivisorID = ceil
    //      ( 
    //        fLog2
    //      ) ;
    //else
        byDivisorID = fLog2 ;
    //wMaxFreq / wFreq - 1 = byDivisorID ;

    //(FID+8)*100/2^divID = freqInMHz | : 100/2^divID
    //(FID+8)             = freqInMHz : ( 100/2^divID ) | - 8
    //(FID+8)             = freqInMHz * ( 2^divID ) : 100 | - 8
    // FID                = freqInMHz * ( 2^divID ) : 100 - 8
    //Test:
    // FID                = 2200 * ( 2^0 ) : 100 - 8
    // FID                = 2200 *    1    : 100 - 8
    // FID                = 2200           : 100 - 8
    // FID                = 22                   - 8
    // FID                = 14

    // FID                = 1100 * ( 2^1 ) : 100 - 8
    // FID                = 1100 *    2    : 100 - 8
    // FID                = 2200           : 100 - 8
    // FID                = 22                   - 8
    // FID                = 14
    BYTE byFID = (float) wFreqInMHz * pow(2.0,byDivisorID) / 100.0f - 8 ;
    //AMD BIOS and kernel dev guide for family 11h: "The frequency specified 
    //by (100 MHz * (CpuFid + 08h)) must always be >50% of and <= 100% of 
    //the frequency specified by F3xD4[MainPllOpFreqId, MainPllOpFreqIdEn]."
    if( //byFID <= //MainPllOpFreqId/2
        //mp_pumastatectrl->
        //mp_model->m_cpucoredata.
        //  m_byMainPLLoperatingFrequencyIDmax / 2
        ( byFID + 8 ) <= 
        ( mp_model->m_cpucoredata.
          //m_byMainPLLoperatingFrequencyIDmax 
          GetMainPLLoperatingFrequencyIDmax() + 8 ) / 2
        && byDivisorID < HIGHEST_EFFECTIVE_DIVISOR_ID )
    {
        ++ byDivisorID ;
        byFID = //MainPllOpFreqId 
          //mp_pumastatectrl->
          mp_model->m_cpucoredata.
            //m_byMainPLLoperatingFrequencyIDmax 
            GetMainPLLoperatingFrequencyIDmax() ;
    }
    DIDandFID didandfid(byDivisorID,byFID) ;
    return didandfid ;
  }

  //Um die maximale Taktfrequenz zu kriegen: CPUID Function 8000_0004: 
  //Register EDX steht bei mir " 28" als Teil von "ZM-82" (->2,2 GHz)
  //Und nicht in p-state "0" nach der max. Frequenz gucken, da diese
  //seit dem Neustart verndert worden sein kann.
  //Returns 0 if unknown Griffin CPU or maybe GetCPUID(...) error.
  WORD GriffinController::GetMaximumFrequencyInMHz()
  {
    WORD wMaximumFrequencyInMHz = 0;
    //if( m_model.m_bUsePstate0AsMaxFreq )
  //  if( mp_model->m_bUsePstate0AsMaxFreq )
  //  {
  //    DEBUG("GetMaximumFrequencyInMHz--getting max. freq from p-state 0\n" );
  //    //We want the values directly from CPU, and not from user!
  //    //if(m_model.m_pstates.m_arp_pstate[0] )
  //    {
  //      DWORD dwLow, dwHigh ;
  //      PState pstateFromMSR ;
  //      //wMaximumFrequencyInMHz = m_model.m_pstates.m_arp_pstate[0]->GetFreqInMHz() ;
  //      if( GetPStateFromMSR(0,
  //        dwLow,
  //        dwHigh,
  //        pstateFromMSR,//1
  //        0
  //        ) )
  //      {
  //        wMaximumFrequencyInMHz = pstateFromMSR.GetFreqInMHz() ;
  //      }
  //    }
  //  }
  //  else
  //  {
  //    DWORD dwEAX;
  //    DWORD dwEBX;
  //    DWORD dwECX;
  //    DWORD dwEDX;
  //    DEBUG("GetMaximumFrequencyInMHz--getting max. freq by CPU name\n" );
  //    //TODO: the CPUID processor name string may be changed as the MSR 
  //    //registers that map to it have read-write access. So determining
  //    //max freq from it is not so safe.
  //    if( GetCPUID(
  //      //AMD: "CPUID Fn8000_000[4:2] Processor Name String Identifier"
  //      0x80000004,
  //      dwEAX,
  //      dwEBX,
  //      dwECX,
  //      dwEDX
  //      ) )
  //    {
  //      DEBUG("GetMaximumFrequencyInMHz()--Calling GetCPUID succeeded\n");
  //      DEBUG("get max freq--EDX register of function 4: %lu 2nd byte: %u\n", 
  //        dwEDX,*(((char *)&dwEDX)+1) );
  //      switch(//2nd byte from left/2nd most significant byte
  //        *( ((char *) &dwEDX) + 1 )
  //        )
  //      {
  //      case '0': //ZM-8>>0<<
  //        wMaximumFrequencyInMHz = 2100 ;
  //        break;
  //      case '2': //ZM-8>>2<<
  //        //DEBUG("PumaStateCtrl::GetMaximumFrequencyInMHz()--CPU is ZM-82\n");
  //        wMaximumFrequencyInMHz = 2200 ;
  //        break;
  //      case '4': //ZM-8>>4<<
  //        wMaximumFrequencyInMHz = 2300 ;
  //        break;
  //      case '6': //ZM-8>>6<<
  //        wMaximumFrequencyInMHz = 2400 ;
  //        break;
  //      default:
  //        this->mp_userinterface->Confirm("unknown CPU type");
  //      }
  //    }
  //#ifdef COMPILE_WITH_DEBUG
  //    else
  //      DEBUG("GetMaximumFrequencyInMHz()--Calling GetCPUID failed\n");
  //#endif
  //  }
    if ( mp_model->m_cpucoredata.
        //m_byMainPLLoperatingFrequencyIDmax 
        GetMainPLLoperatingFrequencyIDmax() == CPU_CORE_DATA_NOT_SET )
    {
      DWORD dwValue ;
      //Read the F3xD4 Clock Power/Timing Control 0 Register to get value 
      //"MainPllOpFreqId: main PLL operating frequency ID." at bits "5:0"
      if( GetCPUMiscControlDWORD(
        F3xD4_CLOCK_POWER_TIMING_CONTROL_0_REGISTER, dwValue) 
        )
      {
          mp_model->m_cpucoredata.//m_byMainPLLoperatingFrequencyIDmax 
            SetMainPLLoperatingFrequencyIDmax(
              dwValue & BITMASK_FOR_LOWMOST_5BIT 
              ) ;
        //DEBUG("main PLL operating frequency ID: %lu\n" , 
        //  dwValue & BITMASK_FOR_LOWMOST_5BIT );
        LOG("Main PLL operating frequency ID: " 
            << ( dwValue & BITMASK_FOR_LOWMOST_5BIT ) << "\n" );
      }
    }
    if ( mp_model->m_cpucoredata.
        //m_byMainPLLoperatingFrequencyIDmax 
        GetMainPLLoperatingFrequencyIDmax() != CPU_CORE_DATA_NOT_SET 
        )
    {
      if( mp_model->m_cpucoredata.//m_byMainPLLoperatingFrequencyIDmax
        GetMainPLLoperatingFrequencyIDmax()
        != NO_CPU_CORE_FREQUENCY_LIMIT 
        )
        //"The maximum frequency is 100 MHz * (MainPllOpFreqIdMax + 08h)"
        wMaximumFrequencyInMHz = 100 * ( mp_model->m_cpucoredata.
            //m_byMainPLLoperatingFrequencyIDmax 
            GetMainPLLoperatingFrequencyIDmax() + 0x08 ) ;
    }
    //DEBUG("maximum core frequency "
    //  "in MHz is %u\n", wMaximumFrequencyInMHz);
    LOG("Maximum core frequency in MHz is " << wMaximumFrequencyInMHz 
        << "\n" );
    mp_model->m_cpucoredata.m_wMaxFreqInMHz = wMaximumFrequencyInMHz ;
    return wMaximumFrequencyInMHz;
  }

  //@return 0.0 = error / false input value
  float GriffinController::GetDefaultVoltageForFrequency(WORD wFrequInMHz)
  {
    float fVoltageInVolt = 0.0f;
    //TODO get Maximum Operating Frequency (=MOF; see AMD BKDG for 11h CPUs "chapter 2.4.2, MOF"

    WORD wAFourthFrequency = //GetMaximumFrequencyInMHz() / 4 ;
        mp_model->m_cpucoredata.m_wMaxFreqInMHz / 4 ;
    if(wAFourthFrequency )
    {
      //DEBUG("a fourth "
      //  "frequency of maximum frequency: %u\n", wAFourthFrequency);
      LOG("A fourth frequency of maximum frequency: " << wAFourthFrequency 
          << "\n" );
      //float fMaxFreqDivWantedFreq = wMaxFrequency / wFrequInMHz ;
      float fWantedFreqDivAFourthFreq = (float) wFrequInMHz / wAFourthFrequency;
      //The the freq is higher than a fourth of the maximal freq.
      //if( fMaxFreqDivWantedFreq <= 4.0f)
      if(fWantedFreqDivAFourthFreq > 1.0f )
        //ex.: max. freq. 2200 <=> 1/4 freq = 550MHz
        //wanted freq: 550 MHz->0,8V ; wanted freq/ 1/4 freq = 1
        //wanted freq: 1100 MHz->0,8+0.075*2=0,8V+0,15V=0,95V
        //wanted freq: 2200 MHz->0,8+0.075*4=0,8V+0,3V=1,1V
        //ex.: max. freq. 2000 <=> 1/4 freq = 500MHz
        //wanted freq: 500 MHz->0,8V ; wanted freq/ 1/4 freq = 1
        //wanted freq: 1000 MHz->0,8+0.075*2=0,8V+0,15V=0,95V
        //wanted freq: 2000 MHz->0,8+0.075*4=0,8V+0,3V=1,1V
        //fVoltageInVolt = 0.8f + fMaxFreqDivWantedFreq * 0.125f ;
        fVoltageInVolt = 0.8f + fWantedFreqDivAFourthFreq * 0.075f ;
        //fVoltageInVolt = 0.8f + fWantedFreqDivAFourthFreq * 0.125f ;
      else
        fVoltageInVolt = 0.8f ;
    }
    //Can get here if unknown Griffin CPU.
//#ifdef COMPILE_WITH_DEBUG
    else
    {
      DEBUG("Getting default voltage for frequency--a fourth frequency == 0\n");
      mp_userinterface->Confirm("error: no maximum frequeny found");
    }
//#endif
    //return wMaxFreqDivWantedFreq ;
    return fVoltageInVolt ;
  }

  //@return 0.0 = error / false input value
  WORD GriffinController::GetMinFreqToPreventOvervoltage(
    const MaxVoltageForFreq & maxvoltageforfreq,
    float fWantedVoltageInVolt
    )
  {
    //TODO accurate and senseful overvoltage protection here:
    //Die ben�tigte Spannung nimmt so zu: (Frequ^x-1) , x<1
    //bei einer Verdopplung der Taktfrq erh�ht sich die Spannung um 0,15V:
    //550->1100 : 0,8V ->0,95V
    //550->2200 : 0,8V ->1,1V (0,8V+0,15V*2*2)=(0,8V+0,15V*2*2)
    //also: falls 1650MHz:
    //1650 / 550 / 2 * 0,15
    //1,1V: 2200 MHz
    //jetzt will man x=0,8V haben-> 1,1V-x(0,8V) /0,15V=z(2) 2200MHz / 2^z(2)= 550
    // Formel der Core-Spannungs-Kurve (Param x ist die Spannung, y die MINIMALE Frequenz, um Overvolting zu vermeiden):
    // y = 2200 / 2^ ( (1.1-x) / 0.15)
    // Bsp.: x=1.1 V: y=2200/2^( (1.1-1.1)/0.15)=2200/2^(0/0.15)=
    //   2200/2^0=2200/1= >>2200 MHz<<
    // Bsp.: x=0.95 V: y = 1100 MHz
    // Bsp.: x=0.8 V: y = 550 MHz

    //Umkehrfunktion: 
    //1650 = 2200 / 2^ ((1.1 - x ) / 0.15 )  |  :2200
    //1650/2200 = 1 / 2 ^ ((1.1 - x ) / 0.15 )
    //1650 * 2 ^ ((1.1 - x) / 0.15) = 2200
    //2 ^ ((1.1 - x) / 0.15 ) = 1.333 | log2 ( to leave exp "(1.1 - x) / 0.15" alone)
    //log2(2 ^ ((1.1 - x) / 0.15 ) = log2 (1.33)
    //(1.1 - x) / 0.15 = log2 (1.33) | * 0.15
    //1.1 - x = log2 (1.33) * 0.15 | - 1.1
    //-x = log2 (1.33) * 0.15 - 1.1 | * (-1)
    //x = log2 (1.33) * (-0.15) + 1.1
    //x = 0,411426245726465 * (-0,15) + 1,1
    //x = 1,03828606314103025

    //Test mit 1100MHz (es muss 0.95 herauskommen):
    //1100 = 2200 / ( 2 ^ ((1.1 - x ) / 0.15 ) )  |  :2200
    //0.5 = 1 / ( 2 ^ ((1.1 - x ) / 0.15 ) )   | Kehrwert
    //2 = 2 ^ ((1.1 - x ) / 0.15 )  |  log[2]
    //log2 (2) = log2(2 ^ ((1.1 - x) / 0.15 )
    //1 = (1.1 - x) / 0.15  | * 0.15
    //0.15 = 1.1 - x | - 0.15
    //0 = 0.95 - x   |  +x
    //x = 0.95
    //========

    //Test mit 550MHz (es muss 0.8 herauskommen):
    //1/ 550 = ( 2 ^ ((1.1 - x ) / 0.15 ) )  / 2200   |  * 2200
    //2200 / 550 = 2 ^ ((1.1 - x ) / 0.15 )    | log[2]
    //log[2]( 2200 / 550) = (1.1 - x ) / 0.15 | * 0.15
    //log[2]( 2200 / 550) * 0.15 = 1.1 - x   | + x 
    //log[2]( 2200 / 550) * 0.15 + x = 1.1   | - ( log[2]( 2200 / 550) * 0.15 )
    //x = 1.1 - ( log[2]( 2200 / 550) * 0.15 )
    //x = 1.1 - ( 2 * 0.15 )
    //x = 1.1 - 0.3
    //x = 0.8
    //=======

    //for low stable voltages:
    //0.8 V (voltage for 1100 MHz) = 1.05 V - ( log[2]( 2200 MHz / 1100 MHz) * x )
    //0.8 V = 1.05 V - ( 1 * x ) | - 0.8 V
    //0 = 0.25 V - x | + x 
    //x = 0.25 V
    WORD wMinFreqToPreventOvervoltage = 
      //If no explicit cast: compiler warning:
      //"[...]converting to `WORD' from `double'"
      (WORD)
      maxvoltageforfreq.m_wFreqInMHz /
      //MUST NOT be an integer (but a floating point type!).
      //(WORD) 
      pow(2,(//fLowestAboveWantedVoltage
      maxvoltageforfreq.m_fVoltageInVolt -
      fWantedVoltageInVolt) / 0.15 );
    //return wMaxFreqDivWantedFreq ;
    //DEBUG("For %f V : MinFreqToPreventOvervoltage: %u \n", 
    //  fWantedVoltageInVolt ,wMinFreqToPreventOvervoltage );
    LOG("For " << fWantedVoltageInVolt 
        << " V : mininum frequency in MHz to prevent overvoltage: " 
        << wMinFreqToPreventOvervoltage << " \n" );
    return wMinFreqToPreventOvervoltage ;
  }

  BYTE GriffinController::GetNumberOfCPUCores()
  {
    BYTE byCoreNumber = 255 ;
    DWORD dwEAX;
    DWORD dwEBX;
    DWORD dwECX;
    DWORD dwEDX;
    DEBUG("PSC--getting number of CPU cores\n");
    if( //CpuidEx(
      mp_cpuaccess->CpuidEx(
      //AMD: "CPUID Fn8000_0008 Address Size And Physical Core Count Information"
      0x80000008,
      &dwEAX,
      &dwEBX,
      &dwECX,
      &dwEDX,
      1
        ) 
      )
    {
      byCoreNumber = ( dwECX & BITMASK_FOR_LOWMOST_7BIT ) 
        //"ECX 7:0 NC: number of physical cores - 1. 
        //The number of cores in the processor is NC+1 (e.g., if
        //NC=0, then there is one core). 
        //See also section 2.9.2 [Number of Cores and Core Number]."
        + 1 ;
      //DEBUG("Number of CPU cores: %u\n", (WORD) byCoreNumber );
      LOG("Number of CPU cores: " << (WORD) byCoreNumber << "\n" );
    }
    return byCoreNumber ;
  }

  WORD GriffinController::getFrequencyInMHz(BYTE byFrequID,BYTE byDivID)
  {
    return (unsigned short)(100 * ( (byFrequID+8) ) ) /
      (unsigned short)(//pow(2.0,(double)CpuDid)
		  1<<byDivID) ;
  }

  UserInterface * GriffinController::GetUserInterface()
  {
    return mp_userinterface;
  }

  unsigned long GriffinController::GetMSRregisterForPstate(
    unsigned char byPstate )
  {
	  //unsigned long index ;
	  return //"AMD: MSR C001_0064 specifies P-state 0"
		  0xC0010064 + byPstate ;
  }
  void GriffinController::GetMSRregisterValue( 
    BYTE byVoltageID, 
    const DIDandFID & didandfid , 
    DWORD & dwHighmostMSRvalue , 
    DWORD & dwLowmostMSRvalue 
    )
  {
    //GetFIDandDID( wFreqInMHz, byFrequencyID, byDivisorID ) ;
    //didandfid
    dwHighmostMSRvalue = 0 ;
    SET_P_STATE_TO_VALID(dwHighmostMSRvalue) ;

    //See AMD Family 11h Processor BKDG (document # 41256), 
    // "MSRC001_00[6B:64] P-state [7:0] Registers"
    //5:0 CpuFid
    dwLowmostMSRvalue = ( didandfid.m_byFreqID & 63) ;
    //CpuDid: core divisor ID.
    dwLowmostMSRvalue |= ( ( (WORD) didandfid.m_byDivisorID ) << 
      START_BIT_FOR_CPU_CORE_DIVISOR_ID ) ;
    //15:9 CpuVid: core VID.
    dwLowmostMSRvalue |= ( ( (WORD) byVoltageID ) << 9) ; //<=>bits 9-15 shifted
  }

  bool GriffinController::isVIDOptionSpecified(BYTE & rbyVID)
  {
    bool bisVIDOptionSpecified = true ;
    if( ! cmdLineParamsContain(_T("-vid"), rbyVID) )
    {
      rbyVID = 255 ;
      bisVIDOptionSpecified = false ;
    }
    return bisVIDOptionSpecified ;
  }

  bool GriffinController::isDivIDOptionSpecified(BYTE & rbyDivID)
  {
    bool bisDivIDOptionSpecified = true ;
    if( ! cmdLineParamsContain(_T("-did"), rbyDivID) )
    {
      rbyDivID = 255 ;
      bisDivIDOptionSpecified = false ;
    }
    return bisDivIDOptionSpecified ;
  }

  bool GriffinController::isFreqIDOptionSpecified(BYTE & rbyFreqID)
  {
    bool bisFreqIDOptionSpecified = true ;
    if(! cmdLineParamsContain(_T("-fid"), rbyFreqID) )
    {
      rbyFreqID = 255 ;
      bisFreqIDOptionSpecified = false ;
    }
    return bisFreqIDOptionSpecified ;
  }

  bool GriffinController::isModifierOptionSpecified(
    BYTE & rbyVID, BYTE & rbyDivID, BYTE & rbyFreqID)
  {
    bool bisVIDOptionSpecified = false ;
    bool bisDivIDOptionSpecified = false ;
    bool bisFreqIDOptionSpecified = false ;
    //bool bModifierOptionIsSpecified = false ;
    bisVIDOptionSpecified =  isVIDOptionSpecified(rbyVID) ;
    bisDivIDOptionSpecified = isDivIDOptionSpecified(rbyDivID) ;
    bisFreqIDOptionSpecified = isFreqIDOptionSpecified(rbyFreqID) ;
    return bisVIDOptionSpecified || bisDivIDOptionSpecified ||
      bisFreqIDOptionSpecified ;
  }

  bool GriffinController::isValidPstateOption(BYTE & rbyP_State)
  {
    return cmdLineParamsContain(_T("-p"),rbyP_State) ;
  }

  bool GriffinController::UseDefaultFormulaForOvervoltageProtection(
    BYTE byVID,
    WORD wWantedFrequInMHz)
  {
    bool bIsSafe = false ;
    DEBUG("Using the default formula to protect against overvolting\n" );
    float fWantedVoltageInVolt = PState::GetVoltageInVolt(byVID);
    //float fDefaultVoltageForWantedFreq = GetDefaultVoltageForFrequency(
    //  wWantedFrequInMHz);
    //if( 
    //  //The maximum voltage.
    //  1.55f 
    //  - ( (float)( byVID ) * 0.0125f ) <= fDefaultVoltageForWantedFreq )
    //  bIsSafe = true ;
    WORD wMaximumFrequencyInMHz = //GetMaximumFrequencyInMHz() ;
        mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    if( wMaximumFrequencyInMHz )
    {
//#ifdef COMPILE_WITH_DEBUG
//          WORD wMinFreqToPreventOvervoltage = GetMinFreqToPreventOvervoltage(
//            MaxVoltageForFreq(1.1, wMaximumFrequencyInMHz ) ,
//            fWantedVoltageInVolt ) ;
//          DEBUG("IsSafe(...)--MinFreqToPreventOvervoltage: %u\n", 
//            wMinFreqToPreventOvervoltage);
//#else
      //DEBUG("UseDefaultFormulaForOvervoltageProtection(...)--wanted "
      //  "freq: %u\n", wWantedFrequInMHz);
      LOG("Use Default Formula For Overvoltage Protection--wanted "
        "frequency in MHz: " << wWantedFrequInMHz << "\n" );
      if( wWantedFrequInMHz >= GetMinFreqToPreventOvervoltage(
        MaxVoltageForFreq(1.1f, wMaximumFrequencyInMHz ) ,
        fWantedVoltageInVolt )
        )
        bIsSafe = true ;
//#endif
    }
    else
      this->mp_userinterface->Confirm("Error: max. freq was not determined\n");
    return bIsSafe ;
  }

  //Used to stabilize the machine if e.g. calculation errors occured because
  //of undervolting.
  void GriffinController::IncreaseVoltageForCurrentPstate(BYTE byCoreID)
  {
    DWORD dwHigh = 0, dwLow = 0;
    if(!m_bPstateSet)
    {
      //DWORD dwLow, dwHigh ;
      GetPStateFromMSR(GetCurrentPstate(//1
        1 << byCoreID )
        ,dwLow
        , dwHigh
        , m_pstate, 
        //1 
        byCoreID
        ) ;
    }
    //else
    {
      //m_pstate.m_byVoltageID += 2 ;
      //Lowering the Voltage ID means increasing the voltage.
      m_pstate.m_byVoltageID -= 2 ;
      //mp_cpuaccess->WrmsrEx();
      if( WriteToPstateOrCOFVIDcontrolRegister(
        GetMSRregisterForPstate(//p_pstateCurrent->m_byNumber
        m_byPstateForFindingLowVoltage),
        //pstateFromMSR.m_byVoltageID,
        //pstateFromMSR.m_byFreqID,
        //pstateFromMSR.m_byDivisorID,
        //pstate,
        //pstate,
        m_pstate,
        m_pstate,
        dwHigh,
        dwLow,
        //byCoreID
        0
        )
        ) 
        //Finally (re)set the p-state (even it's the same as before) 
        //to take the changes effect.
        SetPstate(m_byPstateForFindingLowVoltage,//1 << byCoreID 
          //1 
          0 //Core ID
          ) ;
    }

  }

  //bool
  BYTE GriffinController::GetPstateSafefy(
    BYTE byVID, 
    BYTE byFrequencyID, 
    BYTE byDivisorID
    )
  {
    //bool bIsSafe = false ;
    BYTE byReturn = SETTING_VOLTAGE_IS_UNSAFE ;

    if( byFrequencyID > MAX_VALUE_FOR_FID )
    {
      std::ostrstream ostrstream ;
      ostrstream << "Frequency ID is above the max. Freqency ID \"" 
        << MAX_VALUE_FOR_FID << "\"" ;
      mp_userinterface->Confirm(ostrstream);
    }
    else if(byDivisorID > MAX_VALUE_FOR_DID )
    {
      std::ostrstream ostrstream ;
      ostrstream << "Divisor ID is above the max. Divisor ID \"" 
        << MAX_VALUE_FOR_DID << "\"" ;
      mp_userinterface->Confirm(ostrstream);
    }
    else if( byVID > MAX_VALUE_FOR_VID )
    {
      std::ostrstream ostrstream ;
      ostrstream << "Voltage ID is above the max. Voltage ID \"" 
        << MAX_VALUE_FOR_VID << "\"" ;
      mp_userinterface->Confirm(ostrstream);      
    }
    else
    {
      //DEBUG("IsSafe(VID:%u, FID:%u, DID:%u)--begin\n",
      //  byVID,byFrequencyID,byDivisorID);
      DEBUG("For checking p-state safety: VID:" << 
          (WORD) byVID 
          << ", FID:" << 
          (WORD) byFrequencyID 
          << ", DID:" << 
          (WORD) byDivisorID 
          << "\n" );
      //Safe states are: 
      //-550MHz, voltage<=0.8V, 550/0,8=687,5
      //-1100MHz, voltage<=0.95V, 1157,89 MHz/V
      //-2200MHz, voltage<=1.1V, 2000 MHz/V
      //f(frequency)=frequency/687,5+ frequency/550*0,15=max_voltage ??
      //:f(550)=550/687,5
      WORD wWantedFrequInMHz = GriffinController::getFrequencyInMHz(
        byFrequencyID,byDivisorID);
      //DEBUG("Resulting freq in MHz is:%lu, VID is:%u\n",wWantedFrequInMHz,byVID);
      LOG("Resulting frequency in MHz is:" << wWantedFrequInMHz 
          //<< ", Voltage ID is:" << (WORD) byVID 
          << ", Voltage is:" << PState::GetVoltageInVolt( byVID ) << " Volt"
          << "\n" );

      if( //m_model.m_vecmaxvoltageforfreq.empty() 
        //|| 
        //m_model.m_bUseDefaultFormularForOvervoltageProtection
        mp_model->m_bUseDefaultFormularForOvervoltageProtection
        //true 
        )
      {
        DEBUG(//"Processor name not found in config file or explicitely set to "
          "set to use the default formula for overvoltage protection\n");
        //if( m_model.m_bEnableOvervoltageProtection )
        if( mp_model->m_bEnableOvervoltageProtection )
        {
          //bIsSafe
          byReturn = UseDefaultFormulaForOvervoltageProtection(byVID,
            wWantedFrequInMHz );
        }
        else
        {
          DEBUG("NOT using any overvolting protection\n" );
          byReturn = NOT_USING_ANY_OVERVOLTING_PROTECTION ;
        }
      }
      else
      {
        //if( m_model.m_bEnableOvervoltageProtection )
        if( mp_model->m_bEnableOvervoltageProtection )
        {
          LOG("overvoltage protection is enabled\n") ; 
          float fMaxVoltageForWantedFreq ;
          //byReturn = UseMaxVoltageForFreqForOvervoltageProtection(
          //    byVID, wWantedFrequInMHz) ;
          float fWantedVoltageInVolt = PState::GetVoltageInVolt(byVID);
          //float fMaxVoltageForWantedFreq = GetMaxVoltageForFreq( 
          byReturn = GetMaxVoltageForFreq( 
              wWantedFrequInMHz ,
              fMaxVoltageForWantedFreq 
              ) ;
          LOG("Max. voltage is " << 
              //no scientific (e.g. 1e+015)notation
              std::fixed << 
              //2 digits after decimal point
              std::setprecision(2) <<
              fMaxVoltageForWantedFreq << "\n" ) ; 
          if( byReturn == APPROPRIATE_2_VOLTAGE_FREQUENCY_PAIRS_FOUND || 
              byReturn == ONE_APPROPRIATE_VOLTAGE_FREQUENCY_PAIR_FOUND )
          {
              //LOG("Appropriate voltage-frequency pairs were found.\n" ) ; 
              if( fWantedVoltageInVolt > fMaxVoltageForWantedFreq )
              {
                 LOG("The voltage is too high for the freq.\n" ) ; 
                  byReturn = VOLTAGE_IS_TOO_HIGH_FOR_FREQUENCY ;
              }
              else
              {
                  byReturn = SETTING_VOLTAGE_IS_SAFE ;
                 LOG("Setting the voltage is safe.\n" ) ; 
              }
          }
        }
        else
        {
          DEBUG("NOT using any overvolting protection\n" );
          byReturn = NOT_USING_ANY_OVERVOLTING_PROTECTION ;
        }
      }
    }//FID,DID, and VID <= maximum values
    //DEBUG("Setting P-state is %ssafe\n", //bIsSafe
    //  byReturn ? "" : "NOT " );
    LOG( "Setting P-state is " << ( byReturn ? "" : "NOT " ) << "safe\n" );
    return //bIsSafe
      byReturn ;
  }

  //Use this method rather than the version with "char * &" where the
  //memory is allocated inside the function!
  bool GriffinController::GetProcessorNameByCPUID(std::string & r_stdstr)
  {
    bool bSuccess ;
    char * archCPUID ;
    bSuccess = GetProcessorNameByCPUID(archCPUID) ;
    r_stdstr = std::string( archCPUID ) ;
    //Was allocated on heap inside "GetProcessorNameByCPUID(char * &)".
    delete archCPUID ;
    return bSuccess ;
  }

  //Use the method with std::string parameter rather than this 
  //version with "char * &" where the
  //memory is allocated inside the function!
  bool GriffinController::GetProcessorNameByCPUID(
    //Use a pointer to an array in order to allocate the array within 
    //this method.
    char * & archCPUID )
  {
    bool bSuccess = true ;
    BYTE byCPUID_Address = 0, byCharIndex = 0;
    //char archCPUID[//4*4
    //  CPUID_PROCESSOR_NAME_CHAR_NUMBER
    //  //For string terminating "\0" .
    //  + 1 ] ;
    archCPUID = new char [//4*4
      CPUID_PROCESSOR_NAME_CHAR_NUMBER
      //For string terminating "\0" .
      + 1 ] ;
    archCPUID[//4*4
      CPUID_PROCESSOR_NAME_CHAR_NUMBER ] = '\0';
    for( ; byCPUID_Address < 3 ; ++ byCPUID_Address )
    {
      if( GetCPUID(
        //AMD: "CPUID Fn8000_000[4:2] Processor Name String Identifier"
        0x80000002+byCPUID_Address,
        //dwEAX,
        //*((DWORD *)archCPUID),
        *((DWORD *)(archCPUID + byCharIndex) ),
        //dwEBX,
        //*((DWORD *)(archCPUID+4)),
        *((DWORD *)(archCPUID + byCharIndex + 4 )),
        //dwECX,
        //*((DWORD *)(archCPUID+8)),
        *((DWORD *)(archCPUID + byCharIndex + 8 )),
        //dwEDX
        //*((DWORD *)(archCPUID+12))
        *((DWORD *)(archCPUID + byCharIndex + 12 ))
        ) )
      {
        byCharIndex += 16 ;
        //DEBUG("CPUID address:%lu "
        //  //"EAX:%lu,EBX:%lu ECX:%lu,EDX:%lu "
        //  //"%s\n"
        //  "\n"
        //  ,
        //  (0x80000002+byCPUID_Address)//, 
        //  //dwEAX, dwEBX, dwECX, dwEDX,
        //  //archCPUID
        //  );
        LOG("CPUID address:" << ( 0x80000002 + byCPUID_Address ) << " \n" );
        //archProcessorName[
      }
      else
      {
        DEBUG("Error getting processor name of this CPU\n");
        bSuccess = false ;
        break ;
      }
    }//end for-loop
    if( bSuccess )
      //DEBUG("processor name of this CPU is: %s\n", archCPUID );
      LOG("Processor name of this CPU is: " << archCPUID << "\n" );
    return bSuccess ;
  }

  //For beeing very safe: check also if this is a Turio ULTRA.
  //There are CPUs put there with the same family and model number 
  //as the Turion Ultra, but they are only Turions (e.g. the RM-70).
  bool GriffinController::IsTurionUltra()
  {
    bool bIsTurionUltra = false ;
    //char * archCPUID ;
    std::string strCPUID ;
    //char archProcessorName[4*4*3 
    //  //For string terminating "\0" .
    //  + 1 ] ;
    //char archCPUIDofTurionUltra [] = 
    //  " DMAiruTt(nox )mU 2xartlUad oC-lM erliboMZ e28-";
    char archCPUIDofTurionUltra [] = 
        "AMD Turion(tm) X2 Ultra Dual-Core Mobile ZM-82";
    
    if( GetProcessorNameByCPUID( //archCPUID 
        strCPUID ) )
    {
      //archCPUID [ //CPUID_PROCESSOR_NAME_CHAR_NUMBER - 2 
      //  strlen(archCPUIDofTurionUltra) ] = 
      //  //leave the number after "ZM-" out for comparing.
      //  '\0';
      //DEBUG("Whole processor name:%s\n", //archCPUID
      //  strCPUID.c_str() );
      DEBUG("Whole processor name:" << strCPUID.c_str() << "\n" );
      //return //strcmp(...) returns "0" if strings are identical.
      bIsTurionUltra = !strcmp(archCPUIDofTurionUltra,//archCPUID
          strCPUID.c_str() );
      if( ! bIsTurionUltra )
      {
        std::ostrstream ostrstream ;
        ostrstream << "Only CPUs with CPUID processor starting with \"" << 
          archCPUIDofTurionUltra << "\" are supported. Your CPUID processor "
          "name is/ starts with" << //archCPUID
          strCPUID ;
        mp_userinterface->Confirm(ostrstream);
      }
    }
    return bIsTurionUltra ;
  }

  bool GriffinController::IsSupportedCPUModel()
  {
    DWORD dwEAX;
    DWORD dwEBX;
    DWORD dwECX;
    DWORD dwEDX;
    DEBUG("Supported CPU Model?--begin\n");
    //mp_userinterface->Confirm("PumaStateCtrl::IsSupportedCPUModel()");
//#ifndef LINK_TO_WINRING0_STATICALLY
//    std::string strFuncName = "CpuidEx" ;
//    _CpuidEx pfnCPU_ID;
//    pfnCPU_ID = (_CpuidEx)GetProcAddress(m_hinstanceWinRingDLL,
//      strFuncName.c_str() );
//    if(pfnCPU_ID)
//    {
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
      //"If the function succeeds, the return value is TRUE."
      if( 
//#ifndef LINK_TO_WINRING0_STATICALLY
//        (*pfnCPU_ID)
//#else
//        CpuidEx
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
        mp_cpuaccess->CpuidEx
        //if( CpuidEx
        (0x00000000,&dwEAX,&dwEBX,&dwECX,&dwEDX,1) 
        )
      {
        //std::ostringstream ostrstream;
        //ostrstream<<"dwEBX:"<<dwEBX<<"dwEDX:"<<dwEDX<<"dwECX:"<<dwECX<<"\n";
        //mp_userinterface->Confirm(ostrstream.str());
        //Processor Manufacturer is encoded as ASCII:
        if( dwEBX == 0x68747541 //=1752462657dez = h t u A? -> Auth
          && dwEDX == 0x69746E65 //=1769238117dez = ?i t n e. ->enti
          && dwECX == 0x444D4163 //=1145913699dez = D M A c, ->cAMD
          //-> AuthenticAMD
          && 
//#ifndef LINK_TO_WINRING0_STATICALLY
//          (*pfnCPU_ID)
//#else
//          CpuidEx
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
          mp_cpuaccess->CpuidEx
          (
            //Query CPUID Function 0000_0001 for CPU model and family.
            0x00000001,&dwEAX,&dwEBX,&dwECX,&dwEDX,1)
          )
        {
          BYTE byModel = 0, byFamily = 0 ;
          BYTE byBaseFamily = 0 , byExtendedFamily = 0 ;
          BYTE byExtendedModel = 0 ;
          BYTE byBaseModel = 0 ;
          //mp_userinterface->Confirm("AMD CPU");
          //if(
          //"Model is an 8-bit value and is defined as: Model[7:0] = 
          //{ExtendedModel[3:0], BaseModel[3:0]}".
          byExtendedModel = 
          //from "AMD Family 11h Processor BKDG", "41256 Rev 3.00 - July 07, 2008"
          //   -"F3xFC CPUID Family/Model Register" /
          //   -"CPUID Fn0000_0001_EAX Family, Model, Stepping Identifiers"
          //    :"Family is an 8-bit value and is defined as: Family[7:0] = 
          //      ({0000b,BaseFamily[3:0]} + ExtendedFamily[7:0]).
          //      E.g. If BaseFamily[3:0]=Fh and ExtendedFamily[7:0]=02h, 
          //      then Family[7:0]=11h. This document applies only to family 
          //      11h processors."
          //    "19:16 ExtendedModel."
          //byModel = 
            (BYTE) ( ( dwEAX 
              //Bits 19:16 are the ExtendedModel, so shift 16 bits right.
              >> 16 ) & 
              //0xF = 1111 bin
              0xF) ;

          //DEBUG("ExtendedModel: %u ",byModel)
            //LOG("Extended CPU Model: " <<           
            //  //Convert to integer in order to output as number 
            //  (WORD) //byModel 
            //  byExtendedModel << " ")
          //byModel <<= 4 ;
          //byModel |= 
          byBaseModel = (dwEAX >> 4 & 0xF) ; //"7:4 BaseModel."
          //  "Model is an 8-bit value and is defined as: Model[7:0] = 
          //      {ExtendedModel[3:0], BaseModel[3:0]}. 
          //     E.g. If ExtendedModel[3:0]=Eh and BaseModel[3:0]=8h, 
          //        then Model[7:0] = E8h. 
          //     Model numbers vary with product.
          byModel = ( byExtendedModel << 4 ) | byBaseModel ;
          //DEBUG("Base Model: %u ",dwEAX>>4 & 0xF)
          //LOG("Base Model: " << ( dwEAX >> 4 & 0xF ) << " ")
          //byFamily 
          byBaseFamily = (BYTE) ( ( dwEAX >> 8 ) & 0xF ) ; //"11:8 BaseFamily: Fh."
          //byFamily += 
          byExtendedFamily = (BYTE) ( ( dwEAX 
              ////"27:20 ExtendedFamily: 02h."
              >> 20 ) & 0xFF ) ; 
          //"Family is an 8-bit value and is defined as: 
          //    Family[7:0] = ({0000b,BaseFamily[3:0]} + ExtendedFamily[7:0])."
          byFamily = byBaseFamily + byExtendedFamily ;
          //DEBUG(" model: %u family:%u\n",byModel,byFamily)
          LOG( "Extended CPU model:" 
              << (WORD) byExtendedModel 
              << ", base model: " 
              << (WORD) byBaseModel 
              << "->model:" 
              << (WORD) byModel 
              << "; extended CPU family:" 
              << (WORD) byExtendedFamily
              << "base family:"
              << (WORD) byBaseFamily
              << "->family:" 
              << (WORD) byFamily 
              << ";stepping: "
              //3:0 Stepping: processor stepping (revision) for a specific model.
              << ( dwEAX & 0xF )
              << "\n" 
              ) ;
          if(byModel == 3 && byFamily == 17//(=11hex=Griffin CPU)
            //&& IsTurionUltra() 
            )
            return true;
          else
            mp_userinterface->Confirm("Only AMD family 17, model 3 "
              "CPUs are supported");
        }
      }
      else
        mp_userinterface->Confirm("CPUID function failed");
//#ifndef LINK_TO_WINRING0_STATICALLY
//    } //if(pfnCPU_ID)
//    else
//      mp_userinterface->Confirm("Error getting CPUID function from DLL");
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
    return false;
  }

  bool GriffinController::GetMaxPState(BYTE & byMaxPstate)
  {
    bool bSuccess = false ;
    DWORD dwValue ;
    if( GetCPUMiscControlDWORD(
      F3xDC_CLOCK_POWER_TIMING_CONTROL_2_REGISTER_ADDRESS, dwValue) 
      )
    {
      byMaxPstate = 
        //"10:8 PstateMaxVal"
        (BYTE) ( ( dwValue >> 8 ) & BITMASK_FOR_LOWMOST_3BIT ) ;
      bSuccess = true ;
    }
    return bSuccess ;
  }

  //BYTE GriffinController::GetInterpolatedVoltageFromFreq(
  //  WORD wFreqInMHzToGetVoltageFrom
  //  , const //MaxVoltageForFreq 
  //    VoltageAndFreq & cr_maxfreqandvoltageNearestFreqGreaterEqual
  //  , const //MaxVoltageForFreq 
  //    VoltageAndFreq & cr_maxfreqandvoltageNearestFreqLowerEqual
  //  , float & r_fVoltageInVolt 
  //  )
  //{
  //  if( cr_maxfreqandvoltageNearestFreqGreaterEqual.m_wFreqInMHz ==
  //    cr_maxfreqandvoltageNearestFreqLowerEqual.m_wFreqInMHz )
  //  {
  //    r_fVoltageInVolt = cr_maxfreqandvoltageNearestFreqGreaterEqual.m_fVoltageInVolt ;
  //      return true ;
  //  }
  //  else
  //  {
  //    WORD wFreqInMHzFromNearFreqAboveWantedFreq =
  //      cr_maxfreqandvoltageNearestFreqGreaterEqual.m_wFreqInMHz ;
  //    WORD wFreqInMHzFromNearFreqBelowWantedFreq =
  //      cr_maxfreqandvoltageNearestFreqLowerEqual.m_wFreqInMHz ;
  //    //if( mp_model->m_pstates.m_arp_pstate[1] &&
  //    //  mp_model->m_pstates.m_arp_pstate[1]->GetFreqInMHz() <
  //    //  wFreqInMHzToGetVoltageFrom
  //    //  )
  //    //{
  //      float fVoltageInVoltFromNearFreqAboveWantedFreq ;
  //      float fVoltageInVoltFromNearFreqBelowWantedFreq ;
  //      fVoltageInVoltFromNearFreqAboveWantedFreq =
  //        //mp_model->m_pstates.m_arp_pstate[0]->GetVoltageInVolt() ;
  //        cr_maxfreqandvoltageNearestFreqGreaterEqual.m_fVoltageInVolt ;
  //      fVoltageInVoltFromNearFreqBelowWantedFreq =
  //        //mp_model->m_pstates.m_arp_pstate[1]->GetVoltageInVolt() ;
  //        cr_maxfreqandvoltageNearestFreqLowerEqual.m_fVoltageInVolt ;
  //      //  VoltagefromHigherFreq - x = voltagefromLowerfreq ;
  //      //  x = log2( y ) * voltagefromFreqAboveAndBeowDiff 
  //      // VoltagefromHigherFreq - log2( freqabove / freq ) *
  //      //   x * voltagefromFreqAboveAndBelowDiff 
  //      //   = voltagefromLowerfreq ;
  //      // VoltagefromHigherFreq - log2Result *
  //      //   x * voltagefromFreqAboveAndBelowDiff 
  //      //   = voltagefromLowerfreq ;
  //      //voltagefromLowerfreq = VoltagefromHigherFreq -
  //      //  log2ResultMultvoltagefromFreqAboveAndBelowDiff * x | - VoltagefromHigherFreq
  //      //voltagefromLowerfreq - VoltagefromHigherFreq =
  //      // log2ResultMultvoltagefromFreqAboveAndBelowDiff * x | / 
  //      //            log2ResultMultvoltagefromFreqAboveAndBelowDiff
  //      //(voltagefromLowerfreq - VoltagefromHigherFreq ) /
  //      //  log2ResultMultvoltagefromFreqAboveAndBelowDiff = x

  //      //voltageLowerFreq = VoltagefromHigherFreq - log_x( freqabove / freq ) *
  //      //   voltagefromFreqAboveAndBelowDiff  | - VoltagefromHigherFreq
  //      //voltageLowerFreq - VoltagefromHigherFreq = - log_x( freqaboveDivFreq ) *
  //      //   voltagefromFreqAboveAndBelowDiff | / voltagefromFreqAboveAndBelowDiff
  //      //(voltageLowerFreq - VoltagefromHigherFreq ) / 
  //      //  voltagefromFreqAboveAndBelowDiff = - log_x( freqaboveDivFreq )
  //      //voltageLowerFreqMinusVoltagefromHigherFreq /
  //      // voltagefromFreqAboveAndBelowDiff = - log_x( freqaboveDivFreq )
  //      //givenVoltage = voltageLowerFreqMinusVoltagefromHigherFreq /
  //      //  voltagefromFreqAboveAndBelowDiff 
  //      //givenVoltage = - log_x( freqaboveDivFreq )  x=?
  //      //
  //      //voltageLowerFreq = VoltagefromHigherFreq - log_x( freqabove / freq ) *
  //      //  voltagefromFreqAboveAndBelowDiff | + log_x( freqabove / freq ) *
  //      //      voltagefromFreqAboveAndBelowDiff
  //      //voltageLowerFreq + log_x( freqabove / freq ) *
  //      //   voltagefromFreqAboveAndBelowDiff = VoltagefromHigherFreq | - voltageLowerFreq
  //      //log_x( freqabove / freq ) * voltagefromFreqAboveAndBelowDiff =
  //      //  VoltagefromHigherFreq - voltageLowerFreq 
  //      //log_x( freqabove / freq ) * voltagefromFreqAboveAndBelowDiff =
  //      //   VoltagefromHigherFreqMinusVoltageLowerFreq | /
  //      //log_x( freqabove / freq ) = VoltagefromHigherFreqMinusVoltageLowerFreq / 
  //      //   voltagefromFreqAboveAndBelowDiff
  //      //log_x( freqaboveDivFreq ) = VoltagefromHigherFreqMinusVoltageLowerFreq / 
  //      //   voltagefromFreqAboveAndBelowDiff x=?
  //      //givenVoltage = VoltagefromHigherFreqMinusVoltageLowerFreq / 
  //      //   voltagefromFreqAboveAndBelowDiff
  //      //log_x( freqaboveDivFreq ) = givenVoltage    x=?
  //      //example: log_x(2,0952380952) = 0,2125 V / 0,2125 V 
  //      //highervoltag-lowervoltage = log_x(2,0952380952)

  //      //http://www.mathe-fa.de/en#anchor:
  //      //1.0625-log(4;2200/550)*0.3125= 0.75   (x=4)
  //      //1.0625-log(x;2200/550)*0.3125= 0.75 | + log(x;2200/550)*0.3125
  //      //1.0625 = 0.75 + log(x;2200/550)*0.3125 | - 0.75
  //      //0,3125 = log(x;2200/550)*0.3125 | / 0.3125
  //      //1 = log(x;2200/550)
  //      //1 = log(x;4) = log_x(4)

  //      //example: 1.0625 V - 0.85V = 0,2125 V
  //      float fVoltageFromFreqAboveAndBelowDiff = 
  //        fVoltageInVoltFromNearFreqAboveWantedFreq -
  //        fVoltageInVoltFromNearFreqBelowWantedFreq ;
  //      //example: 2200 MHz / 1050 MHz ~= 2,0952380952
  //      double dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom =
  //          (double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //          (double) wFreqInMHzToGetVoltageFrom ;

  //      //ex.: -0,9371068287545918599662966193649 * 0,2125 = 
  //      //     -0,19913520111035077024283803161504
  //      //fCorrectionFactor = (
  //      //  //example: 0.85
  //      //  fVoltageInVoltFromNearFreqBelowWantedFreq - 
  //      //  //example: 1.0625
  //      //  fVoltageInVoltFromNearFreqAboveWantedFreq ) 
  //      //  //example: -0,2125
  //      //  /
  //      //  //ex.: 0,0481868754067370599298056614097
  //      //  ( //example:  0,22676176661993910555202664192874
  //      //    - log_dualis( 
  //      //    //(double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //      //    //(double) wFreqInMHzToGetVoltageFrom
  //      //    dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom
  //      //    ) 
  //      //  //ex: -0,9371068287545918599662966193649
  //      //    * 
  //      //    //example: 0,2125 V
  //      //    fVoltageFromFreqAboveAndBelowDiff ) ;

  //      r_fVoltageInVolt =
  //        //fVoltageOfOvVoltProtVnf_pairHigherEqualWantedFreq
  //        // 1,0625 - 0,22676176661993910555202664192874
  //        //  = 0,835738233380060894447973358072
  //        fVoltageInVoltFromNearFreqAboveWantedFreq -
  //          //log10(2,0952380952380952380952380952381) =
  //          //  0,32123338175226816317043359884426
  //          //log10(2) = 0,30102999566398119521373889472449
  //          // => log2(2,0952380952380952380952380952381) = 
  //          //  0,32123338175226816317043359884426 / 
  //          //  0,30102999566398119521373889472449 
  //          //  = 1,067114195858536967303654785547
  //          // 1,067114195858536967303654785547 * 0,2125 =
  //          //   0,22676176661993910555202664192874
  //          //log_dualis
  //          log_x
  //          (
  //            (double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //            (double) wFreqInMHzFromNearFreqBelowWantedFreq
  //            ,
  //          //wFreqOfOvVoltProtVnf_pairHigherEqualWantedFreq
  //            //example: 2200 MHz / 1050 MHz ~= 2.0952380952
  //            //example: 2200 MHz / 550 MHz = 4
  //            (double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //            (double) wFreqInMHzToGetVoltageFrom
  //          )
  //          *
  //          //fVoltageDiffBetwOvVoltProtVnf_pairs
  //          (
  //            //example: 1.0625 V - 0.85V = 0,2125 V
  //            //example: 1.0625 V - 0.75V = 0.3125 V
  //            fVoltageInVoltFromNearFreqAboveWantedFreq -
  //            fVoltageInVoltFromNearFreqBelowWantedFreq
  //          )
  //        ;
  //      return true ;
  //    //}
  //  }
  //}

  //BYTE GriffinController::GetInterpolatedVoltageFromFreq(
  //  WORD wFreqInMHzToGetVoltageFrom,
  //  float & r_fVoltageInVolt 
  //  )
  //{
  //  //WORD wFreqInMHzFromNearFreqAboveWantedFreq =
  //    //mp_model->m_pstates.m_arp_pstate[0]->GetFreqInMHz() ;
  //  PState * p_pstateGreaterEqual = mp_model->m_pstates.
  //    GetPstateWithNearestFreqGreaterEqual( wFreqInMHzToGetVoltageFrom , 4) ;
  //  PState * p_pstateLowerEqual = mp_model->m_pstates.
  //    GetPstateWithNearestFreqLowerEqual( wFreqInMHzToGetVoltageFrom, 4 ) ;
  //  if( //mp_model->m_pstates.m_arp_pstate[0] &&
  //    //mp_model->m_pstates.m_arp_pstate[0]->GetFreqInMHz() >
  //    //wFreqInMHzToGetVoltageFrom 
  //    p_pstateGreaterEqual && p_pstateLowerEqual
  //    )
  //  {
  //    if( //This is the case if wFreqInMHzToGetVoltageFrom has the 
  //      //same freq as one of the p-states.
  //      //This case must be catched, else wrong values by the 
  //      //log_x() function (  log_1(1)  is calculated then ) .
  //      p_pstateGreaterEqual == p_pstateLowerEqual 
  //      )
  //    {
  //      r_fVoltageInVolt = p_pstateGreaterEqual->GetVoltageInVolt() ;
  //      return true ;
  //    }
  //    else
  //    {
  //    WORD wFreqInMHzFromNearFreqAboveWantedFreq =
  //      p_pstateGreaterEqual->GetFreqInMHz() ;
  //    WORD wFreqInMHzFromNearFreqBelowWantedFreq =
  //      p_pstateLowerEqual->GetFreqInMHz() ;
  //    //if( mp_model->m_pstates.m_arp_pstate[1] &&
  //    //  mp_model->m_pstates.m_arp_pstate[1]->GetFreqInMHz() <
  //    //  wFreqInMHzToGetVoltageFrom
  //    //  )
  //    //{
  //      float fVoltageInVoltFromNearFreqAboveWantedFreq ;
  //      float fVoltageInVoltFromNearFreqBelowWantedFreq ;
  //      fVoltageInVoltFromNearFreqAboveWantedFreq =
  //        //mp_model->m_pstates.m_arp_pstate[0]->GetVoltageInVolt() ;
  //        p_pstateGreaterEqual->GetVoltageInVolt() ;
  //      fVoltageInVoltFromNearFreqBelowWantedFreq =
  //        //mp_model->m_pstates.m_arp_pstate[1]->GetVoltageInVolt() ;
  //        p_pstateLowerEqual->GetVoltageInVolt() ;
  //      //  VoltagefromHigherFreq - x = voltagefromLowerfreq ;
  //      //  x = log2( y ) * voltagefromFreqAboveAndBeowDiff 
  //      // VoltagefromHigherFreq - log2( freqabove / freq ) *
  //      //   x * voltagefromFreqAboveAndBelowDiff 
  //      //   = voltagefromLowerfreq ;
  //      // VoltagefromHigherFreq - log2Result *
  //      //   x * voltagefromFreqAboveAndBelowDiff 
  //      //   = voltagefromLowerfreq ;
  //      //voltagefromLowerfreq = VoltagefromHigherFreq -
  //      //  log2ResultMultvoltagefromFreqAboveAndBelowDiff * x | - VoltagefromHigherFreq
  //      //voltagefromLowerfreq - VoltagefromHigherFreq =
  //      // log2ResultMultvoltagefromFreqAboveAndBelowDiff * x | / 
  //      //            log2ResultMultvoltagefromFreqAboveAndBelowDiff
  //      //(voltagefromLowerfreq - VoltagefromHigherFreq ) /
  //      //  log2ResultMultvoltagefromFreqAboveAndBelowDiff = x

  //      //voltageLowerFreq = VoltagefromHigherFreq - log_x( freqabove / freq ) *
  //      //   voltagefromFreqAboveAndBelowDiff  | - VoltagefromHigherFreq
  //      //voltageLowerFreq - VoltagefromHigherFreq = - log_x( freqaboveDivFreq ) *
  //      //   voltagefromFreqAboveAndBelowDiff | / voltagefromFreqAboveAndBelowDiff
  //      //(voltageLowerFreq - VoltagefromHigherFreq ) / 
  //      //  voltagefromFreqAboveAndBelowDiff = - log_x( freqaboveDivFreq )
  //      //voltageLowerFreqMinusVoltagefromHigherFreq /
  //      // voltagefromFreqAboveAndBelowDiff = - log_x( freqaboveDivFreq )
  //      //givenVoltage = voltageLowerFreqMinusVoltagefromHigherFreq /
  //      //  voltagefromFreqAboveAndBelowDiff 
  //      //givenVoltage = - log_x( freqaboveDivFreq )  x=?
  //      //
  //      //voltageLowerFreq = VoltagefromHigherFreq - log_x( freqabove / freq ) *
  //      //  voltagefromFreqAboveAndBelowDiff | + log_x( freqabove / freq ) *
  //      //      voltagefromFreqAboveAndBelowDiff
  //      //voltageLowerFreq + log_x( freqabove / freq ) *
  //      //   voltagefromFreqAboveAndBelowDiff = VoltagefromHigherFreq | - voltageLowerFreq
  //      //log_x( freqabove / freq ) * voltagefromFreqAboveAndBelowDiff =
  //      //  VoltagefromHigherFreq - voltageLowerFreq 
  //      //log_x( freqabove / freq ) * voltagefromFreqAboveAndBelowDiff =
  //      //   VoltagefromHigherFreqMinusVoltageLowerFreq | /
  //      //log_x( freqabove / freq ) = VoltagefromHigherFreqMinusVoltageLowerFreq / 
  //      //   voltagefromFreqAboveAndBelowDiff
  //      //log_x( freqaboveDivFreq ) = VoltagefromHigherFreqMinusVoltageLowerFreq / 
  //      //   voltagefromFreqAboveAndBelowDiff x=?
  //      //givenVoltage = VoltagefromHigherFreqMinusVoltageLowerFreq / 
  //      //   voltagefromFreqAboveAndBelowDiff
  //      //log_x( freqaboveDivFreq ) = givenVoltage    x=?
  //      //example: log_x(2,0952380952) = 0,2125 V / 0,2125 V 
  //      //highervoltag-lowervoltage = log_x(2,0952380952)

  //      //http://www.mathe-fa.de/en#anchor:
  //      //1.0625-log(4;2200/550)*0.3125= 0.75   (x=4)
  //      //1.0625-log(x;2200/550)*0.3125= 0.75 | + log(x;2200/550)*0.3125
  //      //1.0625 = 0.75 + log(x;2200/550)*0.3125 | - 0.75
  //      //0,3125 = log(x;2200/550)*0.3125 | / 0.3125
  //      //1 = log(x;2200/550)
  //      //1 = log(x;4) = log_x(4)

  //      //example: 1.0625 V - 0.85V = 0,2125 V
  //      float fVoltageFromFreqAboveAndBelowDiff = 
  //        fVoltageInVoltFromNearFreqAboveWantedFreq -
  //        fVoltageInVoltFromNearFreqBelowWantedFreq ;
  //      //example: 2200 MHz / 1050 MHz ~= 2,0952380952
  //      double dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom =
  //          (double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //          (double) wFreqInMHzToGetVoltageFrom ;

  //      //ex.: -0,9371068287545918599662966193649 * 0,2125 = 
  //      //     -0,19913520111035077024283803161504
  //      //fCorrectionFactor = (
  //      //  //example: 0.85
  //      //  fVoltageInVoltFromNearFreqBelowWantedFreq - 
  //      //  //example: 1.0625
  //      //  fVoltageInVoltFromNearFreqAboveWantedFreq ) 
  //      //  //example: -0,2125
  //      //  /
  //      //  //ex.: 0,0481868754067370599298056614097
  //      //  ( //example:  0,22676176661993910555202664192874
  //      //    - log_dualis( 
  //      //    //(double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //      //    //(double) wFreqInMHzToGetVoltageFrom
  //      //    dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom
  //      //    ) 
  //      //  //ex: -0,9371068287545918599662966193649
  //      //    * 
  //      //    //example: 0,2125 V
  //      //    fVoltageFromFreqAboveAndBelowDiff ) ;

  //      r_fVoltageInVolt =
  //        //fVoltageOfOvVoltProtVnf_pairHigherEqualWantedFreq
  //        // 1,0625 - 0,22676176661993910555202664192874
  //        //  = 0,835738233380060894447973358072
  //        fVoltageInVoltFromNearFreqAboveWantedFreq -
  //          //log10(2,0952380952380952380952380952381) =
  //          //  0,32123338175226816317043359884426
  //          //log10(2) = 0,30102999566398119521373889472449
  //          // => log2(2,0952380952380952380952380952381) = 
  //          //  0,32123338175226816317043359884426 / 
  //          //  0,30102999566398119521373889472449 
  //          //  = 1,067114195858536967303654785547
  //          // 1,067114195858536967303654785547 * 0,2125 =
  //          //   0,22676176661993910555202664192874
  //          //log_dualis
  //          log_x
  //          (
  //            (double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //            (double) wFreqInMHzFromNearFreqBelowWantedFreq
  //            ,
  //          //wFreqOfOvVoltProtVnf_pairHigherEqualWantedFreq
  //            //example: 2200 MHz / 1050 MHz ~= 2.0952380952
  //            //example: 2200 MHz / 550 MHz = 4
  //            (double) wFreqInMHzFromNearFreqAboveWantedFreq /
  //            (double) wFreqInMHzToGetVoltageFrom
  //          )
  //          *
  //          //fVoltageDiffBetwOvVoltProtVnf_pairs
  //          (
  //            //example: 1.0625 V - 0.85V = 0,2125 V
  //            //example: 1.0625 V - 0.75V = 0.3125 V
  //            fVoltageInVoltFromNearFreqAboveWantedFreq -
  //            fVoltageInVoltFromNearFreqBelowWantedFreq
  //          )
  //        ;
  //      return true ;
  //    }
  // }
  //  return false ;
  //}

  BYTE GriffinController::GetCurrentTempInDegCelsius(float & fTempInDegCelsius)
  {
    DWORD dwValue ;
    BYTE byRet = GetCPUMiscControlDWORD(
      F3xA4_REPORTED_TEMPERATURE_CONTROL_REGISTER
      , dwValue
      ) ;
    fTempInDegCelsius = (float)( dwValue >> 21 ) / 8.0f ;
    return byRet ;
  }

    //float
  BYTE GriffinController::GetMaxVoltageForFreq(
      WORD wFreqToGetMaxVoltageFor,
      float & r_fMaxVoltageInVolt )
  {
      BYTE byReturn = SETTING_VOLTAGE_IS_UNSAFE ;
      //float fVoltageInVolt = -1.0 ;
      
      if( //Avoid div by zero.
          wFreqToGetMaxVoltageFor != 0 )
      {
          if( //mp_model->m_vecmaxvoltageforfreq.empty()
              mp_model->m_setmaxvoltageforfreq.empty() )
              byReturn = NO_VOLTAGE_FREQUENCY_PAIR_EXISTANT ;
          else
          {
              //std::vector<MaxVoltageForFreq>::iterator 
              //  iterLowestAboveWantedVoltage = //m_model.m_vecmaxvoltageforfreq.end() ;
              //  mp_model->m_vecmaxvoltageforfreq.end() ;

              MaxVoltageForFreq const * p_maxvoltageforfreqFreqAbove = NULL ;
              MaxVoltageForFreq const * p_maxvoltageforfreqFreqBelow = NULL ;
              //Because it is also used by the paint function: 
              //Too many log data if uncommented.
              //DEBUG("Trying to use the values from config file to get "
              //  "max voltage for freq " << wFreqToGetMaxVoltageFor << "( [MHz] )\n" );
              for( //std::vector<MaxVoltageForFreq>::iterator iter = 
                  std::set<MaxVoltageForFreq>::iterator iter = 
                //m_model.m_vecmaxvoltageforfreq.begin() ; 
                
                //The first max voltage for freq pair ought to be the 
                //highest/ maximum freq and its default voltage.
                //mp_model->m_vecmaxvoltageforfreq.begin() ; 
                mp_model->m_setmaxvoltageforfreq.begin() ; 
                iter != //m_model.m_vecmaxvoltageforfreq.end() ;
                  //mp_model->m_vecmaxvoltageforfreq.end() ;
                  mp_model->m_setmaxvoltageforfreq.end() ;
                ++ iter )
              {
                if( iter->m_wFreqInMHz 
                    // ">=" to be able to assign for the highest freq .
                    >= wFreqToGetMaxVoltageFor )
                    p_maxvoltageforfreqFreqAbove = &(*iter) ;
                      //iter ;
                else
                    if( iter->m_wFreqInMHz < wFreqToGetMaxVoltageFor )
                    {
                        p_maxvoltageforfreqFreqBelow = &(*iter) ;
                          //iter ;
                        break ;
                    }
                ////  if( iter->m_fVoltageInVolt < fMaxVoltage || fMaxVoltage == 0.0 )
                ////    fMaxVoltage = iter->m_fVoltageInVolt ;
                ////if( iter->m_wFreqInMHz <= wFrequInMHz )
                ////  if( iter->m_fVoltageInVolt > fMinVoltage )
                ////    fMinVoltage = iter->m_fVoltageInVolt ;
                //if( iter->m_fVoltageInVolt > fWantedVoltageInVolt )
                //  if( 
                //    //fLowestAboveWantedVoltage == 0.0 
                //    //The test for <> end() must be the first expression
                //    //to evaluate because if it's == end() it's invalid.
                //    iterLowestAboveWantedVoltage == 
                //    //m_model.m_vecmaxvoltageforfreq.end()
                //    mp_model->m_vecmaxvoltageforfreq.end()
                //    ||
                //    iter->m_fVoltageInVolt < //fLowestAboveWantedVoltage 
                //    iterLowestAboveWantedVoltage->m_fVoltageInVolt 
                //    )
                //    //fLowestAboveWantedVoltage = iter->m_fVoltageInVolt ;
                //    iterLowestAboveWantedVoltage = iter ;
              }
              //this->GetVoltageFromVID
              if( 
                //  iterLowestAboveWantedVoltage != 
                ////m_model.m_vecmaxvoltageforfreq.end() 
                //mp_model->m_vecmaxvoltageforfreq.end() 
                p_maxvoltageforfreqFreqAbove != NULL
                && p_maxvoltageforfreqFreqBelow != NULL
                )
              {
                  //fVoltageInVolt =
                  r_fMaxVoltageInVolt =
                  //fVoltageOfOvVoltProtVnf_pairHigherEqualWantedFreq
                  p_maxvoltageforfreqFreqAbove->m_fVoltageInVolt - 
                      log_dualis
                      (//wFreqOfOvVoltProtVnf_pairHigherEqualWantedFreq
                          (double) p_maxvoltageforfreqFreqAbove->m_wFreqInMHz / 
                          (double) wFreqToGetMaxVoltageFor 
                      ) 
                      *
                      //fVoltageDiffBetwOvVoltProtVnf_pairs
                      ( 
                        p_maxvoltageforfreqFreqAbove->m_fVoltageInVolt -
                        p_maxvoltageforfreqFreqBelow->m_fVoltageInVolt 
                      ) 
                    ;
                  byReturn = APPROPRIATE_2_VOLTAGE_FREQUENCY_PAIRS_FOUND ;
              }
              else
                  //If the freq to search the voltage for is the lowest 
                  //from the voltage-freq pairs.
                  if( p_maxvoltageforfreqFreqAbove != NULL )
                  {
                      r_fMaxVoltageInVolt = p_maxvoltageforfreqFreqAbove->
                          m_fVoltageInVolt ;
                      byReturn = ONE_APPROPRIATE_VOLTAGE_FREQUENCY_PAIR_FOUND ;
                      //LOG("Appropriate voltage-frequency pair was found.\n") ;
                  }
                  else
                    byReturn = NO_APPROPRIATE_2_VOLTAGE_FREQUENCY_PAIRS_FOUND ;
          }
      }
      return //fVoltageInVolt ;
          byReturn ;
  }

  void GriffinController::SetFreqAndVoltage(
    BYTE byCoreID 
    //dCoreUsage is element of [0.0 ... 1.0 ]
    , double dCoreUsage
    )
  {
    float fFreqInMHz = (double) 
      //Maximum microops/s
      //(//3200000000
      //m_ullHighestDiff ) / 
      //(double) ullDiff 
      /// (double) m_ullHighestDiff
      dCoreUsage
      //* 2200.0
      //mp_pumastatectrl->GetMaximumFrequencyInMHz() 
      * (double) //mp_cpucoredata->
        mp_model->m_cpucoredata.m_wMaxFreqInMHz 
      ;
    //m_wFreqInMHzOfCurrentActiveCoreSettings = fFreqInMHz ;
    float fVoltageInVolt ;
    DIDandFID didnfid = //mp_pumastatectrl->
      GetNearestPossibleFreqInMHzAsDIDnFID( (WORD) fFreqInMHz ) ;
    WORD wFreqInMHz = didnfid.GetFreqInMHz() ;
    if( wFreqInMHz < //mp_cpucoredata-> 
      mp_model->m_cpucoredata.m_wMaxFreqInMHz / 4 
      //550.0 
      )
    {
      wFreqInMHz = //mp_cpucoredata->
        mp_model->m_cpucoredata.m_wMaxFreqInMHz / 4 
      //550.0 
      ;
      didnfid.m_byDivisorID = 2 ;
      didnfid.m_byFreqID = //mp_cpucoredata->
        mp_model->m_cpucoredata.GetMainPLLoperatingFrequencyIDmax() ;
    }
    if( //AMD Griffin bug that Divisor ID with 0 except 
      //for FID m_byMainPLLoperatingFrequencyIDmax can't be modified 
      //(else system hangs up /freezes).
      didnfid.m_byDivisorID == 0 && didnfid.m_byFreqID != 
      //mp_cpucoredata->
      mp_model->m_cpucoredata.GetMainPLLoperatingFrequencyIDmax()
      //> mp_cpucoredata->m_wMaxFreqInMHz / 2 
      )
    {
      //Set to max. freq.
      didnfid.m_byDivisorID = 0 ;
      didnfid.m_byFreqID = //mp_cpucoredata->
        mp_model->m_cpucoredata.GetMainPLLoperatingFrequencyIDmax() ;
      wFreqInMHz = //didnfid.GetFreqInMHz() ;
        //mp_cpucoredata->
        mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    }
    if( //mp_pumastatectrl->
      GetInterpolatedVoltageFromFreq(
        //2000 
        //fFreqInMHz ,
        wFreqInMHz ,
        fVoltageInVolt ,
        mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
        ) 
      )
    {
      BYTE byCPUcoreID = 0 ;
      DWORD dwLowmostBits, dwEDX ;
      PState pstateDummy ;
      BYTE byCurrentPstate ;
       //if( mp_pumastatectrl->RdmsrEx( 
       //    COFVID_STATUS_REGISTER, 
       //    dwLowmostBits, 
       //    dwEDX , 
       //    1 << byCPUcoreID 
       //    )
       //  )
       //{
       //   mp_pumastatectrl->GetVIDmFIDnDID(dwLowmostBits,pstateDummy) ;
       //   //AMD: "18:16 CurPstate: current P-state."
       //   byCurrentPstate = (dwLowmostBits >> 16 ) & 
       //     BITMASK_FOR_LOWMOST_3BIT ;
       //}
      mp_model->m_cpucoredata.m_arp_percpucoreattributes[byCoreID].
        m_wFreqInMHzCalculatedFromCPUload = wFreqInMHz ;
      mp_model->m_cpucoredata.m_arp_percpucoreattributes[byCoreID].
        m_fVoltageInVoltCalculatedFromCPUload = fVoltageInVolt ;
      PState pstate( 
        PState::GetVoltageID( fVoltageInVolt ) ,
        didnfid.m_byDivisorID ,
        didnfid.m_byFreqID ,
        ////Alternate the p-state ID because of that: 
        ////"The PstateId field must be updated to cause a new 
        //CpuFid value to take effect."
        //m_byPstateID = //(~m_byPstateID) & 1 
        //byCurrentPstate == 2 ?
        //3 : 2
        3
        );
    //  if( //byPstateNumber == 7 
    //    true 
    //     )
    //  {
    ////    mp_pumastatectrl->GetAsLowWord 
    //    mp_pumastatectrl->WriteToCOFVID(pstate,0) ;
    //  }
    //  if( fVoltageInVolt >= 
    //    //As I read some Griffins (the non-ZMs and non-RMs, e.g.
    //    //Athlon QL do not support voltages below 0.95 V.
    //    //So we shouldn't try to set a frequency that is too
    //    //low for the voltags (=overvolting).
    //    PState::GetVoltageID( mp_cpucoredata->
    //      m_byMaxVoltageID )
    //   )
    //  {
    //    
    //  }
      //BYTE byCoreID = 0 ;
    if( // DID=2: < 1/2 max. freq, >= 1/4 ma.x freq
      didnfid.m_byDivisorID == 2 
      )
    {
      BYTE byPstateNumber = 2 ;
      DWORD dwMSRHigh, dwMSRLow ;
      //mp_pumastatectrl->
      //setVidAndFrequencyForPState_Puma(
      //  //mp_pumastatectrl->
      //  GetMSRregisterForPstate(
      //    //Use p-state 3 for setting for dyn. freq. scaling
      //    //because setting via the MSR register 0xC0010070
      //    //did not work.
      //    //2
      //    byPstateNumber
      //    )
      //  ////Setzen im COFVID_CONTROL_REGISTER hat NICHT funktioniert:
      //  ////es wurde nur die Spannung gesenkt, nicht aber die Takfrequenz
      //  //COFVID_CONTROL_REGISTER
      //  , pstate
      //  //m_pstateFromMSR,
      //  //pstate2,
      //  , pstate
      //  , dwMSRHigh
      //  , dwMSRLow
      //  , byCoreID
      //  ) ;
    }
    //mp_pumastatectrl->SetPstate( byPstateNumber,
    //  //1 = 1bin
    //  //1 << m_byCoreID 
    //  byCoreID
    //  );
    //mp_pumastatectrl->
    SetPstate( 
      //byPstateNumber,
      //The p-states should be sorted by frequency:
      //p-state 0 is the highes freq.
      //DID 0 : > 1/2 of max. freq
      didnfid.m_byDivisorID ,
      //1 = 1bin
      //1 << m_byCoreID 
      byCoreID
      );
      }
     //mp_userinterface->CurrenCPUfreqAndVoltageUpdated() ;
  }

//#ifdef COMPILE_WITH_WINRING0 //because of PciBusDevFunc(0,24,3) ;
  bool GriffinController::GetCPUMiscControlDWORD(
    DWORD dwRegisterAddress, DWORD & dwValue )
  {
    bool bSuccess = false ;
//#ifdef LINK_TO_WINRING0_STATICALLY
//    if ( ReadPciConfigDwordEx(
//      //DWORD pciAddress,
//      //Bus 0, Device number 24, Function 3 is "CPU Miscellaneous Control"
//      PciBusDevFunc(0,24,3) ,
//      dwRegisterAddress,
//      &dwValue )
//      )
//#else
//    if( GetHandleToDLLFunction("ReadPciConfigDwordEx") )
//#endif
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    try
    {
#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
      if( mp_cpuaccess->ReadPciConfigDwordEx(
          //Bus 0, Device number 24, Function 3 is "CPU Miscellaneous Control"
          PciBusDevFunc(0,24,3)
          , dwRegisterAddress
          , &dwValue )
        )
      {
        bSuccess = true ;
      }
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    }
    catch(
        CPUaccessException e)
    {

    }
#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    return bSuccess ;
  }
//#endif //#ifdef COMPILE_WITH_WINRING0 //because of PciBusDevFunc(0,24,3) ;

//#ifdef COMPILE_WITH_WINRING0 //because of PciBusDevFunc(0,24,3) ;
  bool GriffinController::SetMaxPState(BYTE byMaxPstate)
  {
    bool bSuccess = false ;
    DWORD dwValue = 0 ;
    //TODO read values from CPU at first because the other values should not 
    //be affected.
    if( GetCPUMiscControlDWORD(
      F3xDC_CLOCK_POWER_TIMING_CONTROL_2_REGISTER_ADDRESS, dwValue) 
      )
    {
      DWORD dwPCIAddress = 0 ;
      std::ostrstream ostrstream ;
      std::string str ;
//      std::string strFuncName = "WritePciConfigDwordEx" ;
      //dwValue |= byMaxPstate ;
      //dwValue <<= 8 ; 
      //Bits 8-10: "PstateMaxVal" -> offset=8, bit length= 3
      INSERT_INTO_DWORD_VALUE(dwValue,8,3,byMaxPstate) ;
      //Bus 0, Device number 24, Function 3 is "CPU Miscellaneous Control"
      dwPCIAddress =  PciBusDevFunc(0,24,3) ;
      //WRITE_TO_OUTPUTSTREAM(ostrstream,
      //  "Before writing " << dwPCIAddress << " to PCI config " << dwValue, str) 
      ostrstream << "Before writing " << dwPCIAddress << " to PCI config " 
        << dwValue  ;
      mp_userinterface->Confirm(ostrstream) ;
      //DEBUG("Before writing %lu to PCI config %lu",dwPCIAddress,dwValue);
      LOG("Before writing " << dwPCIAddress << " to PCI config " << dwValue << "\n" );
      ////info is at "F3xDC" (Function "3", address "DC");
      //DWORD dwPCIAddress = 3;//bits 0- 2 Function Number 
      ////bits 3- 7 Device Number 
      //dwPCIAddress |= (24<<3) ;//24 = AMD CPU Misc. Control.
      //std::string strFuncName = "WritePciConfigDwordEx";
      //if(!m_pfnreadpciconfigdwordex)
      //  m_pfnwritepciconfigdwordex = (pfnReadPciConfigDwordEx)GetProcAddress(
      //    m_hinstanceWinRingDLL,strFuncName.c_str() );

      //if(m_pfnreadpciconfigdwordex)
//  #ifdef LINK_TO_WINRING0_STATICALLY
//      if( WritePciConfigDwordEx (dwPCIAddress,//0xDC
//        F3xDC_CLOCK_POWER_TIMING_CONTROL_2_REGISTER_ADDRESS,dwValue) )
//  #else
//      pfnwritepciconfigdwordex = GetHandleToDLLFunction(
//        strFuncName ) ;
//      if( pfnwritepciconfigdwordex )
//  #endif //#ifdef LINK_TO_WINRING0_STATICALLY
      {
    //    char arch[33];
        //dwPCIAddress = PciBusDevFunc(0, 0x18, 3) ;
  //      DEBUG("dwPCIAddress: %s\n", this->getBinaryRepresentation(arch,dwPCIAddress));
        //DEBUG("dwPCIAddress: %s\n", getBinaryRepresentation(dwPCIAddress).c_str());
        LOG("PCI address: " << getBinaryRepresentation(dwPCIAddress).c_str() 
            << "\n" );
//  #ifndef LINK_TO_WINRING0_STATICALLY
//        if((*m_pfnreadpciconfigdwordex)(dwPCIAddress,0xDC,&dwValue)
//          )
//        {
//  #endif //#ifndef LINK_TO_WINRING0_STATICALLY
        if( mp_cpuaccess->WritePciConfigDwordEx(dwPCIAddress,//0xDC
          F3xDC_CLOCK_POWER_TIMING_CONTROL_2_REGISTER_ADDRESS,dwValue)
          )
        {
//          DEBUG("Calling %s succeeded\n", strFuncName.c_str());
          //byRet = SUCCESS ;
          bSuccess = true ;
//  #ifndef LINK_TO_WINRING0_STATICALLY
        }
//  #endif // #ifndef LINK_TO_WINRING0_STATICALLY
      }
    }
    return bSuccess;
  }
//#endif //#ifdef COMPILE_WITH_WINRING0

#ifdef COMPILE_WITH_WINRING0 //because of PciBusDevFunc(0,24,3) ;
  bool GriffinController::SetCPUMiscControlDWORD(
    //DWORD & dwPCIAddress,
    DWORD & dwRegisterAddress,
    DWORD & dwValue)
  {
    bool bSuccess = false ;
    DWORD dwPCIAddress ;

    dwPCIAddress =  PciBusDevFunc(0,24,3) ;
//#ifdef LINK_TO_WINRING0_STATICALLY
//    if( WritePciConfigDwordEx (dwPCIAddress,//0xDC
    if( mp_cpuaccess->WritePciConfigDwordEx ( dwPCIAddress ,
      dwRegisterAddress, dwValue) )
    {
      bSuccess = true ;
    }
//#endif //#ifdef LINK_TO_WINRING0_STATICALLY
    return bSuccess;
  }
#endif //#ifdef COMPILE_WITH_WINRING0
//  #else
//
//#endif //ifdef WIN32

  //inline 
  bool GriffinController::ReadPerformanceEventCounterRegister(
    BYTE byPerformanceEventCounterNumber ,
    ULONGLONG & r_ull ,
	  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
    )
  {
    DWORD dwLow , dwHigh ;
    bool bRet =
    //TODO better use ReadPMC? : AMD Family 11h Processor BKDG :
    //"The RDPMC instruction is not serializing, and it can be executed 
    //out-of-order with respect to other instructions around it. 
    //Even when bound by serializing instructions, the system environment at
    //the time the instruction is executed can cause events to be counted 
    //before the counter value is loaded into EDX:EAX."
    RdmsrEx( 
      PERFORMANCE_EVENT_COUNTER_0_REGISTER + byPerformanceEventCounterNumber ,
      //PERFORMANCE_EVENT_COUNTER_1_REGISTER ,
      dwLow,
      dwHigh,
      //1=core 0
      dwAffinityMask
      ) ;
    //RdpmcEx seemed to cause a blue screen (maybe because of wrong param values)
    //mp_cpuaccess->RdpmcEx(
    //  PERFORMANCE_EVENT_COUNTER_0_REGISTER + byPerformanceEventCounterNumber ,
    //  //PERFORMANCE_EVENT_COUNTER_1_REGISTER ,
    //  & dwLow,
    //  & dwHigh,
    //  //1=core 0
    //  dwAffinityMask      
    //  ) ;
    r_ull = dwHigh ;
    r_ull <<= 32 ;
    r_ull |= dwLow ;
    return bRet ;
  }

  inline bool // TRUE: success, FALSE: failure
   GriffinController::RdmsrEx(
	  DWORD dwIndex,		// MSR index
	  DWORD & dwLowmostBits,			// bit  0-31 (register "EAX")
	  DWORD & dwHighmostBits,			// bit 32-63 (register "EDX")
	  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
    )
  {
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    switch(dwIndex)
    {
        case COFVID_STATUS_REGISTER:
          dwHighmostBits = ( 2 & BITMASK_FOR_LOWMOST_3BIT ) << ( 56 - 32 )
              | ( 14 & BITMASK_FOR_LOWMOST_6BIT ) << ( 49 - 32 )
              | ( MAXIMUM_VID_FOR_ZM82 & BITMASK_FOR_LOWMOST_7BIT ) << 
                //( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VOLTAGE - 32 )
                ( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VID - 32 )
              | ( MINIMUM_VID_FOR_ZM82 & BITMASK_FOR_LOWMOST_7BIT ) <<
                //( COFVID_STATUS_REGISTER_START_BIT_FOR_MAX_VOLTAGE - 32 )
                ( COFVID_STATUS_REGISTER_START_BIT_FOR_MIN_VID - 32 )
              | ( 0 & BITMASK_FOR_LOWMOST_3BIT )
              ;
          break; 
      case PERF_CTR_0:
         mp_cpuaccess->RdmsrEx( 
           MSR_TIME_STAMP_COUNTER_REGISTER,
          &dwLowmostBits,// bit  0-31 (register "EAX")
          &dwHighmostBits, 
          dwAffinityMask
          ) ;
        break ;
  #ifdef _TEST_PENTIUM_M
          default:
            return mp_cpuaccess->RdmsrEx(
              dwIndex,&dwLowmostBits,&dwHighmostBits,dwAffinityMask);
  #endif //  #ifdef _TEST_PENTIUM_M
    }
    return true ;
#else
    return mp_cpuaccess->RdmsrEx(
      dwIndex,//&dwEAX
      &dwLowmostBits,//&dwEDX
      &dwHighmostBits,
      dwAffinityMask
      );
#endif // #ifdef _EMULATE_TURION_X2_ULTRA_ZM82
  }

  void GriffinController::ResumeFromS3orS4()
  {
    ApplyAllPStates() ;
  }

  //Implement this method here and not in the base class because 
  //the Griffin has a bug that it freezes when frequencies are set
  //where the divisor ID is 0 and the Frequency ID is NOT the max.
  //possible Frequency ID.
  void GriffinController::SetFreqAndVoltageFromFreq(
    WORD wFreqInMHz 
    , BYTE byCoreID )
  {
    //The passed frequency may not be a valid frequency.
    DIDandFID didnfid = GetNearestPossibleFreqInMHzAsDIDnFID( wFreqInMHz ) ;
    float fVoltageInVolt ;
    if ( didnfid.m_byDivisorID == 0 && didnfid.m_byFreqID != 
      mp_model->m_cpucoredata.GetMainPLLoperatingFrequencyIDmax() 
      )
      //Avoid AMD Griffin CPU bug that causes CPU freezes.
      wFreqInMHz = mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    else
    {
      wFreqInMHz = didnfid.GetFreqInMHz() ;
    }

    if( GetInterpolatedVoltageFromFreq(
        wFreqInMHz
        , fVoltageInVolt 
        , mp_model->m_cpucoredata.m_stdsetvoltageandfreqWanted
        ) 
      )
    {
      SetVoltageAndFrequency(//wFreqInMHz
        fVoltageInVolt
        , wFreqInMHz
        , byCoreID
        ) ;
    }
  }

  //This method should be separated because possibly one wants to have
  //dynamic voltage & frequency scaling for less than all cores.
  void GriffinController::SetFrequencyAndVoltageAccCPULoad(BYTE byCPUcoreID)
  {
    bool bChangePstate = false ;
    BYTE byDivisorID, byFrequencyID ;
    CPUcoreData * p_cpucoredata = //& m_model.m_cpucoredata ;
      & mp_model->m_cpucoredata ;
    float fVoltage ;
    //DEBUG("Dynamic voltage & freq scaling for core%u\n",byCPUcoreID);
    LOG("Dynamic voltage & freq scaling for core" << byCPUcoreID << "\n" );
    //DEBUG("Frequency before (possible) change:%u, core load:%f\n",
    //  p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID],p_cpucoredata->
    //  m_arfCPUcoreLoadInPercent[byCPUcoreID]);
    LOG( "Frequency before (possible) change:" 
        << p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID] 
        << ", core load:" 
        << p_cpucoredata->m_arfCPUcoreLoadInPercent[byCPUcoreID] 
        << "\n" );
    if( p_cpucoredata->m_arfCPUcoreLoadInPercent[byCPUcoreID] < 
      p_cpucoredata->m_fCPUcoreLoadThresholdForIncreaseInPercent )
    {
      bChangePstate = true ;
      p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] = 
        (WORD) ( (float) p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID] * 
        //percental usage
        p_cpucoredata->m_arfCPUcoreLoadInPercent[byCPUcoreID] / 100.f
        ) ;
      if( p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] > 
        p_cpucoredata->m_wAQuarterOfMaxFreq )
      {
        ////bChangePstate = true ;
        //if( m_wCurrentFreqInMHz <= m_wAHalfOfMaxFreq )
        //{
        //  //example: max. freq: 2200 MHz: 650 Mhz > 550 MHz, <=1100 MHz
        //  //550 MHz would have been encoded as DivisorID:2, FrequencyID: 14:
        //  //  (14+8)*100/2^2 =2200/4 = 550
        //  //byDivisorID = 1 ;
        //}
      }
      else
      {
        p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] = 
          p_cpucoredata->m_wAQuarterOfMaxFreq ;
      }
    }
    //else // =100% CPU core load
    else // above threshold CPU core load
    {
      //if( m_wCurrentFreqInMHz == m_wMaxFreqInMHz )
      //  bChangePstate = false ;
      //else
      {
        bChangePstate = true ;
        //Before multiplying the increase factor make the frequency to 100%
        //(else it could be that there is no ioncrease if the percental 
        //increase factor is e.g. 1.1 und threshold is 80%: 80%*1.1=0.8*1.1=
        //0,88<1.0 and I want to make the program user friendly so that the
        //user needn't calculate if this is an increase):
        //e.g. load = 80% at 800 MHz: 800*100%/80%=800*100/80=80000/80=1000
        p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] *=
          //If no explicit cast: compiler warning:
          //"[...]converting to `short unsigned int' from `float'"
          (WORD)
          100 /
          p_cpucoredata->m_arfCPUcoreLoadInPercent[byCPUcoreID] ;
        p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] *= 
          p_cpucoredata->m_fPercentalCPUcoreFreqIncrease ;
        if( p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] > 
          p_cpucoredata->m_wMaxFreqInMHz )
          p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] = 
          p_cpucoredata->m_wMaxFreqInMHz ;
      }
    }
    //Just for testing:
      p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] = 1600 ;
    if( p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] > 
      p_cpucoredata->m_wAQuarterOfMaxFreq )
    {
      //bChangePstate = true ;
      if( p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] <= 
        p_cpucoredata->m_wAHalfOfMaxFreq )
      {
        ////example: max. freq: 2200 MHz: 650 Mhz > 550 MHz, <=1100 MHz
        ////550 MHz would have been encoded as DivisorID:2, FrequencyID: 14:
        ////  (14+8)*100/2^2 =2200/4 = 550
        //byDivisorID = 1 ;
        ////Given: freqinMHz: 650, DivisorID:1 -> (FID+8)*100/2^DivisorID = 650
        ////(FID+8)*100/2^1 = 650 <=> (FID+8)*100/2 = 650 <=> (FID+8)*50 = 650 |:50
        ////FID+8 = 13 |-8 <=> FID=5
        ////Test: (FID+8)*100/2^DivisorID = 650 <=> (5+8)*100/2^1 = 650
        ////      13*100/2 = 650 <=> 1300/2 = 650 <=> 650 = 650 <=> true
        //byFrequencyID = (m_wCurrentFreqInMHz/50) - 8 ;
        ////byPstateID = 1 ;

        //When the FID was < 14 there were freezes. So I set the 
        //DID to "2" to get the FID above 14 for the wanted freq in MHz.
        byDivisorID = 2 ;
        //(FID+8)*100/2^DivisorID = FreqInMHz
        //(FID+8)*100/2^2 = FreqInMHz <=> (FID+8)*100/4 = FreqInMHz
        //(FID+8)*25 = FreqInMHz | / 25 <=> (FID+8) = FreqInMHz/25 | -8
        //FID = FreqInMHz/25 - 8
        //======================
        byFrequencyID = (p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ]
          /25) - 8 ;
      }
      else //p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] > wAHalfOfMaxFreq 
      {
        ////example: max. freq: 2200 MHz: 650 Mhz > 550 MHz, <=1100 MHz
        ////1100 MHz would have been encoded as DivisorID:1, FrequencyID: 14:
        ////  (14+8)*100/2^1 =2200/2 = 1100
        //byDivisorID = 0 ;
        ////Given: freqinMHz: 1300, DivisorID:0 -> (FID+8)*100/2^DivisorID = 1300
        ////(FID+8)*100/2^0 = 1300 <=> (FID+8)*100/1 = 1300 <=> (FID+8)*100 = 1300 |:100
        ////FID+8 = 13 |-8 <=> FID=5
        ////Test: (FID+8)*100/2^DivisorID = 1300 <=> (5+8)*100/2^0 = 1300
        ////      13*100/1 = 1300 <=> 1300/1 = 1300 <=> 1300 = 1300 <=> true

        ////(FID+8)*100=FreqinMHz |:100 <=> (FID+8)=FreqinMHz/100 |-8 <=> 
        ////FID=FreqinMHz/100-8
        ////===================
        //byFrequencyID = (m_wCurrentFreqInMHz/100) - 8 ;
        ////byPstateID = 0 ;

        ////Just for testing:
        ////Set to a quarter of max. freq:
        //byDivisorID = 2 ;
        //byFrequencyID = 14 ;

        if( p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] < 
          //p_cpucoredata->m_wMaxFreqInMHz
          //2100
          //2200 //<=>  FID < 14: 14+8*100/1 =2200
          p_cpucoredata->m_wMaxFreqInMHz
          )
        {
          //When the FID was < 14 there were freezes. So I set the 
          //DID to "1" to get the FID above 14 for the wanted freq in MHz.
          byDivisorID = 1 ;
          //(FID+8)*100/2^DivisorID = FreqInMHz
          //(FID+8)*100/2^1 = FreqInMHz <=> (FID+8)*100/2 = FreqInMHz
          //(FID+8)*50 = FreqInMHz | / 50 <=> (FID+8) = FreqInMHz/50 | -8
          //FID = FreqInMHz/50 - 8
          //======================
          //test: FreqInMHz= 1300; FID=FreqInMHz/50 - 8 = 1300/50-8=26-8=18
          //TODO when the AMD power monitor showed 2200 MHz for both cores
          //with dyn voltage & freq scaling by PumaStateCtrl
          //and I ran CPU Rightmark then the real performance was only the 
          //half as of 2200 MHz for both cores with HP power scheme.
          byFrequencyID = (p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ]/50) - 8 ;
        }
        else
        {
          byDivisorID = 0 ;
          //(FID+8)*100/2^DivisorID = FreqInMHz
          //(FID+8)*100/2^0 = FreqInMHz <=> (FID+8)*100/1 = FreqInMHz
          //(FID+8)*100 = FreqInMHz | / 100 <=> (FID+8) = FreqInMHz/100 | -8
          //FID = FreqInMHz/100 - 8
          //======================
          //test: FreqInMHz= 2200; FID=FreqInMHz/100 - 8 = 2200/100-8=22-8=14
          byFrequencyID = (p_cpucoredata->m_arwCurrentFreqInMHz[
            byCPUcoreID ]/100) - 8 ;
        }
      }
    }//if( p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] > p_cpucoredata->m_wAQuarterOfMaxFreq )
    else // p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] <= p_cpucoredata->m_wAQuarterOfMaxFreq
    {
      //Set to a quarter of max. freq of 2200 MHz:
      byDivisorID = 2 ;
      byFrequencyID = 14 ;
      //test: freqInMHz = (byFrequencyID+8)*100/2^byDivisorID
      //   freqInMHz = (14+8)*100/2^2 = (22)*100/4 = 2200/4 = 550
    }
        //if( m_wCurrentFreqInMHz <= 2100 )
        //{
        //  bChangePstate = true ;
        //  m_wCurrentFreqInMHz += 100 ;
        //}
    //  }
    //}
    fVoltage = //1.05f - 
      p_cpucoredata->m_fVoltageForMaxCPUcoreFreq -
      (float) 
      ( 
        //log2f( 
        //MSVC++ has no log2() function (in <math.h>).
        //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
        log10(
          //(double) 
          ( //2200.0f 
            (float) p_cpucoredata->m_wMaxFreqInMHz
            / (float) p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] ) 
          )
        / log10(2.0f)
        * 0.25f 
      ) ;
    //DEBUG("timer--change p-state: %u,current freq:%u->optimal voltage:%f\n", 
    //  (WORD) bChangePstate, 
    //  p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ],
    //  fVoltage);
    LOG( "timer--change p-state: " << (WORD) bChangePstate 
        << ",current freq:" 
        << p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] 
        << "->optimal voltage:" << fVoltage << "\n" );
    //DEBUG("timer--new voltage: %f\n",m_fVoltage);
    if( bChangePstate )
    {
      //m_fVoltage = 1.1f - 
      //  (float) 
      //  ( 
      //    //log2f( 
      //    //MSVC++ has no log2() function (in <math.h>).
      //    //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
      //    log10(
      //      //(double) 
      //      ( 2200.0f / (float) m_wCurrentFreqInMHz ) 
      //         )
      //    / log10(2.0f)
      //    * 0.15f 
      //  ) ;
      //DEBUG("timer--new voltage: %f\n",m_fVoltage);
      p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] = 
        PState::GetFreqInMHz(byFrequencyID,byDivisorID);
      fVoltage = //1.05f - 
        p_cpucoredata->m_fVoltageForMaxCPUcoreFreq -
        (float) 
        ( 
          //log2f( 
          //MSVC++ has no log2() function (in <math.h>).
          //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
          log10(
            //(double) 
            ( //2200.0f 
              (float) p_cpucoredata->m_wMaxFreqInMHz
              / (float) p_cpucoredata->m_arwCurrentFreqInMHz[byCPUcoreID ] )
            )
          / log10(2.0f)
          * 0.25f 
        ) ;
      if( fVoltage < 0.75f )
      {
        fVoltage = 0.75f ;
        //DEBUG("voltage set to minim voltage sent to Voltage Regulator "
        //  "Module: %f\n",fVoltage);
        LOG(" Voltage is set to minimum voltage that is sent to Voltage Regulator "
          "Module: " << fVoltage << "\n" );
      }
      //PState pstate(PState::GetVoltageID(m_fVoltage),byDID,byFID);
      PState pstate(PState::GetVoltageID(fVoltage),byDivisorID,
        byFrequencyID,//byPstateID
        //2
        //0
        //Alternate the p-state ID because of that: 
        //The PstateId field must be updated to cause a new CpuFid value to take effect.
        m_byPstateID = //(~m_byPstateID) & 1 
        ! m_byPstateID
        );
      ////Just for testing:
      //PState pstate(64,2,17,2);
      //PState pstate2(pstate) ;
      //DEBUG("VID: %u byDID: %u ,byFID: %u\n", 
      //  //PState::GetVoltageID(m_fVoltage), byDID,byFID);
      //  PState::GetVoltageID(fVoltage), byDivisorID,byFrequencyID);
      LOG( "VID: "
        //Output as integer and not as character.
        << (WORD) 
            PState::GetVoltageID(fVoltage) 
        << " DID: "
        //Output as integer and not as character.
        << (WORD) 
          byDivisorID
        << " , FID: "
        //Output as integer and not as character.
        << (WORD) 
          byFrequencyID
        << "\n" );
      //DWORD dwHigh = 0, dwLow = 0 ;

      //pstate.AssignChangesToPstateFromMSR(m_pstateFromMSR);

#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
      DWORD dwMSRHigh = 0 ;
      DWORD dwMSRLow = 0 ;
      //TODO the confirmation of e.g. wxWidgets seems to happen in 
      //ANOTHER thread->synchronize here (by e.g. using critical sections)
      WriteToPstateOrCOFVIDcontrolRegister(
        //GetMSRregisterForPstate(
        ////Use p-state 3 for setting for dyn. freq. scaling
        ////because setting via the MSR register 0xC0010070
        ////did not work.
        //2),
        //Setzen im COFVID_CONTROL_REGISTER hat NICHT funktioniert:
        //es wurde nur die Spannung gesenkt, nicht aber die Takfrequenz
        COFVID_CONTROL_REGISTER,
        pstate,
        //m_pstateFromMSR,
        //pstate2,
        pstate,
        dwMSRHigh,
        dwMSRLow,
        byCPUcoreID) ;
      ////Without setting the p-state changes are not applied.
      //SetPstate(2,1 << byCPUcoreID ) ;
#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
    }//if( bChangePstate )
    //DEBUG("Dynamic voltage & freq scaling for core%u--end\n",byCPUcoreID);
    LOG( "Dynamic voltage & freq scaling for core" << 
        //Output as integer and not as character.
        (WORD) 
        byCPUcoreID 
        << "--end\n" );
  }

  void GriffinController::SetFrequencyAndVoltageForAllCoresAccCPULoad()
  {
    CPUcoreData * p_cpucoredata = //& m_model.m_cpucoredata ;
      & mp_model->m_cpucoredata ;
    BYTE byNumberOfCPUcores = p_cpucoredata->GetNumberOfCPUcores() ;
    for(BYTE byCPUcoreID = 0 ; byCPUcoreID < byNumberOfCPUcores ; ++ byCPUcoreID )
    {
      SetFrequencyAndVoltageAccCPULoad(byCPUcoreID);
    }
  }

  BYTE GriffinController::SetNorthBridgeVoltage()
  {
      //TODO
      //return mp_cpuaccess->SetNorthBridgeVoltage() ;
      return 0 ;
  }

  BYTE GriffinController::SetVoltageAndFrequency( 
    float fVoltageInVolt, 
    WORD wFreqInMHz ,
    BYTE byCoreID
    )
  {
    BYTE byRet = false ;
    //BYTE byFrequencyID , byDivisorID ;
    //DWORD dwIndex ; 
    DWORD dwMSRhighmost, dwMSRlowmost ;
    PState pstateMergedFromUserAndMSR ;
    PState pstateFromUser ;
    //GetVIDmFIDnDID(dwLow, pstateMergedFromUserAndMSR ) ;
    pstateFromUser = pstateMergedFromUserAndMSR ;
    BYTE byVoltageID = GetVoltageID( fVoltageInVolt ) ;
    DIDandFID didandfid = GetNearestPossibleFreqInMHzAsDIDnFID( wFreqInMHz ) ;
    GetMSRregisterValue( byVoltageID, didandfid , dwMSRhighmost , dwMSRlowmost ) ;

    DWORD dwMSRregisterIndex = GetMSRregisterForPstate( didandfid.m_byDivisorID ) ;
    //bool_ = WriteToPstateOrCOFVIDcontrolRegister (
	   // dwIndex , 
    // // unsigned char byVID, //unsigned short wFreqInMHz
	   // //unsigned char byFreqID, 
    // // unsigned char byDivID,
    //  //const PState & rpstateFromMSR,

    //  //Used to check against overvolting.
    //  pstateMergedFromUserAndMSR,
    //  //Used to write to MSR.
    //  pstateFromUser,
    //  //PState & r_pstateMergedFromUserAndMSR,
    //  dwMSRhighmost,
    //  dwMSRlowmost,
    //  //DWORD_PTR w64ulAffinityMask
    //  //DWORD dwCoreID
    //  //CPU core number, beginning from number "0"
    //  byCoreID
    //  ) ;
    if( WrmsrEx(
      dwMSRregisterIndex
      , dwMSRlowmost
      , dwMSRhighmost
      , 1 << byCoreID
      ) 
      )
      if( SetPstate( didandfid.m_byDivisorID , byCoreID ) 
        )
        byRet = true ;
    return byRet ;
  }

  BYTE GriffinController::TooHot()
  {
    BYTE byTooHot = false ;
    float fTempInDegCelsius ;
    if( GetCurrentTempInDegCelsius( fTempInDegCelsius ) 
      && fTempInDegCelsius > mp_model->m_cpucoredata.m_fThrottleTemp 
      )
      byTooHot = true ;
    return byTooHot ;
  }

  //BOOL // TRUE: success, FALSE: failure
  //  GriffinController::
  //  //WINAPI
  //  WrmsrEx(
  //    DWORD index,		// MSR index
  //    DWORD dwLow ,//eax,			// bit  0-31
  //    DWORD dwHigh, //edx,			// bit 32-63
  //    DWORD affinityMask	// Thread Affinity Mask
  //  )
  //{

  //}

  void GriffinController::WriteToCOFVID(PState & pstate, BYTE byCoreID)
  {
    DWORD dwCOFVIDcontrolRegisterLowmostBits = pstate.m_byNumber ;
    if( //If "Do not care about ov. protection"
      //! m_model.m_bEnableOvervoltageProtection 
      ! mp_model->m_bEnableOvervoltageProtection 
      || IsSafe(
      //rpstateFromMSR.m_byVoltageID,
      //rpstateFromMSR.m_byFreqID,
      //rpstateFromMSR.m_byDivisorID
      //rpstateFromMSR
      pstate
      )
      )
    {
      dwCOFVIDcontrolRegisterLowmostBits <<= 16 ;
      dwCOFVIDcontrolRegisterLowmostBits |= ( (WORD) pstate.m_byVoltageID << 
        //AMD: "15:9 CpuVid: core VID."
        9 );
      dwCOFVIDcontrolRegisterLowmostBits |= ( (WORD) pstate.m_byDivisorID << 
        //AMD: "8:6 CpuDid: core divisor ID."
        6 );
      dwCOFVIDcontrolRegisterLowmostBits |= 
        //"5:0 CpuFid: core frequency ID."
        pstate.m_byFreqID ;
      mp_cpuaccess->WrmsrEx(COFVID_CONTROL_REGISTER, 
        dwCOFVIDcontrolRegisterLowmostBits, 0, 1 << byCoreID );
    }
  }

  //TODO: maybe this is useful: AMD 11h CPU PDF: PSI_L Bit
  BYTE GriffinController:://setVidAndFrequencyForPState_Puma
    WriteToPstateOrCOFVIDcontrolRegister (
	  DWORD dwRegNr, 
   // unsigned char byVID, //unsigned short wFreqInMHz
	  //unsigned char byFreqID, 
   // unsigned char byDivID,
    //const PState & rpstateFromMSR,

    //Used to check against overvolting.
    const PState & r_pstateMergedFromUserAndMSR,
    //Used to write to MSR.
    PState & r_pstateFromUser,
    //PState & r_pstateMergedFromUserAndMSR,
    //Used to write to to the p-state register. Should contain the MSR representation
    unsigned long dwMSRhigh,
    unsigned long dwMSRlow,
    //DWORD_PTR w64ulAffinityMask
    //DWORD dwCoreID
    //CPU core number, beginning from number "0"
    BYTE byCoreID
    )
  {
    BYTE byReturn = FAILURE ;
    //DEBUG("setVidAndFrequencyForPState_Puma--enable ov. prot.: %u\n",
    //  (WORD) //m_model.m_bEnableOvervoltageProtection);
    //  mp_model->m_bEnableOvervoltageProtection);
    LOG( "Set VID and frequency for p-state--enable overvoltage protection: " 
        << (WORD) mp_model->m_bEnableOvervoltageProtection << "\n" );
    //if( PStates::IsSafe(byVID,byFreqID,byDivID) )
    if( //If "Do not care about ov. protection"
      //! m_model.m_bEnableOvervoltageProtection 
      ! mp_model->m_bEnableOvervoltageProtection 
      || IsSafe(
      //rpstateFromMSR.m_byVoltageID,
      //rpstateFromMSR.m_byFreqID,
      //rpstateFromMSR.m_byDivisorID
      //rpstateFromMSR
      r_pstateMergedFromUserAndMSR
      )
      )
    {
      BYTE byMaxVID = 0 ;
      //char arch[33] ;
      //Defines on which core(s) the MSR funtions should
      //be run on. ex.: 11bin = core 0 & 1, 1bin= core 0, 10bin = core 1
      DWORD_PTR w64ulAffinityMask = 
        //ex.: core id 0:-> 1bin, core id 1 -> 10bin=2 dec
        1 << byCoreID;
      //std::ostringstream ostrstream;
      std::ostrstream ostrstream;
      DEBUG("no overvoltage prot. or voltage is safe\n");
//#ifndef LINK_TO_WINRING0_STATICALLY
//      fnWrMsrExDef * pfnwrmsrex ;
//	    //unsigned char byFreqID ;
//	    //unsigned long msr_register_number ;
//      std::string strFuncName = "WrmsrEx" ;
//      if(g_pfnRdMsrEx)
//      {
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
        DWORD dwCOFVIDStatusRegisterHigh ;
        DWORD dwCOFVIDStatusRegisterLow ;
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
        byMaxVID = 64 ;
#else
        if( 
//#ifndef LINK_TO_WINRING0_STATICALLY
//          (*g_pfnRdMsrEx)
//#else
//          RdmsrEx
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
          mp_cpuaccess->RdmsrEx
          (
          COFVID_STATUS_REGISTER,
          &dwCOFVIDStatusRegisterLow,
          &dwCOFVIDStatusRegisterHigh,
          //dwCoreID
          w64ulAffinityMask)
          )
        {
          DEBUG("setting VID etc.--reading from MSR succeeded\n");
          //byMaxVID = (dwCOFVIDStatusRegisterHigh&BITMASK_FOR_HIGHMOST_32_BIT_FOR_MAX_VID)>>3 ;
          byMaxVID = (BYTE) ( (dwCOFVIDStatusRegisterHigh&
            BITMASK_FOR_HIGHMOST_32_BIT_FOR_MIN_VID)>>10 );
          //DEBUG("the low 32 bits:%s\n", ::getBinaryRepresentation(
          //  dwCOFVIDStatusRegisterLow).c_str());
          LOG( "the low 32 bits:" << ::getBinaryRepresentation(
            dwCOFVIDStatusRegisterLow).c_str() << "\n" );
        }
        else
          mp_userinterface->Confirm("RdMsrEx function failed");
#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
//#ifndef LINK_TO_WINRING0_STATICALLY
//      }
//      else
//        mp_userinterface->Confirm("No handle to RdMsrEx function");
//#endif //#ifndef LINK_TO_WINRING0_STATICALLY
//#ifdef WIN32
//  #ifndef LINK_TO_WINRING0_STATICALLY
//      pfnwrmsrex = 
//        (fnWrMsrExDef *)GetProcAddress(
//        m_hinstanceWinRingDLL,strFuncName.c_str() );
//  #endif //#ifndef LINK_TO_WINRING0_STATICALLY
//#else
//	  pfnwrmsrex = msr_write ;
//#endif
      //DEBUG("For core %lu: setting ", dwCoreID);
      ostrstream << "For core " << 
        //cast to WORD in order to put out as number and not as ASCII char
        //for the number(=ASCII-Code)
        (WORD)byCoreID << ": writing to register " 
        << hex << dwRegNr << dec 
        << ", setting" ;
	    //unsigned long long msrvalue;
	    //msr_register_number = //"AMD: MSR C001_0064 specifies P-state 0"
	    //	//0xC0010064 + byPState ;
	    //	get_msr_register_number_puma(byPState) ;
	    //byFreqID = 

	    //Clear bits (set to 0) for the bitwise ORed ranges.
      //if( rpstateFromMSR.m_byVoltageID != 255 )
      if( r_pstateFromUser.m_byVoltageID != NOT_SPECIFIED_BY_USER )
      {
        //DEBUG("set VID to %u\n", r_pstateFromUser.m_byVoltageID );
        LOG("Set VID to " << //
          //cast to WORD in order to put out as number and not as ASCII char
          //for the number(=ASCII-Code)
            (WORD) r_pstateFromUser.m_byVoltageID << "\n" );
        ostrstream << " VID to " <<
          //cast to WORD in order to put out as number and not as ASCII char
          //for the number(=ASCII-Code)
          //(WORD)rpstateFromMSR.m_byVoltageID ;
          (WORD)r_pstateFromUser.m_byVoltageID << "=" << 
          PState::GetVoltageInVolt(r_pstateFromUser.m_byVoltageID ) << " " ;
        ////TODO remove the foll. 2 lines after testing
        //if(byVID>60)
        //  byVID = 68;
	      dwMSRlow &= 
		      //invert the bitmask
		      ~BITMASK_FOR_CPU_CORE_VOLTAGE_ID ;
        //dwMSRlow |= (rpstateFromMSR.m_byVoltageID << 9) ;
        dwMSRlow |= (r_pstateFromUser.m_byVoltageID << 9) ;
      }
      //if( rpstateFromMSR.m_byFreqID != 255 )
      if( r_pstateFromUser.m_byFreqID != NOT_SPECIFIED_BY_USER )
      {
        //DEBUG("Set Freq ID to %u\n", r_pstateFromUser.m_byFreqID );
        LOG("Set Frequency ID to " << 
            //Output as integer, not as char.
            (WORD) r_pstateFromUser.m_byFreqID << "\n" );
        ostrstream << " FreqID to " <<
          //cast to WORD in order to put out as number and not as ASCII char
          //for the number(=ASCII-Code)
          //(WORD)rpstateFromMSR.m_byFreqID;
          (WORD) r_pstateFromUser.m_byFreqID;
	      dwMSRlow &= 
		      //invert the bitmask
		      ~BITMASK_FOR_CPU_FREQ_ID ;
        //dwMSRlow |= rpstateFromMSR.m_byFreqID ;
        dwMSRlow |= r_pstateFromUser.m_byFreqID ;
      }
      //if( rpstateFromMSR.m_byDivisorID != 255 )
      if( r_pstateFromUser.m_byDivisorID != NOT_SPECIFIED_BY_USER )
      {
        //DEBUG("set Divisor ID to %u\n", r_pstateFromUser.m_byDivisorID );
        LOG( "set divisor ID to " << 
            //Output as integer, not as char.
            (WORD) r_pstateFromUser.m_byDivisorID << "\n" );
        ostrstream << " & DivID to " <<
          //cast to WORD in order to put out as number and not as ASCII char
          //for the number(=ASCII-Code)
          //(WORD)rpstateFromMSR.m_byDivisorID;
          (WORD)r_pstateFromUser.m_byDivisorID << "=" << 
          PState::GetFreqInMHz(r_pstateFromUser.m_byFreqID,
          r_pstateFromUser.m_byDivisorID) ;
	      dwMSRlow &= 
		      //invert the bitmask
		      ~BITMASK_FOR_CPU_DIV_ID ;
        //dwMSRlow |= (rpstateFromMSR.m_byDivisorID << 6 ) ;
        dwMSRlow |= (r_pstateFromUser.m_byDivisorID << 6 ) ;
      }
      //DEBUG("\n");
      ostrstream<<"\n";
	    //ullMSRvalue &= 
	    //	//invert the bitmask
	    //	~BITMASK_FOR_CURRENT_DIVIDOR ;
	    //dwMSRlow |= ( (byVID << 9) | (byDivID << 6 ) ) | byFreqID ;
      //if( byVID != 255 && byVID > byMaxVID)
      //  printf("the VID to set is greater than the maximum VID(%u)->NOT "
      //    "wrtiting to MSR\n",byMaxVID);
      //else
      {
        //BYTE byDummy;
        //printf("the VID to set is equal or less than the maximum VID(%u)->"
        //  "writing to MSR\n",byMaxVID);
	      //printf("  would write:  %s to MSR %lx\n", getBinaryRepresentation(&msrvalue,arch), msr_register_number);
        //if(
        //  //TODO: following lines just for testing
        //  dwMSRlow |= ( 
        //    //The p-state ID.
        //    (dwRegNr-0xC0010064) 
        //    << 16 )
        //  dwRegNr = 0xC0010070 ;//C001_0070 COFVID Control Register
      //DEBUG("the low 32 bits:  %s\n", getBinaryRepresentation(arch,dwMSRlow) );
//      ostrstream<<"the low 32 bits: "<<getBinaryRepresentation(arch,dwMSRlow)<<"\n";
      //DEBUG("Before getBinaryRepresentation(...)\n");
      ostrstream << "the low 32 bits: " << getBinaryRepresentation(dwMSRlow) << "\n";
      //DEBUG("meaning of low 32 bits: 5:0 core frequency ID,8:6 core divisor ID,15:9 core VID\n");
      ostrstream<<"meaning of low 32 bits: 5:0 core frequency ID,8:6 core divisor ID,15:9 core VID\n";
      if(dwRegNr == COFVID_CONTROL_REGISTER)
      {
        //dwMSRhigh = 0 ;
        dwMSRhigh = r_pstateFromUser.m_byNumber ;
      }
      //DEBUG("the high 32 bits: %s\n", getBinaryRepresentation(arch,dwMSRhigh) );
//      ostrstream<<"the high 32 bits: "<<getBinaryRepresentation(arch,dwMSRhigh)<<"\n";
      ostrstream<<"the high 32 bits: "<< getBinaryRepresentation(dwMSRhigh)<<"\n";
      if(dwRegNr != COFVID_CONTROL_REGISTER)
      {
        float fCurrent ;
        BYTE byCurrentDivisor =
          (dwCOFVIDStatusRegisterHigh >> (START_BIT_FOR_CURRENT_DIVISOR_ID - 32) )
            & BITMASK_FOR_LOWMOST_2BIT ;
        BYTE byCurrentValue = dwCOFVIDStatusRegisterHigh & 
          BITMASK_FOR_LOWMOST_8BIT ;
        //DEBUG("high 32 bits: 39:32: current value,41:40: current divisor,63: 1=this P-state is valid\n");
        ostrstream << "high 32 bits: 39:32: current value,41:40: current divisor,63: 1=this P-state is valid\n";
        switch(byCurrentDivisor)
        {
        case 0:
          fCurrent = byCurrentValue ;
          break ;
        case 1:
          fCurrent = byCurrentValue / 10 ;
          break ;
        case 2:
          fCurrent = byCurrentValue / 100 ;
          break ;
        }
        ostrstream << "expected current dissipation of a single core="
          "current value:" <<
          //convert to unsigned int, else the value is put out as a charcter.
          (WORD) byCurrentValue <<"/ 10^current divisor(" << 
          //convert to unsigned int, else the value is put out as a charcter.
          (WORD) byCurrentDivisor << ")=" <<
          //byCurrentValue / byCurrentDivisor 
          fCurrent << "\n" ;
      }
      //DEBUG("before \"ostrstream.put('\0');\"\n");
      //Must set this, else text may follow after the string we want.
      //I had program crashes with the following method:
      //pch[r_ostrstream.pcount()] = '\0' ;
      ostrstream.put('\0'); //the same as "ends()" does.
      //DEBUG("before \"ostrstream.flush();\"\n");
      ostrstream.flush();
      //DEBUG("getBinaryRepresentation(...):%s\n",ostrstream.str());
      LOG( //"getBinaryRepresentation(...):" << 
          ostrstream.str() << "\n" );

      //DEBUG("mind. 1 Zeichen eingeben und ENTER druecken, um in "
      //  "MSR-Register %lx zu schreiben",dwRegNr) ;
      //std::cin >> byDummy ;
      bool bWrite = true ;
      //DEBUG("%s",ostrstream.str().);
      //if(m_pstates.m_bConfirm)
      //if(m_model.m_pstates.m_bConfirm)
      if(mp_model->m_pstates.m_bConfirm)
        //bWrite = mp_userinterface->Confirm(ostrstream.str() ) ;
        bWrite = mp_userinterface->Confirm(ostrstream) ;
//#ifdef COMPILE_WITH_DEBUG
//      else
//        DEBUG("%s\n",ostrstream.str().c_str());
//#endif
      //DEBUG("Write to MSR: %s\n", bWrite? "yes":"no");
      LOG( "Write to MSR: " << ( bWrite? "yes":"no" ) << "\n" );
      if( bWrite )
      {
        //COFVID_CONTROL_REGISTER does not have a "PstateEn" bit
        if( dwRegNr != COFVID_CONTROL_REGISTER )
          //There was a freeze/ crash when this bit was not set for a 
          //p-state higher than "2" (and /or when max p-state was not 
          //set in PCI config space).
          SET_P_STATE_TO_VALID(dwMSRhigh);
        //DEBUG("Now write to MSR\n");
	      //if ((msr_write(msrfile, msr_register_number, &msrvalue)) != OK)
	      //	printf("MSR write failed\n");
        //DEBUG("Adress of mp_cpuaccess: %lx\n", mp_cpuaccess);
        //LOG("Adress of mp_cpuaccess: " << mp_cpuaccess << "\n" );
        if( 
//#ifndef LINK_TO_WINRING0_STATICALLY
//          (*pfnwrmsrex)
//#else
//          WrmsrEx
//#endif
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
          true
#else
          mp_cpuaccess->
            WrmsrEx
            (
              dwRegNr,
              dwMSRlow,
              dwMSRhigh,
              //dwCoreID
              (DWORD) w64ulAffinityMask
            ) 
#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
          )
        {
		      //if (stabilization_time > 100)
			     // usleep(stabilization_time);
		      //else
			     // msr_usleep(stabilization_time);
//#ifdef _WINDOWS
//          //Wait 1 millisecond (> maximum stabilization time).
//          Sleep(1) ;
//#else
//		      usleep(1000);
//#endif//#ifdef _WINDOWS
          mp_cpuaccess->Sleep(1);
          byReturn = SUCCESS ;
        }
        else
        {
          //mp_userinterface->Confirm("Fehler beim Schreiben des P-State in"
          // "die ModelSpecificRegister. Sie sollten versuchen, dieses "
          // "Prog als Admin auszufhren");
          mp_userinterface->Confirm("Error writing p-state into "
           "ModelSpecificRegister. You should try to run this "
           "program with administrative rights");
          byReturn = WRITE_MSR_FAILED ;
        }
      }//if( bWrite )
      }
    }//if( PStates::IsSafe(byVID,byFreqID,byDivID) )
    else
    {
      DEBUG("Setting p-state is not safe\n");
      r_pstateFromUser.m_byIsSafe = FALSE ;
      std::ostrstream ostrstr ;
      ostrstr << "p-state " <<
        //Must be converted to non-char, else if m_byNumber is "\0" this
        //is the string end.
        (WORD) r_pstateFromUser.m_byNumber << " for core " <<
        //Must be converted to non-char, else if m_byNumber is "\0" this
        //is the string end.
        (WORD) byCoreID << " is not set because of overvoltage "
        "protection: the core voltage is too high for the core frequency" ;
      //mp_userinterface->Confirm("1 P-State wird nicht gesetzt aus "
      //  "berspannungs-Schutz:Die Spannung ist zu hoch fr die Frequenz");
      mp_userinterface->Confirm( ostrstr );
    }
    return byReturn ;
  }

  void GriffinController::SetUserInterface(UserInterface * p_userinterface )
  {
    mp_userinterface = p_userinterface ;
    mp_cpuaccess->SetUserInterface(*p_userinterface) ;
  }
  
  BYTE GriffinController::StartOrStopCalculationThread( BYTE byCoreID)
  {
      BYTE byAction = STARTED;
      LOG("start or stop high load calc thread for core " << 
          (WORD) byCoreID << "\n");
      LOG("Thread has " << 
          ( marp_calculationthread[ byCoreID ] ? 
              //is <> NULL 
              "NOT " :
              //is NULL 
              "" 
          ) << "to be created first.\n");
      if( marp_calculationthread[ byCoreID ] )
      {
          if( //marp_calculationthread[ byCoreID ]->m_vbContinue == true 
              marp_calculationthread[ byCoreID ]->IsRunning() )
          {
              LOG("Thread is running\n");
            //marp_calculationthread[ byCoreID ]->m_vbContinue = false ;
              marp_calculationthread[ byCoreID ]->EndCalculationThread() ;
              byAction = ENDED ;
            //::WaitForSingleObject(marp_calculationthread[ byCoreID ]->m_hThread);
            //delete marp_calculationthread[ byCoreID ] ;
            //marp_calculationthread[ byCoreID ] = NULL ;
          }
          else 
          {
             LOG("Thread is NOT running\n");
              marp_calculationthread[ byCoreID ]->StartCalculationThread(byCoreID) ;
              marp_calculationthread[ byCoreID ]->StartCalculationThread(byCoreID) ;
          }
      }
      else
      {
          LOG("Creating high load calc thread for core " 
              << (WORD) byCoreID << "\n");
          marp_calculationthread[ byCoreID ] = new CalculationThread( this ) ;
          if( //Check if allocation succeeded if it was NULL before
              marp_calculationthread[ byCoreID ] )
              marp_calculationthread[ byCoreID ]->
                StartCalculationThread( byCoreID ) ;
      }
      return byAction ;
  }

  BYTE GriffinController::SetPstate(BYTE byPstateID,BYTE byCoreID)
  {
    BYTE byRet = FAILURE ;
    DWORD dwHigh , dwLow ;
    PState pstateToSet ;
    if( GetPStateFromMSR( byPstateID, dwLow, dwHigh, pstateToSet, byCoreID ) )

      if( //If "Do not care about ov. protection"
        //! m_model.m_bEnableOvervoltageProtection 
        ! mp_model->m_bEnableOvervoltageProtection 
        || IsSafe(
        //rpstateFromMSR.m_byVoltageID,
        //rpstateFromMSR.m_byFreqID,
        //rpstateFromMSR.m_byDivisorID
        //rpstateFromMSR
        pstateToSet
        )
        )
      {
        if( byPstateID >= 0 && byPstateID < NUMBER_OF_PSTATES )
        {
          changePstate(byPstateID, (1 << byCoreID) );
        }
      }
      else
        mp_userinterface->Confirm( "p-state not set because of its unsafe "
          "vvalues" );
    return byRet ;
  }

BYTE GriffinController::UseMaxVoltageForFreqForOvervoltageProtection(
    BYTE byVID
    ,
    WORD wWantedFrequInMHz
    )
{
  BYTE byReturn = SETTING_VOLTAGE_IS_UNSAFE ;
  float fMaxVoltage = 0.0 , fMinVoltage = 0.0 ;
  float fLowestAboveWantedVoltage = 0.0 ;
  float fWantedVoltageInVolt = PState::GetVoltageInVolt(byVID);
  //std::vector<MaxVoltageForFreq>::iterator 
  std::set<MaxVoltageForFreq>::iterator 
    iterLowestAboveWantedVoltage = //m_model.m_vecmaxvoltageforfreq.end() ;
    //mp_model->m_vecmaxvoltageforfreq.end() ;
    mp_model->m_setmaxvoltageforfreq.end() ;
  DEBUG("Trying to use the values from config file to protect "
    "against overvolting\n" );
  for( //std::vector<MaxVoltageForFreq>::iterator iter = 
      std::set<MaxVoltageForFreq>::iterator iter = 
    //m_model.m_vecmaxvoltageforfreq.begin() ; 
    
    //The first max voltage for freq pair ought to be the 
    //highest/ maximum freq and its default voltage.
    //mp_model->m_vecmaxvoltageforfreq.begin() ; 
    mp_model->m_setmaxvoltageforfreq.begin() ; 
    iter != //m_model.m_vecmaxvoltageforfreq.end() ;
      //mp_model->m_vecmaxvoltageforfreq.end() ;
      mp_model->m_setmaxvoltageforfreq.end() ;
    ++ iter )
  {
    //if( iter->m_wFreqInMHz >= wFrequInMHz )
    //  if( iter->m_fVoltageInVolt < fMaxVoltage || fMaxVoltage == 0.0 )
    //    fMaxVoltage = iter->m_fVoltageInVolt ;
    //if( iter->m_wFreqInMHz <= wFrequInMHz )
    //  if( iter->m_fVoltageInVolt > fMinVoltage )
    //    fMinVoltage = iter->m_fVoltageInVolt ;
    if( iter->m_fVoltageInVolt > fWantedVoltageInVolt )
      if( 
        //fLowestAboveWantedVoltage == 0.0 
        //The test for <> end() must be the first expression
        //to evaluate because if it's == end() it's invalid.
        iterLowestAboveWantedVoltage == 
        //m_model.m_vecmaxvoltageforfreq.end()
        mp_model->m_setmaxvoltageforfreq.end()
        ||
        iter->m_fVoltageInVolt < //fLowestAboveWantedVoltage 
        iterLowestAboveWantedVoltage->m_fVoltageInVolt 
        )
        //fLowestAboveWantedVoltage = iter->m_fVoltageInVolt ;
        iterLowestAboveWantedVoltage = iter ;
  }
  //this->GetVoltageFromVID
  if( iterLowestAboveWantedVoltage != 
    //m_model.m_vecmaxvoltageforfreq.end() 
    mp_model->m_setmaxvoltageforfreq.end() 
    )
  {
    //DEBUG("Using combination of %f V and %u MHz as base to check for "
    //  "overvolting\n",
    //  iterLowestAboveWantedVoltage->m_fVoltageInVolt, 
    //  iterLowestAboveWantedVoltage->m_wFreqInMHz);
    LOG( "Using combination of " 
        << iterLowestAboveWantedVoltage->m_fVoltageInVolt 
        << " V and " 
        << iterLowestAboveWantedVoltage->m_wFreqInMHz 
        << " MHz as base to check for overvolting\n" );
    WORD wMinFreqInMHzToPreventOvervolting = //wFreqInMHzAboveWantedFreq /
      GetMinFreqToPreventOvervoltage(*iterLowestAboveWantedVoltage,
      fWantedVoltageInVolt) ;
    if( wWantedFrequInMHz >= wMinFreqInMHzToPreventOvervolting )
      //bIsSafe = true ;
      byReturn = SETTING_VOLTAGE_IS_SAFE ;
    else
      byReturn = VOLTAGE_IS_TOO_HIGH_FOR_FREQUENCY ;
  }
  else
  {
    //std::ostrstream ostrstream ;
    //ostrstream << "No voltage/freqency pair with voltage above " <<
    //  fWantedVoltageInVolt "was found"
    //this->mp_userinterface->Confirm(
    //UseDefaultFormulaForOvervoltageProtection(byVID,wWantedFrequInMHz );
    mp_userinterface->Confirm("Setting voltage is unsafe.\n"
      "Possible causes:\n"
      "-the configuration file has not been loaded\n"
      "-Within the config file:\n"
      " insert voltage-frequency-pair(s) and/or\n specify "
      "\'use_default_formula_for_overvoltage_protection=\"true\"\'\n"
      "(see usage/help) ");
    //if( mp_model->m_vecmaxvoltageforfreq.empty() )
    if( mp_model->m_setmaxvoltageforfreq.empty() )
      byReturn = NO_VOLTAGE_FREQUENCY_PAIR_EXISTANT ;
    else
      byReturn = NO_APPROPRIATE_VOLTAGE_FREQUENCY_PAIR_FOUND ;
  }
  return byReturn ;
}

BOOL GriffinController::WrmsrEx(
  DWORD dwIndex,		// MSR index
  DWORD dwLow ,//eax,			// bit  0-31
  DWORD dwHigh, //edx,			// bit 32-63
  DWORD dwAffinityMask	// Thread Affinity Mask
  )
{
  BOOL bool_ = FALSE ;
  if( dwIndex >= MSR_P_STATE_0 && dwIndex <= MSR_P_STATE_7 
    || dwIndex == COFVID_CONTROL_REGISTER )
  {
    BYTE byCoreID ;
    //DWORD dwAffinityMaskCopy = dwAffinityMask ;
    PState pstateMergedFromUserAndMSR ;
    PState pstateFromUser ;
    GetVIDmFIDnDID(dwLow, pstateMergedFromUserAndMSR ) ;
    pstateFromUser = pstateMergedFromUserAndMSR ;
    byCoreID = GetCoreIDFromAffinityMask(dwAffinityMask ) ;
    //while( dwAffinityMaskCopy >>= 1 ) byCoreID
    bool_ = WriteToPstateOrCOFVIDcontrolRegister(
	    dwIndex , 
     // unsigned char byVID, //unsigned short wFreqInMHz
	    //unsigned char byFreqID, 
     // unsigned char byDivID,
      //const PState & rpstateFromMSR,

      //Used to check against overvolting.
      pstateMergedFromUserAndMSR,
      //Used to write to MSR.
      pstateFromUser,
      //PState & r_pstateMergedFromUserAndMSR,
      dwHigh,
      dwLow,
      //DWORD_PTR w64ulAffinityMask
      //DWORD dwCoreID
      //CPU core number, beginning from number "0"
      byCoreID
      ) ;
  }
  else
    bool_ = mp_cpuaccess->WrmsrEx(
      dwIndex , //DWORD dwIndex,		// MSR index
      dwLow , //DWORD dwLow ,//eax,			// bit  0-31
      dwHigh , //DWORD dwHigh, //edx,			// bit 32-63
      dwAffinityMask //DWORD dwAffinityMask	// Thread Affinity Mask
    ) ;
  return bool_ ;
}
