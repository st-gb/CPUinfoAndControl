//If not included: compiler error "C1010" with MSVC++.
#include "stdafx.h"
#include <Windows.h> //for HANDLE

//Make methods non-static (=member methods). So the handle to the DLL
//needn't be obtained for every call to a DLL function.
class MSRaccess
//  :public MSRaccessBase
{
private:
  HANDLE m_handleDLL;
public:
  BYTE changePstate(BYTE byPstate, BYTE byCoreNumber){};
};
