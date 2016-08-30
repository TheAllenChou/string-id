/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
*/
/******************************************************************************/

/*
  This is a simple implementation of SID database, where data is stored linearly and unsorted.
  Ideally, you'll want to use a search structure like a binary search tree.
  Database file format: Number of data entries (U64) followed by a list of SID-string pairs.
*/

#include "siddb/siddb.h"

namespace siddb
{
  static FILE *g_pDbFile = nullptr;
  static const char *g_dbPath = "";

  FILE *GetFile()
  {
    return g_pDbFile;
  }

  const char *GetPath()
  {
    return g_dbPath;
  }

  bool Open(const char *filePath)
  {
    g_dbPath = filePath;
    g_pDbFile = std::fopen(g_dbPath, "r+");

    if (!g_pDbFile)
    {
      const StringId::Storage kZero = 0;
      g_pDbFile = std::fopen(g_dbPath, "w+");
      std::fwrite(&kZero, sizeof(kZero), 1, g_pDbFile);
    }

    if (!g_pDbFile)
      return false;

    return true;
  }

  bool Clean()
  {
    if (!g_pDbFile)
      return false;

    std::fclose(g_pDbFile);
    g_pDbFile = nullptr;

    std::remove(g_dbPath);
    g_dbPath = "";

    return true;
  }

  bool Close()
  {
    if (!g_pDbFile)
      return false;

    std::fclose(g_pDbFile);
    g_pDbFile = nullptr;
    g_dbPath = "";

    return true;
  }

  bool GetSize(StringId::Storage *pOut)
  {
    if (!g_pDbFile)
      return false;

    StringId::Storage numEntries = 0;
    std::fseek(g_pDbFile, 0, SEEK_SET);
    std::fread(&numEntries, sizeof(numEntries), 1, g_pDbFile);

    *pOut = numEntries;

    return true;
  }

  bool ForEach(std::function<bool(StringId, const char*)> callback)
  {
    StringId::Storage numEntries = 0;
    if (!GetSize(&numEntries))
      return false;

    for (StringId::Storage i = 0; i < numEntries; ++i)
    {
      StringId::Storage sidVal;
      std::fread(&sidVal, sizeof(sidVal), 1, g_pDbFile);

      char buffer[kMaxStrLen];
      fscanf_s(g_pDbFile, "%s", buffer, (int)kMaxStrLen);
      std::fseek(g_pDbFile, 1, SEEK_CUR);
      
      if (!callback(StringId(sidVal), buffer))
        break;
    }

    return true;
  }

  bool Find(StringId sid, char *pOut)
  {
    if (!g_pDbFile)
      return false;

    const StringId::Storage sidVal = sid.GetValue();

    StringId::Storage numDataEntries = 0;
    std::fseek(g_pDbFile, 0, SEEK_SET);
    std::fread(&numDataEntries, sizeof(numDataEntries), 1, g_pDbFile);
    for (StringId::Storage i = 0; i < numDataEntries; ++i)
    {
      StringId::Storage sidVal;
      std::fread(&sidVal, sizeof(sidVal), 1, g_pDbFile);

      char buffer[kMaxStrLen];
      fscanf_s(g_pDbFile, "%s", buffer, (int) kMaxStrLen);
      std::fseek(g_pDbFile, 1, SEEK_CUR);

      if (sidVal == sidVal)
      {
        std::strcpy(pOut, buffer);
        return true;
      }
    }

    return false;
  }

  bool Find(const char *str, StringId *pOut)
  {
    if (!g_pDbFile)
      return false;

    StringId::Storage numDataEntries = 0;
    std::fseek(g_pDbFile, 0, SEEK_SET);
    std::fread(&numDataEntries, sizeof(numDataEntries), 1, g_pDbFile);
    for (StringId::Storage i = 0; i < numDataEntries; ++i)
    {
      StringId::Storage sidVal;
      std::fread(&sidVal, sizeof(sidVal), 1, g_pDbFile);

      char buffer[kMaxStrLen];
      fscanf_s(g_pDbFile, "%s", buffer, (int)kMaxStrLen);
      std::fseek(g_pDbFile, 1, SEEK_CUR);
      if (!std::strcmp(str, buffer))
      {
        *pOut = StringId(sidVal);
        return true;
      }
    }

    return false;
  }

  bool Add(const char *str)
  {
    if (!g_pDbFile)
      return false;

    StringId sid;
    if (Find(str, &sid))
      return false;

    StringId::Storage numDataEntries = 0;
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

    return true;
  }

}
