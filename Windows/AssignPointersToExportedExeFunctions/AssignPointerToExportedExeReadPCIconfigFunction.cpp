/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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

#include "AssignPointerToExportedExeReadPCIconfigFunction.h"
