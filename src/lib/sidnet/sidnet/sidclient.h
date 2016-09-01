/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

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

      void Find(StringId sid);
      void Find(const char *str);

    protected:
      virtual int OnReceive(const char *buffer, size_t size);

    private:
      StringIdToStringCallback m_sidToStrCallback;
      StringToStringIdCallback m_strToSidCallback;
  };

}
