/*
 * InstableCPUcoreOperationDetection.cpp
 *
 *  Created on: 19.06.2013
 *      Author: Stefan
 */

#include "InstableCPUcoreOperationDetection.hpp"
#include <algorithms/binary_search/binary_search.hpp>
#include <ModelData/CPUcoreData.hpp> //class CPUcoreData
//#include <winstl/performance/threadtimes_counter.hpp>
#include <preprocessor_macros/check_arithmetic_overflow.h>
#include <wxWidgets/App.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp>

InstableCPUcoreOperationDetection * InstableCPUcoreOperationDetection::
  s_p_instableCPUcoreOperationDetection = NULL;

using namespace MANUFACTURER_ID_NAMESPACE;

InstableCPUcoreOperationDetection::InstableCPUcoreOperationDetection
  (/*UserInterface * p_ui*/
  CPUcoreData & r_cpucoredata) :
  m_bVoltageWasTooLowCalled(false),
  m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI(false),
  m_vbExitFindLowestStableVoltage(false),
  m_r_cpucoredata(r_cpucoredata),
  m_p_cpucontroller(r_cpucoredata.mp_cpucontroller),
//    m_secondsUntilVoltageDecrease(150),
  m_milliSecondsToWaitBeforeSecondsCountDown(1000),
#ifdef _WIN32
  m_hmoduleUnstableVoltageDetectionDynLib(NULL),
#endif //#ifdef _WIN32
  m_p_userinterface( & ::wxGetApp() ),
  m_pfnStartInstableCPUcoreVoltageDetection(NULL),
  m_pfnStopInstableCPUcoreVoltageDetection(NULL),
  m_x86iandc_threadFindLowestStableVoltage(I_Thread::detached),
  m_uiNumberOfSecondsToWaitUntilVoltageIsReduced(100),
  m_std_wstrDynLibPath( L"InstableCPUcoreVoltageDetection.dll"),
  m_fMinCPUcoreUsage(0.9f)//,
  //m_p_userinterface(p_ui)
{
  s_p_instableCPUcoreOperationDetection = this;
}

/**Should be a static/ global function because so no parameter values need to
 * be passed to it when called by the dynamic library. (instance functions (
 * "methods" always get implicitely passed an instance parameter)
 * */
void InstableCPUcoreOperationDetection::HandleVoltageTooLow()
{
  LOGN( FULL_FUNC_NAME << "--called by Dyn lib")
//  wxGetApp().mp_cpucontroller->GetCurrentVoltageAndFrequency(
//    fVoltageInVolt,
//    fMultiplier, fReferenceClockInMHz, 0);

  //Important: else unstable voltage can not be detected after Prime95 torture
  //test ended.
  ExitFindLowestStableVoltageThread();

  //TODO change InstableCPUcoreOperationDetection to a static member variable?
//  InstableCPUcoreOperationDetection & r_instablecpucorevoltagedetection =
//    wxGetApp().m_model.m_instablecpucorevoltagedetection;
  float fVoltageInVolt = m_lastSetCPUcoreVoltageInVolt;
  float fMultiplier = m_lastSetCPUcoreMultiplier;
  float fReferenceClockInMHz = m_fReferenceClockInMHz;
  /** Show user the voltage for instable CPU core operation. */
  m_p_userinterface->StabilizeVoltageAndShowVoltageSettingsChanges(
    fVoltageInVolt,
    fMultiplier,
    fReferenceClockInMHz
    );
  //This flag is important for to know why the find instable CPU core voltage
  //loop ended.
  m_bVoltageWasTooLowCalled = true;
//  if( ! wxGetApp().m_bAutoConfigureVoltage)
//  {
//  wxString wxstrMessage = wxString::Format(
//    wxT("Highest unstable voltage: %f Volt found for "
//        "multiplier %f"), fVoltageInVolt, fMultiplier);
//  ::wxMessageBox( wxstrMessage )
  //TODO exec in GUI thread
//  wxGetApp().MessageWithTimeStamp( GetStdTstring_Inline(wxstrMessage) );
  LOGN( FULL_FUNC_NAME << " waking up threads that wait for "
    "\"VoltageTooLow()\" to be finished")
  LOGN( FULL_FUNC_NAME << "--signalling the condition to end finding the"
      " lowest stable voltage thread")
  //Wake up all threads (the auto-configure voltages thread) waiting on the
  //condition.
  m_conditionFindLowestStableVoltage.Broadcast();
//  LOGN( FULL_FUNC_NAME << " waiting up the for \"VoltageTooLow()\" to be finished")
//  //Wake up all threads (the auto-configure voltages thread) waiting on the
//  //condition.
//  wxGetApp().m_conditionFindLowestStableVoltage.Wait();
  LOGN( FULL_FUNC_NAME << "--end")
}

