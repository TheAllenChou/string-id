/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

namespace sidnet
{

  int Init(short majorVersion = 2, short minorVersion = 2);
  void ShutDown();

  enum class StringIdQueryCommand
  {
    kStringIdToString, 
    kStringToStringId, 
  };

}
