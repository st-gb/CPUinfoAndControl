#pragma once //include guard
#include <wx/dynlib.h> //for wxDynamicLibrary

class CPUIDaccess
//  :public ICPUIDaccess
{
private:
  wxDynamicLibrary m_wxdynamiclibraryHWaccess ;
public:
  CPUIDaccess() ;
  BYTE GetNumberOfPhysicalCores() ;
};
