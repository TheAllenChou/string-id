/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http:// AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/socket.h"

namespace sidnet
{

  Socket::Socket()
    : m_wsSocket(NULL)
    , m_ipStr(NULL)
  {
    InitializeCriticalSection(&m_ioCriticalSection);
  }

  Socket::Socket(SOCKET wsSocket)
  {
    m_wsSocket = wsSocket;
    
    // non-blocking
    u_long iMode = 1;
    ioctlsocket(m_wsSocket, FIONBIO, &iMode);
    
    InitializeCriticalSection(&m_ioCriticalSection);
  }

  Socket::~Socket()
  {
    DeleteCriticalSection(&m_ioCriticalSection);
  }

  const char *Socket::getIP() const
  {
    return m_ipStr;
  }

  int Socket::Connect(const char *ipStr, const unsigned short port)
  {
    // return value
    int err = 0;
    
    // store local address
    hostent* pLocalHost;
    pLocalHost = gethostbyname("");
    m_ipStr = inet_ntoa(*(in_addr*)*pLocalHost->h_addr_list);
    
    // local
    sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;
    localAddress.sin_addr.s_addr  = 
      inet_addr
      (
        inet_ntoa(*(in_addr*)*gethostbyname("")->h_addr_list)
      );
    localAddress.sin_port = 0;

    // remote
    sockaddr_in remoteAddress;
    remoteAddress.sin_family = AF_INET;
    remoteAddress.sin_addr.s_addr = inet_addr(ipStr);
    remoteAddress.sin_port = htons(port);

    // initialize socket
    m_wsSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    
    // error checking
    if (m_wsSocket == INVALID_SOCKET)
    {
      int err = WSAGetLastError();
      std::cout << "Error on creating socket: " << err << std::endl;
      return err;
    }
    
    // bind socket
    err = bind(m_wsSocket, (sockaddr*)&localAddress, sizeof(localAddress));
    if (err == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error on binding socket: " << err << std::endl;
      return err;
    }

    // connect
    err = ::connect(m_wsSocket, (sockaddr*)&remoteAddress, sizeof(remoteAddress));
    if (err == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error on connection: " << err << std::endl;
      return err;
    }
    
    // non-blocking
    u_long iMode = 1;
    ioctlsocket(m_wsSocket, FIONBIO, &iMode);

    return 0;
  }

  int Socket::ShutDown()
  {
    int err = ::shutdown(m_wsSocket, SD_SEND);
    
    if (err == SOCKET_ERROR)
    {
      err = WSAGetLastError();
      return err;
    }

    // clean up
    err = closesocket(m_wsSocket);
    if(err <= 0)
    {
      err = WSAGetLastError();
    }

    return 0;
  }

  int Socket::Read(char *buffer, size_t size)
  {
    EnterCriticalSection(&m_ioCriticalSection);
      int err = ::recv(m_wsSocket, buffer, size, 0);
    LeaveCriticalSection(&m_ioCriticalSection);
    
    return err;
  }

  int Socket::Send(const char *buffer, size_t size)
  {
    EnterCriticalSection(&m_ioCriticalSection);
      int err = ::send(m_wsSocket, buffer, size, 0);
    LeaveCriticalSection(&m_ioCriticalSection);

    return err;
  }

}
