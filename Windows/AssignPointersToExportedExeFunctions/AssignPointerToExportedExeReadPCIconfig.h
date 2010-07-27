/*
 * AssignPointerToExportedExeReadPCIconfig.h
 *
 *  Created on: Jul 14, 2010
 *      Author: Stefan
 */

#ifndef ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_
#define ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_

ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;

void AssignPointerToExportedExeReadPCIconfig()
{
  g_pfnReadPCIconfigSpace = (ReadPCIconfigSpace_func_type)::GetProcAddress(
    ::GetModuleHandle(NULL),
    "ReadPCIconfigSpace");
}

#endif /* ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_ */
