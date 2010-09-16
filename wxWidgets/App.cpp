#define _AFXDLL

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheinlich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

#ifdef _WIN32
  #define THREAD_PROC_CALLING_CONVENTION WINAPI
#else
  #define THREAD_PROC_CALLING_CONVENTION /* -> empty string */
#endif

#ifndef WX_PRECOMP
  #include "wx/app.h"
#endif
#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
#include "App.hpp"

//#include "wxDynFreqScalingTimer.hpp"
//#include "wxDynLinkedCPUcoreUsageGetter.hpp"
#include <Controller/GetNumberOfLogicalCPUcores.h>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
//#include <Controller/character_string/tchar_conversion.h> //for GetCharPointer(...)
#include <Controller/character_string/stdstring_format.hpp> //to_stdstring()
#include <Controller/IPC/I_IPC.hpp> //for "get_current_CPU_data"
#include <Controller/Logger/Logger.hpp> //class Logger
#include <Controller/X86InfoAndControlExceptions.hpp> //for VoltageSafetyException
#include <Controller/multithread/I_Thread.hpp> //class I_Thread
#include <ModelData/ModelData.hpp>
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
#include <wxWidgets/Controller/wxStringHelper.hpp> //getwxString(...)
#include <wxWidgets/UserInterface/MainFrame.hpp>
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  #include <wxWidgets/UserInterface/TaskBarIcon.hpp>
#endif
//#include <wxWidgets/wxStringHelper.h>
#include <Xerces/SAX2DefaultVoltageForFrequency.hpp>
#include <Xerces/XMLAccess.hpp> //for readXMLconfig()

#include "DynFreqScalingThread.hpp"
#ifdef _WIN32 //built-in macro in MinGW/ eclipse under Windows
  //#include <Windows/DynFreqScalingThread.hpp>
  #include <Windows/GetNumberOfLogicalCPUs.h>
  #include <Windows/HideMinGWconsoleWindow.h>
//#include <Windows/GetWindowsVersion.h>
//#include <Windows/LocalLanguageMessageFromErrorCodeA.h>
  #include <Windows/PowerProfAccess/PowerProfDynLinked.hpp>
  //#include <Windows/WinRing0dynlinked.hpp>
  //#include <Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.hpp>
  #include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
#else
  #include <Linux/MSRdeviceFile.h>
#endif
//#include <strstream> //ostrstream
#include <string> //

//#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>

FILE * fileDebug ; //for debug logging.
//This global (important for using preprocessor macros) object is used for 
//easy logging.
Logger g_logger ;
CPUcontrolBase * gp_cpucontrolbase ;
//Needed for the exported functions.
I_CPUaccess * g_p_cpuaccess ;

//Erzeugt ein wxAppConsole-Object auf dem Heap.
IMPLEMENT_APP(wxX86InfoAndControlApp)

wxX86InfoAndControlApp::wxX86InfoAndControlApp()
  //C++ style inits:
  //#ifdef COMPILE_WITH_CPU_SCALING
  //    : mp_wxdynfreqscalingtimer(NULL)
  //#endif //#ifdef COMPILE_WITH_CPU_SCALING
  :
//      mp_cpucontroller(NULL)
//    ,
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  mp_taskbaricon( NULL)
//  , m_wxthreadIPC( )
  ,
#endif //#ifdef COMPILE_WITH_TASKBAR
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  mp_dynfreqscalingaccess(NULL) ,
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  m_maincontroller( this )
  , m_vbRetrieveCPUcoreData( true)
  , m_vbGotCPUcoreData (true)
  //Must explicitely init m_wxmutexIPCthread and m_wxconditionIPCthread
  // in this order here (condition need mutex in c'tor, so init mutex before)?!
  , m_wxmutexIPCthread(wxMUTEX_DEFAULT)
//  , m_wxconditionIPCthread( m_wxmutexIPCthread )
  , m_x86iandc_threadIPC(I_Thread::joinable)
{
#ifdef COMPILE_WITH_DEBUG
//For g++ the std::string object passed to Logger::OpenFile(std::string & )
//has to be declared before. the call
//    ( error if  "logger.OpenFile( std::string("bla");"  )
#endif
}

wxX86InfoAndControlApp::~wxX86InfoAndControlApp()
{
  LOGN("begin of app's destructor")
  LOGN("app's destructor: before leaving IPC2InProgramData crit sec ")
  m_model.m_cpucoredata.wxconditionIPC2InProgramData.Leave() ;
  LOGN("end of app's destructor")
}

bool wxX86InfoAndControlApp::Confirm(const std::string & str)
{
  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    #ifdef wxUSE_WCHAR_T
      std::wstring wstr(str.begin(), str.end() ) ;
      wxString wxstr( wstr.c_str() ) ;
    #else
      wxString wxstr(( const unsigned char * ) str.c_str() ) :
    #endif
    ::wxMessageBox( 
      #ifdef _DEBUG_
      wxT("gg"), wxT("bla"),wxOK
      #else
      wxstr
      #endif
      , m_stdtstrProgramName
      );
//      #ifdef _DEBUG
//    ::wxMessageBox( wxT("This is the message."), wxT("This is the title"),
//      wxOK|wxICON_INFORMATION);
//      #endif
//    wxMessageDialog * pdlg = new wxMessageDialog(wxGetApp().mp_frame,
//      wxT("hh"),wxT("ca")
//      );
//    pdlg->ShowModal() ;
    m_bConfirmedYet = true ;
  }
  //m_bConfirmedYet = true ;
  return true;
}

bool wxX86InfoAndControlApp::Confirm(const std::wstring & cr_stdwstr)
{
  //::AfxMessageBox(str.c_str());

  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    #ifdef wxUSE_WCHAR_T
      wxString wxstr( cr_stdwstr.c_str() ) ;
    #else
      std::string stdstr( cr_stdwstr.begin(), cr_stdwstr.end() ) ;
      wxString wxstr(  ) :
    #endif
//    wxMessageDialog
    ::wxMessageBox(
//    wxmsgdlg( NULL ,
      #ifdef _DEBUG_
      wxT("gg"), wxT("bla"),wxOK
      #else
      wxstr
      #endif
      , m_stdtstrProgramName
      );
//    wxmsgdlg.Show(true) ;
//      #ifdef _DEBUG
//    ::wxMessageBox( wxT("This is the message."), wxT("This is the title"),
//      wxOK|wxICON_INFORMATION);
//      #endif
//    wxMessageDialog * pdlg = new wxMessageDialog(wxGetApp().mp_frame,
//      wxT("hh"),wxT("ca")
//      );
//    pdlg->ShowModal() ;
    m_bConfirmedYet = true ;
  }
  //m_bConfirmedYet = true ;
  return true;
}

