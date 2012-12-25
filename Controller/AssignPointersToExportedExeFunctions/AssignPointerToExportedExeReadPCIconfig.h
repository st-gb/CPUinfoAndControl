/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
#ifdef _WIN32
  #include <Windows/AssignPointersToExportedExeFunctions/AssignPointerToExportedExeReadPCIconfigFunction.h>
#else
void AssignPointerToExportedExeReadPCIconfig(
  ReadPCIconfigSpace_func_type & pfn_readpciconfigspace_func ) ;
#endif

#endif /* ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_ */
