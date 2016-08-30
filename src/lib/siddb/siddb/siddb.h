/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
*/
/******************************************************************************/

#pragma once

#include "sid/sid.h"

namespace siddb
{
  
  static const size_t kMaxStrLen = 256;

  FILE *GetFile();
  const char* GetPath();

  bool Open(const char *filePath);
  bool Clean();
  bool Close();

  bool GetSize(StringId::Storage *pOut);
  bool ForEach(std::function<bool(StringId, const char*)> callback);

  bool Find(StringId sid, char *pOut);
  bool Find(const char* str, StringId *pOut);

  bool Add(const char *str);

}