//bool wxX86InfoAndControlApp::Confirm(
//  //http://fara.cs.uni-potsdam.de/~kaufmann/?page=GenCppFaqs&faq=IntToString#Answ:
//  //"schnell" , "deprecated",
//  //"Ein fehlendes ends f�hrt dazu, dass der Puffer nicht nullterminiert wird."
//  std::ostrstream & r_ostrstream
//  //std::ostream & r_ostream
//  )
//{
//  bool bReturn = true ;
//  DEBUG("App::Confirm--begin\n");
//  //Must set this, else text may follow after the string we want.
//  //I had program crashes with the following method:
//  //pch[r_ostrstream.pcount()] = '\0' ;
//  //r_ostrstream.ends();
//  r_ostrstream.put('\0'); //the same as "ends()" does.
//  char * pch = r_ostrstream.str() ;
//    //#ifdef wxUSE_WCHAR_T
//    //  std::string str(pch) ;
//    //  std::wstring wstr(str.begin(),str.end() ) ;
//    //  wxString wxstr( wstr) ;
//    //#else
//    //  wxString wxstr(( const unsigned char * ) pch ) :
//    //#endif
//  std::string stdstr(pch) ;
//  wxString wxstr( getwxString( stdstr ) ) ;
//  //r_ostrstream.flush();
//  //To not show too many dialogs that the timer would bring up.
//  if( m_bConfirmedYet )
//  {
//    m_bConfirmedYet = false ;
//    int nReturn = ::wxMessageBox(//pch
//      wxstr
//      //, _T("Message")
//      , m_stdtstrProgramName
//      , wxCANCEL | wxOK );
//    if( nReturn == wxCANCEL )
//      bReturn = false ;
//    m_bConfirmedYet = true ;
//  }
//  //return true;
//  DEBUG("App::Confirm--end\n");
//  return bReturn ;
//}

bool wxX86InfoAndControlApp::Confirm(
  std::ostringstream & r_stdostringstream
  )
{
  bool bReturn = true ;
  wxString wxstr( getwxString( r_stdostringstream.str() ) ) ;
  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    int nReturn = ::wxMessageBox(//pch
      wxstr
      //, _T("Message")
      , m_stdtstrProgramName
      , wxCANCEL | wxOK );
    if( nReturn == wxCANCEL )
      bReturn = false ;
    m_bConfirmedYet = true ;
  }
  //return true;
  DEBUG("App::Confirm--end\n");
  return bReturn ;
}

void wxX86InfoAndControlApp::CPUcontrollerChanged()
{
  //May be NULL.
  if( mp_cpucontroller )
  {
    LOGN("before SetUserInterface")
    mp_cpucontroller->SetUserInterface(this) ;
    //Set the CPU access BEFORE getting number of CPU cores in
    //SetModelData(...) .
    //#ifdef _WINDOWS
    //mp_i_cpucontroller->SetCPUaccess( //mp_winring0dynlinked
    //  mp_i_cpuaccess ) ;
    //#else
    ////mp_i_cpucontroller->SetCPUaccess(NULL);
    ////mp_i_cpucontroller->SetCPUaccess( & m_MSRdeviceFile) ;
    //#endif
    
    mp_cpucontroller->SetCPUaccess( mp_i_cpuaccess ) ;
    mp_cpucontroller->SetModelData( //& m_modelData
       mp_modelData ) ;
    LOGN("before GetMaximumFrequencyInMHz. number of CPU cores: " <<
        (WORD) mp_modelData->m_cpucoredata.GetNumberOfCPUcores() )
#ifdef COMPILE_WITH_LOG
    //Needed for drawing the voltage-frequency curves.
    WORD wMaxFreqInMHz = mp_cpucontroller->GetMaximumFrequencyInMHz() ;
#endif //#ifdef COMPILE_WITH_LOG
    LOGN("after GetMaximumFrequencyInMHz: " << wMaxFreqInMHz )
    #if defined(COMPILE_WITH_CALC_THREAD)
      #if defined(_WINDOWS)
      mp_cpucontroller->SetCalculationThread(& m_calculationthread) ;
      #else
      mp_i_cpucontroller->SetCalculationThread(NULL) ;
      #endif
    #endif
//    LOGN("after SetCalculationThread")
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
    mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
    //Gets the data from the CPU and sets the info into the model data
    //(important step for drawing overvolt prot curve)
    mp_cpucontroller->GetMaximumFrequencyInMHz() ;
  }
  //m_modelData.SetCPUcontroller( mp_i_cpucontroller);
  mp_modelData->SetCPUcontroller( mp_cpucontroller);
  #ifdef _WINDOWS
//    m_calculationthread.SetCPUcontroller(mp_cpucontroller);
  #endif
  //At the 1st call of this function mp_frame is NULL.
  if( mp_frame )
  {
    //The CPU controller (dyn lib) (especially for Pentium M or Intel Core where
    // default voltages for multipliers are in PERF_STATUS MSR register ) may
    //have inserted default voltages: so display everything.
    mp_frame->RedrawEverything() ;
    mp_frame->SetCPUcontroller(mp_cpucontroller) ;
  }
}

void wxX86InfoAndControlApp::CPUcontrollerDeleted()
{
  mp_frame->CPUcontrollerDeleted() ;
}

void wxX86InfoAndControlApp::CPUcoreUsageGetterDeleted()
{
  mp_frame->CPUcoreUsageGetterDeleted() ;
}

void wxX86InfoAndControlApp::CurrenCPUfreqAndVoltageUpdated()
{
  //Only when true the cross is drawn.
  mp_frame->m_bDrawFreqAndVoltagePointForCurrCoreSettings = true ;
  //Force redraw of the client area.
  mp_frame->//ReDraw() ;
    Refresh() ;
}

//void wxX86InfoAndControlApp::DisableOwnDVFSAndDVFSbyService()
//{
//
//}

