/*
 * AssignPointerToExportedExeReadPCIconfigFunction.cpp
 *  (file extension must _not_ be ".c" because the include path for the C++
 *  compiler should be used)
 *
 *  Created on: Sep 8, 2010
 *      Author: Stefan
 */
#include <windows.h> //for GetProcAddress(...)
#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.

void AssignPointerToExportedExeReadPCIconfig(
  ReadPCIconfigSpace_func_type & pfn_readpciconfigspace_func
  )
{
  //g_pfnReadPCIconfigSpace =
  pfn_readpciconfigspace_func = (ReadPCIconfigSpace_func_type) ::GetProcAddress(
    ::GetModuleHandle(NULL),
    "ReadPCIconfigSpace");
}
