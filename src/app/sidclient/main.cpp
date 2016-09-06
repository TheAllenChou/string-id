/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidnet.h"
#include "sidnet/sidclient.h"

void StringIdToStringHandler(StringId sid, const char *str)
{
  if (std::strlen(str) > 0)
  {
    std::printf("  0x%016llx -> %s\n", sid.GetValue(), str);
  }
  else
  {
    std::printf("  0x%016llx -> ???\n", sid.GetValue());
  }
}

void StringToStringIdHandler(const char *str, StringId sid)
{
  std::printf("  %s -> 0x%016llx\n", str, sid.GetValue());
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
  if (err)
    return err;

  bool shouldExit = false;
  while (!shouldExit)
  {
    char buffer[256];
    std::printf("sidclient>");
    scanf_s("%s", buffer, (int) sizeof(buffer));

    // option
    if (std::strlen(buffer) > 2 && buffer[0] == '-' && buffer[1] == '-')
    {
      const char* pOption = buffer + 2;

      // exit
      if (!std::strcmp(pOption, "exit"))
      {
        shouldExit = true;
      }
    }
    // hex
    else if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X'))
    {
      StringId::Storage sidVal;
      sscanf_s(buffer, "%llx", &sidVal);

      client.SendFindRequest(StringId(sidVal));
    }
    // dec
    else if (buffer[0] >= '0' && buffer[0] <= '9')
    {
      StringId::Storage sidVal;
      sscanf_s(buffer, "%lld", &sidVal);

      client.SendFindRequest(StringId(sidVal));
    }
    // string
    else
    {
      client.SendFindRequest(buffer);
    }
  }

  sidnet::ShutDown();

  return 0;
}
