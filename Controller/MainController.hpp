/* 
 * File:   MainController.hpp
 * Author: Stefan
 *
 * Created on 28. August 2009, 21:22
 */

#ifndef _MAINCONTROLLER_HPP
#define	_MAINCONTROLLER_HPP

#include <ModelData/ModelData.hpp>
#include <UserInterface.hpp>

class //ISpecificController ;
  I_CPUaccess ;
class I_CPUcontroller ;

class MainController 
{
  Model * mp_model ;
  I_CPUcontroller * mp_cpucontroller ;
public:
  I_CPUcontroller * CreateCPUcontrollerAndUsageGetter(
    ICPUcoreUsageGetter * & r_p_icpucoreusagegetter ) ;
  BYTE CreateCPUcontrollerAndUsageGetter(
    I_CPUcontroller * & cpu  
    , ICPUcoreUsageGetter * & r_p_icpucoreusagegetter) ;
  MainController();
  MainController(const MainController& orig);
  virtual ~MainController();
  void Init(
    Model & model 
    , UserInterface * p_userinterface 
    ) ;
  void SetCPUaccess(//ISpecificController 
    I_CPUaccess * p_ispecificcontroller ) ;
private:
  //ISpecificController 
  I_CPUaccess * mp_ispecificcontroller ;
};

#endif	/* _MAINCONTROLLER_HPP */

