/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidnet.h"
#include "sidnet/sidserver.h"

void PrintUsage()
{
  std::cout << "USAGE:\n"
    "  --list\n"
    "    Lists all SID-string pairs in the database.\n"
    "  --help\n"
    "    Prints help.\n"
    "  --clean\n"
    "    Cleans the database.\n"
    "  --save <file name>\n"
    "    Save SID database to specified file name.\n"
    "  --load <file name>\n"
    "    Load SID database from specified file name.\n"
    "  <string ID>\n"
    "    Looks up corresponding string of specified string ID, \n"
    "    hex (e.g. 0xDEADBEEF) or decimal (e.g. 1048576).\n"
    "  <string>\n"
    "    Looks up corresponding string ID of specified string (unquoted).\n";
}

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
  if (err)
    return err;

  PrintUsage();

  bool shouldExit = false;
  while (!shouldExit)
  {
    char buffer[siddb::kMaxStrLen + 1];
    std::printf("sidserver>");
    scanf_s("%s", buffer, (int) sizeof(buffer));

    // option
    if (std::strlen(buffer) > 2 && buffer[0] == '-' && buffer[1] == '-')
    {
      const char* pOption = buffer + 2;

      // list
      if (!std::strcmp(pOption, "list"))
      {
        StringId::Storage numEntries = siddb::GetSize();
        std::printf("  Number of entries: %lld\n", numEntries);

        siddb::ForEach
        (
            [](StringId sid, const char* str) -> bool
            {
              std::printf("    0x%016llx <-> %s\n", sid.GetValue(), str);
              return true;
            }
        );
      }
      // clean
      else if (!std::strcmp(pOption, "clean"))
      {
        siddb::Clean();
        std::printf("  SID database cleaned.\n");
      }
      // save
      else if (!std::strcmp(pOption, "save"))
      {
        scanf_s("%s", buffer, (int) sizeof(buffer));
        if (siddb::Save(buffer))
        {
          std::printf("  SID database saved: %s\n", buffer);
        }
        else
        {
          std::printf("  Cannot save SID database: %s\n", buffer);
        }
      }
      // load
      else if (!std::strcmp(pOption, "load"))
      {
        scanf_s("%s", buffer, (int) sizeof(buffer));
        if (siddb::Load(buffer))
        {
          std::printf("  SID database loaded: %s\n", buffer);
        }
        else
        {
          std::printf("  Cannot load SID database: %s\n", buffer);
        }
      }
      // delete
      else if (!std::strcmp(pOption, "delete"))
      {
        scanf_s("%s", buffer, (int) sizeof(buffer));
        if (siddb::Delete(buffer))
        {
          std::printf("  SID database deleted: %s\n", buffer);
        }
        else
        {
          std::printf("  Cannot delete SID database: %s\n", buffer);
        }
      }
      // exit
      else if (!std::strcmp(pOption, "exit"))
      {
        shouldExit = true;
      }
      // help
      else /* if (!std::strcmp(pOption, "help")) */
      {
        PrintUsage();
      }
    }
    // hex
    else if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X'))
    {
      StringId::Storage sidVal;
      sscanf_s(buffer, "%llx", &sidVal);

      char str[siddb::kMaxStrLen];

      if (siddb::Find(StringId(sidVal), str))
      {
        std::printf("  0x%016llx -> %s\n", sidVal, str);
      }
      else
      {
        std::printf("  0x%016llx -> ???\n", sidVal);
      }
    }
    // dec
    else if (buffer[0] >= '0' && buffer[0] <= '9')
    {
      StringId::Storage sidVal;
      sscanf_s(buffer, "%lld", &sidVal);

      char str[siddb::kMaxStrLen];
      if (siddb::Find(StringId(sidVal), str))
      {
        std::printf("  0x%016llx -> %s\n", sidVal, str);
      }
      else
      {
        std::printf("  0x%016llx -> ???\n", sidVal);
      }
    }
    // string
    else
    {
      StringId sid;
      if (siddb::Find(buffer, &sid))
      {
        std::printf("  %s -> 0x%016llx\n", buffer, sid.GetValue());
      }
      else
      {
        siddb::Add(buffer);
        std::printf("  New entry saved:\n  %s -> 0x%016llx\n", buffer, SID_VAL(buffer));
      }
    }
  }

  server.ShutDown();

  sidnet::ShutDown();

  return 0;
}
