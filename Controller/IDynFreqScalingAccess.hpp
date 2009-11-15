#pragma once

class IDynFreqScalingAccess
{
public:
    //return: true=success
  virtual bool DisableFrequencyScalingByOS() = 0;
  virtual unsigned char EnableFrequencyScalingByOS() = 0;
  //"Other DVFS" because the implementaion may disably e.g. the OS's
  //DVFS or GNOME's DVFS, RMClock etc.
  virtual bool OtherDVFSisEnabled() 
  { 
    return false ;
  }
};
