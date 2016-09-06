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

  void SidClient::SendFindRequest(StringId sid)
  {
    char buffer[StringIdToStringQueryFormat::GetFixedSize()];
    const size_t bufferSize = StringIdToStringQueryFormat::Write(buffer, Command::kStringIdToString, sid);

    Send(buffer, bufferSize);
  }

  void SidClient::SendFindRequest(const char *str)
  {
    char buffer[StringToStringIdQueryFormat::GetFixedSize() + siddb::kMaxStrLen];
    const size_t bufferSize = StringToStringIdQueryFormat::Write(buffer, Command::kStringToStringId, str);

    Send(buffer, bufferSize);
  }
  
  int SidClient::OnReceive(const char *buffer, size_t size)
  {
    Command command;
    buffer += CommandFormat::Read(buffer, command);

    switch (command)
    {
      case Command::kStringIdToString:
      {
        StringId sid;
        char str[siddb::kMaxStrLen + 1];
        buffer += StringIdFormat::Read(buffer, sid);
        buffer += StringFormat::Read(buffer, str);

        m_sidToStrCallback(sid, str);

        break;
      }

      case Command::kStringToStringId:
      {
        char str[siddb::kMaxStrLen + 1];
        StringId sid;
        buffer += StringFormat::Read(buffer, str);
        buffer += StringIdFormat::Read(buffer, sid);

        m_strToSidCallback(str, sid);

        break;
      }
    }

    return 0;
  }

}
