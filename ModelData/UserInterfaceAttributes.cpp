/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * UserInterfaceAttributes.cpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

#include <ModelData/UserInterfaceAttributes.hpp>

#define MAXWORD 0xffff

namespace Attributes
{
  //definition for static variable.
  int UserInterfaceAttributes::s_defaultFontSizeInPoint;

  UserInterfaceAttributes::UserInterfaceAttributes()
    :
    m_bPreventVoltageAboveDefaultVoltage( true) ,
    m_bPreventVoltageBelowLowestStableVoltage(true) ,
    m_bShowMultiplierControlsIfLessThan2ConfigurableMultipliers(false),
    m_bShowVoltageControlsIfLessThan2ConfigurableVoltages(false),
    m_bSelectAllCPUcores ( true) ,
    m_bShowMainFrameAtStartup (true ) ,
    m_bShowCPUcoreUsagesIconInTaskBar (true),
    m_bShowCPUcoresMultipliersIconInTaskBar (true),
    m_bStartDVFSatStartup(false) ,
    m_bTryToPauseViaServiceControlManagerIfViaIPCfails(
      //Initialize with "false" to avoid sucking message boxes ("connecting to
      // service control manager failed:[...]")
      false ) ,
    m_fOperatingSafetyMarginInVolt(
      //0.1 Volt is a good margin/ minimum for also taking into account (an)
      //attached USB device(s) that else may cause an Operating System freeze/
      //restart.
      //With voltages below, e.g. 0.7 V margin, Ubuntu failed to start on a Pentium
      //M 745 (1.8 GHz). Possibly because of voltage peeks due to powering on
      //many devices (CCFL backlight etc.) at the same time at startup.
      0.1) ,
    m_wToolTipDelay( MAXWORD) ,
    m_nCPUcoreTempTaskBarIconFontSizeInPoint(10),
    //TODO change to ~ "\\\\.\\pipe\\x86IandC_service"
    m_std_wstrServiceAddress(L"\\\\.\\pipe\\CPUcontrollerService")
  {
  }

  UserInterfaceAttributes::~UserInterfaceAttributes()
  {
    // TODO Auto-generated destructor stub
  }

}
