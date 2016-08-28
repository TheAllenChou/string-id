@echo off
cd ..

"%VSINSTALLDIR%/../Common7/IDE/devenv.com" "sln/string-id.sln" /build "Debug"
"%VSINSTALLDIR%/../Common7/IDE/devenv.com" "sln/string-id.sln" /build "Release"
"%VSINSTALLDIR%/../Common7/IDE/devenv.com" "sln/string-id.sln" /build "Ship"

cd bat
@echo on
