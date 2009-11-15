#pragma once //include guard

#include <wx/dynlib.h> //for wxDynamicLibrary
//#include "OlsApi.h" //to get the DLL function signatures
#include <windef.h> //for  WINAPI in OlsApiInitDef.h
#include <OlsApiInitDef.h> //to get the DLL function signatures

//Make methods non-static (=member methods). So the handle to the DLL
//needn't be obtained for every call to a DLL function.
class Controller
  //: public ControllerBase
  //: public CPUIDaccess
{
private:
  bool m_bDLLsuccessfullyLoaded ;
  //m_handleDLL;
  wxDynamicLibrary m_wxdynamiclibraryHWaccess ;
public:
  Controller() ;
  BYTE changePstate(BYTE byPstate, BYTE byCoreNumber) ;
  BYTE GetNumberOfPhysicalCores() ;
  BOOL // TRUE: success, FALSE: failure
  //WINAPI 
  WrmsrEx(
	  DWORD index,		// MSR index
	  DWORD eax,			// bit  0-31
	  DWORD edx,			// bit 32-63
	  DWORD affinityMask	// Thread Affinity Mask
  ) ;
  //{
  //  //if(m_wxdynamiclibraryHWaccess.IsLoaded())
  //  //  m_wxdynamiclibraryHWaccess.GetSymbol()
  //}
};
