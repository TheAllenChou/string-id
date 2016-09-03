/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include "sidnet/server.h"

#include <string>

namespace sidnet
{

  class SidServer : public Server
  {
    public:
      SidServer(const char* dbFilePath = "siddb.db", const char* logFilePath = "siddb.log")
        : m_dbFilePath(dbFilePath)
        , m_logFilePath(logFilePath)
      { }

    protected:
      virtual void OnStart() override;
      virtual void OnShutDown() override;
      virtual int OnReceive(Socket *pSocket, const char *buffer, size_t size) override;

      void Save();

    private:
      std::string m_dbFilePath;
      std::string m_logFilePath;
  };

}
