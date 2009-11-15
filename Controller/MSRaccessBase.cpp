//If not included: compiler error "C1010" with MSVC++.
#include "../stdafx.h"
#include "../global.h" //for type "BYTE"

//This class is the base class for classes implementing the Access to
//the ModelSpecific Registers

//I also could have put every single implementation (e.g. 
//Windows/MFC, wxWidgets, Linux) into a single controller class.
//but then I would have to use conditional compile preprocessor
//macros within the class (code is hard to read/maintain then).

//This is intended to be a superclass with implemented funkions 
//that do nothing ("{}") because so there the methods don't need 
//to be overwrittten as with pure virtual methods.
class MSRaccessBase
{
public:
  BYTE changePstate(BYTE byPstate, BYTE byCoreNumber){};
};
