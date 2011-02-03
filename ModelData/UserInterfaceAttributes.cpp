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

  UserInterfaceAttributes::UserInterfaceAttributes()
    :
    m_bPreventVoltageAboveDefaultVoltage( true) ,
    m_bPreventVoltageBelowLowestStableVoltage(true) ,
    m_bSelectAllCPUcores ( true) ,
    m_bShowMainFrameAtStartup (true ) ,
    m_bStartDVFSatStartup(false) ,
    m_bTryToPauseViaServiceControlManagerIfViaIPCfails(
      //Initialize with "false" to avoid sucking message boxes ("connecting to
      // service control manager failed:[...]")
      false ) ,
    m_fOperatingSafetyMarginInVolt(
      //0.07 Volt is a good margin/ minimum for also taking into account (an)
      //attached USB device(s) that else may cause an Operating System freeze/
      //restart.
      0.07) ,
    m_wMainFrameHeightInPixels( 400 ) ,
    m_wMainFrameWidthInPixels( 500 ) ,
    m_wMainFrameTopLeftCornerXcoordinateInPixels( 50) ,
    m_wMainFrameTopLeftCornerYcoordinateInPixels( 50) ,
    m_wToolTipDelay( MAXWORD)
  {
  }

  UserInterfaceAttributes::~UserInterfaceAttributes()
  {
    // TODO Auto-generated destructor stub
  }

}