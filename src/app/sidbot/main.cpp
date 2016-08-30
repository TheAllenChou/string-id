/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sid/sid.h"
#include "siddb/siddb.h"

void PrintUsage()
{
  std::cout << "USAGE:\n"
               "  - siddb --list\n"
               "    Lists all SID-string pairs in the database.\n\n"
               "  - siddb --help\n"
               "    Prints help.\n\n"
               "  - siddb --clean\n"
               "    Cleans the database.\n\n"
               "  - siddb <string ID>\n"
               "    Looks up corresponding string of provided string ID (hex or decimal).\n\n"
               "  - siddb <string>\n"
               "    Looks up corresponding string ID of provided string (unquoted).\n\n";
}

int main(int argc, const char** argv)
{
  if (!siddb::Open("siddb.db"))
    return -1;

  switch (argc)
  {
    case 2:
      // option
      if (std::strlen(argv[1]) > 2 && argv[1][0] == '-' && argv[1][1] == '-')
      {
        const char* pOption = argv[1] + 2;
        if (!std::strcmp(pOption, "list"))
        {
          StringId::Storage numEntries = 0;
          if (!siddb::GetSize(&numEntries))
            return -1;

          std::printf("Number of entries: %lld\n", numEntries);

          siddb::ForEach
          (
            [](StringId sid, const char* str) -> bool
            {
              std::printf("0x%016llx <-> %s\n", sid.GetValue(), str);
              return true;
            }
          );
        }
        else if (!std::strcmp(pOption, "clean"))
        {
          if (!siddb::Clean())
            return -1;

          std::printf("SID database cleaned.\n");
        }
        else /* if (!std::strcmp(pOption, "help")) */
        {
          PrintUsage();
        }
      }
      // hex
      else if (argv[1][0] == '0' && (argv[1][1] == 'x' || argv[1][1] == 'X'))
      {
        StringId::Storage sidVal;
        sscanf_s(argv[1], "%llx", &sidVal);

        char str[siddb::kMaxStrLen];

        if (siddb::Find(StringId(sidVal), str))
        {
          std::printf("0x%016llx -> %s\n", sidVal, str);
        }
        else
        {
          std::printf("0x%016llx -> ???\n", sidVal);
        }
      }
      // dec
      else if (argv[1][0] >= '0' && argv[1][0] <= '9')
      {
        StringId::Storage sidVal;
        sscanf_s(argv[1], "%lld", &sidVal);

        char str[siddb::kMaxStrLen];
        if (siddb::Find(StringId(sidVal), str))
        {
          std::printf("0x%016llx -> %s\n", sidVal, str);
        }
        else
        {
          std::printf("0x%016llx -> ???\n", sidVal);
        }
      }
      // string
      else
      {
        StringId sid;
        if (siddb::Find(argv[1], &sid))
        {
          std::printf("%s -> 0x%016llx\n", argv[1], sid.GetValue());
        }
        else if (siddb::Add(argv[1]))
        {
          std::printf("New entry saved:\n%s -> 0x%016llx\n", argv[1], SID_VAL(argv[1]));
        }
      }
      break;

    default:
      PrintUsage();
      break;
  }

  siddb::Close();

  return 0;
}
