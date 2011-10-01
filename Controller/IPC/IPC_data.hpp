/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * IPC_data.hpp
 *
 *  Created on: 26.02.2011
 *      Author: Stefan
 */

#ifndef IPC_DATA_HPP_
#define IPC_DATA_HPP_

typedef unsigned char BYTE;
typedef unsigned short WORD;

#ifndef NULL
  #define NULL 0
#endif

class IPC_data
{
public:
  BYTE m_byCommand;
  BYTE * m_ar_byPipeDataRead;
  BYTE * m_ar_byDataToSend;
  WORD m_wPipeDataReadSizeInByte;
  WORD m_wDataToWriteSizeInByte;
  IPC_data()
    : m_ar_byPipeDataRead(NULL),
      m_ar_byDataToSend(NULL),
      m_wPipeDataReadSizeInByte(0),
      m_wDataToWriteSizeInByte(0)
  {

  }
};

#endif /* IPC_DATA_HPP_ */
