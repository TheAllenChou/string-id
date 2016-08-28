@echo off
cd ..

:: remove binary folders
echo Removing release binaries...
if exist "obj/Release" rmdir /Q /S "obj/Release"
if exist "bin/Release" rmdir /Q /S "bin/Release"
for %%i in (working\*_release.exe) do del %%i
for %%i in (working\*_release.ilk) do del %%i
for %%i in (working\*_release.pdb) do del %%i
for %%i in (working\*_release.exp) do del %%i
for %%i in (working\*_release.lib) do del %%i
echo Done.

cd bat
@echo on
