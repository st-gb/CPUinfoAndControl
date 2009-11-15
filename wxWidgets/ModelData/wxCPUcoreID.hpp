/* 
 * File:   wxCPUcoreID.hpp
 * Author: Stefan
 *
 * Created on 24. September 2009, 13:18
 */

#ifndef _WXCPUCOREID_HPP
#define	_WXCPUCOREID_HPP

#include "global.h" //for BYTE

#include <wx/object.h> //class wxObject

class wxCPUcoreID
  : 
  //needs to be public inherited, else no conversion to wxObject is possible
  public wxObject
{
public:
  BYTE m_byID ;
  //WORD w ;
  wxCPUcoreID(BYTE byID)
  {
    m_byID = byID ;
    //m_byID = 55 ;
    //w = 88 ;
  }
  wxCPUcoreID(const wxCPUcoreID& orig);
  virtual ~wxCPUcoreID();
private:

};

#endif	/* _WXCPUCOREID_HPP */

