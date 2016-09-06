/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidserver.h"

namespace sidnet
{
  
  void SidServer::OnStart()
  {
    siddb::Load(m_dbFilePath.c_str());
  }

  void SidServer::OnShutDown()
  {
    Save();
  }

  int SidServer::OnReceive(sidnet::Socket *pSocket, const char *buffer, size_t size)
  {
    size_t bufferPos = 0;

    Command command;
    buffer += CommandFormat::Read(buffer, command);

    switch (command)
    {
      case Command::kStringIdToString:
      {
        StringId sid;
        buffer += StringIdFormat::Read(buffer, sid);

        char str[siddb::kMaxStrLen + 1] = { 0 };
        if (!siddb::Find(sid, str))
        {
          HandleStringIdNotFound(sid);
        }

        char responseBuffer[StringIdToStringResponseFormat::GetFixedSize() + siddb::kMaxStrLen];
        const size_t responseSize = StringIdToStringResponseFormat::Write(responseBuffer, Command::kStringIdToString, sid, str);

        Send(pSocket, responseBuffer, responseSize);

        break;
      }

      case Command::kStringToStringId:
      {
        char str[siddb::kMaxStrLen + 1];
        buffer += StringFormat::Read(buffer, str);

        StringId sid = kInvalidStringId;
        if (!siddb::Find(str, &sid))
        {
          HandleStringNotFound(str);
        }

        char responseBuffer[StringToStringIdResponseFormat::GetFixedSize() + siddb::kMaxStrLen];
        const size_t repsonseSize = StringToStringIdResponseFormat::Write(responseBuffer, Command::kStringToStringId, str, sid);

        Send(pSocket, responseBuffer, repsonseSize);

        break;
      }
    }

    return 0;
  }

}
