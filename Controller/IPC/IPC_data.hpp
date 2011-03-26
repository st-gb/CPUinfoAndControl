/*
 * IPC_data.hpp
 *
 *  Created on: 26.02.2011
 *      Author: Stefan
 */

#ifndef IPC_DATA_HPP_
#define IPC_DATA_HPP_

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
