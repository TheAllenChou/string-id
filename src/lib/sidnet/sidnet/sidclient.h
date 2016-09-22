/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef SIDNET_SID_CLIENT_H
#define SIDNET_SID_CLIENT_H


#include <functional>

#include "sid/sid.h"
#include "sidnet/client.h"

namespace sidnet
{

  class SidClient : public Client
  {
    public:
      typedef std::function<void(StringId, const char *)> StringIdToStringCallback;
      typedef std::function<void(const char *, StringId)> StringToStringIdCallback;

      SidClient(StringIdToStringCallback sidToStrCallback, StringToStringIdCallback strToSidCallback);

      void SendFindRequest(StringId sid);
      void SendFindRequest(const char *str);

    protected:
      virtual int OnReceive(const char *buffer, size_t size);

    private:
      StringIdToStringCallback m_sidToStrCallback;
      StringToStringIdCallback m_strToSidCallback;
  };

}


#endif
