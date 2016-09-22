/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef SIDNET_SERVER_H
#define SIDNER_SERVER_H


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
    
    public:
      Server();
      ~Server();
      
      // start the server
      int Start(const unsigned short port);
      
      // shud down the server
      int ShutDown();

      // send buffer
      int Send(Socket *pSocket, const char *pBuffe, size_t size);
      
    protected:
      // called when started
      virtual void OnStart() { }

      // called when shut down
      virtual void OnShutDown() { }

      // called when a new client socket is accepted
      virtual void OnAccept(Socket *pSocket) { }
      
      // called when a received a message string from a client socket
      virtual int OnReceive(Socket *pSocket, const char *buffer, size_t size) { return 0; }
      
      // Called when a client socket is disconnected
      virtual void OnDisconnect(Socket *pSocket) { }

    private:
      bool m_active;
      CRITICAL_SECTION m_activeCriticalSection;
      ServerSocket *m_pServerSocket;
  };
}


#endif
