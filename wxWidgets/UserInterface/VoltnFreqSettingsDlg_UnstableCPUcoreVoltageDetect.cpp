/*
 * VoltnFreqSettingsDlg_UnstableCPUcoreVoltageDetect.cpp
 *
 *  Created on: Sep 23, 2012
 *      Author: sgebauer
 */
#include "FreqAndVoltageSettingDlg.hpp" //class FreqAndVoltageSettingDlg
//#include <wx/dialog.h> //class wxDialog
#include <wxWidgets/App.hpp> //for wxGetApp()
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include "MainFrame.hpp" //for class MainFrame

#include <images/find_lowest_stable_CPU_core_voltage16x16.xpm>
#include <images/stop_finding_lowest_stable_CPU_core_voltage16x16.xpm>
#include <images/auto-configure_CPU_core_voltage_settings16x16.xpm>

#include <wx/bmpbuttn.h> //class wxBitmapButton
//#include <wx/button.h> //for class wxButton
#include <wx/sizer.h> //for class wxBoxSizer
#include <wx/textctrl.h> //class wxTextCtrl
#include <fastest_data_type.h> //typedef fastestUnsignedDataType

//BEGIN_EVENT_TABLE(FreqAndVoltageSettingDlg, wxDialog)
//END_EVENT_TABLE()
#define START_UNSTABLE_CPU_CORE_OPERATION_DETECTION_LABEL wxT("start finding the " \
  "lowest CPU core voltage for a stable CPU core operation beginning with " \
  "the multiplier and voltage set in this dialog")
#define STOP_UNSTABLE_CPU_CORE_OPERATION_DETECTION_LABEL wxT("stop finding the " \
  "lowest CPU core voltage for a stable CPU core operation")

inline void ShowStartedInstableCPUcoreOperationDetectionInGUIthread(
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg)
{
  wxCommandEvent wxcommand_event(
  //      MyExcitingEvent, //wxEventType commandType = wxEVT_NULL,
  //      wxEVT_NULL,
    wxEVT_COMMAND_STARTED_INSTABLE_CPU_CORE_OPERATION_DETECTION
    );
  wxPostEvent(p_freqandvoltagesettingdlg, wxcommand_event);
}

fastestUnsignedDataType StartFindingLowestStableVoltageAndWaitForItsTermination(
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg,
  float fVoltageInVolt, float fMultiplier
  )
{
  LOGN( "begin")
  InstableCPUcoreOperationDetection & instablecpucorevoltagedetection =
    wxGetApp().m_model.m_instablecpucorevoltagedetection;
  //Reset to initial value in order to differentiate between whether the
  //voltage was too low later.
  instablecpucorevoltagedetection.m_bVoltageWasTooLowCalled = false;
  p_freqandvoltagesettingdlg->FindLowestStableVoltage(
    fVoltageInVolt, fMultiplier );
  LOGN( "waiting for the termination of the \"find "
      "lowest stable voltage by decreasing voltage\" thread.")
  fastestUnsignedDataType findVoltageForLowestStableCPUcoreOpRetVal =
    (fastestUnsignedDataType) instablecpucorevoltagedetection.
    m_x86iandc_threadFindLowestStableVoltage.WaitForTermination();
  LOGN( "after waiting for the termination of the \""
    "find lowest stable voltage by decreasing voltage\" thread.")
  return findVoltageForLowestStableCPUcoreOpRetVal;
}

