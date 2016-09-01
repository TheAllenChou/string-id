/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include "sidnet/socket.h"

namespace sidnet
{
  class ServerSocket
  {
    public:
      ServerSocket();

      // listen for incoming connection
      int Listen(const unsigned short port);

      // accept incoming connection
      Socket *Accept(int *pErrCode);
      Socket *Accept();

      // shut down the server socket
      int ShutDown();

    private:
      SOCKET m_wsSocket;
  };
}
