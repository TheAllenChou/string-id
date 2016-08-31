/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

namespace sidnet
{

  ///Initializes WinSock
  int InitWinSock(short majorVersion = 2, short minorVersion = 2);

  ///Cleans up WinSock
  void CleanUpWinSock();

  class Socket
  {
    private:
      ///Underlying WinSock socket.
      SOCKET m_wsSocket;
      char *m_ipStr;
      CRITICAL_SECTION m_ioCriticalSection;

    public:
      Socket();
      Socket(SOCKET wsSocket);
      ~Socket();
      
      const char *getIP() const;
      
      ///Connects to a remote socket.
      int connect(const char *ipStr, const unsigned short port = 0);

      ///Shuts down local socket.
      int shutdown();
      
      
      ///Sends bytes to remote socket.
      int Send(const char *pBuffer, const size_t size);

      ///Read bytes from remote socket.
      int Read(char *pBuffer, const size_t size);
  };

}