DWORD THREAD_FUNCTION_CALLING_CONVENTION
  AutoConfigureVoltageSettings_ThreadFunc(void * p_v)
{
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg =
    (FreqAndVoltageSettingDlg *) p_v;
  LOGN( "begin--" << p_freqandvoltagesettingdlg)
  if( p_freqandvoltagesettingdlg)
  {
    Model * p_model = p_freqandvoltagesettingdlg->mp_model;
    //::wxMessageBox( s_wxstrInstableCPUcoreVoltageWarning);
    CPUcoreData & c_r_cpucoredata = p_freqandvoltagesettingdlg->
      mp_model->m_cpucoredata;
    const InstableCPUcoreOperationDetection & instablecpucorevoltagedetection =
      p_model->m_instablecpucorevoltagedetection;

    float fMultiplier, fLowestMultiplierWhereInstabilityWasReached = 0.0f,
      fHighestMultiplierWhereInstabilityCouldntBeReached = 0.0f;
    float fVoltageInVolt = c_r_cpucoredata.
      GetStartVoltageForHighestMultiplierForFindingLowestCPUcoreVoltage(
        fMultiplier
        );
    fHighestMultiplierWhereInstabilityCouldntBeReached = fMultiplier;
    if( fVoltageInVolt != 0.0f )
    {
      fastestUnsignedDataType retVal = StartFindingLowestStableVoltageAndWaitForItsTermination(
        p_freqandvoltagesettingdlg, fVoltageInVolt, fMultiplier);
      if( retVal == settingVoltageFailed )
        return retVal;
    //    wxGetApp().m_conditionFindLowestStableVoltage.Wait();
      LOGN( "VoltageWasTooLowCalled:"
        << instablecpucorevoltagedetection.m_bVoltageWasTooLowCalled
        << " ExitFindLowestStableVoltage:"
        << instablecpucorevoltagedetection.m_vbExitFindLowestStableVoltage )
      if( instablecpucorevoltagedetection.m_bVoltageWasTooLowCalled
          || instablecpucorevoltagedetection.m_vbExitFindLowestStableVoltage )
      {
        fLowestMultiplierWhereInstabilityWasReached = fMultiplier;
        LOGN( "\"VoltageTooLow\" was called so "
          " LowestMultiplierWhereInstabilityWasReached ^="
          << fMultiplier)
      }
    }
    LOGN( //" wxGetApp().m_vbExitFindLowestStableVoltage:"
      //<< wxGetApp().m_vbExitFindLowestStableVoltage
      "m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI: "
      << instablecpucorevoltagedetection.
        m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI
      )
    if( ! //wxGetApp().m_vbExitFindLowestStableVoltage
        instablecpucorevoltagedetection.
        m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI
        )
    {
      fVoltageInVolt = c_r_cpucoredata.
        GetStartVoltageForLowestMultiplierForFindingLowestCPUcoreVoltage(
          fMultiplier
          );
//      fLowestMultiplierWhereInstabilityWasReached = fMultiplier;
      if( fVoltageInVolt == 0.0f )
      {
        //Use default or lowest stable voltage from max. FID as starting voltage
        //for finding the lowest stable voltage for the min FID.
        fVoltageInVolt = instablecpucorevoltagedetection.
          m_lastSetCPUcoreVoltageInVolt;
      }
      if( fVoltageInVolt != 0.0f )
      {
//        LOGN( "waiting for \"VoltageTooLow()\" to be finished")
//        //Wait for the ::VoltageTooLow() to be ended.
//        wxGetApp().m_conditionFindLowestStableVoltage.Wait();
//        LOGN( "after waiting for \"VoltageTooLow()\" to be finished")

      //    const VoltageAndFreq & lowestMultiInsertedByCPUcontroller =
      //      voltageandfreqInsertedByCPUcontroller.front();
      //    fVoltageInVolt = lowestMultiInsertedByCPUcontroller.m_fVoltageInVolt;
      //    fMultiplier = mp_cpucontroller->GetClosestMultiplier(
      //      lowestMultiInsertedByCPUcontroller.m_wFreqInMHz);

        StartFindingLowestStableVoltageAndWaitForItsTermination(
          p_freqandvoltagesettingdlg, fVoltageInVolt, fMultiplier);
        LOGN( "m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI: "
          << instablecpucorevoltagedetection.
          m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI
          )
        if( ! instablecpucorevoltagedetection.
            m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI)
        {
          LOGN( "VoltageWasTooLowCalled:"
            << instablecpucorevoltagedetection.m_bVoltageWasTooLowCalled)
          //Didn't reach the lowest stable voltage for the lowest multiplier
          if( ! instablecpucorevoltagedetection.m_bVoltageWasTooLowCalled )
          {
            LOGN( "\"VoltageTooLow\" has not been called"
              ", so the instable voltage for the lowest multiplier could not be "
              "found.")
            fHighestMultiplierWhereInstabilityCouldntBeReached = fMultiplier;
            LOGN( "adding lowest voltage for multiplier \""
              << fMultiplier << "\" as " //lowest stable
              "wanted "
              "CPU core voltage")
            c_r_cpucoredata.//m_stdsetvoltageandfreqLowestStable.
              m_stdsetvoltageandfreqWanted.
              insert( VoltageAndFreq(
                c_r_cpucoredata.m_arfAvailableVoltagesInVolt[0]
                  , (WORD) (fMultiplier *
                  p_freqandvoltagesettingdlg->mp_cpucontroller->
                  m_fReferenceClockInMHz)
                  )
                );

            bool bArentDirectlyNeighbouredMultipliers;
            do
            {
              fVoltageInVolt = c_r_cpucoredata.
                GetStartVoltageForMultiplierForFindingLowestCPUcoreVoltage(
                  //float & fPercentOfMinAndMaxMultiplier
                  fHighestMultiplierWhereInstabilityCouldntBeReached,
                  fLowestMultiplierWhereInstabilityWasReached,
                  fMultiplier
                  );
              StartFindingLowestStableVoltageAndWaitForItsTermination(
                p_freqandvoltagesettingdlg, fVoltageInVolt, fMultiplier);
              LOGN( "m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI: "
                << instablecpucorevoltagedetection.
                  m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI
                )
              if( instablecpucorevoltagedetection.
                  m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI)
                break;
              LOGN( "VoltageWasTooLowCalled:"
                << instablecpucorevoltagedetection.m_bVoltageWasTooLowCalled
                << " ExitFindLowestStableVoltage:"
                << instablecpucorevoltagedetection.m_vbExitFindLowestStableVoltage )
              if( instablecpucorevoltagedetection.m_bVoltageWasTooLowCalled ||
                  instablecpucorevoltagedetection.m_vbExitFindLowestStableVoltage )
              {
                fLowestMultiplierWhereInstabilityWasReached = fMultiplier;
                LOGN( "\"VoltageTooLow\" was called so "
                  " LowestMultiplierWhereInstabilityWasReached ^="
                  << fMultiplier)
              }
              //TODO "VoltageTooLow" is also not called when 2 voltage steps below
              //lowest stable volatge / highest instable voltage from
              // previous config data.
              else
              {
                LOGN( "\"VoltageTooLow\" was NOT called so "
                  " HighestMultiplierWhereInstabilityCouldntBeReached ^="
                  << fMultiplier)
                fHighestMultiplierWhereInstabilityCouldntBeReached = fMultiplier;
              }
              bArentDirectlyNeighbouredMultipliers = c_r_cpucoredata.
                ArentDirectlyNeighbouredMultipliers(
                fLowestMultiplierWhereInstabilityWasReached,
                fHighestMultiplierWhereInstabilityCouldntBeReached);
              LOGN( "bArentDirectlyNeighbouredMultipliers:"
                << bArentDirectlyNeighbouredMultipliers)
            }
            while( bArentDirectlyNeighbouredMultipliers);
            LOGN( "after binary search for lowest multiplier "
              "with lowest stable voltage loop")

            if( ! c_r_cpucoredata.m_stdset_floatAvailableMultipliers.empty() )
            {
              WORD wLowestFreqInMHz = (WORD) (c_r_cpucoredata.m_arfAvailableMultipliers[0]
                * p_freqandvoltagesettingdlg->mp_cpucontroller->
                m_fReferenceClockInMHz);

              if( ! c_r_cpucoredata.m_stdsetvoltageandfreqLowestStable.empty() )
              {
                //For a Pentium M 745: not for 600 MHz, but beginning with 800 MHz
                // the lowest stable voltage was found.
                const VoltageAndFreq & lowestStableVoltageForLowestMultiFound = *
                  c_r_cpucoredata.m_stdsetvoltageandfreqLowestStable.begin();

                VoltageAndFreq lowestStableVoltageForLowestMulti;
                float fLowestStableVoltageInVoltForLowestMulti = c_r_cpucoredata.
                  AddLowestStableVoltageForLowestAvailableMulti(
                    lowestStableVoltageForLowestMultiFound,
                    wLowestFreqInMHz,
                    lowestStableVoltageForLowestMulti);
                if( fLowestStableVoltageInVoltForLowestMulti > 0.0f )
                {
                c_r_cpucoredata.AddWantedVoltageForLowestAvailableMulti(
                  fLowestStableVoltageInVoltForLowestMulti, wLowestFreqInMHz);
                }

                fastestUnsignedDataType extrapolatedWantedFreqInMHzForMinVoltage =
                  c_r_cpucoredata.AddWantedVoltageForLowestVoltage(
                  lowestStableVoltageForLowestMultiFound);

      //          p_model->m_cpucoredata.AddWantedVoltageForMinVoltage(
      //            extrapolatedWantedFreqInMHzForMinVoltage);

                if( extrapolatedWantedFreqInMHzForMinVoltage > wLowestFreqInMHz )
                  c_r_cpucoredata.AddMinVoltageForLowestMulti(wLowestFreqInMHz);

                //redraw p-state crosses
      //          wxGetApp().mp_frame->RedrawEverything() ;
                //Send event to redraw everything because calling it from another
                //thread.
                wxCommandEvent wxcommand_event(wxEVT_COMMAND_REDRAW_EVERYTHING);
                wxPostEvent(wxGetApp().mp_frame, wxcommand_event);
              }
            }

  //          wxGetApp().mp_frame->Refresh() ; //force paint event/ call of "OnPaint()".
  //          //extrapolate lowest multi with wanted voltage &
  //          //  highest multi with wanted voltage
  //          //  wanted voltage for lowest multi
  //          p_model->m_cpucoredata.m_stdsetvoltageandfreqWanted.rbegin()->
  //            GetLinearExtrapolatedVoltage(
  //              * p_model->m_cpucoredata.m_stdsetvoltageandfreqWanted.begin(),
  //            (WORD) (fMultiplier * p_freqandvoltagesettingdlg->
  //                mp_cpucontroller->m_fReferenceClockInMHz),
  //            fVoltageInVolt);
          }
        }
      }
    }
  }
  //Important: else unstable voltage can not be detected after Prime95 torture
  //test ended.
  wxGetApp().ExitFindLowestStableVoltageThread();
  LOGN( "return 0")
  return 0;
}

