/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * UserInterfaceAttributes.hpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

#ifndef USERINTERFACEATTRIBUTES_HPP_
#define USERINTERFACEATTRIBUTES_HPP_

#include <map> //class std::map
#include <string> //class std::string
#include <inttypes.h> //uint16_t
//#include <vector> //class std::vector

namespace Attributes
{
  class TaskBarIcon
  {
  public:
    bool show;
    std::string colour;
  };

  class VoltnFreqSettingsDialog
  {
    bool m_bSelectAllCPUcores ;
  };

  class MainFrame
  {
  public:
    unsigned short m_heightInPixels ;
    unsigned short m_widthInPixels ;
    unsigned short m_topLeftCornerXcoordinateInPixels ;
    unsigned short m_topLeftCornerYcoordinateInPixels ;
    int m_nVoltageScaleSizeInPoint;
    int m_nCurrentCPUcoreInfoSizeInPoint;
    int m_nCPUcoreFrequencyScaleSizeInPoint;
    MainFrame()
      : m_heightInPixels( 400 ) ,
      m_widthInPixels( 500 ) ,
      m_topLeftCornerXcoordinateInPixels( 50) ,
      m_topLeftCornerYcoordinateInPixels( 50) ,
      m_nVoltageScaleSizeInPoint(8),
      m_nCurrentCPUcoreInfoSizeInPoint(10),
      m_nCPUcoreFrequencyScaleSizeInPoint(9)
    {

    }
  };

  class UserInterfaceAttributes
  {
  public:
    bool m_bPreventVoltageAboveDefaultVoltage ;
    bool m_bPreventVoltageBelowLowestStableVoltage ;
    bool m_bShowMultiplierControlsIfLessThan2ConfigurableMultipliers;
    bool m_bShowVoltageControlsIfLessThan2ConfigurableVoltages;

    bool m_bSelectAllCPUcores ;
    bool m_bShowMainFrameAtStartup ;
    bool m_bShowCPUcoreUsagesIconInTaskBar;
    bool m_bShowCPUcoresMultipliersIconInTaskBar;
    bool m_bStartDVFSatStartup ;
    bool m_bTryToPauseViaServiceControlManagerIfViaIPCfails ;
    float m_fOperatingSafetyMarginInVolt ;

    MainFrame mainframe;

    unsigned short m_wToolTipDelay ;

    static int s_defaultFontSizeInPoint;

    int m_nCPUcoreTempTaskBarIconFontSizeInPoint;

    std::map<uint16_t,std::string> m_std_vec_WisdomStrings;


    std::wstring m_std_wstrServiceAddress;

    UserInterfaceAttributes();
    virtual
    ~UserInterfaceAttributes();
  };
}

#endif /* USERINTERFACEATTRIBUTES_HPP_ */
