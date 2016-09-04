/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "sid/sid.h"
#include "siddb/siddb.h"

void PrintUsage()
{
  std::cout << "USAGE:\n"
               "  - --list\n"
               "    Lists all SID-string pairs in the database.\n"
               "  - --help\n"
               "    Prints help.\n"
               "  - --clean\n"
               "    Cleans the database.\n"
               "  - <string ID>\n"
               "    Looks up corresponding string of provided string ID (hex or decimal).\n"
               "  - <string>\n"
               "    Looks up corresponding string ID of provided string (unquoted).\n";
}

int main(int argc, const char** argv)
{
  siddb::Config(std::cout, "siddb.log");
  siddb::Clean();

  bool shouldExit = false;
  while (!shouldExit)
  {
    char buffer[siddb::kMaxStrLen + 1];
    std::printf("sidbot>");
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

  return 0;
}
