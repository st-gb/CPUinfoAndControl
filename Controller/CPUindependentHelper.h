#pragma once 

////#include <yvals.h>
//#include <stdlib.h>
//#include "stdafx.h"
//#include "global.h"
//#ifdef _MSC_VER //MicroSoft C compiler
//  //#include <Windef.h> // DWORD
//  #include <Windows.h> //for DWORD, WINAPI etc.
//  //#include <StdAfx.h>
//  //#include "../StdAfx.h"
//#endif
#include <string>

//Use an object class like std::string instead of "char *". Advantage: 
//So there is no need to care about releasing memory.
//char * getBinaryRepresentation(char arch [],unsigned long ulValue//,long size
std::string getBinaryRepresentation(unsigned long ulValue//,long size
  ) ;
BYTE GetCoreIDFromAffinityMask(DWORD dwAffinityMask ) ;
//This function was created because there is no logarithm for base 2
//in the standard library.
double log_dualis(double fValue);
//Logarithm to a VARIABLE base.
double log_x(double dBase, double dValue) ;
