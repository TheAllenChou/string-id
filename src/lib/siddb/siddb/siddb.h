/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
*/
/******************************************************************************/

#pragma once

#include <cstdio>
#include <functional>

#include "sid/sid.h"

namespace siddb
{
  
  static const size_t kMaxStrLen = 256;

  typedef std::function<bool(StringId, const char *)> ForEachCallback;

  void Config(std::ostream &msgOutput, const char *logFilePath);

  void Add(const char *str);
  void Clean();
  void ForEach(ForEachCallback callback);
  size_t GetSize();

  bool Load(const char *filePath);
  bool Save(const char *filePath);

  bool Find(StringId sid, char *pOut);
  bool Find(const char *str, StringId *pOut);

}