void InstableCPUcoreOperationDetection::StartInDynLib(/*uint16_t m_ui32CPUcoreMask*/)
{
  LOGN( FULL_FUNC_NAME << "--before calling the DynLib's \""
    << START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME "\" function")
  (* m_pfnStartInstableCPUcoreVoltageDetection)(
    //p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.m_byNumberOfCPUCores,
    m_ui32CPUcoreMask,
    & m_external_caller);
  LOGN( FULL_FUNC_NAME << "--after calling the DynLib's \""
    << START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\" function")
}

void InstableCPUcoreOperationDetection::Stop()
{
  LOGN( FULL_FUNC_NAME << "--begin")
//    InstableCPUcoreOperationDetection & instablecpucorevoltagedetection =
//      m_model.m_instablecpucorevoltagedetection;
  if( DynLibAccessInitialized() )
  {
    if( m_pfnStopInstableCPUcoreVoltageDetection)
    {
      if( //m_p_wxbuttonFindLowestStableVoltage->GetLabel() ==
          //wxT("stop finding the lowest stable voltage")
          m_x86iandc_threadFindLowestStableVoltage.IsRunning()
       )
     {
      LOGN( FULL_FUNC_NAME << "--should stop the \"find lowest stable voltage\" thread")

      LOGN( FULL_FUNC_NAME << "--before calling dyn lib's \""
          << STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\" function" )
      (* m_pfnStopInstableCPUcoreVoltageDetection)();
      LOGN( FULL_FUNC_NAME << "--after calling \""
        << STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\"" )

      ExitFindLowestStableVoltageThread();
    }
    else
      ShowMessage( L"Can't stop because: finding the lowest stable "
        "voltage has NOT been started." );
    }
    else
      ShowMessage( L"\"stop\" function pointers for "
          "volt detect DLL access not valid." );
  }
  else
    ShowMessage( L"unstable volt detect DLL access not initialized" );
}

/** Must be static as thread proc. */
DWORD THREAD_PROC_CALLING_CONVENTION
  InstableCPUcoreOperationDetection::StartInDynLib_ThreadProc(void * p_v )
{
  LOGN( FULL_FUNC_NAME << " begin")
//  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg =
//    (FreqAndVoltageSettingDlg *) p_v;
  //if( p_freqandvoltagesettingdlg)
  {
//    uint32_t m_ui32CPUcoreMask = p_freqandvoltagesettingdlg->GetCPUcoreMask();

    LOGN( FULL_FUNC_NAME << "--before setting thread affinity mask"
        //" to CPU core 0"
        )

    //GetThreadAffinityMask();
    LOGN( FULL_FUNC_NAME << "--after setting thread affinity mask"
        //" to CPU core 0"
        )
    s_p_instableCPUcoreOperationDetection->StartInDynLib(
      /*s_p_instableCPUcoreOperationDetection->m_ui32CPUcoreMask*/);

    LOGN( FULL_FUNC_NAME << " return 0")
    return 0;
  }
  LOGN( FULL_FUNC_NAME << " return 1")
  return 1;
}

