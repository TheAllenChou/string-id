@echo off
cd ..

:: remove solutionn folder
echo Removing Visual Studio solution...
if exist sln rmdir /Q /S sln
echo Done.

cd bat
@echo on