void wxX86InfoAndControlApp::EndDVFS()
{
  LOGN("wxX86InfoAndControlApp::EndDVFS() begin")
  PerCPUcoreAttributes * p_percpucoreattributes = & //mp_cpucoredata->
    mp_modelData->m_cpucoredata.
    m_arp_percpucoreattributes[ //p_atts->m_byCoreID
    0 ] ;
  //If the dyn freq scaling thread is active the program would crash when it
  //tries to get the CPU usage the next time and the usage getter is
  //destroyed.
  if ( p_percpucoreattributes->mp_dynfreqscalingthread )
    mp_frame->
    EndDynVoltAndFreqScalingThread(p_percpucoreattributes) ;
  LOGN("wxX86InfoAndControlApp::EndDVFS() end")
}

//http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
//"Another aspect of the application shutdown is OnExit which is called when 
//the application exits but before wxWidgets cleans up its internal 
//structures. You should delete all wxWidgets object that you created by 
//the time OnExit finishes. 
//In particular, do not destroy them from application class' destructor!"
int wxX86InfoAndControlApp::OnExit()
{
  LOGN("OnExit() begin")
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  if( mp_taskbaricon )
  {
//  Removing the icon is neccessary to exit the app/
//  else the icon may not be hidden after exit.
    mp_taskbaricon->RemoveIcon() ;
    LOGN("after removing the system tray icon")
    //Also deleted in the tbtest sample (not automatically deleted?!).
    delete mp_taskbaricon;
    LOGN("OnExit() after deleting the system tray icon")
  }
#endif //#ifdef COMPILE_WITH_TASKBAR

  //Release heap mem.
#ifdef COMPILE_WITH_CPU_SCALING
    //if(mp_wxdynfreqscalingtimer)
    //  delete mp_wxdynfreqscalingtimer ;
#endif //#ifdef COMPILE_WITH_CPU_SCALING
  //Release dynamically allocated memory (inside OnInit() ) :
  //#ifdef _WINDOWS
  //delete mp_winring0dynlinked ;
  //#endif
//  if( mp_i_cpuaccess )
//    delete mp_i_cpuaccess ;
  delete [] m_arartchCmdLineArgument ;
  //delete mp_frame ;
//  if( mp_cpucontroller )
//    delete mp_cpucontroller ;
  FreeRessources() ;
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  if( mp_dynfreqscalingaccess )
    delete mp_dynfreqscalingaccess ;
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
//  if( mp_modelData )
//    delete mp_modelData ;

#ifdef COMPILE_WITH_SHARED_MEMORY
  if( mp_voidMappedViewStartingAddress )
    ::UnmapViewOfFile(mp_voidMappedViewStartingAddress);
  if( m_handleMapFile != NULL )
    ::CloseHandle(m_handleMapFile);
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY
  LOGN("OnExit() before return 0")
  return 0;
}

//TODO make this function a member of wxX86InfoAndControlApp
void //wxX86InfoAndControlApp::
  FetchCPUcoreDataFromIPC(wxX86InfoAndControlApp * p_wxx86infoandcontrolapp)
{
  LOGN("FetchCPUcoreDataFromIPC begin")
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  NamedPipeClient & r_namedpipeclient = p_wxx86infoandcontrolapp->
      m_ipcclient ;
  if( //sending command succeeded
    r_namedpipeclient.SendCommandAndGetResponse(get_current_CPU_data) &&
  //    ::wxGetApp().m_ipcclient.SendCommand(get_current_CPU_data) ;
//  if(
    r_namedpipeclient.m_arbyIPCdata &&
    // > 0 bytes
    r_namedpipeclient.m_dwSizeInByte
    )
  {
//      mp_wxx86infoandcontrolapp->m_ipc_current_cpu_data_handler
//      mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler.
//        Parse( r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwSizeInByte ) ;
//      XERCES_CPP_NAMESPACE::MemBufInputSource membufinputsource(
////        arby,
////        dwSizeInBytes ,
//        r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwSizeInByte ,
//        L"IPC_buffer" ) ;
    {
//      wxCriticalSectionLocker locker( m_sax2_ipc_current_cpu_data_handler.
//        m_wxcriticalsection ) ;
//      ReadXMLdocumentInitAndTermXerces(
      if( ReadXMLdocumentWithoutInitAndTermXerces(
  //        membufinputsource,
        r_namedpipeclient.m_arbyIPCdata ,
        r_namedpipeclient.m_dwSizeInByte ,
        L"IPC_buffer" ,
        p_wxx86infoandcontrolapp->m_model ,
        p_wxx86infoandcontrolapp ,
        p_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler
        ) == FAILURE
        )
      {
        //exception-> leave crit sec (endDocument(), where Leave() is
        // called is not reached)
        LOGN("wxconditionIPC2InProgramData.Leave")
        p_wxx86infoandcontrolapp->m_model.m_cpucoredata.
          wxconditionIPC2InProgramData.Leave() ;
        LOGN("after wxconditionIPC2InProgramData.Leave")
      }
    }
//      p_cpucontroller = & p_wxx86infoandcontrolapp->
//          m_sax2_ipc_current_cpu_data_handler ;
//      p_cpucoreusagegetter = & p_wxx86infoandcontrolapp->
//          m_sax2_ipc_current_cpu_data_handler ;
//      //The number of CPU cores is known if the IPC data were got at first.
//      WORD wNumCPUcores = p_cpucoreusagegetter->GetNumberOfLogicalCPUcores() ;
//      if( wNumCPUcores < mp_cpucoredata->m_byNumberOfCPUCores )
//        mp_cpucoredata->SetCPUcoreNumber( wNumCPUcores ) ;
  }
  p_wxx86infoandcontrolapp->m_vbGotCPUcoreData = true ;
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE_
}

//This function should be executed in a separate thread.
DWORD THREAD_PROC_CALLING_CONVENTION
  GetCurrentCPUcoreDataViaIPCthreadFunc(void * p_v )
{
  wxX86InfoAndControlApp * p_wxx86infoandcontrolapp =
    ( wxX86InfoAndControlApp * ) p_v ;
  if( p_wxx86infoandcontrolapp )
  {
    FetchCPUcoreDataFromIPC( p_wxx86infoandcontrolapp ) ;
    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;
  }
  return 0 ;
}

