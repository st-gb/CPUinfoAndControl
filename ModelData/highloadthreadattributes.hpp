/* 
 * File:   HighLoadThreadAttributes.hpp
 * Author: Stefan
 *
 * Created on 24. September 2009, 20:45
 */

#ifndef _HIGHLOADTHREADATTRIBUTES_HPP
#define	_HIGHLOADTHREADATTRIBUTES_HPP

#include "StdAfx.h" //for WINAPI, LPVOID
//#include <windef.h> //WINAPI, LPVOID
#include "SpecificCPUcoreActionAttributes.hpp"

#ifdef _WIN32
  #define WINAPI __stdcall
#else
  #define WINAPI
#endif
#define far
typedef void far            * LPVOID;

class HighLoadThreadAttributes
  : public SpecificCPUcoreActionAttributes
{
public:
  DWORD ( WINAPI * mpfn_CalculationThreadProc) (LPVOID ) ;
  HighLoadThreadAttributes(
    BYTE byCoreID ,
    DWORD ( WINAPI * pfn_CalculationThreadProc) (LPVOID )
    )
  {
    mpfn_CalculationThreadProc = pfn_CalculationThreadProc ;
  }
  HighLoadThreadAttributes(const HighLoadThreadAttributes& orig);
  virtual ~HighLoadThreadAttributes();
//private:

};

#endif	/* _HIGHLOADTHREADATTRIBUTES_HPP */

