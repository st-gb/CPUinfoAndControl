/* 
 * File:   SpecificCPUcoreActionData.hpp
 * Author: Stefan
 *
 * Created on 24. September 2009, 20:43
 */

#ifndef _SPECIFICCPUCOREACTIONDATA_HPP
#define	_SPECIFICCPUCOREACTIONDATA_HPP

#include "global.h" //for BYTE

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

