@echo off
cd ..

"%VSINSTALLDIR%/../Common7/IDE/devenv.com" "sln/string-id.sln" /build "Release"

cd bat
@echo on
