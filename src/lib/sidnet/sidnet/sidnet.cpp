/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http:// AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "sidnet/sidnet.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS // I want to reuse old code written in college so deal with it.
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

namespace sidnet
{

  int Init(short majorVersion, short minorVersion)
  {
    int err = 0;

    // our WSAData structure.  This holds information about the version of winsock that we are using
    // not necessarily the version that we are requesting
    WSADATA wsaData;
    // let's start with zeroing wsaData, not a bad idea
    SecureZeroMemory(&wsaData, sizeof(wsaData));

    // you must call this function to initialize winsock.  You must call WSACleanup when you are finished.
    // this uses a reference counter, so for each call to WSAStartup, you must call WSACleanup or suffer
    // memory issues
    err = WSAStartup(MAKEWORD(majorVersion, minorVersion), &wsaData);

    // check that WSAStartup was successful. 
    if(err)
    {
      std::cout << "Error initializing WinSock!  Error was: " << err << std::endl;
    }

    return 0;
  }

  void ShutDown()
  {
    // don't forget to call this at the end.  in total, call it as many times as you have called
    // WSAStartup()
    WSACleanup();
  }

}
  