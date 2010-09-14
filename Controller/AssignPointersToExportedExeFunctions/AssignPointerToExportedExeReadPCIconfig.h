/*
 * AssignPointerToExportedExeReadPCIconfig.h
 *
 *  Created on: Jul 14, 2010
 *      Author: Stefan
 */

#ifndef ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_
#define ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_

//ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;

//Implementation/ definition of this function should be in a source file within
//the appropriate folder ( "[...]/Windows[...]" / "[...]/Linux[...]" etc.)
void AssignPointerToExportedExeReadPCIconfig(
  ReadPCIconfigSpace_func_type & pfn_readpciconfigspace_func ) ;

#endif /* ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_ */
