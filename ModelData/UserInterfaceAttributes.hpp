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
#include <vector> //class std::vector
#include <map> //class std::map

namespace Attributes
{

  class UserInterfaceAttributes
  {
  public:
    bool m_bPreventVoltageAboveDefaultVoltage ;
    bool m_bPreventVoltageBelowLowestStableVoltage ;
    bool m_bSelectAllCPUcores ;
    bool m_bShowMainFrameAtStartup ;
    bool m_bStartDVFSatStartup ;
    bool m_bTryToPauseViaServiceControlManagerIfViaIPCfails ;
    float m_fOperatingSafetyMarginInVolt ;
    unsigned short m_wMainFrameHeightInPixels ;
    unsigned short m_wMainFrameWidthInPixels ;
    unsigned short m_wMainFrameTopLeftCornerXcoordinateInPixels ;
    unsigned short m_wMainFrameTopLeftCornerYcoordinateInPixels ;
    unsigned short m_wToolTipDelay ;
    int m_nVoltageScaleSizeInPoint;
    int m_nCurrentCPUcoreInfoSizeInPoint;
    int m_nCPUcoreFrequencyScaleSizeInPoint;
    std::map<uint16_t,std::string> m_std_vec_WisdomStrings;
    bool m_bShowCPUcoreUsagesIconInTaskBar;
    bool m_bShowCPUcoresMultipliersIconInTaskBar;
    UserInterfaceAttributes();
    virtual
    ~UserInterfaceAttributes();
  };

}

#endif /* USERINTERFACEATTRIBUTES_HPP_ */
