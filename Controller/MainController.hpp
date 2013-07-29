/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/* 
 * File:   MainController.hpp
 * Author: Stefan
 *
 * Created on 28. August 2009, 21:22
 */

#ifndef _MAINCONTROLLER_HPP
#define	_MAINCONTROLLER_HPP

#include <ModelData/ModelData.hpp>
//#include <UserInterface/UserInterface.hpp>
#include <Controller/character_string/stdtstr.hpp>
#include <Controller/MSVC_adaption/tchar.h>

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class //ISpecificController ;
  I_CPUaccess ;
class ICPUcoreUsageGetter ;
class I_CPUcontroller ;
class UserInterface ;

class MainController 
{
private:
  //ISpecificController
  I_CPUaccess * mp_cpuaccess ;
  I_CPUcontroller * mp_cpucontroller ;
  Model * mp_model ;
  UserInterface * mp_userinterface ;
public:
  inline void CreateBuiltInCPUcontrollerAndUsageGetter() ;
  BYTE CreateCPUcontrollerAndUsageGetter(
    I_CPUcontroller * & cpu
    , ICPUcoreUsageGetter * & r_p_icpucoreusagegetter) ;
  MainController( UserInterface * p_userinterface);
  MainController(const MainController& orig);
  virtual ~MainController();
  bool GetCPUvendorFamilyModelStepping();
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
//    stdtstr = _T("Intel Pentium M") ;
//    r_stdvectstr.push_back( stdtstr ) ;
    return 1 ;
  }
  BYTE ReadMainAndPstateConfig(
    Model & model 
    , UserInterface * p_userinterface 
    ) ;
  void ReadMainConfig(Model & model, UserInterface * p_userinterface ) ;
  BYTE ReadPstateConfig(
    Model & model
    , UserInterface * p_userinterface
    ) ;
  void SetAttributeData( Model * p_model )
  {
    mp_model = p_model ;
  }
  void SetCPUaccess(//ISpecificController 
    I_CPUaccess * p_ispecificcontroller ) ;
  BYTE ReadDLLName( std::string & r_stdstrFilePath ) ;
  void ReadRegisterDataConfig(
    std::string & strFamilyAndModelFilePath
    , UserInterface * p_userinterface
    ) ;
  void ReadRegisterDataConfig();
};

#endif	/* _MAINCONTROLLER_HPP */
