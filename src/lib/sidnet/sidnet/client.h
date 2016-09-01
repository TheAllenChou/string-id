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
      CRITICAL_SECTION m_socketCriticalSection;
      
    public:
      Client();
      ~Client();
      
      int Connect(const char *ipStr, const unsigned short port);
      int ShutDown();
      
    protected:
      // processe received message
      // return non-zero to end connection
      virtual int OnReceive(const char *buffer, size_t size);
      
    public:
      // send string
      int Send(const char *buffer, size_t size);
  };
}