void InstableCPUcoreOperationDetection::ShowMessage(const wchar_t * const msg)
{
  //m_p_userinterface->MessageWithTimeStamp(msg);
  m_p_userinterface->ShowMessageGUIsafe(msg);
}

BYTE InstableCPUcoreOperationDetection::Start()
{
  BYTE ret = 0;
  if( DynLibAccessInitialized() )
  {
      if( //m_p_wxbuttonFindLowestStableVoltage->GetLabel() ==
         //wxT("stop finding the lowest stable voltage")
           m_x86iandc_threadFindLowestStableVoltage.IsRunning()
         )
      {
        LOGN( "Finding the lowest stable voltage has already "
          "been started.")
        ShowMessage(L"Finding the lowest stable voltage has already "
           "been started." );
        return 1;
      }
      else
      {
        LOGN( FULL_FUNC_NAME << "--should start the find lowest stable "
            "voltage thread")
      //wxGetApp().
        m_vbExitFindLowestStableVoltage = false;

        //Start finding voltage in another thread. Else it couldn't be stopped
        //when it was started in the GUI thread.
      ret =
      //wxGetApp().
        m_x86iandc_threadFindLowestStableVoltage.start(
          FindLowestStableVoltage_ThreadProc ,
          //(void *) c_p_freqandvoltagesettingdlg,
          NULL,
          "FindVoltForLowestStabCPUcoreOp") ;
      } //else
      if( ret == I_Thread::no_error )
      {
        ret = 0;
      }
  //    }
  //    else
  //    {
  //      ::wxMessageBox( wxT("no start and/or stop function pointers for "
  //          "volt detect DLL access") );
  //    }
  }
  else
  {
    ShowMessage( L"unstable volt detect DLL access not initialized" );
  }
  LOGN( FULL_FUNC_NAME << " return " << (WORD) ret)
  return ret;
}

void InstableCPUcoreOperationDetection::CountSecondsDown(
  /*FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg,*/
  //DWORD dwMilliSecondsToWait//,
  //const x86IandC::thread_type & x86iandc_threadFindLowestStableVoltageInDLL
  )
{
  LOGN( FULL_FUNC_NAME << " begin")

#ifdef _WIN32
  LOGN_DEBUG( FULL_FUNC_NAME << "--thread handle of dyn libs \"find instable"
    "CPU core operation thread: "
    << m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.GetThreadHandle() )
  ULONG64 ul64ThreadTimeBefore = GetThreadUserModeStartTime(
    /*x86iandc_threadFindLowestStableVoltageInDLL.GetThreadHandle()*/ );
#endif

  const float fMinCPUcoreUsage = m_fMinCPUcoreUsage;
  for( /* unsigned uiSeconds */ m_secondsUntilCPUcoreVoltageDecrease =
        m_uiNumberOfSecondsToWaitUntilVoltageIsReduced //+ 1
      ; m_secondsUntilCPUcoreVoltageDecrease != 0 &&
      //Is set to true when "VoltageTooLow" was called.
      ! m_vbExitFindLowestStableVoltage;// -- uiSeconds
     )
  {
    LOGN( FULL_FUNC_NAME << "--waiting max. "
      << m_milliSecondsToWaitBeforeSecondsCountDown
      << " milliseconds")
    if( m_conditionFindLowestStableVoltage.WaitTimeOut(
        m_milliSecondsToWaitBeforeSecondsCountDown) == I_Condition::signaled
        )
      //If it got signaled (-> finding lowest stable voltage should be canceled).
    {
      LOGN_DEBUG( FULL_FUNC_NAME << "--condition got signalled->probably cancelled" )
//      break;
      m_secondsUntilCPUcoreVoltageDecrease = 0;
      m_conditionFindLowestStableVoltage.ResetEvent();
    }
    else
      LOGN( FULL_FUNC_NAME << "--timed out" )
    LOGN_DEBUG( FULL_FUNC_NAME << "--after waiting for either thread's end or "
        "timeout ")
    if( m_secondsUntilCPUcoreVoltageDecrease ) // "VoltageTooLow" not called
    {
#ifdef _WIN32
      //platformstl::threadtimes_counter ttcounter;
      m_fCPUcoreUsageOfDynLibThread = GetCPUcoreUsageForDynLibThread();
//      LOGN( FULL_FUNC_NAME << " CPU core usage for instable CPU core operation "
//          "detection DLL thread:" << fCPUcoreUsage * 100.0f )
      //seconds = uiSeconds;
//      p_freqandvoltagesettingdlg->m_instablecpuoperationdetectiondata.
//        CPUusageInPercent = fCPUcoreUsage;

//      updateUIforCountSecondsDown(wxcommand_eventCountSecondsDown,
//        uiSeconds,
//        p_freqandvoltagesettingdlg);

      m_p_userinterface->UpdateInstableCPUcoreOpDetectInfo(/*this*/);

      if( m_fCPUcoreUsageOfDynLibThread < fMinCPUcoreUsage )
      {
//        LOGN( FULL_FUNC_NAME << "-- < " << fMinCPUcoreUsage * 100.0f << " "
//          << "% user time for unstable CPU core operation detection procedure")
      }
      else
      {
        //-- uiSeconds;
        -- m_secondsUntilCPUcoreVoltageDecrease;
      }
#endif
    }
    //else
    if( m_bVoltageWasTooLowCalled )
    {
      LOGN_DEBUG( FULL_FUNC_NAME << " waiting for \"VoltageTooLow()\" to be finished")
      //Wait for "::VoltageTooLow()" to be ended (after messagebox was shown).
      m_conditionFindLowestStableVoltage.Wait();
      LOGN_DEBUG( FULL_FUNC_NAME << " after waiting for \"VoltageTooLow()\" to be finished")
    }
  }
  LOGN( FULL_FUNC_NAME << " end")
}

