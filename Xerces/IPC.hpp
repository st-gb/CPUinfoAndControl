/*
 * IPCdataHandler.hpp
 *
 *  Created on: May 18, 2010
 *      Author: Stefan
 */

#ifndef IPC_HPP_
#define IPC_HPP_

#include <Controller/IPC/I_IPC_DataHandler.hpp>
#include <ModelData/ModelData.hpp>

class Model ;

namespace Xerces
{
  class IPCdataHandler
    : public I_IPC_DataHandler
  {
  public:
    Model & mr_model ;
    IPCdataHandler( Model & r_model ) ;
    void GetCurrentCPUcoreAttributeValues() ;
    DWORD IPCdataHandler::GetResponse(BYTE byCommand,
      BYTE * & r_arbyResponseData ) ;
  };
} ;

#endif /* IPC_HPP_ */
