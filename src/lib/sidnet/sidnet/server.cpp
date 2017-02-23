/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http:// AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/server.h"

namespace sidnet
{
  // server accept main
  // call Socket::onConnect here
  DWORD WINAPI ServerAcceptMain(void *pData)
  {
    ServerAcceptThreadData *pSatData = static_cast<ServerAcceptThreadData *>(pData);
    Server *pServer = pSatData->server;
    delete pSatData; // not needed any more
    
    // continuously accept new client sockets
    while (1)
    {
      // break when shut down
      EnterCriticalSection(&pServer->m_activeCriticalSection);
        if (!(pServer->m_active))
        {
          LeaveCriticalSection(&pServer->m_activeCriticalSection);
          break;
        }
      LeaveCriticalSection(&pServer->m_activeCriticalSection);
      
      
      int err = 0;
      Socket *pClientSocket = pServer->m_pServerSocket->Accept(&err);
      
      if (!pClientSocket)
      {
        std::cout << "Error accepting socket: " << err << std::endl;
        continue;
      }
      
      // pass to Server::onAccept
      pServer->OnAccept(pClientSocket);
      
      // start thread to handle client
      ServerClientHandlerData *schData = new ServerClientHandlerData;
      schData->server = pServer;
      schData->clientSocket = pClientSocket;
      CreateThread(0, 0, ServerClientHandlerMain, static_cast<void *>(schData), 0, 0);
    }
    
    return 0;
  }
  
  // server client handler
  // call Server::OnReceive and Server::OnDisconnect here
  DWORD WINAPI ServerClientHandlerMain(void *pData)
  {
    ServerClientHandlerData *pSchData = static_cast<ServerClientHandlerData *>(pData);
    Server *pServer = pSchData->server;
    Socket *pClientSocket = pSchData->clientSocket;
    delete pSchData; // no longer needed;
    
    
    unsigned int messageSize = 256;
    std::vector<char> buffer;
    buffer.resize(messageSize);
    
    // start handling receive
    while (1)
    {
      // break when server is shut down
      EnterCriticalSection(&(pServer->m_activeCriticalSection));
      if (!(pServer->m_active))
      {
        LeaveCriticalSection(&pServer->m_activeCriticalSection);
        break;
      }
      LeaveCriticalSection(&(pServer->m_activeCriticalSection));
      
      
      size_t charsToReceive = 0;
      size_t dataSize = 0;
      
      // receive unsigned int portion
      dataSize = charsToReceive = sizeof(size_t);
      while (charsToReceive)
      {
        int err = pClientSocket->Read(&buffer[0] + (dataSize - charsToReceive), charsToReceive);

        if (err == SOCKET_ERROR)
        {
          int err = WSAGetLastError();
          if (err == WSAEWOULDBLOCK)
          {
            // non-blocking (not an error)
            Sleep(1);
            continue;
          }
          else
          {
            // error, shut down
            pServer->OnDisconnect(pClientSocket);
            pClientSocket->ShutDown();
            delete pClientSocket;
            return err;
          }
        }
        else if (err == 0)
        {
          // client connection ended
          pServer->OnDisconnect(pClientSocket);
          int err =  pClientSocket->ShutDown();
          delete pClientSocket;
          return err;
        }

        // decrement chars to receive
        charsToReceive -= err;
        Sleep(1);
      }
      
      
      // deserialize size_t
      size_t deserializedMessageSize = 0;
      for (size_t i = 0; i < sizeof(size_t); ++i)
      {
        deserializedMessageSize |= static_cast<size_t>(buffer[i]) << (8 * (sizeof(size_t) - 1 - i));
      }

      // buffer not large enough, double each iteration
      if (deserializedMessageSize > messageSize)
      {
        do 
        {
          messageSize <<= 1;
        } while (deserializedMessageSize > messageSize);

        buffer.resize(messageSize);
      }


      // receive string portion
      dataSize = charsToReceive = deserializedMessageSize;
      while (charsToReceive)
      {
        int err = pClientSocket->Read(&buffer[0] + (dataSize - charsToReceive), charsToReceive);

        if (err == SOCKET_ERROR)
        {
          int err = WSAGetLastError();
          if (err == WSAEWOULDBLOCK)
          {
            // non-blocking (not an error)
            Sleep(1);
            continue;
          }
          else
          {
            // error, shut down
            pServer->OnDisconnect(pClientSocket);
            pClientSocket->ShutDown();
            return err;
          }
        }
        else if (err == 0)
        {
          // client connection ended
          pServer->OnDisconnect(pClientSocket);
          int err = pClientSocket->ShutDown();
          delete pClientSocket;
          return err;
        }

        // decrement chars to receive
        charsToReceive -= err;
        Sleep(1);
      }


      // sufficient string information, call Server::OnReceive
      int err = pServer->OnReceive(pClientSocket, &buffer[0], dataSize);
      if (err)
      {
        // shut down
        pServer->OnDisconnect(pClientSocket);
        int err = pClientSocket->ShutDown();
        delete pClientSocket;
        return err;
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
      m_pServerSocket->ShutDown();
      delete m_pServerSocket;
    }
    DeleteCriticalSection(&m_activeCriticalSection);
  }
  
  int Server::Start(const unsigned short port)
  {
    // server already started
    if (m_pServerSocket) return -1;
    
    m_pServerSocket = new ServerSocket();
    
    // start listen port
    int err = m_pServerSocket->Listen(port);
    
    // start accept thread on successful listen
    if (err == 0)
    {
      m_active = true;
      
      ServerAcceptThreadData *stData = new ServerAcceptThreadData;
      stData->server = this;
      
      // start accept thread
      CreateThread(0, 0, ServerAcceptMain, static_cast<void *>(stData), 0, 0);
    }
    
    return err;
  }
  
  int Server::Send(Socket *pSocket, const char *buffer, size_t size)
  {
    // serialize size_t
    char sizeBuffer[sizeof(size_t)];
    size_t deserializedMessageSize = 0;
    for (size_t i = 0; i < sizeof(size_t); ++i)
    {
      sizeBuffer[i] = static_cast<char>((size >> (8 * (sizeof(size_t) - 1 - i))) & 0xFF);
    }

    size_t charsToSend = 0;

    // send string size
    charsToSend = sizeof(size_t);
    while (charsToSend)
    {
      int err = pSocket->Send(sizeBuffer + (sizeof(size_t) - charsToSend), charsToSend);

      if (err == SOCKET_ERROR)
      {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {
          // non-blocking (not an error)
          Sleep(1);
          continue;
        }
        else
        {
          // error, shut down
          pSocket->ShutDown();
          return err;
        }
      }
      charsToSend -= err;
      Sleep(1);
    }

    // send string
    charsToSend = size;
    while (charsToSend)
    {
      int err = pSocket->Send(buffer + (size - charsToSend), charsToSend);

      if (err == SOCKET_ERROR)
      {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
        {
          // non-blocking (not an error)
          Sleep(1);
          continue;
        }
        else
        {
          // error, shut down
          pSocket->ShutDown();
          return err;
        }
      }
      charsToSend -= err;
      Sleep(1);
    }

    return 0;
  }

  int Server::ShutDown()
  {
    EnterCriticalSection(&m_activeCriticalSection);
      m_active = false;
    LeaveCriticalSection(&m_activeCriticalSection);

    return 0;
  }

}