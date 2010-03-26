#pragma once //include guard

#include <wx/dynlib.h> //for wxDynamicLibrary
#include <ModelData/CPUcoreData.hpp>
#include <Controller/ICPUcoreUsageGetter.hpp>
#include <string>

//class ICPUcoreUsageGetter ;
class I_CPUaccess ;

//WINAPI is replaced by __stdcall (a calling convention) that is needed 
//    because else runtime error may occur: something with CPU register "ESP".
typedef float (
  //WINAPI
  * _GetCPUcoreUsage) (WORD wCoreNumber);
typedef void (
  //WINAPI
  * dll_usage_getter_init_type)(
      //I_CPUaccess *
      WORD wNumLogicalCPUcores
      );

class wxDynLibCPUcoreUsageGetter
  : public ICPUcoreUsageGetter
{
  bool m_bDLLsuccessfullyLoaded ;
  CPUcoreData * mp_cpucoredata ;
  _GetCPUcoreUsage m_pfngetcpucoreusage ; //= NULL ;
  dll_usage_getter_init_type m_pfn_dll_init_type ;

  wxDynamicLibrary m_wxdynamiclibraryCPUcoreUsage ;

public:
  wxDynLibCPUcoreUsageGetter(
    //std::string & strDLLname 
    wxString & r_wxstrFilePath 
    //, std::string & strDLLfunctionName
    , I_CPUaccess * p_cpuaccess 
    , CPUcoreData & cpucoredata
    ) ;

  BYTE Init() { return 0 ; }

  BYTE GetPercentalUsageForAllCores( float arf [] ) ;
};
