/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter 
  eAllenChou
 */
/******************************************************************************/

#include "server-socket.h"

namespace sidnet
{
  ServerSocket::ServerSocket()
  { }

  int ServerSocket::Listen(const unsigned short port)
  { //return value
    int ret = 0;

    //listener address
    sockaddr_in listenerAddress;
    hostent* pLocalHost;
    char* localIpStr;
    pLocalHost = gethostbyname("");
    localIpStr = inet_ntoa(*(in_addr*)*pLocalHost->h_addr_list);
    listenerAddress.sin_family = AF_INET;
    listenerAddress.sin_addr.s_addr  = inet_addr(localIpStr);
    listenerAddress.sin_port = htons(port);
    
    std::cout << "Local host: " << pLocalHost->h_name << std::endl;
    std::cout << "Local IP: " << localIpStr << std::endl;
    std::cout << "Port: " << port << std::endl;
    
    //listener socket
    m_wsSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

    //handle errors
    if(m_wsSocket == INVALID_SOCKET)
    {
      int err = WSAGetLastError();
      std::cout << "Error creating server socket: " << err << std::endl;
      return err;
    }
    
    //bind socket
    ret = bind(m_wsSocket, (SOCKADDR*)&listenerAddress, sizeof(listenerAddress));
    if(ret == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error binding server socket: " << err << std::endl;
      return err;
    }
    
    //listen for connection
    ret = ::listen(m_wsSocket, 10);
    if(ret == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error listening to port " << port << ": " << err << std::endl;
      return err;
    }
    
    return 0;
  }

  Socket *ServerSocket::Accept(int *pErrorCode)
  {
    SOCKET client;
    sockaddr_in clientEndpoint;
    int clientEndpointSize = sizeof(clientEndpoint);
    SecureZeroMemory(&clientEndpoint, clientEndpointSize);
    client = ::accept(m_wsSocket, (sockaddr*)&clientEndpoint, &clientEndpointSize);
    
    //non-blocking
    u_long iMode = 1;
    ioctlsocket(client, FIONBIO, &iMode);
    
    if(client == INVALID_SOCKET)
    {
      
      int err = WSAGetLastError();
      *pErrorCode = err;
      std::cout << "Error accepting client: " << err << std::endl;
      return NULL;
    }
    
    return new Socket(client);
  }

  Socket *ServerSocket::Accept()
  {
    int *error = new int;
    Socket *socket = Accept(error);
    delete error;
    return socket;
  }

  int ServerSocket::Shutdown()
  {
    int ret = 0;
    
    ret = ::shutdown(m_wsSocket, SD_BOTH);
    if (ret == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error shutting down server socket." << std::endl;
      return err;
    }
    
    ret = ::closesocket(m_wsSocket);
    if (ret == SOCKET_ERROR)
    {
      int err = WSAGetLastError();
      std::cout << "Error closing server socket." << std::endl;
      return err;
    }
    
    return 0;
  }
}