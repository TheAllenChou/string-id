/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef SIDNET_H
#define SIDNET_H


#include "sid/sid.h"
#include "sidnet/buffer-format.h"

namespace sidnet
{

  int Init(short majorVersion = 2, short minorVersion = 2);
  void ShutDown();

  enum class Command : unsigned char
  {
    kStringIdToString, 
    kStringToStringId, 
  };

  typedef BufferFormat<Command> CommandFormat;
  typedef BufferFormat<StringId> StringIdFormat;
  typedef BufferFormat<char *> StringFormat;
  typedef BufferFormat<Command, StringId> StringIdToStringQueryFormat;
  typedef BufferFormat<Command, char *> StringToStringIdQueryFormat;
  typedef BufferFormat<Command, StringId, char *> StringIdToStringResponseFormat;
  typedef BufferFormat<Command, char *, StringId> StringToStringIdResponseFormat;

}


#endif
