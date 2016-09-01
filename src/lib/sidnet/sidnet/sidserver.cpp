/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidserver.h"

#include "siddb/siddb.h"
#include "sidnet/sidnet.h"

namespace sidnet
{
  
  void SidServer::OnStart()
  {
    siddb::Open("siddb.db");
  }

  void SidServer::OnShutDown()
  {
    siddb::Close();
  }

  int SidServer::OnReceive(sidnet::Socket *pSocket, const char *buffer, size_t size)
  {
    size_t bufferPos = 0;

    const size_t commandSize = sizeof(StringIdQueryCommand);
    const StringIdQueryCommand command = *reinterpret_cast<const StringIdQueryCommand *>(buffer + bufferPos);
    bufferPos += commandSize;

    switch (command)
    {
      case StringIdQueryCommand::kStringIdToString:
      {
        const size_t sidValSize = sizeof(StringId::Storage);
        const StringId sid = StringId(*reinterpret_cast<const StringId::Storage *>(buffer + bufferPos));
        bufferPos += sidValSize;

        size_t strLen = 0;
        char str[siddb::kMaxStrLen + 1];
        if (siddb::Find(sid, str))
        {
          strLen = std::strlen(str);
        }

        const size_t strLenSize = sizeof(size_t);
        const size_t strMaxSize = siddb::kMaxStrLen + 1;

        size_t responseBufferPos = 0;
        char responseBuffer[commandSize + strLenSize + strMaxSize];
        
        *reinterpret_cast<StringIdQueryCommand *>(responseBuffer + responseBufferPos) = command;
        responseBufferPos += commandSize;

        *reinterpret_cast<size_t *>(responseBuffer + responseBufferPos) = strLen;
        responseBufferPos += strLenSize;

        std::memcpy(responseBuffer + responseBufferPos, str, strLen);
        responseBufferPos += strLen;

        pSocket->Send(responseBuffer, responseBufferPos);

        break;
      }

      case StringIdQueryCommand::kStringToStringId:
      {
        const size_t strLenSize = sizeof(size_t);
        const size_t strLen = *reinterpret_cast<const size_t *>(buffer + bufferPos);
        bufferPos += strLenSize;

        const size_t strLenMaxSize = siddb::kMaxStrLen;
        char str[strLenMaxSize + 1];
        std::memcpy(str, buffer + bufferPos, strLen);
        str[strLen + 1] = '\0';

        StringId sid = kInvalidStringId;
        siddb::Find(str, &sid);

        const size_t sidValSize = sizeof(StringId::Storage);

        size_t responseBufferPos = 0;
        char responseBuffer[commandSize + strLenSize + strLenMaxSize + sidValSize];

        *reinterpret_cast<StringIdQueryCommand *>(responseBuffer + responseBufferPos) = command;
        responseBufferPos += commandSize;

        *reinterpret_cast<size_t *>(responseBuffer + responseBufferPos) = strLen;
        responseBufferPos += strLenSize;

        std::memcpy(responseBuffer + responseBufferPos, str, strLen);
        responseBufferPos += strLen;

        *reinterpret_cast<StringId::Storage *>(responseBuffer + responseBufferPos) = sid.GetValue();
        responseBufferPos += sidValSize;

        pSocket->Send(responseBuffer, responseBufferPos);

        break;
      }
    }

    return 0;
  }

}

