/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/socket.h"

namespace sidnet
{

  int InitWinSock(short majorVersion, short minorVersion)
  {
    int ret = 0;

    //our WSAData structure.  This holds information about the version of winsock that we are using
    //not necessarily the version that we are requesting
    WSADATA wsaData;
    //let's start with zeroing wsaData, not a bad idea
    SecureZeroMemory(&wsaData, sizeof(wsaData));

    //you must call this function to initialize winsock.  You must call WSACleanup when you are finished.
    //this uses a reference counter, so for each call to WSAStartup, you must call WSACleanup or suffer
    //memory issues
    ret = WSAStartup(MAKEWORD(majorVersion, minorVersion), &wsaData);

    //check that WSAStartup was successful. 
    if(ret)
    {
      std::cout << "Error initializing WinSock!  Error was: " << ret << std::endl;
    }

    return 0;
  }

  void CleanUpWinSock()
  {
    //don't forget to call this at the end.  in total, call it as many times as you have called
    //WSAStartup()
    WSACleanup();
  }

  Socket::Socket()
    : m_wsSocket(NULL)
    , m_ipStr(NULL)
  {
    InitializeCriticalSection(&m_ioCriticalSection);
  }

  Socket::Socket(SOCKET wsSocket)
  {
    m_wsSocket = wsSocket;
    
    //non-blocking
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

  int Socket::connect(const char *ipStr, const unsigned short port)
  {
    //return value
    int ret = 0;
    
    //store local address
    hostent* pLocalHost;
    pLocalHost = gethostbyname("");
    m_ipStr = inet_ntoa(*(in_addr*)*pLocalHost->h_addr_list);
    
    //local
    sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;
    localAddress.sin_addr.s_addr  = 
      inet_addr
      (
        inet_ntoa(*(in_addr*)*gethostbyname("")->h_addr_list)
      );
    localAddress.sin_port = 0;

    //remote
    sockaddr_in remoteAddress;
    remoteAddress.sin_family = AF_INET;
    remoteAddress.sin_addr.s_addr = inet_addr(ipStr);
    remoteAddress.sin_port = htons(port);

    //initialize socket
    m_wsSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    
    //error checking
    if (m_wsSocket == INVALID_SOCKET)
    {
      int err = WSAGetLastError();
      std::cout << "Error on creating socket: " << err << std::endl;
      return err;
    }
    
    //bind socket
    ret = bind(m_wsSocket, (sockaddr*)&localAddress, sizeof(localAddress));
    if (ret == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error on binding socket: " << err << std::endl;
      return err;
    }

    //connect
    ret = ::connect(m_wsSocket, (sockaddr*)&remoteAddress, sizeof(remoteAddress));
    if (ret == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error on connection: " << err << std::endl;
      return err;
    }
    
    //non-blocking
    u_long iMode = 1;
    ioctlsocket(m_wsSocket, FIONBIO, &iMode);

    return 0;
  }

  int Socket::shutdown()
  {
    int ret = ::shutdown(m_wsSocket, SD_SEND);
    
    if (ret == SOCKET_ERROR)
    {
      ret = WSAGetLastError();
      return ret;
    }

    //clean up
    ret = closesocket(m_wsSocket);
    if(ret <= 0)
    {
      ret = WSAGetLastError();
    }

    return 0;
  }

  int Socket::Read(char *buffer, size_t size)
  {
    EnterCriticalSection(&m_ioCriticalSection);
      int ret = ::recv(m_wsSocket, buffer, size, 0);
    LeaveCriticalSection(&m_ioCriticalSection);
    
    return ret;
  }

  int Socket::Send(const char *buffer, size_t size)
  {
    EnterCriticalSection(&m_ioCriticalSection);
      int ret = ::send(m_wsSocket, buffer, size, 0);
    LeaveCriticalSection(&m_ioCriticalSection);

    return ret;
  }

}
