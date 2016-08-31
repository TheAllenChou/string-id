/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/server.h"

namespace sidnet
{
  //server accept main
  //call Socket::onConnect here
  DWORD WINAPI ServerAcceptMain(void *pData)
  {
    ServerAcceptThreadData *stData = static_cast<ServerAcceptThreadData *>(pData);
    Server *server = stData->server;
    delete stData; //not needed any more
    
    //continuously accept new client sockets
    while (1)
    {
      //break when shut down
      EnterCriticalSection(&(server->m_activeCriticalSection));
        if (!(server->m_active))
        {
          LeaveCriticalSection(&server->m_activeCriticalSection);
          break;
        }
      LeaveCriticalSection(&(server->m_activeCriticalSection));
      
      
      int err = 0;
      Socket *clientSocket = server->m_pServerSocket->Accept(&err);
      
      if (!clientSocket)
      {
        std::cout << "Error accepting socket: " << err << std::endl;
        continue;
      }
      
      //pass to Server::onAccept
      server->OnAccept(clientSocket);
      
      //start thread to handle client
      ServerClientHandlerData *schData = new ServerClientHandlerData;
      schData->server = server;
      schData->clientSocket = clientSocket;
      CreateThread(0, 0, ServerClientHandlerMain, static_cast<void *>(schData), 0, 0);
    }
    
    return 0;
  }
  
  //server client handler
  //call Server::OnReceive and Server::OnDisconnect here
  DWORD WINAPI ServerClientHandlerMain(void *pData)
  {
    ServerClientHandlerData *schData = static_cast<ServerClientHandlerData *>(pData);
    Server *server = schData->server;
    Socket *clientSocket = schData->clientSocket;
    delete schData; //no longer needed;
    
    
    unsigned int messageSize = 256;
    char *buffer = new char[messageSize];
    
    //start handling receive
    while (1)
    {
      //break when server is shut down
      EnterCriticalSection(&(server->m_activeCriticalSection));
      if (!(server->m_active))
      {
        LeaveCriticalSection(&server->m_activeCriticalSection);
        break;
      }
      LeaveCriticalSection(&(server->m_activeCriticalSection));
      
      
      int charsToReceive = 0;
      int dataSize = 0;
      
      //receive unsigned int portion (4 bytes)
      dataSize = charsToReceive = 4;
      while (charsToReceive)
      {
        int ret = clientSocket->Read(buffer + (dataSize - charsToReceive), charsToReceive);

        if (ret == SOCKET_ERROR)
        {
          int err = WSAGetLastError();
          if (err == WSAEWOULDBLOCK)
          {
            //non-blocking (not an error)
            Sleep(1);
            continue;
          }
          else
          {
            //error, shut down
            server->OnDisconnect(clientSocket);
            clientSocket->shutdown();
            delete clientSocket;
            return err;
          }
        }
        else if (ret == 0)
        {
          //client connection ended
          server->OnDisconnect(clientSocket);
          int ret =  clientSocket->shutdown();
          delete clientSocket;
          return ret;
        }

        //decrement chars to receive
        charsToReceive -= ret;
        Sleep(1);
      }
      
      
      //deserialize unsigned int
      unsigned int deserializedMessageSize = 0;
      deserializedMessageSize |= static_cast<unsigned char>(buffer[0]) << 24;
      deserializedMessageSize |= static_cast<unsigned char>(buffer[1]) << 16;
      deserializedMessageSize |= static_cast<unsigned char>(buffer[2]) <<  8;
      deserializedMessageSize |= static_cast<unsigned char>(buffer[3]) <<  0;

      //buffer not large enough, double each iteration
      if (deserializedMessageSize > messageSize)
      {
        delete buffer;

        do 
        {
          messageSize <<= 1;
        } while (deserializedMessageSize > messageSize);

        buffer = new char[messageSize];
      }


      //receive string portion
      dataSize = charsToReceive = deserializedMessageSize;
      while (charsToReceive)
      {
        int ret = clientSocket->Read(buffer + (dataSize - charsToReceive), charsToReceive);

        if (ret == SOCKET_ERROR)
        {
          int err = WSAGetLastError();
          if (err == WSAEWOULDBLOCK)
          {
            //non-blocking (not an error)
            Sleep(1);
            continue;
          }
          else
          {
            //error, shut down
            server->OnDisconnect(clientSocket);
            clientSocket->shutdown();
            return err;
          }
        }
        else if (ret == 0)
        {
          //client connection ended
          server->OnDisconnect(clientSocket);
          int ret = clientSocket->shutdown();
          delete clientSocket;
          return ret;
        }

        //decrement chars to receive
        charsToReceive -= ret;
        Sleep(1);
      }


      //sufficient string information, call Client::OnReceive
      int ret = server->OnReceive(clientSocket, buffer, dataSize);
      if (ret)
      {
        //shut down
        server->OnDisconnect(clientSocket);
        int ret = clientSocket->shutdown();
        delete clientSocket;
        return ret;
      }
    }
    
    return 0;
  }
  
  
  