float InstableCPUcoreOperationDetection::DecreaseVoltageStepByStep(
  //FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg,
  unsigned wCPUcoreVoltageArrayIndex,
  const float fCPUcoreMultiplierToSet//,
  //const x86IandC::thread_type & x86iandc_threadFindLowestStableVoltageInDLL
  )
{
  float fVoltageInVoltToSet = //0.0f;
    m_r_cpucoredata.m_arfAvailableVoltagesInVolt[wCPUcoreVoltageArrayIndex];
  LOGN( FULL_FUNC_NAME << "wCPUcoreVoltageArrayIndex:"
    << wCPUcoreVoltageArrayIndex << " (=" << fVoltageInVoltToSet << "V),"
    << "CPU core multiplier:" << fCPUcoreMultiplierToSet << " begin")
//  DWORD dwMilliSecondsToWait = 1000;
  float currentVoltageInVolt, currentMultiplier, currentReferenceClockInMHz;

  for( //-- wCPUcoreVoltageArrayIndex
    ; //wCPUcoreVoltageArrayIndex != MAXWORD &&
      NO_NEGATIVE_OVERFLOW(wCPUcoreVoltageArrayIndex, MAXWORD) &&
    //Is set to true when "VoltageTooLow" was called.
    ! m_vbExitFindLowestStableVoltage;
    -- wCPUcoreVoltageArrayIndex )
  {
    LOGN(FULL_FUNC_NAME << " index:" << wCPUcoreVoltageArrayIndex )
    fVoltageInVoltToSet =
      m_r_cpucoredata.m_arfAvailableVoltagesInVolt[wCPUcoreVoltageArrayIndex];
    m_lastSetCPUcoreVoltageInVolt = fVoltageInVoltToSet;

    //The reference clock can be changed at runtime via special tools like
    //"SetFSB" etc., so get it every time.
    const float r_fReferenceClockInMHz =
      //p_freqandvoltagesettingdlg->mp_cpucontroller->
      m_p_cpucontroller->m_fReferenceClockInMHz;

    if( m_p_userinterface->PreventVoltageBelowLowestStableVoltage()
        //wxGetApp()::PreventVoltageBelowLowestStableVoltage()
        &&
        //p_freqandvoltagesettingdlg->mp_cpucontroller->
        m_p_cpucontroller->
        //CheckWhetherVoltageIsBelowLowestStableVoltage(
        CheckWhetherCPUcoreVoltageIsBelowHighestInstableVoltage(
          fVoltageInVoltToSet,
          r_fReferenceClockInMHz * fCPUcoreMultiplierToSet) ==
          I_CPUcontroller::VoltageIsOutsideSafeRange
      )
    {
      LOGN( FULL_FUNC_NAME << "--voltage " << fVoltageInVoltToSet
        << " is below "
          //"lowest stable "
          "highest instable "
          "CPU core voltage "
          " for CPU core multiplier " << fCPUcoreMultiplierToSet)
      m_p_userinterface->StabilizeVoltageAndShowVoltageSettingsChanges(
        fVoltageInVoltToSet,
        fCPUcoreMultiplierToSet,
        r_fReferenceClockInMHz);
      Stop();
      //In order to set the multi as lowest multi for instable CPU operation in
      // AutoConfigureVoltageSettings_ThreadFunc(...) function.
      m_bVoltageWasTooLowCalled = true;
      break;
    }
    LOGN( FULL_FUNC_NAME << "--before setting voltage[Volt] to:"
        << fVoltageInVoltToSet << " , multi to " << fCPUcoreMultiplierToSet)
    m_p_cpucontroller->SetCurrentVoltageAndMultiplier(
        fVoltageInVoltToSet, //fMultiplier
        fCPUcoreMultiplierToSet,
        //TODO set for more than 1 CPU core
        0);
    m_p_cpucontroller->GetCurrentVoltageAndFrequency(
        currentVoltageInVolt, //fMultiplier
        currentMultiplier,
        currentReferenceClockInMHz, 0);
    if( fVoltageInVoltToSet != currentVoltageInVolt ||
        currentMultiplier != fCPUcoreMultiplierToSet )
    {
      LOGN(FULL_FUNC_NAME << " setting p-state failed->aborting")
//      wxString wxstr = wxString::Format(
//        wxT("for instable CPU core operation detection:"
//        "setting voltage to %f and/or multiplier to %f failed"
//        "->aborting instable CPU core operation detection"),
//        fVoltageInVoltToSet, fCPUcoreMultiplierToSet);
//      wxGetApp().ShowMessageGUIsafe(wxstr);
      //wxString::Format()
      wchar_t ar_wchMessage[500];
      wsprintf(ar_wchMessage, L"for instable CPU core operation detection:"
        L"setting voltage to %f and/or multiplier to %f failed"
        L"->aborting instable CPU core operation detection",
        fVoltageInVoltToSet, fCPUcoreMultiplierToSet);

      ShowMessage(ar_wchMessage);
      //break;
      return 0.0f;
    }

    CountSecondsDown(//p_freqandvoltagesettingdlg, dwMilliSecondsToWait,
      /*x86iandc_threadFindLowestStableVoltageInDLL*/);
  }
  LOGN( FULL_FUNC_NAME << " return " << fVoltageInVoltToSet)
  return fVoltageInVoltToSet;
}

