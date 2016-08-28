@echo off
cd ..

"%VSINSTALLDIR%/../Common7/IDE/devenv.com" "sln/string-id.sln" /rebuild "Ship"

cd bat
@echo on
