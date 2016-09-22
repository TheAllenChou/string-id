/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef SIDNET_SERVER_SOCKET_H
#define SIDNET_SERVER_SOCKET_H


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


#endif