DWORD InstableCPUcoreOperationDetection::FindLowestStableVoltage()
{
  {
//    const x86IandC::thread_type & x86iandc_threadFindLowestStableVoltageInDLL =
//    uint32_t m_ui32CPUcoreMask = p_freqandvoltagesettingdlg->GetCPUcoreMask();
//    StartInstableCPUcoreVoltageDetectionInDynLibInSeparateThread(
//      p_freqandvoltagesettingdlg);
    StartInDynLibInSeparateThread();

//    float fVoltageInVolt = p_freqandvoltagesettingdlg->
//      GetVoltageInVoltFromSliderValue() ;

//    wxString wxstrMessageFromService;
    //TODO exec in GUI thread.
//    p_freqandvoltagesettingdlg->DisableOSesDVFSandServiceDVFS(
//      wxstrMessageFromService);

    const std::set<float> & c_r_stdset_floatAvailableVoltagesInVolt =
      m_r_cpucoredata.m_stdset_floatAvailableVoltagesInVolt;
    const unsigned voltageArraySize = c_r_stdset_floatAvailableVoltagesInVolt.size();

    BinarySearch::arrayIndexType wCPUcoreVoltageArrayIndex = BinarySearch::
      GetArrayIndexForClosestGreaterOrEqual(
      m_r_cpucoredata.m_arfAvailableVoltagesInVolt,
      voltageArraySize,
//      fVoltageInVolt
      m_lastSetCPUcoreVoltageInVolt
      );
    const float fMultiplier = m_lastSetCPUcoreMultiplier;

//    p_freqandvoltagesettingdlg->m_p_wxbuttonFindLowestStableVoltage->SetLabel(
//      wxT("stop finding the lowest stable voltage") );

//    p_freqandvoltagesettingdlg->
//      ChangeToStopFindingLowestStableCPUcoreVoltageButton();

//    //Multipliers can also be floats: e.g. "5.5" for AMD Griffin.
//    const float fMultiplierFromSliderValue = p_freqandvoltagesettingdlg->
//      GetMultiplierFromSliderValue();
//    p_freqandvoltagesettingdlg->mp_model->m_instablecpucorevoltagedetection.
//      m_lastSetCPUcoreMultiplier = fMultiplierFromSliderValue;

      m_fReferenceClockInMHz = m_p_cpucontroller->
      m_fReferenceClockInMHz;

    const float fLastSetVoltageInVolt =
      DecreaseVoltageStepByStep(
      //p_freqandvoltagesettingdlg,
      wCPUcoreVoltageArrayIndex, fMultiplier//,
      /*x86iandc_threadFindLowestStableVoltageInDLL*/);

//    //When the min. reached voltage and no unstable voltage was detected it may
//    // be unstable though: the voltage where calculation errors may occur may
//    //be very close to the minimal voltage: e.g. for a Pentium M 1.8 GHz
//    //when reached the min voltage of 0.7 V at 600 MHz: no calculation error is
//    //detected this minmal voltage of 0.7V is close to the voltage (ca. 0.68V)
//    //where a calculation error is likely to happen.
//    if( ! wxGetApp().m_bVoltageWasTooLowCalled )
//    {
////      float fVoltageInVolt;
////      float fMultiplier;
////      float fReferenceClockInMHz;
////      wxGetApp().mp_cpucontroller->GetCurrentVoltageAndFrequency(
////        fVoltageInVolt,
////        fMultiplier, fReferenceClockInMHz, 0);
//
//      LOGN(FULL_FUNC_NAME << " stabilizing voltage because we reached the "
//          "lowest voltage and may be close to an unstable voltage")
//      wxGetApp().StabilizeVoltage(
////        fVoltageInVolt,
//        fLastSetVoltageInVolt,
////        fMultiplier,
//        fMultiplier,
////        fReferenceClockInMHz
//        p_freqandvoltagesettingdlg->mp_model->m_instablecpucorevoltagedetection.
//        m_fReferenceClockInMHz
//        );
//    }
//    else
//      wxGetApp().m_bVoltageWasTooLowCalled = false;

    m_p_userinterface->SetStartFindingLowestStableVoltageButton();
//    wxGetApp().ExitFindLowestStableVoltageThread();
    //wxGetApp().StopInstableCPUcoreVoltageDetection();
    Stop();

    if(fLastSetVoltageInVolt == 0.0f )
      return settingVoltageFailed;
//    p_freqandvoltagesettingdlg->m_p_wxbuttonFindLowestStableVoltage->SetLabel(
//      wxT("find lowest stable voltage") );

//    wxBitmap wxbitmapFindLowestStableCPUcoreVoltage = wxBitmap(
//      find_lowest_stable_CPU_core_voltage16x16_xpm ) ;
//    p_freqandvoltagesettingdlg->
//      m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetImageLabel(
//        wxbitmapFindLowestStableCPUcoreVoltage);
//    p_freqandvoltagesettingdlg->
//      m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip(
//      wxT("start finding the lowest stable CPU core voltage") );

    LOGN( FULL_FUNC_NAME << "--return 0")
    return 0;
  }
  LOGN( FULL_FUNC_NAME << "--return 1")
  return 1;
}

