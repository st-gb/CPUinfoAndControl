/*
 * ControlOS_DVFSviaShell.hpp
 *
 *  Created on: Mar 20, 2013
 *      Author: stefan
 */

#ifndef CONTROLOS_DVFSVIASHELL_HPP_
#define CONTROLOS_DVFSVIASHELL_HPP_

#include <Controller/IDynFreqScalingAccess.hpp>
#include <stdlib.h> //system(...)

class ControlOS_DVFSviaShell
  : public IDynFreqScalingAccess
{
private:
  bool m_bEnablingDVFSsucceeded;
public:
  ControlOS_DVFSviaShell();
  virtual
  ~ControlOS_DVFSviaShell();
  bool ChangeOtherDVFSaccessPossible() { //from www.yolinux.com
    int ret = system("./cmd/changing_OS's_DVFS_is_possible.sh");
    return ret != -1;
}
  bool DisableFrequencyScalingByOS(const char * const shellfilePath) {
    //from www.yolinux.com
    int ret = system(/*"./cmd/disable_OS's_DVFS.sh"*/ shellfilePath);
    return ret != -1;
  }

  //Return value:
  //-already enabled
  //-
  unsigned char EnableFrequencyScalingByOS() {
    //from www.yolinux.com
    int ret = system("./cmd/enable_OS's_DVFS.sh");
    return ret != -1;
  }
  virtual bool EnablingIsPossible() { return false ; }
  string_type GetEnableDescription() {
    //return std::tstring(_T("") ) ;
    return std::wstring(L"" ) ;
  }
  //Name "Other DVFS" because the implementation may disable e.g. the OS's
  //DVFS or GNOME's DVFS (cpufreqd) , RMClock etc.
  bool OtherDVFSisEnabled()
  {
    //from www.yolinux.com
    int ret = system("./cmd/disable_OS's_DVFS.sh");
    return ret != -1;
  }
};

#endif /* CONTROLOS_DVFSVIASHELL_HPP_ */
