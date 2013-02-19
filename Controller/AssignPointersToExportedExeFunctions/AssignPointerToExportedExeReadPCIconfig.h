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
#endif
#ifdef __linux__
  #include <dlfcn.h> //for dlopen(...)
  #include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()

//  #include <Linux/AssignPointersToExportedExeFunctions/AssignPointerToExportedExeReadPCIconfigFunction.h>
  void AssignPointerToExportedExeReadPCIconfig(
    ReadPCIconfigSpace_func_type & pfn_readpciconfigspace_func
    )
  {
    //Adapted from http://linux.die.net/man/3/dlopen
    void * p_vModuleHandle = dlopen(
      //http://linux.die.net/man/3/dlopen:
      //"If filename is NULL, then the returned handle is for the main program."
      NULL
      , RTLD_LAZY
      ) ;
//    DEBUGN("p_vModuleHandle:" << p_vModuleHandle )
    if( p_vModuleHandle )
    {
      //from http://linux.die.net/man/3/dlopen
      char * pchError;
      dlerror();    /* Clear any existing error */
      pfn_readpciconfigspace_func = (ReadPCIconfigSpace_func_type) dlsym(
        p_vModuleHandle, "ReadPCIconfigSpace");
//      DEBUGN("pfn_readpciconfigspace_func:" << pfn_readpciconfigspace_func )
      if ( ( pchError = dlerror() ) )
      {
        DEBUGN ( "Error assigning function pointer:" << pchError );
      }
    }
  }
#endif

//void AssignPointerToExportedExeReadPCIconfig(
//  ReadPCIconfigSpace_func_type & pfn_readpciconfigspace_func ) ;

#endif /* ASSIGNPOINTERTOEXPORTEDEXEREADPCICONFIG_H_ */
