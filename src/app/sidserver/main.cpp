/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidnet.h"
#include "sidnet/sidserver.h"

static bool s_shouldExit = false;

int main(int argc, const char** argv)
{
  // default config
  const char *cfgPath = "sidserver.cfg";
  int port = 2266;

  FILE *pCfgFile = std::fopen(cfgPath, "r");
  if (pCfgFile)
  {
    std::fscanf(pCfgFile, "%d", &port);
  }
  else if (pCfgFile = std::fopen(cfgPath, "w+"))
  {
    std::fprintf(pCfgFile, "%d", port);
  }
  if (pCfgFile)
  {
    std::fclose(pCfgFile);
  }

  int err = 0;

  err = sidnet::Init();
  if (err)
    return err;

  sidnet::SidServer server;
  err = server.Start(port);
  if (!err)
    return err;

  while (!s_shouldExit)
  {
    
  }

  sidnet::ShutDown();

  return 0;
}
