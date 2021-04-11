/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard
#ifndef WX_DYN_LIB_CPU_CORE_USAGE_GETTER_HPP
#define WX_DYN_LIB_CPU_CORE_USAGE_GETTER_HPP

#include <wx/dynlib.h> //for wxDynamicLibrary
#include <ModelData/CPUcoreData.hpp>
#include <Controller/DynLibCPUcoreUsageGetter.hpp>
#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <string>

//class ICPUcoreUsageGetter ;

class wxDynLibCPUcoreUsageGetter
  : public ICPUcoreUsageGetter
    , public DynLibCPUcoreUsageGetter
{
  bool m_bDLLsuccessfullyLoaded ;
  CPUcoreData * mp_cpucoredata ;
//  _GetCPUcoreUsage m_pfngetcpucoreusage ; //= NULL ;
  dll_usage_getter_init_type m_pfn_dll_init_type ;
  dll_usage_getter_num_logical_cpu_cores_type
    m_pfn_dll_usage_getter_num_logical_cpu_cores_type ;

  wxDynamicLibrary m_wxdynamiclibraryCPUcoreUsage ;

public:
  I_CPUaccess * mp_cpuaccess ;
  wxDynLibCPUcoreUsageGetter(
    //std::string & strDLLname 
    wxString & r_wxstrFilePath 
    //, std::string & strDLLfunctionName
    , I_CPUaccess * p_cpuaccess
    , CPUcoreData & cpucoredata
    ) ;
  ~wxDynLibCPUcoreUsageGetter() ;

  WORD GetNumberOfCPUcores() ;
  WORD GetNumberOfLogicalCPUcores() ;

  BYTE GetPercentalUsageForAllCores( float arf [] ) ;

  void PossiblyAssignFunctionPointers();
  template <typename func_type> void PossiblyAssignFunctionPointer(
    const wxString & wxstrFuncName, func_type & p_function//, func_type functyp
    );

  BYTE Init() { return 0 ; }

};
#endif //#ifndef WX_DYN_LIB_CPU_CORE_USAGE_GETTER_HPP