//This function should be executed in a separate thread.
DWORD THREAD_PROC_CALLING_CONVENTION
  GetCurrentCPUcoreDataViaIPCinLoopThreadFunc(void * p_v )
{
  LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc begin")
  wxX86InfoAndControlApp * p_wxx86infoandcontrolapp =
    ( wxX86InfoAndControlApp * ) p_v ;
  if( p_wxx86infoandcontrolapp )
  {
    //Wait until another function calls Leave().
//    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Enter() ;
//    DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Leave")
//    //Let the other thread continue at its "Enter()"
//    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;

//    //MainFrame::Onclose() locks the mutex
//    if( p_wxx86infoandcontrolapp->m_wxmutexIPCthread.TryLock() ==
//        //http://docs.wxwidgets.org/2.6/wx_wxmutex.html#wxmutextrylock:
//        //"The mutex is already locked by another thread."
//        wxMUTEX_BUSY )
//      return 0 ;

//    LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Lock")
//    //http://docs.wxwidgets.org/2.6/wx_wxcondition.html#wxconditionwait:
//    //"it must be locked prior to calling Wait"
//    p_wxx86infoandcontrolapp->m_wxmutexIPCthread.Lock() ;

//    Sleep(4000) ;
    LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc after Lock()")

    //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionwait:
    //"This method atomically releases the lock on the mutex associated with
    //this condition"
//    p_wxx86infoandcontrolapp->m_wxconditionIPCthread.Wait() ;
    p_wxx86infoandcontrolapp->m_condition_type_eventIPCthread.Wait() ;
    //After waking up from Wait() the mutex is locked by this thread:
    //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionwait:
    //"It then locks the mutex again and returns."
    LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc retrieve CPU core data?"
      << p_wxx86infoandcontrolapp->m_vbRetrieveCPUcoreData )
    while( p_wxx86infoandcontrolapp->m_vbRetrieveCPUcoreData )
    {
      LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc after Wait()")
      FetchCPUcoreDataFromIPC( p_wxx86infoandcontrolapp ) ;
      p_wxx86infoandcontrolapp->m_vbGotCPUcoreData = true ;
//      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Enter")
//      //Wait until another function calls Leave().
//      p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Enter() ;
//      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Leave")
//      //Let the other thread continue at its "Enter()"
//      p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;
//    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;
//      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before TryLock")
//      //MainFrame::Onclose() locks the mutex
//      if( p_wxx86infoandcontrolapp->m_wxmutexIPCthread.TryLock() ==
//          //http://docs.wxwidgets.org/2.6/wx_wxmutex.html#wxmutextrylock:
//          //"The mutex is already locked by another thread."
//          wxMUTEX_BUSY )
//        return 0 ;

//      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Lock")

//      LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Lock")
//      //http://docs.wxwidgets.org/2.6/wx_wxcondition.html#wxconditionwait:
//      //"it must be locked prior to calling Wait"
//      p_wxx86infoandcontrolapp->m_wxmutexIPCthread.Lock() ;

      if( ! p_wxx86infoandcontrolapp->m_vbRetrieveCPUcoreData )
        break ;
//      DEBUGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Wait()")
      LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc before Wait()")
      //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionwait:
      //"This method atomically releases the lock on the mutex associated with
      //this condition (this is why it must be locked prior to calling Wait)
      //and puts the thread to sleep until Signal or Broadcast is called."
//      p_wxx86infoandcontrolapp->m_wxconditionIPCthread.Wait() ;
      p_wxx86infoandcontrolapp->m_condition_type_eventIPCthread.Wait() ;
//      p_wxx86infoandcontrolapp->m_condition_type_eventIPCthread.ResetEvent() ;
    }
    LOGN("InterProcessCommunication client thread: ended get CPU core data loop")
  }
  else
    LOGN("GetCurrentCPUcoreDataViaIPCinLoopThreadFunc app pointer == NULL")
  return 2 ;
}

void wxX86InfoAndControlApp::EndGetCPUcoreDataViaIPCthread()
{
  //TODO the IPC get CPU core data thread should have finished before any
  //other Xerces function is executed (else program crash AFAIR: error at;
  //"14 _fu71___ZN11xercesc_3_116XMLPlatformUtils15fgMemoryManagerE()
  //T:\SourceCodeManagement\X86Info_and_Control\Xerces\PStateConfig.cpp:522
  //0x0044152a  "
  //).
  //  wxCriticalSectionLocker wxcriticalsectionlocker( mp_wxx86infoandcontrolapp->
  //    m_wxcriticalsectionIPCthread ) ;

  {
//    LOGN("before locking the mutex for the \"get CPU core data via IPC\" "
//      "condition ")
//    //  //The IPC thread checks if the mutex is locked. If yes: it returns.
//    //Gets the lock when the IPC calls m_wxconditionIPCthread.Wait()
//    //Lock the mutex so that it is ensured that the "Broadcast()" called by
//    //this function really ends the thread.
//    wxMutexLocker wxmutexlocker( m_wxmutexIPCthread);
//    LOGN("after locking the mutex for the \"get CPU core data via IPC\" "
//      "condition ")
    //-> do not get CPU core data via IPC in the thread/ ->exit loop in thread
    m_vbRetrieveCPUcoreData = false ;
    //  //Let the IPC thread waiting via "Enter()" continue.
    //  mp_wxx86infoandcontrolapp->m_wxcriticalsectionIPCthread.Leave() ;
  //This destroys the wxMutexLocker object and so unlocks the mutex.
  //(MUST be unlocked because Wait() locks the mutex after woken up after
  //Wait() ).
  }

  //Wake up the IPC thread.
  LOGN("before waking up the get CPU core data via IPC thread")
  //The IPC thread may not Wait() for the condition currently.
//  m_wxconditionIPCthread.Broadcast();
  m_condition_type_eventIPCthread.Broadcast() ;

  LOGN("waiting for the end of the IPC thread")
  //http://docs.wxwidgets.org/2.6/wx_wxthread.html#wxthreadwait:
  //"you must Wait() for a joinable thread or the system resources used by it
  //will never be freed,
#ifdef COMPILE_WITH_LOG
  DWORD dwThreadReturnCode = (DWORD) m_x86iandc_threadIPC.WaitForTermination() ;
#endif
  LOGN("after waiting for the end of the IPC thread. return code: " <<
    dwThreadReturnCode )
  //http://docs.wxwidgets.org/2.6/wx_wxthread.html#wxthreadwait:
  //"and you also must delete the corresponding wxThread
  //object yourself"
  m_x86iandc_threadIPC.Delete() ;
  LOGN("After possibly freeing \"get CPU core data via IPC\" thread ressources")
}

