@echo off

echo.
echo ===== Updating git submodules...
pushd ..\..\
call git submodule update --init --recursive --remote
popd

echo.
echo ===== Generating Visual Studio 2022 workspace...
pushd ..\..\
call vendor\tools\premake\windows\premake5.exe vs2022
popd

echo.
echo ===== Done.
pause
