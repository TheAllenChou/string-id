/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidclient.h"

#include "siddb/siddb.h"
#include "sidnet/sidnet.h"

namespace sidnet
{
  
  SidClient::SidClient(StringIdToStringCallback sidToStrCallback, StringToStringIdCallback strToSidCallback)
    : m_sidToStrCallback(sidToStrCallback)
    , m_strToSidCallback(strToSidCallback)
  { }

  void SidClient::Find(StringId sid)
  {
    const size_t commandSize = sizeof(StringIdQueryCommand);
    const size_t sidValSize = sizeof(StringId::Storage);

    char buffer[commandSize + sidValSize];
    *reinterpret_cast<StringIdQueryCommand *>(buffer) = StringIdQueryCommand::kStringIdToString;
    *reinterpret_cast<StringId::Storage *>(buffer + commandSize) = sid.GetValue();

    Send(buffer, commandSize + sidValSize);
  }

  void SidClient::Find(const char *str)
  {
    const size_t commandSize = sizeof(StringIdQueryCommand);
    const size_t strLenSize = sizeof(size_t);
    const size_t maxStrSize = siddb::kMaxStrLen + 1;
    const size_t strLen = std::strlen(str);

    size_t bufferPos = 0;
    char buffer[commandSize + strLenSize + maxStrSize];

    *reinterpret_cast<StringIdQueryCommand *>(buffer + bufferPos) = StringIdQueryCommand::kStringToStringId;
    bufferPos += commandSize;

    *reinterpret_cast<size_t *>(buffer + bufferPos) = strLen;
    bufferPos += strLenSize;

    std::memcpy(buffer + bufferPos, str, strLen);
    bufferPos += strLen;

    Send(buffer, bufferPos);
  }
  
  int SidClient::OnReceive(const char *buffer, size_t size)
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

        const size_t strLenSize = sizeof(size_t);
        const size_t strLen = *reinterpret_cast<const size_t*>(buffer + bufferPos);
        bufferPos += strLenSize;

        const size_t maxStrSize = siddb::kMaxStrLen;
        char str[maxStrSize + 1];
        std::memcpy(str, buffer + bufferPos, strLen);
        str[strLen] = '\0';
        bufferPos += strLen;

        m_sidToStrCallback(sid, str);

        break;
      }

      case StringIdQueryCommand::kStringToStringId:
      {
        const size_t strLenSize = sizeof(size_t);
        const size_t strLen = *reinterpret_cast<const size_t*>(buffer + bufferPos);
        bufferPos += strLenSize;

        const size_t maxStrSize = siddb::kMaxStrLen;
        char str[maxStrSize + 1];
        std::memcpy(str, buffer + bufferPos, strLen);
        str[strLen] = '\0';
        bufferPos += strLen;

        const size_t sidValSize = sizeof(StringId::Storage);
        const StringId sid = StringId(*reinterpret_cast<const StringId::Storage *>(buffer + bufferPos));
        bufferPos += sidValSize;

        m_strToSidCallback(str, sid);

        break;
      }
    }

    return 0;
  }

}
