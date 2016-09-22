/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef SIDNET_SID_SERVER_H
#define SIDNET_SID_SERVER_H


#include "sidnet/server.h"

#include "siddb/siddb.h"
#include "sidnet/sidnet.h"

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

      void Save() { siddb::Save(m_dbFilePath.c_str()); }
      
      virtual void HandleStringIdNotFound(StringId sid) { }
      virtual void HandleStringNotFound(const char *str) { }
      virtual void HandleHashCollision(StringId sid, const char *str0, const char *str1) { }

    protected:
      virtual void OnStart() override;
      virtual void OnShutDown() override;
      virtual int OnReceive(Socket *pSocket, const char *buffer, size_t size) override;

    private:
      std::string m_dbFilePath;
      std::string m_logFilePath;
  };

}


#endif
