------------------------------------------------------------------
-- premake 5 test solution
------------------------------------------------------------------

local solutionName = "string-id"

-- Table of windowed application projects to create.  These names should have the
--   same name as the folder the project's source code is located in
local winApps =
{
  
}
-- Table of console application projects to create.  These names should have the
--   same name as the folder the project's source code is located in
local consoleApps =
{
  "siddb",      -- SID database
  "sidpp",      -- SID preprocessor
  "unit-test",  -- unit test
}
-- Table of projects that use Unicode.  These names should have the
--   same name as the folder the project's source code is located in
local unicodeApps = 
{
  
}
-- Table of static librari projects to create.  These names should have the
--   same name as the folder the project's source code is located in.
local staticLibs =
{
  "sid",  -- SID library
}
-- Table of static librari projects to create.  These names should have the
--   same name as the folder the project's source code is located in.
local dynamicLibs = 
{
  "sidvsdebug",  -- SID Visual Studio autoexp addin
}

                       
-- Table of paths to external libraries' include folder
local includeDirs =
{
  "./lib/CppUnit/Include", 
}
-- Table of paths to external libraries lib folder
local libDirs =
{
  "./lib/CppUnit/Lib", 
}
-- Table of paths to external libraries lib files
local libFiles = 
{
  
}
-- Table of flags for different game configurations
-- possible flag values can be found at
-- http://industriousone.com/flags
local debugFlags =
{
  "Symbols",
  "NoMinimalRebuild",
  "NoEditAndContinue",
  "WinMain"
}
local releaseFlags =
{
  "Symbols",
  "NoMinimalRebuild",
  "NoEditAndContinue",
  "WinMain"
}
local shipFlags =
{
  "Symbols", 
  "NoMinimalRebuild",
  "NoEditAndContinue",
  "WinMain"
}

-- Table of linker options
local linkerOptions = 
{
  "/ignore:4075", 
  "/ignore:4098", 
  "/ignore:4099"
}

-- Table of preprocessor definitions for different game configuration
local debugDefines = 
{
  "_DEBUG",
  "_CRT_SECURE_NO_WARNINGS"
}
local releaseDefines =
{
  "_NDEBUG", 
  "_RELEASE", 
  "_CRT_SECURE_NO_WARNINGS"
}
local shipDefines = 
{
  "_NDEBUG", 
  "_SHIP", 
  "_CRT_SECURE_NO_WARNINGS"
}

-- paths
-- Variables for paths to files, in case project layout changes
local solutionDir = "./sln"
local projectDir  = solutionDir
local objDir      = "./obj"
local sourceDir   = "./src"
local outDir      = "./bin"
local debugDir    = "./working"


-- Search through all directories and subdirectories of path and return all of them
-- t - file directory table (to be filled)
-- paht - the string name of the directory to search
function FindDirectoriesRecursive(t, path)
  for dir in io.popen("dir " .. "\"" .. path .. "\"" .. " /b /ad"):lines()
  do
    table.insert(t, path .. "/" .. dir)
    FindDirectoriesRecursive(t, path .. "/" .. dir)
  end
end

-- Opens file and stores all project dependencies into a table
function ReadDependencies(fileName)
  t = {}
  
  -- check file existence
  local f = io.open(fileName)
  if f == nil then
    return t
  else
    io.close(f)
  end
  
  -- add dependencies
  for line in io.lines(fileName)
  do
    table.insert(t, line)
  end
  
  return t
end

