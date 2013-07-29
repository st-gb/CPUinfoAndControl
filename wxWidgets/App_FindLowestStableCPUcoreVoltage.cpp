/*
 * App_FindLowestStableCPUcoreVoltage.cpp
 *
 *  Created on: 03.10.2011
 *      Author: Stefan
 */

#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
#include <wx/dynlib.h> //class wxDynamicLibrary::GetDllExt()
#include <wx/event.h>//void wxQueueEvent(wxEvtHandler * dest,wxEvent * event)
#include <wx/filename.h> //wxFileName::GetPathSeparator(...)

#include "App.hpp" //START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME

//GetArrayIndexForClosestGreaterOrEqual(...)
//#include <algorithms/binary_search/binary_search.h>
#include <algorithms/binary_search/binary_search.hpp>
//::GetErrorMessageFromLastErrorCodeA();
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
//unsigned long int SetThreadAffinityMask(DWORD dwThreadAffinityMask)
#include <Controller/SetThreadAffinityMask.h>
//THREAD_PROC_CALLING_CONVENTION
#include <preprocessor_macros/thread_proc_calling_convention.h>

#ifdef wxUSE_BITMAPTOGGLEBUTTUN
  #include <wx/tglbtn.h> //class wxBitmapToggleButton
#else
  #include <wxWidgets/UserInterface/wx2.9compatibility/wxBitmapToggleButton.hpp>
#endif
//class FreqAndVoltageSettingDlg
#include <wxWidgets/UserInterface/FreqAndVoltageSettingDlg.hpp>
//class wxTextControlDialog
#include <wxWidgets/UserInterface/wxTextControlDialog.hpp>
//::getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <wxWidgets/UserInterface/MainFrame.hpp> //class MainFrame

void SetThreadAffinityMask()
{
#ifdef _WIN32
  DWORD dwProcessAffinityMask;
  DWORD dwSystemAffinityMask;
  DWORD dwThreadAffinityMask = 1;
  //http://msdn.microsoft.com/en-us/library/windows/desktop/ms683213%28v=vs.85%29.aspx:
  //"A process affinity mask is a bit vector in which each bit represents the
  //processors that a process is allowed to run on. A system affinity mask is
  //a bit vector in which each bit represents the processors that are
  //configured into a system."
  ::GetProcessAffinityMask(
    ::GetCurrentThread(),
    & dwProcessAffinityMask //_out  PDWORD_PTR lpProcessAffinityMask
    , & dwSystemAffinityMask // __out  PDWORD_PTR lpSystemAffinityMask
    );
#endif
  for( BYTE byCPUcoreIndex = 0; byCPUcoreIndex < sizeof(DWORD_PTR) * 8 ;
      ++ byCPUcoreIndex)
  {
    //This process may run on core "byCPUcoreIndex".
    if( ( (dwProcessAffinityMask >> byCPUcoreIndex) & 1) == 1)
    {
      dwThreadAffinityMask = (1 << byCPUcoreIndex);
      break;
    }
  }
  //Must "pin" this thread to a specific CPU core. Else it usally is being
  //executed on different CPU cores and the CPU load does not reach 100%.
  //TODO: if this _process_ does not have affinity to core 0 (e.g. when changed
  //in task manager), the call fails?!
  ::SetThreadAffinityMask(//1
      dwThreadAffinityMask);
}

inline void updateUIforCountSecondsDown(
  wxCommandEvent & wxcommand_eventCountSecondsDown,
  unsigned uiSeconds,
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg)
{
  //user interface control updates should/ must be executed in _GUI_ thread
  //    //Adapted from http://docs.wxwidgets.org/2.9.4/classwx_thread_helper.html
  //    wxQueueEvent(p_freqandvoltagesettingdlg,
  //      new wxThreadEvent(wxEVT_COMMAND_MYTHREAD_UPDATE));

  wxcommand_eventCountSecondsDown.SetInt(uiSeconds - 1);
//  wxcommand_eventCountSecondsDown.SetClientData();
  wxPostEvent(p_freqandvoltagesettingdlg, wxcommand_eventCountSecondsDown);
  //    //http://docs.wxwidgets.org/2.8/wx_wxthreadoverview.html:
  //    wxEvtHandler::AddPendingEvent();
  //    p_freqandvoltagesettingdlg->wxEvtHandler::QueueEvent  (   wxEvent *   event )
}

