## String ID

String ID (SID) is a tool that converts strings into fix-sized hashed values, commonly used in games for looking up resources. The hash function is chosen to support string concatenation. This project uses FNV-1a hash.

Notable advantages of SIDs over strings:
  * Each SID takes up small constant amount of memory (the underlying integer type).
  * SID comparison is constant-time and typically more efficient than string comparison.
  * Looking up hash tables using SID keys is typically more efficient than using strings.
  * SIDs can be used in switch cases, which is not possible for strings (better efficiency & prettier syntax).
  * Concatenation of prefix SID and suffix string does not require original prefix string (saves memory).

Main disadvantage and solutions:
  * SIDs can be hard to debug, as they are just hashed integers, so it is preferrable to keep a database of string-SID pairs for two-way lookups, as well as a debugger plug-in to translate SIDs into corresponding strings in the watch window.
 
For more explanations, check out these articles:
  * [Preprocessed Strings for Asset IDs](http://www.randygaul.net/2015/12/11/preprocessed-strings-for-asset-ids/) by Randy Gaul
  * [Practical Hash IDs](http://cowboyprogramming.com/2007/01/04/practical-hash-ids/) by Mick West
  * [Hashed Strings](https://blog.molecular-matters.com/2011/06/24/hashed-strings/) by Stefan Reinalter
  * [Static Hash Values](http://bitsquid.blogspot.com/2010/10/static-hash-values.html) by Niklas

This repository contains several projects:  
  * sid: Core library (single header file).
  * siddb: SID database library example.
  * sidbot: Local SID database terminal example.
  * sidnet: Networking library for SID server and client.
  * sidserver: Remote SID database server example.
  * sidclient: Remove SID database client example.
  * sidvsdebug: Autoexp add-in for Visual Studio debugging (WIP).
  * unit-test: Library unit tests.

The core library (sid), database library example (siddb), and local database terminal example (sidbot) are platform-agnostic. The rest are targeted for Windows.

To build the projects on Windows, you need:  
  * Sublime Text 3 (optional): for convenient premake & build command shortcuts without having to open VS2015
  * Premake 5: for building VS2015 solution & projects
  * Visual Studio 2015: for building binaries


## Code Examples
```C++
/*
  The SID macro evaluates to a StringId object.
  The SID_VAL macro evaluates to a StringId's underlying raw integer data.
  These macros evaluate to constants at compiled-time if given compile-time constant strings.
  StringId::GerValue() returns a StringId's underlying raw integer data.
  There is intentionally no implicit conversion between StringId and the underlying raw integer type.
*/

// comparison
const StringId sid0 = SID("123");
const StringId sid1 = SID("abc");
ASSERT(sid0 != sid1);

// concatenation
const StringId sid2 = SID("12");
const StringId sid3 = sid2.Concat("3");
ASSERT(sid0 == sid3);

// switch cases
const StringId sid4 = SID("print");
switch (sid4.GetValue())
{
  case SID_VAL("print"):
    printStuff();
    break;

  case SID_VAL("draw"):
    drawStuff();
    break;
}

// asset look-up
Texture* pTexture = g_textureMgr.FindTexture(SID("cloud"));
```

----
**Ming-Lun "Allen" Chou**  
[AllenChou.net](http://AllenChou.net)  
[@TheAllenChou](http://twitter.com/TheAllenChou)  
