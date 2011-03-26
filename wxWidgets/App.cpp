#define _AFXDLL

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheinlich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #define THREAD_PROC_CALLING_CONVENTION WINAPI
#else
  #define THREAD_PROC_CALLING_CONVENTION /* -> empty string */
#endif

#ifndef WX_PRECOMP
  #include "wx/app.h"
#endif
#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
//#include <wx/tooltip.h> //for wxToolTip::SetDelay(...)
#include <wx/filename.h> //wxFileName::GetPathSeparator(...)

#include "App.hpp"

//#include "wxDynFreqScalingTimer.hpp"
//#include "wxDynLinkedCPUcoreUsageGetter.hpp"
//for ::GetErrorMessageFromErrorCodeA(DWORD)
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Controller/GetNumberOfLogicalCPUcores.h>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
//#include <Controller/character_string/tchar_conversion.h> //for GetCharPointer(...)
#include <Controller/character_string/stdstring_format.hpp> //to_stdstring()
//GetFilenameWithoutExtension(const std::string &)
#include <Controller/FileSystem/GetFilenameWithoutExtension/\
GetFilenameWithoutExtension.hpp>
#include <InputOutput/WriteFileContent/WriteFileContent.hpp>
#include <Controller/IPC/I_IPC.hpp> //for "get_current_CPU_data"
#include <Controller/IPC/IPC_data.hpp> //class IPC_data
#include <Controller/I_CPUaccess.hpp> //class I_CPUaccess, CPUaccessException
#include <Controller/IDynFreqScalingAccess.hpp> //class IDynFreqScalingAccess
#include <Controller/Logger/Logger.hpp> //class Logger
#include <Controller/X86InfoAndControlExceptions.hpp> //for VoltageSafetyException
#include <Controller/multithread/I_Thread.hpp> //class I_Thread
#include <ModelData/ModelData.hpp>
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
//getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <wxWidgets/UserInterface/MainFrame.hpp>
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  #include <wxWidgets/UserInterface/TaskBarIcon.hpp>
#endif
//#include <wxWidgets/wxStringHelper.h>
//class SAX2VoltagesForFrequencyHandler
#include <Xerces/SAX2VoltagesForFrequencyHandler.hpp>
#include <Xerces/UserInterface/SAX2UserInterfaceConfigHandler.hpp>
#include <Xerces/XMLAccess.hpp> //for readXMLconfig()

#include "DynFreqScalingThread.hpp"
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  //#include <Windows/DynFreqScalingThread.hpp>
//  #include <Windows/GetNumberOfLogicalCPUs.h>
//  #include <Windows/HideMinGWconsoleWindow.h>
//#include <Windows/GetWindowsVersion.h>
//#include <Windows/LocalLanguageMessageFromErrorCodeA.h>
  #include <Windows/PowerProfAccess/PowerProfDynLinked.hpp>
  #include <Windows/Service/ServiceBase.hpp> //ServiceBase::PauseService(...)
  //#include <Windows/WinRing0dynlinked.hpp>
  //#include <Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.hpp>
  #include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
#else
  #include <Linux/MSRdeviceFile.hpp>
#endif
//class NonBlocking::wxServiceSocketClient
#include <wxWidgets/Controller/non-blocking_socket/client/\
wxServiceSocketClient.hpp>
//#include <strstream> //ostrstream
#include <string> //class std::string
//#include <errno.h> //for "errno"

//#include <wxWidgets/multithread/wxThreadBasedI_Thread.hpp>

FILE * fileDebug ; //for debug logging.
//This global (important for using preprocessor macros) object is used for 
//easy logging.
//Logger g_logger ;
CPUcontrolBase * gp_cpucontrolbase ;
////Needed for the exported functions.
//I_CPUaccess * g_p_cpuaccess ;

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
  CPUcontrolBase(this) ,
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
//  m_sax2_ipc_current_cpu_data_handler(m_model),
#endif
#ifdef COMPILE_WITH_SYSTEM_TRAY_ICON
  mp_taskbaricon( NULL)
//  , m_wxthreadIPC( )
  ,
#endif //#ifdef COMPILE_WITH_TASKBAR
//#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
//  mp_dynfreqscalingaccess(NULL) ,
//#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  //m_ipcclient( m_model ) ,
  m_p_i_ipcclient(NULL) ,
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//  m_maincontroller( this ),
  m_vbRetrieveCPUcoreData( true)
  , m_vbGotCPUcoreData (true)
  //Must explicitely init m_wxmutexIPCthread and m_wxconditionIPCthread
  // in this order here (condition need mutex in c'tor, so init mutex before)?!
  , m_wxmutexIPCthread(wxMUTEX_DEFAULT)
//  , m_wxconditionIPCthread( m_wxmutexIPCthread )
  , m_x86iandc_threadIPC(I_Thread::joinable)
  , m_xerces_voltage_for_frequency_configuration( & m_model )
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
  LOGN("app's destructor: before freeing taskbar object")
//  if( mp_taskbaricon)
//    delete mp_taskbaricon;
  DeleteTaskBarIcon();
  LOGN("end of app's destructor")
}

