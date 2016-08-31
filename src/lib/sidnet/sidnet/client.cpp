/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "client.h"

namespace sidnet
{
  DWORD WINAPI ClientReceiveMain(void *data)
  {
    ClientReceiveMainData *pCrmData = static_cast<ClientReceiveMainData *>(data);
    Client *pClient = pCrmData->m_pClient;
    delete pCrmData; //no longer needed
    
    size_t messageSize = 256;
    char *pBuffer = new char[messageSize];
    
    
    //main receive loop
    //TODO: find a way to stop client
    while (1)
    {
      size_t charsToReceive = 0;
      size_t dataSize = 0;
      
      
      //receive size_t portion (4 bytes)
      dataSize = charsToReceive = 4;
      while (charsToReceive)
      {
        int ret = pClient->m_pSocket->Read(pBuffer + (dataSize - charsToReceive), charsToReceive);
        
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
            pClient->m_pSocket->shutdown();
            return err;
          }
        }
        else if (ret == 0)
        {
          //connection ended
          return pClient->m_pSocket->shutdown();
        }
        
        //decrement chars to receive
        charsToReceive -= ret;
        Sleep(1);
      }
      
      
      //deserialize size_t
      size_t deserializedMessageSize = 0;
      deserializedMessageSize |= static_cast<unsigned char>(pBuffer[0]) << 24;
      deserializedMessageSize |= static_cast<unsigned char>(pBuffer[1]) << 16;
      deserializedMessageSize |= static_cast<unsigned char>(pBuffer[2]) <<  8;
      deserializedMessageSize |= static_cast<unsigned char>(pBuffer[3]) <<  0;
      
      //buffer not large enough, double each iteration
      if (deserializedMessageSize > messageSize)
      {
        delete pBuffer;
        
        do 
        {
          messageSize <<= 1;
        } while (deserializedMessageSize > messageSize);
        
        pBuffer = new char[messageSize];
      }
      
      
      //receive string portion
      dataSize = charsToReceive = deserializedMessageSize;
      while (charsToReceive)
      {
        int ret = pClient->m_pSocket->Read(pBuffer + (dataSize - charsToReceive), charsToReceive);
        
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
            pClient->m_pSocket->shutdown();
            return err;
          }
        }
        else if (ret == 0)
        {
          //connection ended
          return pClient->m_pSocket->shutdown();
        }

        //decrement chars to receive
        charsToReceive -= ret;
        Sleep(1);
      }
      
      
      //sufficient string information, call Client::OnReceive
      int ret = pClient->OnReceive(pBuffer, dataSize);
      if (ret)
      {
        //shut down
        pClient->m_pSocket->shutdown();
        break;
      }
    }
    
    return 0;
  }
  
  
  
  Client::Client()
    : m_pSocket(nullptr)
  {
    InitializeCriticalSection(&m_socketCriticalSection);
  }
  
  Client::~Client()
  {
    if (m_pSocket) delete m_pSocket;
    DeleteCriticalSection(&m_socketCriticalSection);
  }

  int Client::Connect(const char *ipAddress, const unsigned short port)
  {
    m_pSocket = new Socket();
    int ret = m_pSocket->connect(ipAddress, port);
    
    ClientReceiveMainData *data = new ClientReceiveMainData;
    data->m_pClient = this;
    
    //create client receive main thread
    CreateThread(0, 0, ClientReceiveMain, static_cast<void *>(data), 0, 0);
    
    if (ret) return ret;
    return 0;
  }
  
  int Client::ShutDown()
  {
    if (m_pSocket) return m_pSocket->shutdown();
    return 0;
  }

  int Client::OnReceive(const char *pBuffer, size_t size)
  {
    return 0;
  }
  
  int Client::Send(const char *pBuffer, size_t size)
  {
    //4-byte size_t for string size
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
      int ret = m_pSocket->Send(sizeBuffer + (4 - charsToSend), charsToSend);
        
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
          m_pSocket->shutdown();
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
      int ret = m_pSocket->Send(pBuffer + (size - charsToSend), charsToSend);
      
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
          m_pSocket->shutdown();
          return err;
        }
      }
      charsToSend -= ret;
      Sleep(1);
    }
    
    return 0;
  }
  
}