//Getting the CPU core data (->depends on the implementation) can take many
//milliseconds, especially via IPC. So start a thread for this.
void wxX86InfoAndControlApp::GetCurrentCPUcoreDataViaIPCNonBlockingCreateThread()
{
  m_wxcriticalsectionIPCthread.Enter() ;
  x86IandC::thread_type x86iandc_threadIPC(I_Thread::detached) ;
  x86iandc_threadIPC.start( GetCurrentCPUcoreDataViaIPCthreadFunc , this ) ;
//  x86IandC::thread_type::start( GetCurrentCPUcoreDataViaIPCthreadFunc , this ) ;
}

//Getting the CPU core data (->depends on the implementation) can take many
//milliseconds, especially via IPC. So start a thread for this.
void wxX86InfoAndControlApp::GetCurrentCPUcoreDataViaIPCNonBlocking()
{
  LOGN("GetCurrentCPUcoreDataViaIPCNonBlocking before possibly waking up the "
    "\"get current CPU core data\" thread" )
//  //Let the IPC thread waiting on "Enter()" continue.
//  m_wxcriticalsectionIPCthread.Leave() ;
//  DEBUGN("GetCurrentCPUcoreDataViaIPCNonBlocking before Enter")
//  //Enter the crit section for the next call to Leave().
//  m_wxcriticalsectionIPCthread.Enter() ;

//  wxMutexLocker lock(m_wxmutexIPCthread);

  //This happened especially after a resume from standby (seen in log file):
  // when the thread that should be woken up had not called "Lock()" and
  // "Wait()" for a wxCriticalSection and Signal() was called then the program
  // crashed.
  //Wake up the thread
//  m_wxconditionIPCthread.Signal();
  m_condition_type_eventIPCthread.Broadcast() ;
  LOGN("GetCurrentCPUcoreDataViaIPCNonBlocking after possibly waking up the "
    "\"get current CPU core data\" thread" )
}

void wxX86InfoAndControlApp::InitSharedMemory()
{
#ifdef COMPILE_WITH_SHARED_MEMORY
  bool bSharedMemOk = false ;
  m_handleMapFile = OpenFileMapping(
    FILE_MAP_ALL_ACCESS,   // read/write access
    FALSE,                 // do not inherit the name
    m_stdstrSharedMemoryName.c_str()  // name of mapping object
    );

  if ( m_handleMapFile == NULL)
  {
    DWORD dwError = ::GetLastError() ;
    LOG("unable to open shared memory: \""
      << ::LocalLanguageMessageFromErrorCodeA(
        dwError
        )
      << "\" (error code: " << dwError << ")"
      );
    //return 1;
  }
  else
  {
    mp_voidMappedViewStartingAddress = MapViewOfFile(
      m_handleMapFile ,   // handle to map object
      FILE_MAP_ALL_ACCESS, // read/write permission
      0,
      0,
      sizeof(Model)
      );
  LOGN("Size of attributes:" << sizeof(Model) ) ;

    if ( mp_voidMappedViewStartingAddress == NULL)
    {
      DWORD dwError = ::GetLastError() ;
      LOG("Could not map view of file : \"" <<
        ::LocalLanguageMessageFromErrorCodeA(
          dwError
          )
        << "\" (error code: " << dwError << ")"
        );
      CloseHandle(m_handleMapFile);
      //return 1 ;
    }
    else
    {
      bSharedMemOk = true ;
      mp_modelData = (Model*) mp_voidMappedViewStartingAddress ;
  LOGN("Address of attributes:" << mp_modelData ) ;
    }
  }
//  if( ! bSharedMemOk )
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY
}

//wxX86InfoAndControlApp::CreateAndStartIPCthread()
//{
//  m_wxthreadIPC.Create() ;
//  m_wxthreadIPC.Run() ;
//}

BYTE wxX86InfoAndControlApp::GetConfigDataViaInterProcessCommunication()
{
  LOGN("GetConfigDataViaInterProcessCommunication begin")
  SAX2DefaultVoltageForFrequency sax2defaultvoltageforfrequency(
    * this ,
    m_model ) ;
  if( //sending command succeeded
    m_ipcclient.SendCommandAndGetResponse(get_configuration_data) &&
  //    ::wxGetApp().m_ipcclient.SendCommand(get_current_CPU_data) ;
  //  if(
    m_ipcclient.m_arbyIPCdata &&
    // > 0 bytes
    m_ipcclient.m_dwSizeInByte
    )
  {
    LOGN("GetConfigDataViaInterProcessCommunication-- number of IPC bytes:"
      << m_ipcclient.m_dwSizeInByte )
  //      mp_wxx86infoandcontrolapp->m_ipc_current_cpu_data_handler
  //      mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler.
  //        Parse( r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwSizeInByte ) ;
  //      XERCES_CPP_NAMESPACE::MemBufInputSource membufinputsource(
  ////        arby,
  ////        dwSizeInBytes ,
  //        r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwSizeInByte ,
  //        L"IPC_buffer" ) ;
  //      wxCriticalSectionLocker locker( m_sax2_ipc_current_cpu_data_handler.
  //        m_wxcriticalsection ) ;
  //      ReadXMLdocumentInitAndTermXerces(
    if( ReadXMLdocumentWithoutInitAndTermXerces(
//        membufinputsource,
      m_ipcclient.m_arbyIPCdata ,
      m_ipcclient.m_dwSizeInByte ,
      L"IPC_buffer" ,
      m_model ,
      this ,
//      m_sax2_ipc_current_cpu_data_handler
      sax2defaultvoltageforfrequency
      ) == FAILURE
      )
    {
      //exception-> leave crit sec (endDocument(), where Leave() is
      // called is not reached)
//      LOGN("wxconditionIPC2InProgramData.Leave")
//      m_model.m_cpucoredata.wxconditionIPC2InProgramData.Leave() ;
//      LOGN("after wxconditionIPC2InProgramData.Leave")
      return 1 ;
    }
    else
      return 0 ;
  }
  return 2 ;
}