DWORD THREAD_PROC_CALLING_CONVENTION
  InstableCPUcoreOperationDetection::FindLowestStableVoltage_ThreadProc(
  void * p_v )
{
  LOGN( FULL_FUNC_NAME << "--begin")
  return s_p_instableCPUcoreOperationDetection->FindLowestStableVoltage(/*p_v*/);
//  //Reset to initial value in order to differentiate between whether the
//  //voltage was too low later.
//  wxGetApp().m_bVoltageWasTooLowCalled = false;
//  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg =
//    (FreqAndVoltageSettingDlg *) p_v;
//  if( p_freqandvoltagesettingdlg)
}

void InstableCPUcoreOperationDetection::SetUserInterface(
  /*UserInterface * p_ui*/ userinterface_type * p_ui)
{
  m_p_userinterface = p_ui;
}

const x86IandC::thread_type &
  //StartInstableCPUcoreVoltageDetectionInDynLibInSeparateThread(
  InstableCPUcoreOperationDetection::StartInDynLibInSeparateThread(
    /*FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg*/)
{
  LOGN( FULL_FUNC_NAME << " begin" )
//  if( //1threadForEveryCore
//      true)
//  {
//    uint32_t m_ui32CPUcoreMask = p_freqandvoltagesettingdlg->GetCPUcoreMask();
//    const BYTE byNumCores = GetNumberOfSelectedCPUcores(m_ui32CPUcoreMask);
//    x86IandC::thread_type x86iandc_threadFindLowestStableVoltage[byNumCores];
//    for( BYTE selectedCoreIndex = 0; selectedCoreIndex < byNumCores ;
//        ++ selectedCoreIndex)
//    {
//      x86iandc_threadFindLowestStableVoltage[selectedCoreIndex].start(
//        StartInstableCPUcoreVoltageDetectionInDLL ,
//        p_freqandvoltagesettingdlg ) ;
//  }
//  else
//  {
//    x86IandC::thread_type x86iandc_threadFindLowestStableVoltage;
//    uint32_t m_ui32CPUcoreMask = p_freqandvoltagesettingdlg->GetCPUcoreMask();
    m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.start(
      //StartInstableCPUcoreVoltageDetectionInDLL ,
      InstableCPUcoreOperationDetection::StartInDynLib_ThreadProc,
      /*p_freqandvoltagesettingdlg*/ NULL,
      "StartInstableCPUcoreOpDetectionInDynLib",
      I_Thread::maximum_priority //thread priority
      ) ;
  int nThreadPriority = m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.
    GetThreadPriority();
  LOGN_DEBUG(FULL_FUNC_NAME << "--thread priority: " << nThreadPriority )
//    StartInstableCPUcoreVoltageDetectionInDLL(p_freqandvoltagesettingdlg);
//  }
  LOGN( FULL_FUNC_NAME << "--thread handle: "
    << m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.
    m_handleThread)

  LOGN( FULL_FUNC_NAME << " end")
  return //x86iandc_threadFindLowestStableVoltage;
    m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL;
}

void InstableCPUcoreOperationDetection::VoltageTooLow()
{
  s_p_instableCPUcoreOperationDetection->HandleVoltageTooLow();
}
