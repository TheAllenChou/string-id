/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidnet.h"
#include "sidnet/sidclient.h"

static bool s_shouldExit = false;

void StringIdToStringHandler(StringId sid, const char *str)
{
  // TODO
}

void StringToStringIdHandler(const char *str, StringId sid)
{
  // TODO
}

int main(int argc, const char** argv)
{
  // default config
  const char *cfgPath = "sidclient.cfg";
  int ip[4] = {127, 0, 0, 1};
  char ipStr[128];
  int port = 2266;

  FILE *pCfgFile = std::fopen(cfgPath, "r");
  if (pCfgFile)
  {
    std::fscanf(pCfgFile, "%d.%d.%d.%d:%d", ip, ip + 1, ip + 2, ip + 3, &port);
  }
  else if (pCfgFile = std::fopen(cfgPath, "w+"))
  {
    std::fprintf(pCfgFile, "%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], port);
  }
  if (pCfgFile)
  {
    std::fclose(pCfgFile);
  }

  std::sprintf(ipStr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  int err = 0;
  err = sidnet::Init();
  if (err)
    return err;

  sidnet::SidClient client(StringIdToStringHandler, StringToStringIdHandler);
  err = client.Connect(ipStr, port);
  if (!err)
    return err;

  while (!s_shouldExit)
  {
    char input[256];
    scanf_s("%s", input, int(sizeof(input)));
    if (!std::strcmp(input, "--exit"))
      s_shouldExit = true;
  }

  sidnet::ShutDown();

  return 0;
}