//inline
  void FreqAndVoltageSettingDlg::AddAutoConfigureVoltageSettingsButton(
  wxSizer * p_wxsizerSuperordinate )
{
  wxString wxstrToolTip = wxT("auto-configure CPU core voltage settings:\n"
    "find lowest stable CPU core voltage and set the desired voltage based on it");
  m_p_wxbitmapbuttonAutoConfigureVoltageSettings = new wxBitmapButton(
    (wxWindow * ) this , // wxWindow *    parent,
    // wxWindowID id,
    (wxWindowID) ID_AutoConfigureVoltageSettings,
    //const wxBitmap & label,
    wxBitmap( auto_configure_CPU_core_voltage_settings16x16_xpm )
    );
  m_p_wxbitmapbuttonAutoConfigureVoltageSettings->SetToolTip(
    wxstrToolTip ) ;
  p_wxsizerSuperordinate->Add(
    m_p_wxbitmapbuttonAutoConfigureVoltageSettings,
    0 ,//0=the control should not take more space if the sizer is enlarged
    wxLEFT | wxRIGHT,
    0);
}

void FreqAndVoltageSettingDlg::AddSecondsUntilNextVoltageDecreaseTextControl(
  wxSizer * p_wxsizer )
{
  LOGN( "begin")
  wxSize wxsize(40, 20);
  m_p_wxtextctrlSecondsUntilNextVoltageDecrease = new wxTextCtrl(
    this,
    wxID_ANY,
    wxString::Format( wxT("%u"), mp_model->
      m_instablecpucorevoltagedetection.
      m_uiNumberOfSecondsToWaitUntilVoltageIsReduced ),
    wxDefaultPosition,
    wxsize
    );
  m_p_wxtextctrlSecondsUntilNextVoltageDecrease->SetToolTip(
    wxT("For detecting an instable CPU core voltage:\n"
      "seconds until the voltage is decreased") );
  p_wxsizer->Add(m_p_wxtextctrlSecondsUntilNextVoltageDecrease//,
    //1
    );
  LOGN( "end")
}

