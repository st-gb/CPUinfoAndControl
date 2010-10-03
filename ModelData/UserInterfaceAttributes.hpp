/*
 * UserInterfaceAttributes.hpp
 *
 *  Created on: Oct 1, 2010
 *      Author: Stefan
 */

#ifndef USERINTERFACEATTRIBUTES_HPP_
#define USERINTERFACEATTRIBUTES_HPP_

namespace Attributes
{

  class UserInterfaceAttributes
  {
  public:
    bool m_bSelectAllCPUcores ;
    bool m_bShowMainFrameAtStartup ;
    float m_fOperatingSafetyMarginInVolt ;
    unsigned short m_wMainFrameHeightInPixels ;
    unsigned short m_wMainFrameWidthInPixels ;
    unsigned short m_wMainFrameTopLeftCornerXcoordinateInPixels ;
    unsigned short m_wMainFrameTopLeftCornerYcoordinateInPixels ;
    unsigned short m_wToolTipDelay ;
    UserInterfaceAttributes();
    virtual
    ~UserInterfaceAttributes();
  };

}

#endif /* USERINTERFACEATTRIBUTES_HPP_ */
