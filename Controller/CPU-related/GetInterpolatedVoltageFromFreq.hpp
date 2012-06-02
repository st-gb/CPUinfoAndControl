/*
 * GetInterpolatedVoltageFromFreq.hpp
 *
 *  Created on: 11.04.2012
 *      Author: Stefan
 */

#ifndef GETINTERPOLATEDVOLTAGEFROMFREQ_HPP_
#define GETINTERPOLATEDVOLTAGEFROMFREQ_HPP_

#include <windef.h> //BYTE
#include <set> //class std::set
#include <ModelData/VoltageAndFreq.hpp> //class VoltageAndFreq
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)

BYTE //I_CPUcontroller::
  GetInterpolatedVoltageFromFreq(
  WORD wFreqInMHzToGetVoltageFrom
  , float & r_fVoltageInVolt
  , const std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
  );

#endif /* GETINTERPOLATEDVOLTAGEFROMFREQ_HPP_ */