BYTE GetNumberOfSelectedCPUcores(uint32_t ui32CPUcoreMask)
{
  BYTE byNumberOfSelectedCPUcores = 0;
  for(BYTE byCPUcoreIndex = 0; byCPUcoreIndex < 32; ++ byCPUcoreIndex)
    byNumberOfSelectedCPUcores += ( (ui32CPUcoreMask >> byCPUcoreIndex) & 1);
  return byNumberOfSelectedCPUcores;
}

void wxX86InfoAndControlApp::InitUnstableCPUcoreOperationDetection()
{
  PossiblyAskForOSdynFreqScalingDisabling();
  InitUnstableVoltageDetectionDynLibAccess();
}

BYTE wxX86InfoAndControlApp::InitUnstableVoltageDetectionDynLibAccess()
{
  LOGN( FULL_FUNC_NAME << " --begin")
//  LPTSTR lptstrUnstableVoltageDetectionDynLib = _T(//"Prime95.DLL"
//    //"UnstableVoltageDetection.dll"
//      "InstableCPUcoreOperationDetection.dll"
//      );
//  MessageWithTimeStamp();
  wxTextControlDialog wxtextcontroldialog( wxNow() +
    wxT(" loading the dynamic library ") +
    m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath,
    wxT("current action"), 0, 0);
  //Loading may take some seconds depending on e.g. DLL size and CPU speed so
  //inform the user by via user interface.
  wxtextcontroldialog.Show(true);

  m_model.m_instablecpucorevoltagedetection.LoadDynLib();

  wxtextcontroldialog.Show(false);
  std::string std_strUnstableVoltageDetectionDynLib = GetStdString_Inline(
//    GetStdTstring_Inline(lptstrUnstableVoltageDetectionDynLib)
//    m_std_wstrInstableCPUcoreVoltageDynLibPath
    m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath
    );
  if( m_model.m_instablecpucorevoltagedetection.DynLibSuccessFullyLoaded() )
  {
    LOGN(FULL_FUNC_NAME << "--Successfully loaded \"" <<
      std_strUnstableVoltageDetectionDynLib << "\"")

    if( m_model.m_instablecpucorevoltagedetection.AssignStartFunctionPointer() )
    {
      if( m_model.m_instablecpucorevoltagedetection.AssignStopFunctionPointer() )
      {
  //      wxString wxstrDynLibFilePath = ::getwxString(
  ////        m_std_wstrInstableCPUcoreVoltageDynLibPath
  //        m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath );
        //TODO should only be called in GUI thread
  //      mp_frame->m_p_wxmenuitemUnloadDetectInstableCPUcoreVoltageDynLib->
  //        SetHelp(wxstrDynLibFilePath);
        return 0;
      }
      else
      {
        std::wstring std_wstrMessage = L"assigning function \"";
        std_wstrMessage +=
            _T(STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME);
        std_wstrMessage += L"\" failed.";
        MessageWithTimeStamp( std_wstrMessage);
      }
    }
    else
    {
      std::wstring std_wstrMessage = L"assigning function \"";
      std_wstrMessage +=
          _T(START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME);
      std_wstrMessage += L"\" failed.";
      MessageWithTimeStamp( std_wstrMessage);
    }
  }
  else
  {
    LOGN(FULL_FUNC_NAME << "--Failed to load \"" //"Prime95 DLL""
      << std_strUnstableVoltageDetectionDynLib
      << "\"" )
    std::wstring std_wstrMessage = L"Loading unstable voltage detection "
      "dynamic library \"" ;
    std_wstrMessage += //lptstrUnstableVoltageDetectionDynLib;
//      m_std_wstrInstableCPUcoreVoltageDynLibPath;
      m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath;
    std_wstrMessage += L"\" failed: ";
    std::string std_strErrorMessageFromLastErrorCode =
      ::GetErrorMessageFromLastErrorCodeA();
    std_wstrMessage += GetStdWstring(
      std_strErrorMessageFromLastErrorCode);
    MessageWithTimeStamp( std_wstrMessage);
  }
  return 1;
}

BYTE wxX86InfoAndControlApp::StartInstableCPUcoreVoltageDetection(
    const FreqAndVoltageSettingDlg * c_p_freqandvoltagesettingdlg)
{
  LOGN( FULL_FUNC_NAME << " begin")
  BYTE ret = 1;
  //wxGetApp().
//    InitUnstableVoltageDetectionDynLibAccess();

  InstableCPUcoreOperationDetection & instablecpucorevoltagedetection =
    m_model.
    m_instablecpucorevoltagedetection;
  instablecpucorevoltagedetection.m_ui32CPUcoreMask =
    c_p_freqandvoltagesettingdlg->GetCPUcoreMask();
  instablecpucorevoltagedetection.Start();

  return ret;
}