void FreqAndVoltageSettingDlg::
  AddInstableCPUdetectDynLibCPUcoreUsagePercentageTextControl(
  wxSizer * p_wxsizer )
{
  LOGN( "begin")
  wxSize wxsize(40, 20);
  m_p_wxtextctrlInstableCPUdetectDynLibCPUcoreUsagePercentage = new wxTextCtrl(
    this,
    wxID_ANY,
    wxEmptyString,
    wxDefaultPosition,
    wxsize
    );
  m_p_wxtextctrlInstableCPUdetectDynLibCPUcoreUsagePercentage->SetToolTip(
    wxT("For detecting an instable CPU core operation:\n"
      "% of CPU core usage for \"detect instable CPU core thread\"") );
  p_wxsizer->Add(m_p_wxtextctrlInstableCPUdetectDynLibCPUcoreUsagePercentage//,
    //1
    );
  LOGN( "end")
}

void FreqAndVoltageSettingDlg::
  ChangeToStopFindingLowestStableCPUcoreVoltageButton()
{
  wxBitmap wxbitmapStopFindingLowestStableCPUcoreVoltage = wxBitmap(
    stop_finding_lowest_stable_CPU_core_voltage16x16_xpm ) ;
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetImageLabel(
    wxbitmapStopFindingLowestStableCPUcoreVoltage);
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip(
//    wxT("stop finding the lowest stable CPU core voltage")
    STOP_UNSTABLE_CPU_CORE_OPERATION_DETECTION_LABEL);
}

