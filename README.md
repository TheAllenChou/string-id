## String ID

String ID (SID) is a tool that converts character strings into fix-sized hashed values, commonly used in games for looking up resources. Also, SIDs can be used in switch cases, which is not possible for strings.

For more explanations, check out these articles:
  * [Preprocessed Strings for Asset IDs](http://www.randygaul.net/2015/12/11/preprocessed-strings-for-asset-ids/) by Randy Gaul
  * [Hashed Strings](https://blog.molecular-matters.com/2011/06/24/hashed-strings/) by Stefan Reinalter
  * [Static Hash Values](http://bitsquid.blogspot.com/2010/10/static-hash-values.html) by Niklas

This repository contains several projects:  
  * sid: Core library (single header file).
  * siddb: Lightweight database to keep track of SID hashes for debug purposes (not implemented yet).
  * unit-test: Library unit tests.

To build the projects, you need:  
  * Sublime Text 3 (optional): for convenient premake & build command shortcuts without having to open VS2015
  * Premake 5: for building VS2015 solution & projects
  * Visual Studio 2015: for building binaries


## Code Examples
```C++
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
```

----
**Ming-Lun "Allen" Chou**  
[AllenChou.net](http://AllenChou.net)  
[@TheAllenChou](http://twitter.com/TheAllenChou)  