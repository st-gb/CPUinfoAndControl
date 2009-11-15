//If not included: compiler error "C1010" with MSVC++.
#include "stdafx.h"

#include <wx/dynlib.h> //for wxDynamicLibrary
#include <wx/utils.h> //for ::wxSleep
//#include "OlsApi.h" //to get the DLL function signatures

#include <OlsApiInitDef.h> //to get the DLL function signatures
#include "Controller.hpp"

//don't compile as inline:
//#define PUMASTATECTRL_INLINE_OPTION
#define PUMASTATECTRL_INLINE_OPTION inline
//__forceinline is MS specific: "The __forceinline keyword overrides the cost/benefit analysis and relies on the judgment of the programmer instead."
//#define PUMASTATECTRL_INLINE_OPTION __forceinline

////Make methods non-static (=member methods). So the handle to the DLL
////needn't be obtained for every call to a DLL function.
//class Controller
//  : public ControllerBase
//{
//private:
//  bool m_bDLLsuccessfullyLoaded ;
//  //m_handleDLL;
//  wxDynamicLibrary m_wxdynamiclibraryHWaccess ;
//public:
//}

Controller::Controller()
  :m_bDLLsuccessfullyLoaded(false)
{
  m_bDLLsuccessfullyLoaded = m_wxdynamiclibraryHWaccess.Load(
    "WinRing0.dll"//, int flags = wxDL_DEFAULT
    ) ;
}

BYTE Controller::changePstate(BYTE byPstate, BYTE byCoreNumber)
{
  //if(m_bDLLsuccessfullyLoaded)
  if(m_wxdynamiclibraryHWaccess.IsLoaded() )
  {
    //m_wxdynamiclibraryHWaccess.GetSymbol
    //wxDYNLIB_FUNCTION(WrmsrEx, WrmsrEx, m_wxdynamiclibraryHWaccess)
    wxDYNLIB_FUNCTION(_WrmsrEx, WrmsrEx, m_wxdynamiclibraryHWaccess);
    //the name "pfnWrmsrEx" was created by "wxDYNLIB_FUNCTION"
    //pfnWrmsrEx->
  }
  //  m_wxdynamiclibraryHWaccess
  return 1 ;
}

////If this method is inline it's faster
////PUMASTATECTRL_INLINE_OPTION void Sleep(WORD wMilliSeconds)
//PUMASTATECTRL_INLINE_OPTION void Controller::Sleep(WORD wMilliSeconds)
//{
//  ::wxSleep(wMilliSeconds) ;
//}

BOOL // TRUE: success, FALSE: failure
  //WINAPI 
Controller::WrmsrEx(
  DWORD index,		// MSR index
  DWORD eax,			// bit  0-31
  DWORD edx,			// bit 32-63
  DWORD affinityMask	// Thread Affinity Mask
)
{
  //if(m_bDLLsuccessfullyLoaded.IsLoaded())
  //  m_bDLLsuccessfullyLoaded
  return TRUE ;
}
