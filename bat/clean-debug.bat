@echo off
cd ..

:: remove binary folders
echo Removing debug binaries...
if exist "obj/Debug" rmdir /Q /S "obj/Debug"
if exist "bin/Debug" rmdir /Q /S "bin/Debug"
for %%i in (working\*_debug.exe) do del %%i
for %%i in (working\*_debug.ilk) do del %%i
for %%i in (working\*_debug.pdb) do del %%i
for %%i in (working\*_debug.exp) do del %%i
for %%i in (working\*_debug.lib) do del %%i
echo Done.

cd bat
@echo on
