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
#include <Controller/stdtstr.hpp>
#include <Controller/MSVC_adaption/tchar.h>

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
  BYTE GetPstatesDirPath(std::string & r_strCPUtypeRelativeDirPath ) ;
  BYTE GetPstateSettingsFileName( std::string & r_strPstateSettingsFileName ) ;
  static BYTE GetSupportedCPUs(//std::tstring & tstr
    std::vector<std::tstring> & r_stdvectstr
    )
  {
    std::tstring stdtstr ;
    #ifdef COMPILE_WITH_AMD_GRIFFIN
      stdtstr = _T("AMD Griffin") ;
      r_stdvectstr.push_back( stdtstr ) ;
    #endif
    stdtstr = _T("Intel Pentium M") ;
    r_stdvectstr.push_back( stdtstr ) ;
    return 1 ;
  }
  BYTE Init(
    Model & model 
    , UserInterface * p_userinterface 
    ) ;
  void SetCPUaccess(//ISpecificController 
    I_CPUaccess * p_ispecificcontroller ) ;
private:
  //ISpecificController 
  I_CPUaccess * mp_cpuaccessmethod ;
};

#endif	/* _MAINCONTROLLER_HPP */

