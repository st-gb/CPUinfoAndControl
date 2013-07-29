/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * I_IPC_DataHandler.hpp
 *
 *  Created on: May 18, 2010
 *      Author: Stefan
 */

#ifndef I_IPC_DATAHANDLER_HPP_
#define I_IPC_DATAHANDLER_HPP_

typedef unsigned char BYTE ;
typedef unsigned long DWORD ;

class I_IPC_DataHandler
{
public:
  I_IPC_DataHandler();
  virtual DWORD GetResponse(BYTE byCommand, BYTE * & r_arbyData ) { return 0 ; }
  virtual
  ~I_IPC_DataHandler();
};

#endif /* I_IPC_DATAHANDLER_HPP_ */
