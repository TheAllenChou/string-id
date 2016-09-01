/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include "sidnet/server.h"

namespace sidnet
{

  class SidServer : public Server
  {
    protected:
      virtual void OnStart() override;
      virtual void OnShutDown() override;
      virtual int OnReceive(Socket *pSocket, const char *buffer, size_t size) override;
  };

}
