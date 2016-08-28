@echo off
cd ..

"%VSINSTALLDIR%/../Common7/IDE/devenv.com" "sln/string-id.sln" /rebuild "Release"

cd bat
@echo on