bool wxX86InfoAndControlApp::OnInit()
{
  //Init to NULL for "CPUcontrollerChanged()"
  mp_frame = NULL ;
  gp_cpucontrolbase = this ;

  //from http://www.kharchi.eu/algierlib/tips.html:
  //If using MinGW then pass "-mwindows" as linker flag in order to hide the
  //console window.
//  HideMinGWconsoleWindow() ;

#ifdef COMPILE_WITH_SHARED_MEMORY
  mp_voidMappedViewStartingAddress = NULL ;
  m_handleMapFile = NULL ;
  mp_modelData = NULL ;
  m_stdstrSharedMemoryName = "CPUcontrolService" ;
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY
  m_stdtstrProgramName = _T("X86_info_and_control") ;

  m_arartchCmdLineArgument = new TCHAR * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
//  mp_modelData = new Model() ;
  mp_modelData = & m_model ;
  m_sax2_ipc_current_cpu_data_handler.m_cpc_cpucoredata =
    & mp_modelData->m_cpucoredata ;
  //If allocation succeeded.
  if( m_arartchCmdLineArgument && mp_modelData )
  {
    //ISpecificController
    //MyFrame * p_frame ;
    std::tstring stdtstrLogFilePath ;
    if( argc > 0 )
    {
      std::tstring tstrArg0(argv[0]) ;
      stdtstrLogFilePath = std::tstring( //Getstdtstring(argv) +
        tstrArg0
//        + _T("_log.txt")
        ) ;
    }
    else
      stdtstrLogFilePath = std::tstring( mp_modelData->m_stdtstrProgramName
        //+ _T("_log.txt")
        ) ;

    //Because more than 1 GUI is possible at a time: append a process ID.
    //So the log files are not overwritten by the GUI instances.
    DWORD dwProcID = wxGetProcessId() ;
    stdtstrLogFilePath += Getstdtstring( to_stdstring<DWORD>(dwProcID) ) ;
    stdtstrLogFilePath += _T("_log.txt") ;

    //Maybe it's better to use a file name for the log file that is derived 
    //from THIS executable's file name: e.g. so different log files for the 
    //x86I&C service and the x86I&C GUI are possible.
    g_logger.OpenFile( stdtstrLogFilePath ) ;
    LOGN("process ID of this process: ")

    //Intitialise to be valid.
    m_arartchCmdLineArgument[ 0 ] = _T("") ;
    m_arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] = 
      (TCHAR * ) (mp_modelData->m_stdtstrProgramName + _T("_config.xml") ).c_str() ;

    mp_userinterface = //p_frame ;
      this ;
#ifdef COMPILE_WITH_SHARED_MEMORY
    InitSharedMemory() ;
#endif //COMPILE_WITH_SHARED_MEMORY
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
    //CreateAndStartIPCthread() ;
//    m_wxthreadIPC.Create() ;
    //Let the looped IPC thread function wait at its
    //  "m_wxcriticalsectionIPCthread.Enter()"
    m_wxcriticalsectionIPCthread.Enter() ;
//    m_wxmutexIPCthread.Lock() ;
//    DEBUGN("before creating IPC thread")
    DEBUGN("before starting IPC thread")
//    m_x86iandc_threadIPC.start( GetCurrentCPUcoreDataViaIPCinLoopThreadFunc , this ) ;
    DEBUGN("after starting IPC thread")
  	if( m_ipcclient.Init() )
  	  LOGN("initializing IPC (for connection to the service) succeeded")
#endif
    //if( mp_modelData )
    {

  	  //TODO program malfunction when the IPC thread is started.
//      m_x86iandc_threadIPC.start( GetCurrentCPUcoreDataViaIPCinLoopThreadFunc , this ) ;
      try //catch CPUaccessexception
      {
    #ifdef _WINDOWS
      //WinRing0dynLinked winring0dynlinked(p_frame) ;
      //If allocated statically within this block / method the object 
      //gets invalid after leaving the block where it was declared.
      //mp_winring0dynlinked 
      //mp_i_cpuaccess = new WinRing0dynLinked(//p_frame
#ifdef _MSC_VER_ //possible because the import library is for MSVC
      mp_i_cpuaccess = new WinRing0_1_3LoadTimeDynLinked(
        this ) ;
#else //Use runtime dynamic linking because no import library is available for
      //MinGW.
      mp_i_cpuaccess = new WinRing0_1_3RunTimeDynLinked(
        this ) ;
#endif
      //m_maincontroller.SetCPUaccess( //mp_winring0dynlinked
      //  mp_i_cpuaccess ) ;
    #else
      //m_maincontroller.SetCPUaccess(NULL) ;
      //m_MSRdeviceFile.SetUserInterface(this) ;
      mp_i_cpuaccess = new MSRdeviceFile(this, GetNumberOfLogicalCPUcores() ) ;
      //m_maincontroller.SetCPUaccess(&m_MSRdeviceFile) ;
    #endif
      //Assign to the global variable so that the functions (ReadMSR(...) etc.)
      //that are exported by this executable can access the CPU registers.
      g_p_cpuaccess = mp_i_cpuaccess ;
      //the main controller needs CPUID (I_CPUaccess class ) access in order to
      //retrieve the CPU by model, family etc.
      m_maincontroller.SetCPUaccess( mp_i_cpuaccess );
      mp_i_cpuaccess->mp_model = mp_modelData ;
      }
      catch(//ReadMSRexception
          CPUaccessException & r_cpuaccessexception )
      {
        LOGN("caught a CPUaccessException:"
          << r_cpuaccessexception.m_stdstrErrorMessage )
        //We may continue to use this program: e.g. for testing usage getter
        //DLLs or for showing the usage etc. via IPC.
        mp_i_cpuaccess = NULL ;
      }
      m_maincontroller.SetAttributeData( mp_modelData ) ;
      //m_winring0dynlinked.SetUserInterface(p_frame);
     
      #ifdef _WINDOWS
      std::wstring stdwstrProgramName = GetStdWstring(m_stdtstrProgramName ) ;
      mp_dynfreqscalingaccess = new PowerProfDynLinked( //m_stdtstrProgramName
        stdwstrProgramName ) ;
      DEBUGN("after creating Windows power prof access")
      #else
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
      mp_dynfreqscalingaccess = NULL ;
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
      #endif
      //mp_i_cpucontroller = //CPUcontrollerFactory::
      mp_cpucontroller = NULL ;
      if(
        m_maincontroller.
        //Creates e.g. an AMD Griffin oder Intel Pentium M controller
        CreateCPUcontrollerAndUsageGetter( 
          mp_cpucontroller 
          , mp_cpucoreusagegetter
          ) 
        )
      {
//        //If neither a built-in or dynamically linked CPU controller or usage getter
//        //returned a core number > 0
//        if( mp_modelData->m_cpucoredata.m_byNumberOfCPUCores == 0 )
//        {
//          LOGN("number of CPU cores not set neither by CPU controller nor by "
//              "usage getter -> getting it from Windows")
//#ifdef _WINDOWS
//          mp_modelData->m_cpucoredata.m_byNumberOfCPUCores = GetNumLogicalCPUs() ;
//          LOGN("number of CPU core reported by Windows:"
//            << mp_modelData->m_cpucoredata.m_byNumberOfCPUCores )
//#endif
//        }
//        else
//          LOGN("number of CPU cores:"
//            << mp_modelData->m_cpucoredata.m_byNumberOfCPUCores )
        if( mp_i_cpuaccess )
          //Now we have created the CPU controller. It knows how many cores it has.
          //The core count is an important information e.g. for the Linux MSR device
          //file access.
          mp_i_cpuaccess->InitPerCPUcoreAccess( mp_cpucontroller->
            GetNumberOfCPUcores() ) ;
        mp_cpucontroller->SetCmdLineArgs(
          NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS,
          m_arartchCmdLineArgument ) ;
        
        //#ifdef _WINDOWS
        //mp_dynfreqscalingaccess = new PowerProfDynLinked( m_stdtstrProgramName ) ;
        //#else
        //mp_dynfreqscalingaccess = NULL ;
        //mp_i_cpucontroller->SetOtherDVFSaccess( NULL ) ;
        //#endif
        CPUcontrollerChanged() ;
      }
      m_maincontroller.ReadMainConfig( //m_modelData
        * mp_modelData, this );
      //Get the default, min and max voltages.
      BYTE byGetConfigDataViaInterProcessCommunicationReturnValue =
        GetConfigDataViaInterProcessCommunication() ;
      if( byGetConfigDataViaInterProcessCommunicationReturnValue )
        m_maincontroller.ReadPstateConfig( //m_modelData
          * mp_modelData, this );

      DEBUGN("before creating the main frame")
      wxString wxstrMainFrameTitle = //wxT("GUI") ;
          wxString(mp_modelData->m_stdtstrProgramName) + wxT(" GUI") ;
      ////The user interface must be created before the controller because
      ////it should show error messages because of e.g. missing privileges.
      //p_frame = new MyFrame(
      mp_frame = new MainFrame(
        //_T(PROGRAM_NAME)
        //m_stdtstrProgramName
//        mp_modelData->m_stdtstrProgramName +_T(" GUI")
        wxstrMainFrameTitle
        ,
        wxPoint(50,50),
        wxSize(450,340)
        , mp_cpucontroller
        //, & m_modelData.m_cpucoredata
        //, & mp_modelData->m_cpucoredata
        , mp_modelData
        , this
        );
      if( mp_frame )
      {
        LOGN("after main frame creation")
        //p_frame->Show(TRUE);
        //SetTopWindow(p_frame);
        mp_frame->Show(true);
  //      p_wxframe->Show( true ) ;
        LOGN("after showing the main frame")
  //      ShowTaskBarIcon() ;
        //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
        //"You call wxApp::SetTopWindow to let wxWidgets know about the top window."
        SetTopWindow(mp_frame);
      }
      LOGN("after setting the main frame as top level window")
    //#ifdef _WINDOWS
    //  m_calculationthread.SetCPUcontroller(mp_i_cpucontroller);
    //#endif
//      mp_frame->SetCPUcontroller(mp_cpucontroller) ;
//#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//    if( m_ipcclient.Init() )
//      LOGN("initializing IPC (for connection to the service) succeeded")
//#endif

      if( mp_cpucontroller )
      {
        //char * archCPUID ;
        std::string strCPUID ;
//        DEBUG("initialization of dialog--after get processor name\n");
        //if( ! m_modelData.m_bSkipCPUtypeCheck &&
        //  ! mp_pstatectrl->IsSupportedCPUModel() )
        //{
        //  mp_userinterface->Confirm("This CPU model is not supported by this program."
        //    "Use \"skip_cpu_type_check\" (see help) if you think it makes "
        //    "sense.\n->exiting");
        //  return FALSE;
        //}
  //#endif //	#ifdef _EMULATE_TURION_X2_ULTRA_ZM82

//        DEBUG("initialization of dialog--after a possible CPU type check\n");

        mp_cpucontroller->SetCmdLineArgs(
          NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ,
          m_arartchCmdLineArgument
          ) ;
//        BYTE byReturn = mp_cpucontroller->HandleCmdLineArgs( ) ;

//        DEBUG("initialization of dialog--after handling cmd line args\n");
//        //DEBUG("return value of handleCmdLineArgs(): %u\n",(WORD)byReturn);
//        LOG("return value of handling command line args: " << (WORD) byReturn
            //<< "\n"
//            )

//        switch(byReturn)
//        {
//          case FAILURE:
//            mp_userinterface->Confirm("An error occured (a message should have been "
//              "shown previously)->exiting");
//            return FALSE;
//            break;
//          case EXIT:
//            return FALSE;
//            break;
////          default:
//  //          DEBUG("Before starting timer\n");
////            DWORD dwValue = 0 ;
//            //TODO read values from CPU at first because the other values should not
//            //be affected.
//        }
      } //if( mp_i_cpucontroller )
      //else //CreateCPUcontrollerAndUsageGetter(...) failed
      //  mp_userinterface->Confirm("got no CPU controller and/ or CPU usage getter");
//      }
//      catch(//ReadMSRexception
//          CPUaccessException & r_cpuaccessexception )
//      {
//          //this->Confirm("Reading from MSR failed kjj");
//          //mp_frame->Confirm("CPU access error.\n(insuff rights?)");
//          return FALSE ;
//      }
//      catch( VoltageSafetyException e )
//      {
//        Confirm( //GetCharPointer( ( e.m_stdtstrMessage + _T("\n->Exiting") ).
//          //c_str() )
//          GetStdString( e.m_stdtstrMessage + _T("\n->Exiting") )
//          ) ;
//        return FALSE ;
//      }
    //TODO program hangs when message that DLL function is missing
    m_x86iandc_threadIPC.start( GetCurrentCPUcoreDataViaIPCinLoopThreadFunc ,
      this ) ;
    }// if (mp_modelData)
  }
  else
    return FALSE ;
  return TRUE;
} 

