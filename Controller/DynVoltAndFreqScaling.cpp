/* 
 * File:   DynVoltAndFreqScaling.cpp
 * Author: sgebauer
 * 
 * Created on 17. November 2009, 19:47
 */

#include "DynVoltAndFreqScaling.hpp"

//DynVoltAndFreqScaling::DynVoltAndFreqScaling() {
//}
DynVoltAndFreqScaling::DynVoltAndFreqScaling(
  ICPUcoreUsageGetter * p_icpu
  //, GriffinController * p_pumastatectrl
  , I_CPUcontroller * p_cpucontroller
  , CPUcoreData & r_cpucoredata
  )
  //Call the base class' c'tor so that its instructions are executed.
  : DynFreqScalingThreadBase(p_icpu, p_cpucontroller, r_cpucoredata)
{

}

//DynVoltAndFreqScaling::DynVoltAndFreqScaling(const DynVoltAndFreqScaling& orig) {
//}

DynVoltAndFreqScaling::~DynVoltAndFreqScaling() {
}