void wxX86InfoAndControlApp::CheckForChangedVoltageForFrequencyConfiguration()
{
  LOGN("wxX86InfoAndControlApp::CheckForChangedVoltageForFrequency"
    "Configuration() begin");
  std::string strCPUtypeRelativeDirPath ;
  std::string strPstateSettingsFileName ;
  if( //wxGetApp().m_maincontroller.GetPstatesDirPath(
    //mp_wxx86infoandcontrolapp->
      m_maincontroller.GetPstatesDirPath(
      strCPUtypeRelativeDirPath )
    && //wxGetApp().m_maincontroller.GetPstateSettingsFileName(
    //mp_wxx86infoandcontrolapp->m_maincontroller.GetPstateSettingsFileName(
    m_maincontroller.GetPstateSettingsFileName(
      strPstateSettingsFileName )
    )
  {
    std::string stdstrCPUtypeRelativeFilePath = strCPUtypeRelativeDirPath + "/"
      + strPstateSettingsFileName ;
    LOGN("before checking for a changed p-states config ")
    //TODO uncomment
    if( //mp_frame->
      m_xerces_voltage_for_frequency_configuration.IsConfigurationChanged(
      //strPstateSettingsFileName
      stdstrCPUtypeRelativeFilePath ) )
    {
      int nReturn = ::wxMessageBox(
        wxT("The performance state configuration has changed.\n")
        wxT("Save changes?"),
        //wxGetApp().m_stdtstrProgramName
        m_stdtstrProgramName
        , wxYES | wxNO
        );
      if( nReturn == wxYES )
      {
//        wxCommandEvent evt ;
//        mp_frame->OnSaveVoltageForFrequencySettings( evt) ;
        SaveVoltageForFrequencySettings();
      }
    }
  }
  LOGN("wxX86InfoAndControlApp::CheckForChangedVoltageForFrequency"
    "Configuration() end");
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

void wxX86InfoAndControlApp::CreateAndShowMainFrame()
{
  LOGN("before creating the main frame")
  wxString wxstrMainFrameTitle = //wxT("GUI") ;
    wxString(mp_modelData->m_stdtstrProgramName) + wxT(" GUI") ;
  ////The user interface must be created before the controller because
  ////it should show error messages because of e.g. missing privileges.
  //p_frame = new MyFrame(
  mp_frame = new MainFrame(
    //_T(PROGRAM_NAME)
    //m_stdtstrProgramName
  //        mp_modelData->m_stdtstrProgramName +_T(" GUI")
    wxstrMainFrameTitle ,
  //        wxPoint(50,50),
    wxPoint( m_model.m_userinterfaceattributes.
      m_wMainFrameTopLeftCornerXcoordinateInPixels ,
      m_model.m_userinterfaceattributes.
      m_wMainFrameTopLeftCornerYcoordinateInPixels ) ,
    //wxSize(450,340)
    wxSize( m_model.m_userinterfaceattributes.m_wMainFrameWidthInPixels
      , m_model.m_userinterfaceattributes.m_wMainFrameHeightInPixels)
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
    if( m_model.m_userinterfaceattributes.m_bShowMainFrameAtStartup )
      mp_frame->Show(true);
  //      p_wxframe->Show( true ) ;
    LOGN("after showing the main frame")
  //      ShowTaskBarIcon() ;
    //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
    //"You call wxApp::SetTopWindow to let wxWidgets know about the top window."
    SetTopWindow(mp_frame);
    LOGN("after setting the main frame as top level window")
  }
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
  #ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
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

bool wxX86InfoAndControlApp::ConnectIPCclient(
  const wxString & cr_wxstrIPCclientURL
//  , std::string & r_stdstrMessage
  )
{
  if( cr_wxstrIPCclientURL != wxEmptyString )
  {
    //Guard against concurrent operations on the wx86InfoAndControlApp's
    // m_p_i_ipclient object: no operation should occur until the object's
    // creation has finished.
    wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
      //mp_wxx86infoandcontrolapp->
      m_wxcriticalsectionIPCobject ) ;

    if( //mp_wxx86infoandcontrolapp->
        m_p_i_ipcclient )
      delete //mp_wxx86infoandcontrolapp->
      m_p_i_ipcclient ;
    //see http://msdn.microsoft.com/en-us/library/aa365783%28v=VS.85%29.aspx:
    //valid pipe names:
    // "\\ServerName\pipe\PipeName"
    // "\\.\pipe\PipeName" ( period (".") means: local computer )
    const wxChar * cp_wxchPipeNameRegEx = //wxT("\\\\?*\\pipe\\*") ;
      wxT("*?*\\pipe\\*") ;
      //wxT("\\*") ;
    if( cr_wxstrIPCclientURL.Matches( //wxT("\\\\?*\\pipe")
          cp_wxchPipeNameRegEx
          )
      )
    {
      LOGN("address \"" << GetStdString(cr_wxstrIPCclientURL)
        << "\" is a pipe address")
//      mp_model->m_stdwstrPipeName = GetStdWstring(cr_wxstrIPCclientURL) ;
      m_model.m_stdwstrPipeName = GetStdWstring(cr_wxstrIPCclientURL) ;
      //mp_wxx86infoandcontrolapp->
      m_p_i_ipcclient = new
        Windows::NamedPipeClient( //* mp_model
          m_model ) ;
    }
    else
    {
      wxString wxstrPipeName( cp_wxchPipeNameRegEx) ;
      LOGN("address \"" << GetStdString(cr_wxstrIPCclientURL)
        << "\"is not a pipe address: \""
        << GetStdString( cr_wxstrIPCclientURL )
        << "\" does not match \""
        << GetStdString( wxstrPipeName ) << "\"-> trying to connect via socket" )
      //mp_wxx86infoandcontrolapp->
//        m_p_i_ipcclient = new
//        NonBlocking::wxServiceSocketClient(cr_wxstrIPCclientURL) ;
    }
    //mp_wxx86infoandcontrolapp->
    m_wxstrDataProviderURL = cr_wxstrIPCclientURL ;
    //ConnectToDataProvider_Inline() ;
    return ConnectToDataProviderAndShowResult() ;
  }
  return false ;
}

bool wxX86InfoAndControlApp::ConnectToDataProviderAndShowResult()
{
  std::string stdstrMessage ;
  bool bConnected =
    //::wxGetApp().m_ipcclient.ConnectToDataProvider(stdstrMessage)
    //mp_wxx86infoandcontrolapp->
    IPCclientConnectToDataProvider(
      stdstrMessage ) ;
  if( bConnected )
  {
    //When connected then this is already shown in the main frame's title bar.
//    ::wxMessageBox( wxT("connected to the service now")
//      ) ;
  }
  else
    MessageWithTimeStamp( L"Could not connect to the service" +
      ( stdstrMessage.empty() ? //wxT("")
//          GetStdWstring( L("") )
        std::wstring( L"" )
        : //wxT(":\n")
        //+ wxString( wxT(":\n") ) + getwxString(stdstrMessage) )
        GetStdWstring( ":\n" + stdstrMessage )
      )
    );
  return bConnected ;
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

void wxX86InfoAndControlApp::DynVoltnFreqScalingEnabled()
{
  mp_frame->DynVoltnFreqScalingEnabled() ;
}

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

//    //Also deleted in the tbtest sample (not automatically deleted?!).
//    delete mp_taskbaricon;
    DeleteTaskBarIcon();

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
  //Already called by ~CPUcontrolBase()
//  FreeRessources() ;
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
//  NamedPipeClient & r_namedpipeclient = p_wxx86infoandcontrolapp->
//      m_ipcclient ;
  //Lock concurrent access to p_i_ipcclient from another thread.
  wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
    p_wxx86infoandcontrolapp->m_wxcriticalsectionIPCobject ) ;
  IPC_Client * p_i_ipcclient = p_wxx86infoandcontrolapp->m_p_i_ipcclient ;
  p_wxx86infoandcontrolapp->s_ipc_data.m_byCommand = get_current_CPU_data;
  p_wxx86infoandcontrolapp->s_ipc_data.m_wDataToWriteSizeInByte = 1;
  if( p_i_ipcclient &&
      //sending command succeeded
//    r_namedpipeclient.SendCommandAndGetResponse(get_current_CPU_data) &&
      p_i_ipcclient->SendCommandAndGetResponse(//get_current_CPU_data
        p_wxx86infoandcontrolapp->s_ipc_data) &&
  //    ::wxGetApp().m_ipcclient.SendCommand(get_current_CPU_data) ;
//  if(
//    r_namedpipeclient.m_arbyIPCdata &&
      p_i_ipcclient->m_arbyIPCdata &&
    // > 0 bytes
//    r_namedpipeclient.m_dwIPCdataSizeInByte
      p_i_ipcclient->m_dwIPCdataSizeInByte
    )
  {
//      mp_wxx86infoandcontrolapp->m_ipc_current_cpu_data_handler
//      mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler.
//        Parse( r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwIPCdataSizeInByte ) ;
//      XERCES_CPP_NAMESPACE::MemBufInputSource membufinputsource(
////        arby,
////        dwSizeInBytes ,
//        r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwIPCdataSizeInByte ,
//        L"IPC_buffer" ) ;
    {
//      wxCriticalSectionLocker locker( m_sax2_ipc_current_cpu_data_handler.
//        m_wxcriticalsection ) ;
//      ReadXMLdocumentInitAndTermXerces(
      if( ReadXMLdocumentWithoutInitAndTermXerces(
  //        membufinputsource,
//        r_namedpipeclient.m_arbyIPCdata ,
        p_i_ipcclient->m_arbyIPCdata ,
//        r_namedpipeclient.m_dwIPCdataSizeInByte ,
        p_i_ipcclient->m_dwIPCdataSizeInByte ,
        (WCHAR * ) L"IPC_buffer" ,
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
#else
  m_x86iandc_threadIPC.WaitForTermination() ;
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

//@return 0=success
BYTE wxX86InfoAndControlApp::GetConfigDataViaInterProcessCommunication()
{
  LOGN("GetConfigDataViaInterProcessCommunication begin")
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  SAX2VoltagesForFrequencyHandler sax2voltages_for_frequency_handler(
    * this ,
    m_model ) ;
  if( //sending command succeeded
    //m_ipcclient.SendCommandAndGetResponse(get_configuration_data) &&
    IPC_ClientSendCommandAndGetResponse_Inline(get_configuration_data) &&
  //    ::wxGetApp().m_ipcclient.SendCommand(get_current_CPU_data) ;
  //  if(
    //m_ipcclient.m_arbyIPCdata &&
    m_p_i_ipcclient->m_arbyIPCdata &&
    // > 0 bytes
    //m_ipcclient.m_dwIPCdataSizeInByte
    m_p_i_ipcclient->m_dwIPCdataSizeInByte
    )
  {
    LOGN("GetConfigDataViaInterProcessCommunication-- number of IPC bytes:"
      //<< m_ipcclient.m_dwIPCdataSizeInByte
      << m_p_i_ipcclient->m_dwIPCdataSizeInByte
      )
  //      mp_wxx86infoandcontrolapp->m_ipc_current_cpu_data_handler
  //      mp_wxx86infoandcontrolapp->m_sax2_ipc_current_cpu_data_handler.
  //        Parse( r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwIPCdataSizeInByte ) ;
  //      XERCES_CPP_NAMESPACE::MemBufInputSource membufinputsource(
  ////        arby,
  ////        dwSizeInBytes ,
  //        r_ipcclient.m_arbyIPCdata , r_ipcclient.m_dwIPCdataSizeInByte ,
  //        L"IPC_buffer" ) ;
  //      wxCriticalSectionLocker locker( m_sax2_ipc_current_cpu_data_handler.
  //        m_wxcriticalsection ) ;
  //      ReadXMLdocumentInitAndTermXerces(
    if( ReadXMLdocumentWithoutInitAndTermXerces(
//        membufinputsource,
      //m_ipcclient.m_arbyIPCdata ,
      m_p_i_ipcclient->m_arbyIPCdata ,
      //m_ipcclient.m_dwIPCdataSizeInByte ,
      m_p_i_ipcclient->m_dwIPCdataSizeInByte ,
      //Avoid g++ warning "deprecated conversion from string constant to 'WCHAR*'"
      (WCHAR *) L"IPC_buffer" ,
      m_model ,
      this ,
//      m_sax2_ipc_current_cpu_data_handler
      sax2voltages_for_frequency_handler
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
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  LOGN("GetConfigDataViaInterProcessCommunication return 2")
  return 2 ;
}

void wxX86InfoAndControlApp::IPCclientDisconnect()
{
  wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
    m_wxcriticalsectionIPCobject ) ;
  if( //Must be the 1st evaluation: check whether pointer is NULL
    m_p_i_ipcclient )
    return m_p_i_ipcclient->Disconnect() ;
}

bool wxX86InfoAndControlApp::IPCclientConnectToDataProvider(
  std::string & r_stdstrMessage )
{
  return IPCclientConnect_Inline( r_stdstrMessage) ;
}

inline bool wxX86InfoAndControlApp::IPCclientConnect_Inline(
  std::string & r_stdstrMessage)
{
  //Lock concurrent access to p_i_ipcclient from another thread.
  wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
    m_wxcriticalsectionIPCobject ) ;
  if( //Must be the 1st evaluation: check whether pointer is NULL
    m_p_i_ipcclient )
    return m_p_i_ipcclient->ConnectToDataProvider(r_stdstrMessage) ;
  return false ;
}

bool wxX86InfoAndControlApp::IPC_ClientIsConnected()
{
  return IPC_ClientIsConnected_Inline() ;
}

//Accumulates operations needed for determining if the IPC client object is
//connected to the ICP server.
inline bool wxX86InfoAndControlApp::IPC_ClientIsConnected_Inline()
{
//  x86IandC::critical_section_locker_type criticalsection_locker_typeIPCobject(
//    mp_wxx86infoandcontrolapp->m_criticalsection_typeIPCobject ) ;
  wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
    m_wxcriticalsectionIPCobject ) ;
//  IPC_Client * p_i_ipc_client = mp_wxx86infoandcontrolapp->m_p_i_ipcclient ;
  return
    //Must be the 1st evaluation: check whether pointer is NULL
    m_p_i_ipcclient
    && m_p_i_ipcclient->IsConnected() ;
}

BYTE wxX86InfoAndControlApp::IPC_ClientSendCommandAndGetResponse(
  BYTE byCommand,
  DWORD dwSizeOfDataToSendInBytes /* = 1 */,
  BYTE ar_byDataToSend [] /* = NULL */
  )
{
  return IPC_ClientSendCommandAndGetResponse_Inline(
    byCommand,
    dwSizeOfDataToSendInBytes,
    ar_byDataToSend);
}

inline BYTE wxX86InfoAndControlApp::IPC_ClientSendCommandAndGetResponse_Inline(
  BYTE byCommand,
  DWORD dwSizeOfDataToSendInBytes /* = 1 */,
  BYTE ar_byDataToSend [] /* = NULL */
  )
{
  LOGN("IPC_ClientSendCommandAndGetResponse_Inline(" << (WORD) byCommand
    << "," << dwSizeOfDataToSendInBytes << "," << ar_byDataToSend
    << ") begin")
  //Lock concurrent access to p_i_ipcclient from another thread.
  wxCriticalSectionLocker wxcriticalsectionlockerIPCobject(
    m_wxcriticalsectionIPCobject ) ;
  if( //Must be the 1st evaluation: check whether pointer is NULL
    m_p_i_ipcclient )
  {
    IPC_data ipc_data;
    ipc_data.m_byCommand = byCommand;
    ipc_data.m_ar_byDataToSend = ar_byDataToSend;
    ipc_data.m_wDataToWriteSizeInByte = dwSizeOfDataToSendInBytes
//      //for the command byte
//      + 1
      ;
    return m_p_i_ipcclient->SendCommandAndGetResponse( //byCommand
      ipc_data) ;
  }
  return 0 ;
}

void wxX86InfoAndControlApp::MessageWithTimeStamp(
  const LPWSTR cp_lpwstrMessage)
{
  ::wxMessageBox( //::wxGetCurrentTime() ::wxGetLocalTimeMillis
    ::wxNow() + wxT(" ") + getwxString( cp_lpwstrMessage),
     getwxString(m_stdtstrProgramName)
     );
}

void wxX86InfoAndControlApp::MessageWithTimeStamp(
  const std::wstring & cr_stdwstrMessage)
{
  ::wxMessageBox( //::wxGetCurrentTime() ::wxGetLocalTimeMillis
    ::wxNow() + wxT(" ") + getwxString( cr_stdwstrMessage),
     getwxString(m_stdtstrProgramName)
     );
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
  m_stdtstrProgramName = //_T("X86_info_and_control") ;
    _T("x86_info_and_control") ;

  m_arartchCmdLineArgument = new TCHAR * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
//  mp_modelData = new Model() ;
  mp_modelData = & m_model ;
#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
  m_sax2_ipc_current_cpu_data_handler.m_cpc_cpucoredata =
    & mp_modelData->m_cpucoredata ;
#endif
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
    Xerces::SAX2UserInterfaceConfigHandler sax2userinterfaceconfighandler(
      m_model , this
      ) ;
    if( //return value: 0 = success
      ReadXMLfileWithoutInitAndTermXercesInline(
        "UserInterface.xml" ,
//        m_model ,
        this,
        sax2userinterfaceconfighandler
        )
      )
      Confirm( "loading UserInterface.xml failed" ) ;
    else
      m_wxstrDataProviderURL = getwxString( m_model.m_stdwstrPipeName ) ;
    DWORD dwProcID = wxGetProcessId() ;
    if( mp_modelData->m_bAppendProcessID )
    {
      //Because more than 1 GUI is possible at a time: append a process ID.
      //So the log files are not overwritten by the GUI instances.
      stdtstrLogFilePath += Getstdtstring( to_stdstring<DWORD>(dwProcID) ) ;
    }
    stdtstrLogFilePath += _T("_log.txt") ;

    //Maybe it's better to use a file name for the log file that is derived 
    //from THIS executable's file name: e.g. so different log files for the 
    //x86I&C service and the x86I&C GUI are possible.
    g_logger.OpenFile( stdtstrLogFilePath ) ;
    LOGN("process ID of this process:" << dwProcID )
    m_maincontroller.ReadMainConfig( //m_modelData
      * mp_modelData, this );
    LOGN("address of attribute data:" << & m_model)

    //Initialize to be valid.
    m_arartchCmdLineArgument[ 0 ] =
      //For an empty string: "_T("")" causes Linux g++ warning or error->use value
      // "\0" / "0"
      //_T("") ;
      //String terminating NULL
      0 ;
    m_arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] = 
      (TCHAR * ) (mp_modelData->m_stdtstrProgramName + _T("_config.xml") ).
      c_str() ;

//    mp_userinterface = //p_frame ;
//      this ;
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
//    std::string stdstrMessage ;
    wxString wxstrIPCdataProviderURL( getwxString( m_model.m_stdwstrPipeName)
      ) ;
    ConnectIPCclient(//stdstrMessage
      wxstrIPCdataProviderURL
      ) ;
//  	if( //m_ipcclient.Init()
//      //m_p_i_ipcclient && m_p_i_ipcclient->ConnectToDataProvider(stdstrMessage)
//  	  ConnectIPCclient(//stdstrMessage
//  	    wxstrIPCdataProviderURL
//  	    )
//      )
  	  LOGN("initializing IPC (for connection to the service) succeeded")
#endif
    //if( mp_modelData )
    {
  	  //TODO program malfunction when the IPC thread is started.
//      m_x86iandc_threadIPC.start( GetCurrentCPUcoreDataViaIPCinLoopThreadFunc , this ) ;
  	  CreateHardwareAccessObject() ;
      m_maincontroller.SetAttributeData( mp_modelData ) ;
      //m_winring0dynlinked.SetUserInterface(p_frame);
     
      #ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
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
//        //If neither a built-in or dynamically linked CPU controller nor
//        //usage getter returned a core number > 0
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
//        if( mp_i_cpuaccess )
//          //Now we have created the CPU controller. It knows how many cores it has.
//          //The core count is an important information e.g. for the Linux MSR device
//          //file access.
//          mp_i_cpuaccess->InitPerCPUcoreAccess( mp_cpucontroller->
//            GetNumberOfCPUcores() ) ;
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
      //Get the default, min and max voltages.
      BYTE byGetConfigDataViaInterProcessCommunicationReturnValue =
        GetConfigDataViaInterProcessCommunication() ;
      if( byGetConfigDataViaInterProcessCommunicationReturnValue )
        m_maincontroller.ReadPstateConfig( //m_modelData
          * mp_modelData, this );
      else
      {
//        mp_cpucontroller = & m_sax2_ipc_current_cpu_data_handler ;
        //For getting the reference clock.
        ::FetchCPUcoreDataFromIPC( this ) ;
        if( m_sax2_ipc_current_cpu_data_handler.
          m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock.size() > 0 )
          m_sax2_ipc_current_cpu_data_handler.m_fReferenceClockInMHz =
            m_sax2_ipc_current_cpu_data_handler.
            m_stdmap_wCoreNumber2VoltageAndMultiAndRefClock.begin()->second.
            m_fReferenceClock ;
      }

      CreateAndShowMainFrame() ;
    //#ifdef _WINDOWS
    //  m_calculationthread.SetCPUcontroller(mp_i_cpucontroller);
    //#endif
//      mp_frame->SetCPUcontroller(mp_cpucontroller) ;
//#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//    if( m_ipcclient.ConnectToDataProvider() )
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
    if( m_model.m_userinterfaceattributes.m_bStartDVFSatStartup )
      StartDynamicVoltageAndFrequencyScaling() ;
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

#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION
void wxX86InfoAndControlApp::PauseService(
  bool bTryToPauseViaServiceControlManagerIfViaIPCfails )
{
  bool bTryToPauseViaServiceControlManager = false ;
  //The connection may have broken after it was established, so check it here.
  if( ! //::wxGetApp().
      //m_ipcclient.IsConnected()
      IPC_ClientIsConnected_Inline()
    )
  {
    LOGN("not connected to the service")
    std::string stdstrMessage ;
    if( ! //::wxGetApp().
        //m_ipcclient.Init()
        IPCclientConnect_Inline(stdstrMessage)
      && bTryToPauseViaServiceControlManagerIfViaIPCfails )
      bTryToPauseViaServiceControlManager = true ;
  }
  if( //::wxGetApp().
      //m_ipcclient.IsConnected()
      IPC_ClientIsConnected_Inline()
    )
  {
    LOGN("OnPauseService--connected to the service")
    //TODO possibly make IPC communication into a separate thread because it
    // may freeze the whole GUI.
    //::wxGetApp().
    //m_ipcclient.SendCommandAndGetResponse(pause_service) ;
    IPC_ClientSendCommandAndGetResponse_Inline(pause_service) ;
//    wxString wxstr = getwxString( //::wxGetApp().
//      m_ipcclient.m_stdwstrMessage ) ;
//    ::wxMessageBox( wxT("message from the service:\n") + wxstr ) ;
    if( m_p_i_ipcclient )
    {
      wxString wxstr = getwxString( m_p_i_ipcclient->m_stdwstrMessage ) ;
      ::wxMessageBox( wxT("message from the service:\n") + wxstr ) ;
    }
  }
  else
    if( bTryToPauseViaServiceControlManagerIfViaIPCfails )
      bTryToPauseViaServiceControlManager = true ;
  if( bTryToPauseViaServiceControlManager )
    try
    {
      std::string stdstrMsg ;
      if( ServiceBase::PauseService( //mp_model->m_strServiceName.c_str()
        //We need a _T() macro (wide char-> L"", char->"") for EACH
        //line to make it compatible between char and wide char.
        //TODO make name variable because the user can change the name when
        //installing the service.
        _T("X86_info_and_control")
        , stdstrMsg
        )
        )
        ::wxMessageBox( wxT("successfully paused the service via the service "
          "control manager")) ;
      else
      {
        wxString wxstr = wxT("not connected to the service\n"
          "->tried to pause via service control manager\n"
          "Error pausing the service via the service control manager:\n")
          + getwxString( stdstrMsg) ;
        ::wxMessageBox( wxstr ) ;
      }
    }
    catch( const ConnectToSCMerror & cr_connecttoscmerror )
    {
      ::wxMessageBox( wxString(
        //We need a _T() macro (wide char-> L"", char->"") for EACH
        //line to make it compatible between char and wide char.
        _T("connecting to service control manager failed: ")
        ) +
//        (const wxChar *) //::LocalLanguageMessageFromErrorCodeA(
        getwxString(
          ::GetErrorMessageFromErrorCodeA( cr_connecttoscmerror.m_dwErrorCode ).
          c_str()
          )
        ) ;
    }
}
#endif //#ifdef COMPILE_WITH_INTER_PROCESS_COMMUNICATION

#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
void wxX86InfoAndControlApp::PossiblyAskForOSdynFreqScalingDisabling()
{
//  PossiblyAskForOSdynFreqScalingDisabling() ;
  if( mp_dynfreqscalingaccess->OtherDVFSisEnabled()
    )
    if( ::wxMessageBox(
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
      _T("The OS's dynamic frequency scaling must be disabled ")
      _T("in order that the p-state isn't changed by the OS afterwards.")
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

//bool wxX86InfoAndControlApp::GetRelativeCPUspecificDirPathAndCurrentWorkDir(
//  )
//{
//
//}

bool wxX86InfoAndControlApp::AbsoluteCPUspecificDirPathExists(
  wxString & wxstrAbsoluteCPUspecificDirPath)
{
  std::string strCPUtypeRelativeDirPath ;
  if( m_maincontroller.GetPstatesDirPath(strCPUtypeRelativeDirPath ) )
  {
    //If the program is executed (->current working dir is elsewhere)
    //in another path than where it is stored then THIS (current working dir)
    //path should be used for storing files.
    wxString wxstrCurrentWorkingDir = ::wxGetCwd() ;
    return ::wxDirExists(wxstrCurrentWorkingDir.c_str() );
  }
  return false;
}

wxString wxX86InfoAndControlApp::GetAbsoluteCPUspecificDirPath(
  const std::string & cr_strCPUtypeRelativeDirPath,
  wxString & r_wxstrCurrentWorkingDir
  )
{
  if( r_wxstrCurrentWorkingDir.empty() )
  {
    r_wxstrCurrentWorkingDir = ::wxGetCwd() ;
  }
  LOGN( "current working dir path:" << GetStdString( r_wxstrCurrentWorkingDir ) )
  LOGN( "for file dialog: relative dir path=" << cr_strCPUtypeRelativeDirPath )
  wxString wxstrAbsoluteCPUspecificDirPath =
    r_wxstrCurrentWorkingDir
    + wxFileName::GetPathSeparator()
    + Getstdtstring( cr_strCPUtypeRelativeDirPath ).c_str()
    ;
  return wxstrAbsoluteCPUspecificDirPath;
}

void wxX86InfoAndControlApp::SaveAsCPUcontrollerDynLibForThisCPU()
{
  std::string stdstrCPUtypeRelativeDirPath ;
  if( m_maincontroller.GetPstatesDirPath(stdstrCPUtypeRelativeDirPath ) )
  {
    wxString wxstrCurrentWorkingDir;
    wxString wxstrAbsoluteCPUspecificDirPath = GetAbsoluteCPUspecificDirPath(
      stdstrCPUtypeRelativeDirPath,
      wxstrCurrentWorkingDir );
    //  //If the program is executed (->current working dir is elsewhere)
    //  //in another path than where it is stored then THIS (current working dir)
    //  //path should be used for storing files.
    //  wxString wxstrCurrentWorkingDir = ::wxGetCwd() ;
    if( ! ::wxDirExists(wxstrAbsoluteCPUspecificDirPath.c_str() ) )
    {
      if( ::wxMessageBox(
        wxT("The service/ daemon and Graphical User Interface expect the "
          "CPU controller configuration/ settings file in the "
          "subdirectory \n\"") + getwxString( stdstrCPUtypeRelativeDirPath )
        + wxT("\"\n relative to its (current) working directory:\n\"")
        + wxstrCurrentWorkingDir +
        wxT("\"\nBut the directory \"") + wxstrAbsoluteCPUspecificDirPath
        + wxT("\" does not exist\n"
          "Should the path be created now?")
        , wxT("Question")
        , wxICON_QUESTION | wxYES_NO
          )
          == wxYES
        )
      {
        if( //::wxMkdir( wxstrFullPstateFileDirPath )
          //from http://groups.google.com/group/wx-users/browse_thread/thread/
          // 9f211a3f06e44233:
            wxFileName::Mkdir(wxstrAbsoluteCPUspecificDirPath,0777,
          //http://docs.wxwidgets.org/stable/wx_wxfilename.html#wxfilenamemkdir:
          //"if the flags contain wxPATH_MKDIR_FULL flag, try to create each
          //directory in the path and also don't return an error if the target
          //directory already exists."
              wxPATH_MKDIR_FULL
              )
          )
        {
          LOGN("Successfully created directory \""
            << GetStdString(wxstrAbsoluteCPUspecificDirPath) << "\"." )
        }
        else
        {
          LOGN("Failed to create directory \""
            << GetStdString(wxstrAbsoluteCPUspecificDirPath) << "\"." )
          ::wxMessageBox(wxT("Failed to create directory " +
            wxstrAbsoluteCPUspecificDirPath
            + wxT("\nYou may/ should create the directory manually."))
            ) ;
        }
      }
    }
    if( ::wxDirExists(wxstrAbsoluteCPUspecificDirPath.c_str() ) )
    {
//      if( //wxGetApp().m_maincontroller.GetPstatesDirPath(
//          m_maincontroller.GetPstatesDirPath(
//          stdstrCPUtypeRelativeDirPath )
//        )

      stdstrCPUtypeRelativeDirPath += "/" ;
      std::string stdstrCPUcontrollerConfigFilePath =
        //wxGetApp().GetCPUcontrollerConfigFilePath(
        GetCPUcontrollerConfigFilePath(
          stdstrCPUtypeRelativeDirPath ) ;
      LOGN("CPU controller dyn lib config file path: " <<
        stdstrCPUcontrollerConfigFilePath )
      std::string stdstrCPUcontrollerDynLibFilePath =
        GetStdString( //wxGetApp().m_wxstrCPUcontrollerDynLibFilePath
          m_wxstrCPUcontrollerDynLibFilePath) ;
      LOGN("CPU controller dyn lib file path: " <<
        stdstrCPUcontrollerDynLibFilePath )
      std::string stdstrFilenameWithoutExtension = GetFilenameWithoutExtension(
        stdstrCPUcontrollerDynLibFilePath ) ;
      LOGN("File name w/out extension: " << stdstrFilenameWithoutExtension )
      if( ! WriteFileContent(
  //      stdstrCPUtypeRelativeDirPath,
          stdstrCPUcontrollerConfigFilePath ,
          stdstrFilenameWithoutExtension
          )
        )
      {
        ::wxMessageBox( wxT("Successfully wrote to file \"") +
          GetwxString_Inline(stdstrCPUcontrollerConfigFilePath.c_str() ) +
          wxT("\"") );
      }
      else
        ::wxMessageBox( wxT("Failed to write to file \"") +
          GetwxString_Inline(stdstrCPUcontrollerConfigFilePath.c_str() ) +
          wxT("\"") );
    }
  }
}

void wxX86InfoAndControlApp::SaveVoltageForFrequencySettings()
{
  std::string strPstateSettingsFileName, stdstrCPUtypeRelativeDirPath ;
  wxString wxstrCurrentWorkingDir;
  wxString wxstrAbsoluteCPUspecificDirPath = GetAbsoluteCPUspecificDirPath(
    stdstrCPUtypeRelativeDirPath,
    wxstrCurrentWorkingDir );
  if( //GetAbsoluteCPUspecificDirPath() &&
      m_maincontroller.GetPstateSettingsFileName(
    strPstateSettingsFileName )
    )
  {
    if( ! //AbsoluteCPUspecificDirPathExists(wxstrAbsoluteCPUspecificDirPath)
        ::wxDirExists(wxstrAbsoluteCPUspecificDirPath.c_str() )
      )
    {
      if( ::wxMessageBox(
        wxT("The service/ daemon and Graphical User Interface expect the "
          "performance state/ \"voltage for frequency\" settings file in the "
          "subdirectory \"") + getwxString( stdstrCPUtypeRelativeDirPath )
          + wxT("\" relative to its (current) working directory.\n"
          "But the directory \"") + wxstrAbsoluteCPUspecificDirPath
        + wxT("\" does not exist\n"
          "Should the path be created now?")
        , wxT("Question")
        , wxICON_QUESTION | wxYES_NO
          )
          == wxYES
        )
      {
        if( //::wxMkdir( wxstrFullPstateFileDirPath )
          //from http://groups.google.com/group/wx-users/browse_thread/thread/
          // 9f211a3f06e44233:
            wxFileName::Mkdir(wxstrAbsoluteCPUspecificDirPath,0777,
          //http://docs.wxwidgets.org/stable/wx_wxfilename.html#wxfilenamemkdir:
          //"if the flags contain wxPATH_MKDIR_FULL flag, try to create each
          //directory in the path and also don't return an error if the target
          //directory already exists."
              wxPATH_MKDIR_FULL
              )
          )
        {
          LOGN("Successfully created directory \""
            << GetStdString(wxstrAbsoluteCPUspecificDirPath) << "\"." )
        }
        else
        {
          LOGN("Failed to create directory \""
            << GetStdString(wxstrAbsoluteCPUspecificDirPath) << "\"." )
          ::wxMessageBox(wxT("Failed to create directory " +
            wxstrAbsoluteCPUspecificDirPath
            + wxT("\nYou may/ should create the directory manually."))
            ) ;
        }
      }
    }
    wxString wxstrFilePath = ::wxFileSelector(
      wxT("Select file path")
      , //wxstrFullPstateFileDirPath
      wxstrAbsoluteCPUspecificDirPath
      , Getstdtstring( strPstateSettingsFileName ) .c_str()
      , wxT("xml")
      , wxT("*.*")
      , wxFD_SAVE
      ) ;
    if ( ! wxstrFilePath.empty() )
    {
        // work with the file
        //...
      //readXMLfileDOM( wxstrFilePath.c_str() ) ;
      //mergeXMLfileDOM(
      //mp_configurationHandler->

      //The check whether the DOM tree differs from program data should be
      //redone because in the meantime between asking "save changes" other
      //file modifications could have been done.
      //TODO uncomment
      m_xerces_voltage_for_frequency_configuration.
        MergeWithExistingConfigFile(
        GetStdString( std::tstring( wxstrFilePath.c_str() ) ).c_str(),
//        * mp_model ,
        m_model ,
        strPstateSettingsFileName ) ;
    }
    //mp_configfileaccess->
  }
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

void wxX86InfoAndControlApp::StartService()
{
  try
  {
    if( ! ServiceBase::StartService( //mp_model->m_strServiceName.c_str()
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
  //    _T("CPUcontrolService"
  //      )
        //mp_wxx86infoandcontrolapp->m_stdtstrProgramName.c_str()
        m_stdtstrProgramName.c_str()
        )
      )
      ::wxMessageBox( wxT("starting the service succeeded") ) ;
    else
      ::wxMessageBox( wxT("error starting the service:") +
        getwxString( //::LocalLanguageMessageAndErrorCodeA( ::GetLastError() )
          ::GetErrorMessageFromLastErrorCodeA()
           )
        ) ;
  }
  catch( const ConnectToSCMerror & cr_connecttoscmerror )
  {
    ::wxMessageBox( wxT("error connecting to the service control manager") ) ;
  }
}

void wxX86InfoAndControlApp::StopService()
{
  try
  {
    if( ! ServiceBase::StopService( //mp_model->m_strServiceName.c_str()
      //We need a _T() macro (wide char-> L"", char->"") for EACH
      //line to make it compatible between char and wide char.
  //    _T("CPUcontrolService"
  //      )
//        mp_wxx86infoandcontrolapp->m_stdtstrProgramName.c_str()
        m_stdtstrProgramName.c_str()
        )
      )
      ::wxMessageBox( wxT("stopping the service succeeded") ) ;
    else
      ::wxMessageBox( wxT("error stopping the service:") +
        getwxString( ::GetErrorMessageFromLastErrorCodeA()
          //::LocalLanguageMessageAndErrorCodeA( ::GetLastError() )
           )
        ) ;
  }
  catch( const ConnectToSCMerror & cr_connecttoscmerror )
  {
    ::wxMessageBox( wxT("error connecting to the service control manager") ) ;
  }
}
