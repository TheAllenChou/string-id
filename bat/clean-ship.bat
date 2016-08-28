@echo off
cd ..

:: remove binary folders
echo Removing ship binaries...
if exist "obj/Ship" rmdir /Q /S "obj/Ship"
if exist "bin/Ship" rmdir /Q /S "bin/Ship"
for %%i in (working\*.exe) do del %%i
for %%i in (working\*.ilk) do del %%i
for %%i in (working\*.pdb) do del %%i
for %%i in (working\*.exp) do del %%i
for %%i in (working\*.lib) do del %%i
echo Done.

cd bat
@echo on
