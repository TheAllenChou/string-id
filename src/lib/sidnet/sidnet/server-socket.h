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
    private:
      ///underlying WinSock socket
      SOCKET m_wsSocket;

    public:
      ServerSocket();

      ///Listens for incoming connection.
      int Listen(const unsigned short port);

      ///Accepts incoming connection.
      Socket *Accept(int *pErrCode);
      Socket *Accept();

      ///Shuts down the server socket.
      int Shutdown();
  };
}
