/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef SIDNET_SOCKET_H
#define SIDNET_SOCKET_H


#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS // I want to reuse old code written in college so deal with it.
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

namespace sidnet
{

  class Socket
  {
    public:
      Socket();
      Socket(SOCKET wsSocket);
      ~Socket();
      
      const char *getIP() const;
      
      // connect to a remote socket
      int Connect(const char *ipStr, const unsigned short port = 0);

      // shuts down local socket
      int ShutDown();
      
      // send buffer to remote socket
      int Send(const char *buffer, const size_t size);

      // read buffer from remote socket
      int Read(char *buffer, const size_t size);

    private:
      SOCKET m_wsSocket;
      char *m_ipStr;
      CRITICAL_SECTION m_ioCriticalSection;
  };

}


#endif
