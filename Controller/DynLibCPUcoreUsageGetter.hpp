/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/** DynLibCPUcoreUsageGetter.hpp
 *  Created on: Oct 31, 2010
 *  Author: Stefan */

#ifndef DYNLIBCPUCOREUSAGEGETTER_HPP_
#define DYNLIBCPUCOREUSAGEGETTER_HPP_

#include <windef.h>///WORD

typedef float (
  //WINAPI
  * _GetCPUcoreUsage)(WORD wCoreNumber);

class I_CPUaccess ;

#define INIT_LITERAL  "Init"

//WINAPI is replaced by __stdcall (a calling convention) that is needed 
//    because else runtime error may occur: something with CPU register "ESP".
typedef BYTE (
  //WINAPI
  * dll_usage_getter_init_type)(
      I_CPUaccess *
      //WORD wNumLogicalCPUcores
      );
typedef WORD (
  //WINAPI
  * dll_usage_getter_num_logical_cpu_cores_type)();


class DynLibCPUcoreUsageGetter
{
protected:
  _GetCPUcoreUsage m_pfngetcpucoreusage ; //= NULL ;
} ;

#endif /* DYNLIBCPUCOREUSAGEGETTER_HPP_ */
