/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef UNIT_TEST_PRECOMPILED_H
#define UNIT_TEST_PRECOMPILED_H


// CppUnit
#pragma warning (disable: 4267)
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#if defined _DEBUG
#pragma comment(lib, "cppunit_debug.lib")
#elif defined _RELEASE
#pragma comment(lib, "cppunit_release.lib")
#else
#pragma comment(lib, "cppunit.lib")
#endif

// STL
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>


#endif
