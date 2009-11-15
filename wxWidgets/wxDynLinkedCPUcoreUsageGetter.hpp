#pragma once //include guard

#include <wx/dynlib.h> //for wxDynamicLibrary
#include "ModelData/CPUcoreData.hpp"
#include <Controller/ICPUcoreUsageGetter.hpp>

//class ICPUcoreUsageGetter ;

//WINAPI is replaced by __stdcall (a calling convention) that is needed 
//    because else runtime error may occur: something with CPU register "ESP".
typedef float (WINAPI * _GetCPUcoreUsage) (BYTE byCoreNumber);


class wxDynLinkedCPUcoreUsageGetter
  : public ICPUcoreUsageGetter
{
  bool m_bDLLsuccessfullyLoaded ;
  CPUcoreData * mp_cpucoredata ;
  _GetCPUcoreUsage pfnGetCPUcoreUsage ; //= NULL ;

  wxDynamicLibrary m_wxdynamiclibraryCPUcoreUsage ;

public:
  wxDynLinkedCPUcoreUsageGetter(
    std::string & strDLLname 
    , std::string & strDLLfunctionName
    , CPUcoreData & cpucoredata
    )
  {
    m_bDLLsuccessfullyLoaded = m_wxdynamiclibraryCPUcoreUsage.Load(
      strDLLname//, int flags = wxDL_DEFAULT
    ) ;
    mp_cpucoredata = & cpucoredata ;
    ////function signature (type), function name, wxDynLib object
    //wxDYNLIB_FUNCTION( _GetCPUcoreUsage, strDLLfunctionName , 
    //  m_wxdynamiclibraryCPUcoreUsage);
    if( m_wxdynamiclibraryCPUcoreUsage.IsLoaded() )
      pfnGetCPUcoreUsage = (_GetCPUcoreUsage) 
        m_wxdynamiclibraryCPUcoreUsage.GetSymbol(strDLLfunctionName) ;
  }

  BYTE Init() { return 0 ; }

  BYTE GetPercentalUsageForAllCores( float arf [] )
  {
    if( pfnGetCPUcoreUsage )
    {
      for(BYTE byCPUcoreNumber = 0 ; byCPUcoreNumber < 
        mp_cpucoredata->GetNumberOfCPUcores() ; ++ byCPUcoreNumber 
        )
#ifdef _DEBUG
      {
        float f = (*pfnGetCPUcoreUsage)(byCPUcoreNumber) ;
        arf[byCPUcoreNumber] = f ;
      }
#else
        arf[byCPUcoreNumber] = (*pfnGetCPUcoreUsage)(byCPUcoreNumber);
#endif
    }
    return 0 ;
  }
};
