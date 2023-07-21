@echo off

echo.
echo ===== Downloading Vulkan SDK...
REM see also:
REM https://vulkan.lunarg.com/doc/sdk
powershell -Command "Invoke-WebRequest https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe -OutFile vulkan-sdk.exe"
pause

echo.
echo ===== Installing Vulkan SDK...
call vulkan-sdk.exe
echo Install Vulkan SDK...
pause

rem echo.
rem echo ===== Generating Visual Studio 2022 workspace...
rem pushd ..\..\
rem call vendor\premake\windows\premake5.exe vs2022
rem popd

echo.
echo ===== Done.
pause
