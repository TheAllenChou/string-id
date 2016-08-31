/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include "socket.h"

namespace sidnet
{
  class Client;
  
  struct ClientReceiveMainData
  {
    Client *m_pClient;
  };
  
  DWORD WINAPI ClientReceiveMain(void *data);
  
  class Client
  {
    friend DWORD WINAPI ClientReceiveMain(void *data);
    
    private:
      Socket *m_pSocket;
      CRITICAL_SECTION m_socketCriticalSection; //locking send and receive
      
    public:
      Client();
      ~Client();
      
      int Connect(const char *ipStr, const unsigned short port);
      int ShutDown();
      
    protected:
      ///Processes received message.
      ///Return non-zero to end connection.
      virtual int OnReceive(const char *pBuffer, size_t size);
      
    public:
      
      ///Sends string. Returns 0 when successful.
      int Send(const char *pBuffer, size_t size);
  };
}