void wxX86InfoAndControlApp::SetStartFindingLowestStableVoltageButton()
{
  m_p_freqandvoltagesettingdlgInstCPUcoreDetect->
    SetStartFindingLowestStableVoltageButton();
}

void wxX86InfoAndControlApp::StopInstableCPUcoreVoltageDetection()
{
  m_model.m_instablecpucorevoltagedetection.Stop();
}

void wxX86InfoAndControlApp::UnloadDetectInstableCPUcoreVoltageDynLib()
{
  LOGN( FULL_FUNC_NAME << "--begin")
#ifdef _WIN32
  m_model.m_instablecpucorevoltagedetection.UnloadDynLib();
#endif
}

void wxX86InfoAndControlApp::UpdateInstableCPUcoreOpDetectInfo()
{
  //    //http://wiki.wxwidgets.org/Custom_Events:
  //    wxEvent wxcommand_eventCountSecondsDown(uiSeconds - 1);
      wxCommandEvent wxcommand_eventCountSecondsDown(
  //      MyExcitingEvent, //wxEventType commandType = wxEVT_NULL,
  //      wxEVT_NULL,
        wxEVT_COMMAND_COUNT_SECONDS_DOWN_UPDATE
  //      uiSeconds - 1 //int winid = 0
        );

//    wxcommand_eventCountSecondsDown.SetClientData(& p_freqandvoltagesettingdlg->
//      m_instablecpuoperationdetectiondata);

//    wxcommand_eventCountSecondsDown.SetInt(uiSeconds - 1);

  //Create a new object to have a copy that is safe to cause no problems in
  //the GUI thread.
  //Its memory should be freed in the message procedure.
  InstableCPUoperationDetectionData * p_InstableCPUoperationDetectionData
    = new InstableCPUoperationDetectionData(
      m_model.m_instablecpucorevoltagedetection);
  wxcommand_eventCountSecondsDown.SetClientData(
    p_InstableCPUoperationDetectionData);
  wxPostEvent(
    m_p_freqandvoltagesettingdlgInstCPUcoreDetect,
    wxcommand_eventCountSecondsDown);
}

void wxX86InfoAndControlApp::LoadDetectInstableCPUcoreVoltageDynLib(
  //wxCommandEvent & event
  )
{
  wxString wxstrDynLibExtension = wxDynamicLibrary::GetDllExt();
  wxString wxstrDynLibFilePath = ::wxFileSelector(
    wxT("Select dynamic library for detecting an unstable CPU core operation"
        //"voltage"
      )
    //"default_path"
    , //wxEmptyString
      m_wxstrDirectoryForLastSelectedInstableCPUcoreVoltageDynLib
    , wxEmptyString
    , wxDynamicLibrary::GetDllExt()
    , wxT("*") + wxstrDynLibExtension
    , wxFD_OPEN
    ) ;
  if ( ! wxstrDynLibFilePath.empty() )
  {
    m_wxstrDirectoryForLastSelectedInstableCPUcoreVoltageDynLib =
      wxstrDynLibFilePath.Left(
        //0-based index of path separator = # of chars before it.
        wxstrDynLibFilePath.rfind(
        wxFileName::GetPathSeparator() )
        );
    //Unload attached dyn lib at first.
    UnloadDetectInstableCPUcoreVoltageDynLib();
//    std::wstring std_wstrInstableCPUcoreVoltageDynLibPath = ::GetStdWstring(
//      wxstrDynLibFilePath);
    //m_std_wstrInstableCPUcoreVoltageDynLibPath =
      m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath =
      ::GetStdWstring(wxstrDynLibFilePath);

    if( //LoadDetectInstableCPUcoreVoltageDynLib(
      //std_wstrInstableCPUcoreVoltageDynLibPath);
      InitUnstableVoltageDetectionDynLibAccess() == 0)
//      m_p_wxmenuitemUnloadDetectInstableCPUcoreVoltageDynLib->SetHelp(
//        wxstrDynLibFilePath);
    { //By using a block/ braces: avoid g++ warning "Suspicious semicolon"
      ;
    }
  }
}