-- This function sets up a visual studio project
local allProjectIncludeDirs = {}
function SetUpProj(projName, projType, locPath, pchFile, fileDir, outDir)
  kind(projType)
  language("C++")
  location(locPath)
  libdirs(libDirs)
  objdir(objDir)
  debugdir(debugDir)
  linkoptions(linkerOptions)
  architecture("x64")
  includedirs(fileDir)
  
  -- recursively scan project directory
  directories = {fileDir}
  FindDirectoriesRecursive(directories, fileDir)
  
  -- include library projects
  for _, proj in ipairs(staticLibs) do
    includedirs(sourceDir .. "/lib/" .. proj)
  end
  for _, proj in ipairs(dynamicLibs) do
    includedirs(sourceDir .. "/dll/" .. proj)
  end
  
  -- additional includes
  includedirs(includeDirs)
  
  -- Set up filter
  vpaths({[".proj"] = {"**/*.txt"}})
  --vpaths({[".proj"] = {"**/*main.cpp", "**/*Main.cpp"}})
  --vpaths({[".proj"] = {"**/*precompiled.h", "**/*precompiled.cpp", "**/*precompiled.c", "**/*Precompiled.h", "**/*Precompiled.cpp", "**/*Precompiled.c"}})
  vpaths({[".proj"] = {"**/*.rc" }})

  for _, folder in ipairs(directories)
  do
    vpaths({[string.sub(folder, string.len(fileDir) + 2)]
                     = {folder .. "/*.h"  ,
                        folder .. "/*.hpp",
                        folder .. "/*.c"  ,
                        folder .. "/*.cpp"}})
  end

  -- include all files following these patterns
  files({fileDir .. "/**.h"  ,
         fileDir .. "/**.hpp",
         fileDir .. "/**.c"  ,
         fileDir .. "/**.cpp", 
         fileDir .. "/**.txt"})
  
  -- include a precompiled header the fullpath should NOT be specified
  pchheader(pchFile .. ".h")
  -- set the pch creator, specified by fullpath name
  if projName ~= "Lua" then
    pchsource(fileDir .. "/" .. pchFile .. ".cpp")
  else
    pchsource(fileDir .. "/" .. pchFile .. ".c")
  end
  -- set build options
  opt = "/FI" .. pchFile .. ".h"  -- force include precompiled header
  buildoptions({opt})	                        -- apply build options to project
  
  -- set character set
  for _, name in ipairs(unicodeApps)
  do
    if projName == name then
      flags({"Unicode"})
    end
  end

  -- set configuration-specific stuff
  -- NOTE: the target directory for applications are set to the working directory
  --       because Visual Studio 2012 ignores debug working directory (a known bug)
  -- set debug configuration
  configuration("Debug")
    optimize("Off")
    targetname(projName .. "_debug")
    defines(debugDefines)
    flags(debugFlags)
    targetdir(outDir)

  -- set release configuration
  configuration("Release")
    optimize("Speed")
    targetname(projName .. "_release")
    defines(releaseDefines)
    flags(releaseFlags)
    targetdir(outDir)

  -- set ship configuration
  configuration("Ship")
    optimize("Full")
    targetname(projName)
    defines(shipDefines)
    flags(shipFlags)
    targetdir(outDir)
end

-- premake5 main function
-- checking for nil lets --help action work
if _ACTION ~= nil then
  -- Create Solution
  workspace(solutionName)
    configurations({"Debug", "Release", "Ship"})
    platforms("x64")
    location(solutionDir)
    debugdir(workingDir)
    --postbuildcommands { "\"$(TargetPath)\"" } -- actions to run after build
    
    for _, lib in ipairs(libFiles) do
      links(lib)
    end

    -- steps to perform on clean action
    if _ACTION == "clean" then
      os.rmdir(solutionDir)
      return
    end
    
    -- create windowed apps, libraries, and console apps
    for j = 1, 4 do
    -- set default variable names
      precompiledName = "precompiled"
      locationPath = projectDir
      if j == 1 then
        appType = "WindowedApp"
        for _, proj in ipairs(winApps) do
          project(proj)
          SetUpProj(proj, appType, locationPath, precompiledName, sourceDir .. "/app/" .. proj, outDir)
        end
      elseif j == 2 then
        appType = "ConsoleApp"
        for _, proj in ipairs(consoleApps) do
          project(proj)
          SetUpProj(proj, appType, locationPath, precompiledName, sourceDir .. "/app/" .. proj, outDir)
        end
      elseif j == 3 then
        appType = "StaticLib"
        for _, proj in ipairs(staticLibs) do
          project(proj)
          SetUpProj(proj, appType, locationPath, precompiledName, sourceDir .. "/lib/" .. proj, outDir)
        end
      elseif j == 4 then
        appType = "SharedLib"
        for _, proj in ipairs(dynamicLibs) do
          project(proj)
          SetUpProj(proj, appType, locationPath, precompiledName, sourceDir .. "/dll/" .. proj, outDir)
        end
      end -- if/else
    end -- outer for loop
end -- action is nil check
