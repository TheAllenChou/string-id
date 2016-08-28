@echo off
cd ..

:: remove binary folders
echo Removing all binaries...
if exist obj rmdir /Q /S obj
if exist bin rmdir /Q /S bin
for %%i in (working\*.exe) do del %%i
for %%i in (working\*.ilk) do del %%i
for %%i in (working\*.pdb) do del %%i
for %%i in (working\*.exp) do del %%i
for %%i in (working\*.lib) do del %%i
echo Done.

cd bat
@echo on
