/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   SpecificCPUcoreActionData.hpp
 * Author: Stefan
 *
 * Created on 24. September 2009, 20:43
 */

#ifndef _SPECIFICCPUCOREACTIONDATA_HPP
#define	_SPECIFICCPUCOREACTIONDATA_HPP

#ifndef _WIN32 ///Avoid GCC "warning: "_tcslen" redefined"
  #include "global.h" //for BYTE
#endif

class SpecificCPUcoreActionAttributes
{
public:
  //BYTE
  unsigned char m_byCoreID ;
  SpecificCPUcoreActionAttributes() ;
  SpecificCPUcoreActionAttributes(//BYTE
    unsigned char byCoreID)
  {
    m_byCoreID = byCoreID ;
  }
  SpecificCPUcoreActionAttributes(const SpecificCPUcoreActionAttributes& orig);
  virtual ~SpecificCPUcoreActionAttributes();
//private:

};

#endif	/* _SPECIFICCPUCOREACTIONDATA_HPP */

