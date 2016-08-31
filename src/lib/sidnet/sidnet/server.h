/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include "sidnet/server-socket.h"

namespace sidnet
{ 
  class Server;
  struct ServerAcceptThreadData
  {
    Server *server;
  };
  
  struct ServerClientHandlerData
  {
    Server *server;
    Socket *clientSocket;
  };
  
  DWORD WINAPI ServerAcceptMain(void *pData);
  DWORD WINAPI ServerClientHandlerMain(void *pData);
   
  class Server
  {
    friend DWORD WINAPI ServerAcceptMain(void *pData);
    friend DWORD WINAPI ServerClientHandlerMain(void *pData);
    
    private:
      bool m_active;
      CRITICAL_SECTION m_activeCriticalSection;
      ServerSocket *m_pServerSocket;
      
    public:
      Server();
      ~Server();
      
      ///Starts the server.
      int Start(const unsigned short port);
      
      ///Shuts down the server.
      int Shutdown();

      ///Sends string. Returns 0 when successful.
      int Send(Socket *pSocket, const std::string &message);

      ///Sends string. Returns 0 when successful.
      int Send(Socket *pSocket, const char *message);
      
    protected:
      ///Called when a new client socket is accepted.
      virtual int OnAccept(Socket *pSocket);
      
      ///Called when a received a message string from a client socket.
      virtual int OnReceive(Socket *pSocket, const std::string &message);
      
      ///Called when a client socket is disconnected.
      virtual int OnDisconnect(Socket *pSocket);
  };
}
