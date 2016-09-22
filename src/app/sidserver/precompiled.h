/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef SIDSERVER_PRECOMPILED_H
#define SIDSERVER_PRECOMPILED_H


// STL
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>

// Windows & Winsock
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS // I want to reuse old code written in college so deal with it.
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


#endif