  Server::Server()
    : m_active(false)
    , m_pServerSocket(nullptr)
  {
    InitializeCriticalSection(&m_activeCriticalSection);
  }
  
  Server::~Server()
  {
    if (m_pServerSocket)
    {
      m_pServerSocket->Shutdown();
      delete m_pServerSocket;
    }
    DeleteCriticalSection(&m_activeCriticalSection);
  }
  
  
  int Server::Start(const unsigned short port)
  {
    //server already started
    if (m_pServerSocket) return -1;
    
    m_pServerSocket = new ServerSocket();
    
    //start listen port
    int ret = m_pServerSocket->Listen(port);
    
    //start accept thread on successful listen
    if (ret == 0)
    {
      m_active = true;
      
      ServerAcceptThreadData *stData = new ServerAcceptThreadData;
      stData->server = this;
      
      //start accept thread
      CreateThread(0, 0, ServerAcceptMain, static_cast<void *>(stData), 0, 0);
    }
    
    return ret;
  }
  
  int Server::Send(Socket *pSocket, const char *pBuffer, size_t size)
  {
    //4-byte unsigned int for string size
    char sizeBuffer[4] = {0};
    sizeBuffer[0] = char((size & 0xFF000000) >> 24);
    sizeBuffer[1] = char((size & 0x00FF0000) >> 16);
    sizeBuffer[2] = char((size & 0x0000FF00) >>  8);
    sizeBuffer[3] = char((size & 0x000000FF) >>  0);


    size_t charsToSend = 0;

    //send string size (4 bytes)
    charsToSend = 4;
    while (charsToSend)
    {
      int ret = pSocket->Send(sizeBuffer + (4 - charsToSend), charsToSend);

      if (ret == SOCKET_ERROR)
      {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {
          //non-blocking (not an error)
          Sleep(1);
          continue;
        }
        else
        {
          //error, shut down
          pSocket->shutdown();
          return err;
        }
      }
      charsToSend -= ret;
      Sleep(1);
    }


    //send string
    charsToSend = size;
    while (charsToSend)
    {
      int ret = pSocket->Send(pBuffer + (size - charsToSend), charsToSend);

      if (ret == SOCKET_ERROR)
      {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {
          //non-blocking (not an error)
          Sleep(1);
          continue;
        }
        else
        {
          //error, shut down
          pSocket->shutdown();
          return err;
        }
      }
      charsToSend -= ret;
      Sleep(1);
    }

    return 0;
  }

  int Server::Shutdown()
  {
    return 0;
  }

  int Server::OnAccept(Socket *pSocket)
  {
    return 0;
  }
  
  int Server::OnReceive(Socket *pSocket, const char *pBuffer, size_t size)
  {
    return 0;
  }
  
  int Server::OnDisconnect(Socket *pSocket)
  {
    return 0;
  } 
}