void FreqAndVoltageSettingDlg::CreateFindLowestStableCPUcoreVoltageButton()
{
  wxBitmap wxbitmapFindLowestStableCPUcoreVoltage = wxBitmap(
    find_lowest_stable_CPU_core_voltage16x16_xpm
//    stop_finding_lowest_stable_CPU_core_voltage16x16_xpm
    ) ;
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage =
    new wxBitmapButton(
   this
   , ID_findLowestStableVoltage
  //    , wxT("&+")
   , wxbitmapFindLowestStableCPUcoreVoltage
   , wxDefaultPosition,
   wxDefaultSize,
   wxBU_AUTODRAW |
   //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
   // "If you need to use this style in order to get the arrows or etc., but
   // would still like to have normal keyboard navigation take place, you
   // should create and send a wxNavigationKeyEvent in response to the key
   // events for Tab and Shift-Tab."
   //To get EVT_CHAR events when the button is focused.
   wxWANTS_CHARS
   ) ;
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip(
    START_UNSTABLE_CPU_CORE_OPERATION_DETECTION_LABEL );
}

void FreqAndVoltageSettingDlg::CreateStopFindingLowestStableCPUcoreVoltageButton()
{
//  //Setting an bitmap button after creation failed with wxWidgets 2.8.10.
//  //So create a second button for stopping the low CPU core voltage detection.
//  wxBitmap wxbitmapStopFindingLowestStableCPUcoreVoltage = wxBitmap(
//    stop_finding_lowest_stable_CPU_core_voltage16x16_xpm
//    ) ;
//  m_p_wxbitmapbuttonStopFindingLowestStableCPUcoreVoltage =
//    new wxBitmapButton(
//   this
//   , ID_stopFindingLowestStableVoltage
//  //    , wxT("&+")
//   , wxbitmapStopFindingLowestStableCPUcoreVoltage
//   , wxDefaultPosition,
//   wxDefaultSize,
//   wxBU_AUTODRAW |
//   //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
//   // "If you need to use this style in order to get the arrows or etc., but
//   // would still like to have normal keyboard navigation take place, you
//   // should create and send a wxNavigationKeyEvent in response to the key
//   // events for Tab and Shift-Tab."
//   //To get EVT_CHAR events when the button is focused.
//   wxWANTS_CHARS
//   ) ;
//  m_p_wxbitmapbuttonStopFindingLowestStableCPUcoreVoltage->SetToolTip(
//    wxT("stop finding the lowest stable CPU core voltage") );
}

void FreqAndVoltageSettingDlg::OnAutoConfigureVoltageSettingsButton(
  wxCommandEvent & r_wxcommandevent )
{
  LOGN( "begin")
  mp_model->m_instablecpucorevoltagedetection.
    m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI = false;
  ::wxGetApp().InitUnstableCPUcoreOperationDetection();

  ::wxGetApp().m_p_freqandvoltagesettingdlgInstCPUcoreDetect = this;

//  if( wxGetApp().StartInstableCPUcoreVoltageDetection(this) == 0 )
//    ShowInstableCPUcoreVoltageWarning();
  //TODO: what happens when function ends and thread object is destroyed?
  x86IandC::thread_type x86IandC_thread_typeAutoConfigureVoltageSettingsButton;
  //Must start in another thread because it should be executed in UI thread
  // to be able to stop the voltage search.
  //Or send a "lowest CPU core voltage reached" event
  x86IandC_thread_typeAutoConfigureVoltageSettingsButton.start(
    AutoConfigureVoltageSettings_ThreadFunc, this);

//  StartedInstableCPUcoreVoltageDetection();
  ShowStartedInstableCPUcoreOperationDetectionInGUIthread(this);
//  AutoConfigureVoltageSettings_ThreadFunc(this);
  LOGN( "end")
}

