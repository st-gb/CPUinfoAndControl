/*
 * family10h.hpp
 *
 *  Created on: 26.02.2013
 *      Author: Stefan
 */

#ifndef FAMILY10H_HPP_
#define FAMILY10H_HPP_


namespace AMD { namespace family16 {
  float GetMultiplier2(unsigned frequencyID, unsigned divisorID)
  {
    //AMD K10 doc:
    //"The CPU COF specified by CpuFid and CpuDid is: CPU COF = 100 MHz *
    //(CpuFid + 10h) / (2^CpuDid)."
   return (float) (frequencyID + 0x10) / (float) ( 1 << divisorID );
  }

  float GetMultiplier(unsigned frequencyID, unsigned divisorID)
  {
    return GetMultiplier2(frequencyID, divisorID + 1);
  }
} }

#endif /* FAMILY10H_HPP_ */
