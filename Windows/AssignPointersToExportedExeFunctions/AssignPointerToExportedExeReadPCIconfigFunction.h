/*
 * AssignPointerToExportedExeReadPCIconfigFunction.h
 *
 *  Created on: 14.12.2012
 *      Author: Stefan
 */

#ifndef ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIGFUNCTION_H_
#define ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIGFUNCTION_H_

void AssignPointerToExportedExeReadPCIconfig(
  ReadPCIconfigSpace_func_type & pfn_readpciconfigspace_func
  )
{
  //g_pfnReadPCIconfigSpace =
  pfn_readpciconfigspace_func = (ReadPCIconfigSpace_func_type) ::GetProcAddress(
    ::GetModuleHandle(NULL),
    "ReadPCIconfigSpace");
}

#endif /* ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIGFUNCTION_H_ */