#include <preprocessor_macros/thread_proc_calling_convention.h>

void FreqAndVoltageSettingDlg::OnStopFindingLowestStableCPUcoreVoltageButton(
    wxCommandEvent & //WXUNUSED(event)
    r_wxcommandevent )
{
  StopFindingLowestStableCPUcoreVoltage();
}

void FreqAndVoltageSettingDlg::HideInstableCPUcoreVoltageWarning()
{
  p_wxboxsizerTop->//Remove(//
    Detach(
      //1
      m_p_wxtextctrlInstableCPUcoreVoltageWarning
    );
  p_wxboxsizerTop->Insert(0, p_wxboxsizerCPUcorePstate);
  //"lay out the window and its children"
  Layout();
}

void FreqAndVoltageSettingDlg::ShowInstableCPUcoreVoltageWarning()
{
  LOGN("begin")
  if( ! m_p_wxtextctrlInstableCPUcoreVoltageWarning)
  {
    m_p_wxtextctrlInstableCPUcoreVoltageWarning = new wxTextCtrl(
      this //wxWindow* parent
      , wxID_ANY //wxWindowID id
      , s_wxstrInstableCPUcoreVoltageWarning //const wxString& value = ""
      , wxDefaultPosition //const wxPoint& pos = wxDefaultPosition,
      , wxDefaultSize //const wxSize& size = wxDefaultSize
      , wxTE_READONLY | wxTE_MULTILINE //long style = 0
      );
    m_p_wxtextctrlInstableCPUcoreVoltageWarning->SetForegroundColour( * wxRED);
  }
//  p_wxboxsizerTop->Remove(p_wxboxsizerCPUcorePstate);
  p_wxboxsizerTop->Detach(p_wxboxsizerCPUcorePstate);
  p_wxboxsizerTop->Insert(0, //m_p_wxstatictextInstableCPUcoreVoltageWarning
    m_p_wxtextctrlInstableCPUcoreVoltageWarning,
    1 //proportion
    , wxEXPAND //flags
    );
  //"lay out the window and its children"
  Layout();
  //http://docs.wxwidgets.org/2.8/wx_wxsizer.html#wxsizerdetach:
  //"call wxSizer::Layout to update the layout "on screen" after removing a
  //child from the sizer."
  p_wxboxsizerTop->Layout();
}

void FreqAndVoltageSettingDlg::StopFindingLowestStableCPUcoreVoltage()
{
  LOGN( "begin")
  mp_model->m_instablecpucorevoltagedetection.
    m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI = true;
#ifdef _WIN32
  wxGetApp().StopInstableCPUcoreVoltageDetection();
#endif
//  HideInstableCPUcoreVoltageWarning();
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetBitmapLabel(
    wxBitmap(find_lowest_stable_CPU_core_voltage16x16_xpm) );
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip( //wxT("start "
//    "finding the lowest voltage for a stable CPU core operation")
    START_UNSTABLE_CPU_CORE_OPERATION_DETECTION_LABEL  );
//  SetStopFindingLowestStableCPUcoreVoltageButton();
  LOGN( "end")
}

void FreqAndVoltageSettingDlg::OnFindLowestStableVoltageButton(
    wxCommandEvent & //WXUNUSED(event)
    r_wxcommandevent )
{
  LOGN( "begin")
//  if( wxGetApp().StartInstableCPUcoreVoltageDetection(this) == 0 )
//    ShowInstableCPUcoreVoltageWarning();
#ifdef _WIN32
  InstableCPUcoreOperationDetection & instableCPUcoreOperationDetection =
    mp_model->m_instablecpucorevoltagedetection;
  if( instableCPUcoreOperationDetection.IsRunning() )
    StopFindingLowestStableCPUcoreVoltage();
  else
  {
    ::wxGetApp().m_p_freqandvoltagesettingdlgInstCPUcoreDetect = this;
    ::wxGetApp().InitUnstableCPUcoreOperationDetection();
    instableCPUcoreOperationDetection.SetStopRequestedViaGUI(false);
    //Multipliers can also be floats: e.g. "5.5" for AMD Griffin.
    const float fMultiplierFromSliderValue = GetMultiplierFromSliderValue();
    instableCPUcoreOperationDetection.m_lastSetCPUcoreMultiplier =
      fMultiplierFromSliderValue;

    const float fVoltageInVoltFromSliderValue =
      GetVoltageInVoltFromSliderValue() ;
    instableCPUcoreOperationDetection.m_lastSetCPUcoreVoltageInVolt =
      fVoltageInVoltFromSliderValue;
    FindLowestStableVoltage();
  }
#endif
  LOGN( "end")
}

