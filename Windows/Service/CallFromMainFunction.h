/*
 * CallFromMainFunction.h
 *
 *  Created on: 20.09.2011
 *      Author: Stefan
 */

#ifndef CALLFROMMAINFUNCTION_H_
#define CALLFROMMAINFUNCTION_H_

#include <tchar.h> //TCHAR

//Forward decl.
class CPUcontrolService;

int CallFromMainFunction(int argc, //char
    TCHAR * argv[],
    CPUcontrolService * p_cpucontrolservice);
bool OpenLogFile(TCHAR * argv[]);

#endif /* CALLFROMMAINFUNCTION_H_ */