void wxX86InfoAndControlApp::outputAllPstates(
  unsigned char byCurrentP_state, int & vid)
{

}

#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
void wxX86InfoAndControlApp::PossiblyAskForOSdynFreqScalingDisabling()
{
  if( mp_dynfreqscalingaccess->OtherDVFSisEnabled()
    )
    if( ::wxMessageBox(
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      _T("The OS's dynamic frequency scaling must be disabled ")
      _T("in order that the p-state isn' changed by the OS afterwards.")
      _T("If the OS's dynamic frequency isn't disabled, should it be done now?")
      ,
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      _T("Question")
      , wxYES_NO | wxICON_QUESTION )
      == wxYES
     )
    mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ;
}
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS

void wxX86InfoAndControlApp::RedrawEverything()
{
  mp_frame->RedrawEverything() ;
}

void wxX86InfoAndControlApp::SetCPUcontroller( 
  I_CPUcontroller * p_cpucontrollerNew )
{
  if( p_cpucontrollerNew )
  {
    //Avoid porgram crash because of the mainframe tries to get the current
    //performance state.
    mp_frame->DenyCPUcontrollerAccess() ;
    if( mp_cpucontroller )
    {
      //Release memory.
      delete mp_cpucontroller ;
      LOGN(" current CPU controller deleted")
    }
    LOGN("address of model: " << mp_modelData )
    mp_cpucontroller = p_cpucontrollerNew ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( p_cpucontrollerNew ) ;
    LOGN("after setting CPU controller for usage getter")
    //mp_i_cpucontroller->SetModelData( //& m_modelData
    //  mp_modelData ) ;
    CPUcontrollerChanged() ;
    LOGN("after CPUcontrollerChanged")

    //mp_frame->mp_i_cpucontroller = mp_cpucontroller ;
    mp_frame->AllowCPUcontrollerAccess() ;
    //Force an update of the canvas.
    mp_frame->RedrawEverything() ;
    LOGN("after redraweverything")
  }
}