void FreqAndVoltageSettingDlg::FindLowestStableVoltage(
  const float fVoltageInVolt,
  const float fMultiplier
  )
{
  LOGN( fMultiplier << "," << fVoltageInVolt << "V")
  mp_model->m_instablecpucorevoltagedetection.m_lastSetCPUcoreMultiplier =
    fMultiplier;
  mp_model->m_instablecpucorevoltagedetection.m_lastSetCPUcoreVoltageInVolt =
    fVoltageInVolt;
//  SetMultiplierSliderToClosestValue(fMultiplier);
//  SetVoltageSliderToClosestValue(fVoltageInVolt);

  FindLowestStableVoltage();
}

void FreqAndVoltageSettingDlg::StartedInstableCPUcoreVoltageDetection(
  wxCommandEvent & event)
{
  //      ShowInstableCPUcoreVoltageWarning();
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetBitmapLabel(
    wxBitmap(stop_finding_lowest_stable_CPU_core_voltage16x16_xpm) );
  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip(
//    wxT("stop finding the lowest stable CPU core voltage")
    STOP_UNSTABLE_CPU_CORE_OPERATION_DETECTION_LABEL );
}

/** Should be called from the GUI thread because UserInterface controls are
 * being accessed. */
void FreqAndVoltageSettingDlg::FindLowestStableVoltage()
{
  LOGN( "begin")
  const wxString & wxstrSecondsUntilNextVoltageDecrease =
    m_p_wxtextctrlSecondsUntilNextVoltageDecrease->GetLabel();
  unsigned long ulSecondsUntilNextVoltageDecrease;
  //TODO program crash when "§" is entered.
  if( wxstrSecondsUntilNextVoltageDecrease.ToULong(
    & ulSecondsUntilNextVoltageDecrease, 10) )
  {
    LOGN("seconds to wait until voltage decrease:"
      << ulSecondsUntilNextVoltageDecrease)
    mp_model->m_instablecpucorevoltagedetection.
      m_uiNumberOfSecondsToWaitUntilVoltageIsReduced =
      ulSecondsUntilNextVoltageDecrease;
#ifdef _WIN32
    if( wxGetApp().StartInstableCPUcoreVoltageDetection(this) == //ERROR_SUCCESS
        EXIT_SUCCESS )
    {
//      StartedInstableCPUcoreVoltageDetection();
      ShowStartedInstableCPUcoreOperationDetectionInGUIthread(this);
//      LOGN( "waking up  \"VoltageTooLow()\" to be finished")
//      //Wake up all threads (the thread that started "VoltageTooLow()")
//      //waiting on the condition.
//      wxGetApp().m_conditionFindLowestStableVoltage.Broadcast();
    }
#endif
//      wxGetApp().StopInstableCPUcoreVoltageDetection();
//      }
//      else
//      {
//      }

//  m_p_wxboxsizerOK_Cancel->Replace(
//    m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage,
//    m_p_wxstatictextSecondsUntilNextVoltageDecrease );

//  m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->Hide();
//  m_p_wxstatictextSecondsUntilNextVoltageDecrease->Show();
//  //http://docs.wxwidgets.org/stable/wx_wxsizer.html#wxsizerreplace:
//  //"[...] call wxSizer::Layout to update the layout "on screen" after
//  //replacing a child from the sizer."
//  m_p_wxboxsizerOK_Cancel->Layout();
//  Layout();
  }
  else
  {
    std::wstring std_wstr = L"For getting the number of seconds unstable CPU core voltage detection:"
      "can't convert \"" +
      ::GetStdWstring( wxstrSecondsUntilNextVoltageDecrease) + L"\" to int";
    wxGetApp().MessageWithTimeStamp( std_wstr);
  }
  LOGN( "end")
}
