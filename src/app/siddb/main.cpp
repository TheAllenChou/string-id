/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sid/sid.h"

/*
  This is a simple implementation of SID database, where data is stored linearly and unsorted.
  Ideally, you'll want to use a search structure like a binary search tree.
  Database file format: Number of data entries (U64) followed by a list of SID-string pairs.
*/

typedef unsigned long long U64;
static const size_t kMaxStrLen = 128;

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

const static char* kDatabaseFile = "siddb.db";
std::FILE* g_pDbFile = nullptr;

bool InitDatabase()
{
  g_pDbFile = std::fopen(kDatabaseFile, "r+");

  if (!g_pDbFile)
  {
    const U64 kZero = 0;
    g_pDbFile = std::fopen(kDatabaseFile, "w+");
    std::fwrite(&kZero, sizeof(kZero), 1, g_pDbFile);
  }

  if (!g_pDbFile)
    return false;

  return true;
}

void CleanDatabase()
{
  if (g_pDbFile)
  {
    std::fclose(g_pDbFile);
    g_pDbFile = nullptr;
  }

  std::remove(kDatabaseFile);

  std::printf ("SID database cleaned.\n");
}

void ListDatabase()
{
  if (!g_pDbFile)
      return;

  U64 numDataEntries = 0;
  std::fseek(g_pDbFile, 0, SEEK_SET);
  std::fread(&numDataEntries, sizeof(numDataEntries), 1, g_pDbFile);

  std::printf("Number of entries: %lld\n", numDataEntries);

  for (U64 i = 0; i < numDataEntries; ++i)
  {
    StringId::Storage sidVal;
    std::fread(&sidVal, sizeof(sidVal), 1, g_pDbFile);

    char buffer[kMaxStrLen];
    fscanf_s(g_pDbFile, "%s", buffer, (int)kMaxStrLen);
    std::fseek(g_pDbFile, 1, SEEK_CUR);
    
    std::printf("0x%016llx <-> %s\n", sidVal, buffer);
  }
}

bool FindStringId(StringId::Storage data, char* pOut)
{
  if (!g_pDbFile)
    return false;

  U64 numDataEntries = 0;
  std::fseek(g_pDbFile, 0, SEEK_SET);
  std::fread(&numDataEntries, sizeof(numDataEntries), 1, g_pDbFile);
  for (U64 i = 0; i < numDataEntries; ++i)
  {
    StringId::Storage sidVal;
    std::fread(&sidVal, sizeof(sidVal), 1, g_pDbFile);

    char buffer[kMaxStrLen];
    fscanf_s(g_pDbFile, "%s", buffer, (int) kMaxStrLen);
    std::fseek(g_pDbFile, 1, SEEK_CUR);

    if (data == sidVal)
    {
      std::strcpy(pOut, buffer);
      return true;
    }
  }

  return false;
}

bool FindStringId(const char* str, StringId::Storage* pOut)
{
  if (!g_pDbFile)
    return false;

  U64 numDataEntries = 0;
  std::fseek(g_pDbFile, 0, SEEK_SET);
  std::fread(&numDataEntries, sizeof(numDataEntries), 1, g_pDbFile);
  for (U64 i = 0; i < numDataEntries; ++i)
  {
    StringId::Storage sidVal;
    std::fread(&sidVal, sizeof(sidVal), 1, g_pDbFile);

    char buffer[kMaxStrLen];
    fscanf_s(g_pDbFile, "%s", buffer, (int)kMaxStrLen);
    std::fseek(g_pDbFile, 1, SEEK_CUR);
    if (!std::strcmp(str, buffer))
    {
      *pOut = sidVal;
      return true;
    }
  }

  return false;
}

void SaveStringId(const char* str)
{
  if (!g_pDbFile)
      return;

  U64 numDataEntries = 0;
  std::fseek(g_pDbFile, 0, SEEK_SET);
  std::fread(&numDataEntries, sizeof(numDataEntries), 1, g_pDbFile);
  ++numDataEntries;
  std::fseek(g_pDbFile, 0, SEEK_SET);
  std::fwrite(&numDataEntries, sizeof(numDataEntries), 1, g_pDbFile);

  std::fseek(g_pDbFile, 0, SEEK_END);

  const StringId::Storage sidVal = SID_VAL(str);
  std::fwrite(&sidVal, sizeof(sidVal), 1, g_pDbFile);

  const char kWhiteSpace = ' ';
  std::fwrite(str, 1, std::min(kMaxStrLen, std::strlen(str)), g_pDbFile);
  std::fwrite(&kWhiteSpace, 1, 1, g_pDbFile);
}

int main(int argc, const char** argv)
{
  if (!InitDatabase())
    return -1;

  char* ss = "asdf";
  StringId sid = SID(ss);

  switch (argc)
  {
    case 2:
      // option
      if (std::strlen(argv[1]) > 2 && argv[1][0] == '-' && argv[1][1] == '-')
      {
        const char* pOption = argv[1] + 2;
        if (!std::strcmp(pOption, "list"))
        {
          ListDatabase();
        }
        else if (!std::strcmp(pOption, "clean"))
        {
          CleanDatabase();
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

        char str[kMaxStrLen];
        if (FindStringId(sidVal, str))
        {
          std::printf("0x%016llx -> %s", sidVal, str);
        }
        else
        {
          std::printf("0x%016llx -> ???", sidVal);
        }
      }
      // dec
      else if (argv[1][0] >= '0' && argv[1][0] <= '9')
      {
        StringId::Storage sidVal;
        sscanf_s(argv[1], "%lld", &sidVal);

        char str[kMaxStrLen];
        if (FindStringId(sidVal, str))
        {
          std::printf("0x%016llx -> %s", sidVal, str);
        }
        else
        {
          std::printf("0x%016llx -> ???", sidVal);
        }
      }
      // string
      else
      {
        StringId::Storage sidVal;
        if (FindStringId(argv[1], &sidVal))
        {
          std::printf("%s -> 0x%016llx", argv[1], sidVal);
        }
        else
        {
          SaveStringId(argv[1]);
          std::printf("New entry saved:\n%s -> 0x%016llx", argv[1], SID_VAL(argv[1]));
        }
      }
      break;

    default:
      PrintUsage();
      break;
  }

  if (g_pDbFile)
  {
    std::fclose(g_pDbFile);
    g_pDbFile = nullptr;
  }

  return 0;
}
