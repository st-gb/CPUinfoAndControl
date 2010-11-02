/*
 * DynLibCPUcoreUsageGetter.hpp
 *
 *  Created on: Oct 31, 2010
 *      Author: Stefan
 */

#ifndef DYNLIBCPUCOREUSAGEGETTER_HPP_
#define DYNLIBCPUCOREUSAGEGETTER_HPP_

typedef float (
  //WINAPI
  * _GetCPUcoreUsage) (WORD wCoreNumber);

class DynLibCPUcoreUsageGetter
{
protected:
  _GetCPUcoreUsage m_pfngetcpucoreusage ; //= NULL ;
} ;

#endif /* DYNLIBCPUCOREUSAGEGETTER_HPP_ */