bool wxX86InfoAndControlApp::ShowTaskBarIcon(MainFrame * p_mf )
{
//      m_systemtray_icon_notification_window = mp_frame->GetHandle() ;
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  mp_frame = p_mf ;
      //from wxWidgets sample tbtest.cpp
      // Created:     01/02/97
      // RCS-ID:      $Id: tbtest.cpp 36336 2005-12-03 17:55:33Z vell $
  if( ! mp_taskbaricon )
  {
    mp_taskbaricon = new MyTaskBarIcon(mp_frame);
//      m_taskbaricon.SetMainFrame(mp_frame) ;
    if( mp_taskbaricon )
    {
//    #if defined(__WXCOCOA__)
//      m_dockIcon = new MyTaskBarIcon(wxTaskBarIcon::DOCK);
//    #endif
//      wxIcon wxicon(
//        //Use wxT() macro to enable to compile with both unicode and ANSI.
//        wxT("x86IandC.ico") ,
//        wxBITMAP_TYPE_ICO
//        ) ;
//      if( wxicon.IsOk() )
      wxIcon wxicon ;
      if( //http://docs.wxwidgets.org/stable/wx_wxicon.html:
          //"true if the operation succeeded, false otherwise."
        wxicon.LoadFile(
        //Use wxT() macro to enable to compile with both unicode and ANSI.
        wxT("x86IandC.ico") ,
        wxBITMAP_TYPE_ICO
        )
        )
      {
        if( mp_taskbaricon->SetIcon( //wxICON(sample),
            //m_taskbaricon.SetIcon(
            wxicon ,
            mp_modelData->m_stdtstrProgramName
            )
          )
        {
          LOGN("set system tray icon")
          return true ;
        }
        else
          ::wxMessageBox(wxT("Could not set task bar icon."),
              getwxString(m_stdtstrProgramName) );
      }
    }
  }
#endif //#ifdef COMPILE_WITH_TASKBAR

#ifdef _WINDOWS
  #ifdef USE_WINDOWS_API_DIRECTLY_FOR_SYSTEM_TRAY_ICON
      HICON hicon = (HICON) ::LoadImage(
           //http://msdn.microsoft.com/en-us/library/ms648045%28v=VS.85%29.aspx:
           //"To load a stand-alone resource (icon, cursor, or bitmap file)�for
      //     example, c:\myimage.bmp�set this parameter to NULL."
           NULL ,//__in  HINSTANCE hinst,
      //     __in  LPCTSTR lpszName,
           "x86IandC.ico" ,
           IMAGE_ICON ,//__in  UINT uType,
      //     0 ,//__in  int cxDesired,
      //     0, //__in  int cyDesired,
           16,
           16,
           LR_LOADFROMFILE //__in  UINT fuLoad
         );
//      std::cout << "wndproc hicon:" << hicon << "\n" ;
      m_systemtrayaccess.m_hwndReceiveIconNotifications =
          (HWND) m_systemtray_icon_notification_window ;
      m_systemtrayaccess.ShowIconInSystemTray(hicon,"x86Info&Control") ;
  #endif //  #ifdef USE_WINDOWA_API_SYSTEM_TRAY_ICON
#endif
  return false ;
}

void wxX86InfoAndControlApp::DeleteCPUcontroller( )
{
  DEBUGN("wxX86InfoAndControlApp::DeleteCPUcontroller() cpu controller:" <<
      mp_cpucontroller )
  //if( p_cpucontroller )
  //{
    //Avoid program crash because of the mainframe tries to get the current
    //performance state.
    mp_frame->DenyCPUcontrollerAccess() ;
    if( mp_cpucontroller )
      //Release memory.
      delete mp_cpucontroller ;
    mp_cpucontroller = NULL ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
    {
      DEBUGN("wxX86InfoAndControlApp::DeleteCPUcontroller()--before "
          "setting CPU controller access for the usage getter")
      mp_cpucoreusagegetter->SetCPUcontroller( NULL ) ;
      DEBUGN("wxX86InfoAndControlApp::DeleteCPUcontroller()--after "
          "setting CPU controller access for the usage getter")
    }
    //mp_i_cpucontroller->SetModelData( //& m_modelData
    //  mp_modelData ) ;
//    CPUcontrollerChanged() ;
    //For the draw functions to function properly.
    mp_frame->SetCPUcontroller( mp_cpucontroller ) ;
    mp_frame->AllowCPUcontrollerAccess() ;
    DEBUGN("wxX86InfoAndControlApp::DeleteCPUcontroller()--after "
        "calling AllowCPUcontrollerAccess()")
    //Force an update of the canvas.
    mp_frame->RedrawEverything() ;
  //}
  DEBUGN("wxX86InfoAndControlApp::DeleteCPUcontroller() end")
}
